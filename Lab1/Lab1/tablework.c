#include "tablework.h"

int initTable(struct FileMapping* a, struct table_data* new_table) {
	struct map_file_of_view* page = openMyPage(a, table_block_start);
	struct table_data* now = (struct table_data*) page->my_page_start;
	int while_flag = 0;
	int return_value = -1;
	while (1) {
		for (int i = 0; i < tableStructOnList; i++) {
			if (now->flag != TABLE_ACTIVE) {
				now->flag = TABLE_ACTIVE;
				now->rows_count = 0;

				int num = getFreePage(a);
				now->page_num = num;
				setTableStartOnPage(a, num);
				return_value = num;

				now->row_size = new_table->row_size;
				now->table_num = new_table->table_num;
				while_flag = 1;
				break;
			}
			now++;
		}
		if (while_flag) {
			break;
		}
		else {
			int num = getTransportCell(page);
			if (num == -1) {
				int temp = getFreePage(a);
				setTransportCell(page, temp);
				num = temp;
			}
			UnmapViewOfFile(page->start);
			page = openMyPage(a, num);
			now = (struct table_data*)page->my_page_start;
		}
	}

	UnmapViewOfFile(page->start);
	return return_value;
}


int deleteTable(struct FileMapping* a, int num) {
	struct map_file_of_view* page = openMyPage(a, table_block_start);
	struct table_data* now = (struct table_data*)page->my_page_start;
	int while_flag = 0;
	int list_to_delete = -1;
	int ret_value = 0;
	while (1) {
		for (int i = 0; i < tableStructOnList; i++) {
			if (now->table_num == num) {
				now->flag = TABLE_DELETED;
				list_to_delete = now->page_num;
				while_flag = 1;
				break;
			}
			now++;
		}
		if (while_flag) {
			break;
		}
		else {
			int num = getTransportCell(page);
			if (num == -1) {
				ret_value = 1;
				break;
			}
			UnmapViewOfFile(page->start);
			page = openMyPage(a, num);
			now = (struct table_data*)page->my_page_start;
		}
	}

	while (list_to_delete != -1) {
		struct map_file_of_view* deleted_list = openMyPage(a, table_block_start);
		int temp = getTransportCell(deleted_list);
		UnmapViewOfFile(deleted_list->start);

		setMyPageFree(a, list_to_delete);
		registerFreePage(a, list_to_delete);
		list_to_delete = temp;
	}
	
	UnmapViewOfFile(page->start);
	return ret_value;
}

int getTableData(struct FileMapping* a, int num, struct table_data* table) {
	struct map_file_of_view* page = openMyPage(a, table_block_start);
	struct table_data* now = (struct table_data*)page->my_page_start;
	int while_flag = 0;
	int list_to_delete = -1;
	int ret_value = 0;
	while (1) {
		for (int i = 0; i < tableStructOnList; i++) {
			if (now->table_num == num && now->flag == TABLE_ACTIVE) {
				table->flag = now->flag;
				table->page_num = now->page_num;
				table->row_size = now->row_size;
				table->table_num = now->table_num;
				while_flag = 1;
				break;
			}
			now++;
		}
		if (while_flag) {
			break;
		}
		else {
			int num = getTransportCell(page);
			if (num == -1) {
				ret_value = -1;
				break;
			}
			UnmapViewOfFile(page->start);
			page = openMyPage(a, num);
			now = (struct table_data*)page->my_page_start;
		}
	}

	UnmapViewOfFile(page->start);
	return ret_value;
}

int modifyTableData(struct FileMapping* a, int num, struct table_data* table) {
	struct map_file_of_view* page = openMyPage(a, table_block_start);
	struct table_data* now = (struct table_data*)page->my_page_start;
	int while_flag = 0;
	int list_to_delete = -1;
	int ret_value = 0;
	while (1) {
		for (int i = 0; i < tableStructOnList; i++) {
			if (now->table_num == num && now->flag == TABLE_ACTIVE) {
				now->flag = table->flag;
				now->page_num = table->page_num;
				now->row_size = table->row_size;
				now->table_num = table->table_num;
				while_flag = 1;
				break;
			}
			now++;
		}
		if (while_flag) {
			break;
		}
		else {
			int num = getTransportCell(page);
			if (num == -1) {
				ret_value = -1;
				break;
			}
			UnmapViewOfFile(page->start);
			page = openMyPage(a, num);
			now = (struct table_data*)page->my_page_start;
		}
	}

	UnmapViewOfFile(page->start);
	return ret_value;
}


