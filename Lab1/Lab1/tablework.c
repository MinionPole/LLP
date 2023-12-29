#include "tablework.h"
#include <stdio.h>

int initTable(struct FileMapping* a, struct table_data* new_table) {
	struct map_file_of_view page;
	openMyPage(a, table_block_start, &page);
	struct table_data* now = (struct table_data*) page.my_page_start;
	int while_flag = 0;
	int return_value = -1;
	int now_page = table_block_start;
	while (1) {
		for (int i = 0; i < tableStructOnList; i++) {
			if (now->flag != TABLE_ACTIVE) {
				now->flag = TABLE_ACTIVE;
				now->rows_count = 0;

				int num = getFreePage(a);
				now->start_page_num = num;
				now->last_page_num = num;
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
			int num = getTransportCell(&page);
			if (num == -1) {
				int temp = getFreePage(a);
				setTransportCell(a, now_page, temp);
				num = temp;
			}
			UnmapViewOfFile(page.start);
			openMyPage(a, num, &page);
			now = (struct table_data*)page.my_page_start;
			now_page = num;
		}
	}

	UnmapViewOfFile(page.start);
	return return_value;
}


int deleteTable(struct FileMapping* a, int num) {
	struct map_file_of_view page;
	openMyPage(a, table_block_start, &page);
	struct table_data* now = (struct table_data*)page.my_page_start;
	int while_flag = 0;
	int list_to_delete = -1;
	int ret_value = 0;
	while (1) {
		for (int i = 0; i < tableStructOnList; i++) {
			if (now->table_num == num) {
				now->flag = TABLE_DELETED;
				list_to_delete = now->start_page_num;
				while_flag = 1;
				break;
			}
			now++;
		}
		if (while_flag) {
			break;
		}
		else {
			int num = getTransportCell(&page);
			if (num == -1) {
				ret_value = 1;
				break;
			}
			UnmapViewOfFile(page.start);
			openMyPage(a, num, &page);
			now = (struct table_data*)page.my_page_start;
		}
	}

	while (list_to_delete != -1) {
		struct map_file_of_view deleted_list;
		openMyPage(a, table_block_start, &deleted_list);
		int temp = getTransportCell(&deleted_list);
		UnmapViewOfFile(deleted_list.start);

		setMyPageFree(a, list_to_delete);
		registerFreePage(a, list_to_delete);
		list_to_delete = temp;
	}
	
	UnmapViewOfFile(page.start);
	return ret_value;
}

int getTableData(struct FileMapping* a, int num, struct table_data* table) {
	struct map_file_of_view page;
	openMyPage(a, table_block_start, &page);
	struct table_data* now = (struct table_data*)page.my_page_start;
	int while_flag = 0;
	int list_to_delete = -1;
	int ret_value = 0;
	while (1) {
		for (int i = 0; i < tableStructOnList; i++) {
			if (now->table_num == num && now->flag == TABLE_ACTIVE) {
				table->flag = now->flag;
				table->start_page_num = now->start_page_num;
				table->last_page_num = now->last_page_num;
				table->row_size = now->row_size;
				table->table_num = now->table_num;
				table->rows_count = now->rows_count;
				while_flag = 1;
				break;
			}
			now++;
		}
		if (while_flag) {
			break;
		}
		else {
			int num = getTransportCell(&page);
			if (num == -1) {
				ret_value = -1;
				break;
			}
			UnmapViewOfFile(page.start);
			openMyPage(a, num, &page);
			now = (struct table_data*)page.my_page_start;
		}
	}

	UnmapViewOfFile(page.start);
	return ret_value;
}

int modifyTableData(struct FileMapping* a, int num, struct table_data* table) {
	struct map_file_of_view page;
	openMyPage(a, table_block_start, &page);
	struct table_data* now = (struct table_data*)page.my_page_start;
	int while_flag = 0;
	int list_to_delete = -1;
	int ret_value = 0;
	while (1) {
		for (int i = 0; i < tableStructOnList; i++) {
			if (now->table_num == num && now->flag == TABLE_ACTIVE) {
				now->flag = table->flag;
				now->start_page_num = table->start_page_num;
				now->last_page_num = table->last_page_num;
				now->row_size = table->row_size;
				now->table_num = table->table_num;
				now->rows_count = table->rows_count;
				while_flag = 1;
				break;
			}
			now++;
		}
		if (while_flag) {
			break;
		}
		else {
			int num = getTransportCell(&page);
			if (num == -1) {
				ret_value = -1;
				break;
			}
			UnmapViewOfFile(page.start);
			openMyPage(a, num, &page);
			now = (struct table_data*)page.my_page_start;
		}
	}

	UnmapViewOfFile(page.start);
	return ret_value;
}


//самое сложное начинается здесь
int addRawToTable(struct FileMapping* a, int table_num, struct cell raw[], int el_in_array) {
	struct table_data table = { .table_num = table_num };
	getTableData(a, table_num, &table);

	int in_raw = countCellsInRaw(raw, el_in_array);
	int return_value = 0;
	if (in_raw == -1 || in_raw != table.row_size + 1) {
		return_value = -1;
	}
	else {
		int cells_required = el_in_array;

		int page_start = -1;
		int now_page = table.last_page_num;

		int count = countFreeCellsOnPage(a, now_page);
		if (count >= cells_required) {
			page_start = now_page;
		}

		if (page_start == -1) { // нужна новая страница под данную запись
			int new_page_num = getFreePage(a); // получили номер страницы, где мы будем хранить информацию
			int last_table_page = table.last_page_num;
			setTransportCell(a, last_table_page, new_page_num); // теперь наша первая страница переходит на новую
			page_start = new_page_num;
		}

		int find_first_free = get_first_free_cell(a, page_start);
		int new_last_page = fillerDataToPage(a, page_start, find_first_free, raw, el_in_array);
		table.last_page_num = new_last_page;
		//table.rows_count++;
		modifyTableData(a, table.table_num, &table);
		//free(&table);
	}
	return return_value;
}

int deleteRawFromTable(struct FileMapping* a, int table_num, int raw_num) {
	struct table_data table = { .table_num = table_num };
	getTableData(a, table_num, &table);
	int return_value = 0;
	if (0) {
		return_value = -1;
	}
	else {
		int now_page = table.start_page_num;
		struct map_file_of_view page;
		openMyPage(a, now_page, &page); // открыли первую страницу таблицы
		struct cell* now_cell = (struct cell*)page.my_page_start;

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
				now_cell++;
			}
			if (while_flag)
				continue;


			int num = getTransportCell(&page);
			if (num == -1) {
				return_value = -1;
				while_flag = 1;
				continue;;
			}
			UnmapViewOfFile(page.start);
			openMyPage(a, num, &page);
			now_cell = (struct table_data*)page.my_page_start;
		}
		UnmapViewOfFile(page.start);

		if (our_raw_page_start != -1 && our_raw_cell_start != -1) {
			struct map_file_of_view page;
			openMyPage(a, our_raw_page_start, &page);
			struct cell* now_cell = (struct cell*)page.my_page_start;
			now_cell += our_raw_cell_start;
			while_flag = 0;
			int our_raw_page_now = our_raw_page_start;
			while (!while_flag) {
				for (int j = our_raw_cell_start; j < cellsOnList; j++) {
					if ((now_cell->flag != RAW_NUM && now_cell->flag != TEMPEST) || (now_cell->flag == RAW_NUM && now_cell->int_data == raw_num)) {
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

				int num = getTransportCell(&page);
				if (num == -1) {
					while_flag = 1;
					continue;
				}
				UnmapViewOfFile(page.start);

				if (our_raw_page_now != our_raw_page_start) { // пометить свободной
					setMyPageFree(a, our_raw_page_now);
					registerFreePage(a, our_raw_page_now);
				}

				openMyPage(a, num, &page);
				our_raw_page_now = num;
				now_cell = (struct table_data*)page.my_page_start;
				our_raw_cell_start = 0;
			}
			UnmapViewOfFile(page.start);

			int not_free_in_raw_page_start = pageCompresser(a, our_raw_page_start);

			if (our_raw_page_now != our_raw_page_start) {
				openMyPage(a, our_raw_page_start, &page);
				pageCompresser(a, our_raw_page_now);
				setTransportCell(a, our_raw_page_start, our_raw_page_now);
				UnmapViewOfFile(page.start);
			}
			if (not_free_in_raw_page_start == 0) {
				if (our_raw_page_start == table.start_page_num) {
					openMyPage(a, our_raw_page_start, &page);
					int new_table_page_start = getTransportCell(&page);
					UnmapViewOfFile(page.start);
					if (new_table_page_start != -1) {
						setMyPageFree(a, our_raw_page_start);
						registerFreePage(a, our_raw_page_start);
						table.start_page_num = new_table_page_start;
						modifyTableData(a, table_num, &table);
					}
				}
				else {
					openMyPage(a, table.start_page_num, &page);
					int previous = table.start_page_num;
					int next_page_in_table = getTransportCell(&page);
					while (next_page_in_table != our_raw_page_start) {
						UnmapViewOfFile(page.start);
						openMyPage(a, next_page_in_table, &page);
						previous = next_page_in_table;
						next_page_in_table = getTransportCell(&page);
					}
					UnmapViewOfFile(page.start);

					openMyPage(a, our_raw_page_start, &page);
					int new_table_page_start = getTransportCell(&page);
					UnmapViewOfFile(page.start);
					setMyPageFree(a, our_raw_page_start);
					registerFreePage(a, our_raw_page_start);
					setTransportCell(a, previous, new_table_page_start);
				}
			}
			
		}

	}
	return return_value;
}


int selectTableData(struct FileMapping* a, int table_num) {
	struct table_data table = { .table_num = table_num };
	getTableData(a, table_num, &table);
	struct map_file_of_view page;
	openMyPage(a, table.start_page_num, &page);
	struct cell* now = (struct cell*)page.my_page_start;
	
	int end_flag = 1;
	int cnt = 0;
	while (end_flag) {
		for (int i = 0; i < cellsOnList; i++, now++) {
			if (now->flag == RAW_NUM) {
				//printf("%s", "\n");
				cnt++;
				continue;
			}
			if (now->flag == TABLE_START) {
				continue;
			}
			if (now->flag == STRING_END) {
				for (int j = 0; j < string_data_size_in_cell; j++) {
					if (now->string_data[j] == '\0') {
						break;
					}
					//printf("%c", now[j]);
				}
				//printf("%s", " ");
				continue;
			}
			if (now->flag == STRING_CONTINUE) {
				for (int j = 0; j < string_data_size_in_cell; j++) {
					//printf("%c", now[j]);
				}
				continue;
			}

			if (now->flag != TEMPEST) {
				if (now->type_of == INT_MYTYPE) {
					//printf("%d ", now->int_data);
				}
				else {
					//printf("%f ", now->double_data);
				}
			}
		}
		int new_page_num = now->int_data;
		if (new_page_num == -1) {
			end_flag = 0;
			continue;
		}
		UnmapViewOfFile(page.start);
		openMyPage(a, new_page_num, &page);
		now = (struct cell*)page.my_page_start;;
	}
	UnmapViewOfFile(page.start);
	return cnt;
}

int changeRaw(struct FileMapping* a, int table_num, struct cell value[], int el_in_array) {
	deleteRawFromTable(a, table_num, value[0].int_data);
	addRawToTable(a, table_num, value, el_in_array);
}