//самое сложное начинается здесь
int addRawToTable(struct FileMapping* a, int num, struct cell raw[], int el_in_array) {
	struct table_data table = { .table_num = num };
	getTableData(a, num, &table);

	int in_raw = countCellsInRaw(raw, el_in_array);
	int return_value = 0;
	if (in_raw == -1 || in_raw != table.row_size + 1) {
		return_value = -1;
	}
	else {
		int cells_required = el_in_array;

		int page_start = -1;
		int now_page = table.page_num;

		while (now_page != -1 && cells_required < cellsOnList) {

			int count = countFreeCellsOnPage(a, now_page);
			if (count >= cells_required) {
				page_start = now_page;
				break;
			}
			struct map_file_of_view* page = openMyPage(a, now_page);
			now_page = getTransportCell(page);
			UnmapViewOfFile(page->start);
		}

		if (page_start == -1) { // нужна новая страница под данную запись
			int new_page_num = getFreePage(a); // получили номер страницы, где мы будем хранить информацию
			int first_table_page = table.page_num;
			struct map_file_of_view* page = openMyPage(a, first_table_page); // открыли первую страницу таблицы
			int second_page = getTransportCell(page); // получили значение перехода в первой странице таблицы 
			setTransportCell(page, new_page_num); // теперь наша первая страница переходит на новую
			UnmapViewOfFile(page->start);

			page = openMyPage(a, new_page_num);
			setTransportCell(page, second_page);
			fillerDataToPage(a, new_page_num, 0, raw, el_in_array);

		}
		else { // значит знаем куда писать
			int find_first_free = get_first_free_cell(a, page_start);
			fillerDataToPage(a, page_start, find_first_free, raw, el_in_array);
		}
		table.rows_count++;
		modifyTableData(a, table.table_num, &table);
	}
	return return_value;
}

int deleteRawFromTable(struct FileMapping* a, int table_num, int raw_num) {
	struct table_data table = { .table_num = table_num };
	getTableData(a, table_num, &table);
	int return_value = 0;
	if (raw_num > table.rows_count) {
		return_value = -1;
	}
	else {
		int now_page = table.page_num;
		struct map_file_of_view* page = openMyPage(a, now_page); // открыли первую страницу таблицы
		struct cell* now_cell = (struct cell*)page->my_page_start;

		int our_raw_page_start = -1;
		int our_raw_cell_start = -1;
		
		int while_flag = 0;
		while (!while_flag) {
			for (int i = 0; i < cellsOnList; i++) {
				if (now_cell->flag == RAW_NUM && now_cell->int_data == raw_num) {
					our_raw_page_start = now_page;
					our_raw_cell_start = i;
					while_flag = 1;
					break;
				}
			}
			if (while_flag)
				continue;


			int num = getTransportCell(page);
			if (num == -1) {
				return_value = -1;
				while_flag = 1;
				continue;;
			}
			UnmapViewOfFile(page->start);
			page = openMyPage(a, num);
			now_cell = (struct table_data*)page->my_page_start;
		}

		if (our_raw_page_start != -1 && our_raw_cell_start != -1) {
			struct map_file_of_view* page = openMyPage(a, our_raw_page_start);
			struct cell* now_cell = (struct cell*)page->my_page_start;
			now_cell += our_raw_cell_start;
			while_flag = 0;
			int our_raw_page_now = our_raw_page_start;
			while (!while_flag) {
				for (int j = our_raw_cell_start; j < cellsOnList; j++) {
					if (now_cell->flag != RAW_NUM || (now_cell->flag == RAW_NUM && now_cell->int_data == raw_num)) {
						now_cell->flag = TEMPEST;
					}
					else {
						while_flag = 1;
						break;
					}
					now_cell++;
					our_raw_cell_start++;
				}
				if (while_flag)
					continue;

				int num = getTransportCell(page);
				if (num == -1) {
					while_flag = 1;
					continue;
				}
				UnmapViewOfFile(page->start);

				if (our_raw_page_now != our_raw_page_start) { // пометить свободной
					setMyPageFree(a, our_raw_page_now);
					registerFreePage(a, our_raw_page_now);
				}

				page = openMyPage(a, num);
				our_raw_page_now = num;
				now_cell = (struct table_data*)page->my_page_start;
				our_raw_cell_start = 0;
			}
			UnmapViewOfFile(page->start);

			pageCompresser(a, our_raw_page_start);
			if (our_raw_page_now != our_raw_page_start) {
				page = openMyPage(a, our_raw_page_start);
				pageCompresser(a, our_raw_page_now);
				setTransportCell(page, our_raw_page_now);
			}
			
		}

	}
	return return_value;
}