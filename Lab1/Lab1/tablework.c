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
	return 0;
}

int deleteDataFromTable(struct FileMapping* a, int table_num, struct queryCondition conds[], int el_in_conds) {
	// количество записей
	int cnt = 0;
	struct table_data table = { .table_num = table_num };
	getTableData(a, table_num, &table);

	int while_flag = 1;
	int current_page = table.start_page_num;
	int current_cell = 0;
	while (while_flag) {
		int next_raw_size = getRawSize(a, current_page, current_cell);
		struct cell* current_raw = malloc(next_raw_size * sizeof(struct cell));
		copyRawToArray(a, current_page, current_cell, current_raw);
		if (compareRawWithCondition(a, current_raw, next_raw_size, conds, el_in_conds)) {
			deleteRaw(a, current_page, current_cell);
			cnt++;
		}
		free(current_raw);
		int getNextRes = getNextRaw(a, &current_page, &current_cell);
		if (getNextRes == -1)
			while_flag = 0;
	}

	int previos = -1;
	current_page = table.start_page_num;
	while_flag = 1;
	while (while_flag) {
		int compressed = pageCompresser(a, current_page);
		int next_page_val = getTransportCellFromNum(a, current_page);
		if (next_page_val == -1) {
			while_flag = 0;
			continue;
		}
		if (compressed == 0) {
			if (previos == -1) {
				table.start_page_num = current_page;
				modifyTableData(a, table_num, &table);
			}
			else {
				setTransportCell(a, previos, next_page_val);
			}
			setMyPageFree(a, current_page);
			registerFreePage(a, current_page);
		}
		previos = current_page;
		current_page = next_page_val;
	}

	return cnt;
}

int deleteRaw(struct FileMapping* a, int start_page_num, int start_cell_num) {
	struct map_file_of_view page;
	openMyPage(a, start_page_num, &page);
	struct cell* now_cell = (struct cell*)page.my_page_start;
	now_cell += start_cell_num;

	int raw_num = now_cell->int_data;
	int while_flag = 1;
	while (while_flag) {
		if (now_cell->flag == TEMPEST || (now_cell->flag == RAW_NUM && now_cell->int_data != raw_num)) {
			while_flag = 0;
			continue;
		}
		if (now_cell->flag == TRANSPORT) {

			if (now_cell->int_data == -1) {
				while_flag = 0;
			}
			else {
				int new_page = now_cell->int_data;
				UnmapViewOfFile(page.start);
				openMyPage(a, new_page, &page);
				now_cell = (struct cell*)page.my_page_start;
			}
			continue;
		}
		now_cell->flag = TEMPEST;
		now_cell++;
	}
	UnmapViewOfFile(page.start);
	return 0;
}


int selectTableData(struct FileMapping* a, int table_num, struct queryCondition conds[], int el_in_conds){
	// количество записей
	int cnt = 0;
	struct table_data table = { .table_num = table_num };
	getTableData(a, table_num, &table);
	
	int while_flag = 1;
	int current_page = table.start_page_num;
	int current_cell = 0;
	while (while_flag) {
		int next_raw_size = getRawSize(a, current_page, current_cell);
		struct cell* current_raw = malloc(next_raw_size * sizeof(struct cell));
		copyRawToArray(a, current_page, current_cell, current_raw);
		if (compareRawWithCondition(a, current_raw, next_raw_size, conds, el_in_conds)) {
			//printRaw(current_raw, next_raw_size);
			cnt++;
		}
		free(current_raw);
		int getNextRes = getNextRaw(a, &current_page, &current_cell);
		if (getNextRes == -1)
			while_flag = 0;
	}

	return cnt;
}

int getRawSize(struct FileMapping* a, int start_page_num, int start_cell_num) {
	struct map_file_of_view page;
	openMyPage(a, start_page_num, &page);
	struct cell* now_cell = (struct cell*)page.my_page_start;
	now_cell += start_cell_num;

	int cnt = 0;
	int raw_num = now_cell->int_data;
	int while_flag = 1;
	while (while_flag) {
		if (now_cell->flag == TEMPEST || (now_cell->flag == RAW_NUM && now_cell->int_data != raw_num)) {
			while_flag = 0;
			continue;
		}
		if (now_cell->flag == TRANSPORT) {

			if (now_cell->int_data == -1) {
				while_flag = 0;
			}
			else {
				int new_page = now_cell->int_data;
				UnmapViewOfFile(page.start);
				openMyPage(a, new_page, &page);
				now_cell = (struct cell*)page.my_page_start;
			}
			continue;
		}
		cnt++;
		now_cell++;
		
	}
	UnmapViewOfFile(page.start);
	return cnt;
}

int copyRawToArray(struct FileMapping* a, int start_page_num, int start_cell_num, struct cell current_raw[]) {
	struct map_file_of_view page;
	openMyPage(a, start_page_num, &page);
	struct cell* now_cell = (struct cell*)page.my_page_start;
	now_cell += start_cell_num;

	int raw_num = now_cell->int_data;
	int while_flag = 1;
	int current_raw_index = 0;
	while (while_flag) {
		if (now_cell->flag == TEMPEST || (now_cell->flag == RAW_NUM && now_cell->int_data != raw_num)) {
			while_flag = 0;
			continue;
		}
		if (now_cell->flag == TRANSPORT) {

			if (now_cell->int_data == -1) {
				while_flag = 0;
			}
			else {
				int new_page = now_cell->int_data;
				UnmapViewOfFile(page.start);
				openMyPage(a, new_page, &page);
				now_cell = (struct cell*)page.my_page_start;
			}
			continue;
		}
		current_raw[current_raw_index++] = (*now_cell);
		now_cell++;

	}
	UnmapViewOfFile(page.start);
}

int compareRawWithCondition(struct FileMapping* a, struct cell current_raw[], int current_raw_el, struct queryCondition conds[], int el_in_conds) {

	for (int i = 0; i < el_in_conds; i++) {
		int l = 0, r = 0;
		getColumn(current_raw, current_raw_el, conds[i].stolbec_num, &l, &r);
		int int_value = -1e9;
		double double_value = -1e9;
		int ret_value = 0;
		if (current_raw[l].flag == STRING_CONTINUE || current_raw[l].flag == STRING_END) {
			int_value = (r - l - 1) * string_data_size_in_cell;
			for (int j = 0; j < string_data_size_in_cell; j++) {
				if (current_raw[r - 1].string_data[j] == '\0')
					break;
				int_value++;
			}
		}
		else {
			if (current_raw[l].type_of == INT_MYTYPE)
				int_value = current_raw[l].int_data;
			else
				double_value = current_raw[l].double_data;
		}

		if (conds[i].sign == '>') {
			if (int_value != -1e9) {
				ret_value = (int_value > conds[i].int_data);
			}
			else {
				ret_value = (double_value > conds[i].double_data);
			}
		}

		if (conds[i].sign == '<') {
			if (int_value != -1e9) {
				ret_value = (int_value < conds[i].int_data);
			}
			else {
				ret_value = (double_value < conds[i].double_data);
			}
		}

		if (conds[i].sign == '=') {
			if (int_value != -1e9) {
				ret_value = (int_value == conds[i].int_data);
			}
			else {
				ret_value = (double_value == conds[i].double_data);
			}
		}

		if (ret_value == 0)
			return 0;

	}
	return 1;
}

int getColumn(struct cell current_raw[], int current_raw_el, int column_num, int* l, int* r) {
	int cnt = 0;
	*l = -1;
	*r = -1;
	for (int i = 0; i < current_raw_el; i++) {
		if (cnt == column_num && *l == -1) {
			*l = i;
		}
		if (cnt != column_num && *l != -1) {
			*r = i;
			break;
		}
		if (current_raw[i].flag != STRING_CONTINUE)
			cnt++;
	}
	if (*r == -1)
		*r = current_raw_el;
	return 0;
}


int printRaw(struct cell current_raw[], int raw_size) {
	for (int i = 0; i < raw_size; i++) {

		if (current_raw[i].flag == STRING_END) {
			for (int j = 0; j < string_data_size_in_cell; j++) {
				if (current_raw[i].string_data[j] == '\0')
					break;
				printf("%c", current_raw[i].string_data[j]);
			}
			printf("\n");
			continue;
		}

		if (current_raw[i].flag == STRING_CONTINUE) {
			for (int j = 0; j < string_data_size_in_cell; j++) {
				printf("%c", current_raw[i].string_data[j]);
			}
			continue;
		}

		if (current_raw[i].flag == RAW_NUM) {
			//printf("%d ", current_raw[i].int_data);
			continue;
		}

		if (current_raw[i].type_of == INT_MYTYPE) {
			printf("%d ", current_raw[i].int_data);
		}
		else {
			printf("%f ", current_raw[i].double_data);
		}
	}
	printf("\n");
	return 0;
}

int getNextRaw(struct FileMapping* a, int *start_page_num, int *start_cell_num) {
	struct map_file_of_view page;
	openMyPage(a, *start_page_num, &page);
	struct cell* now_cell = (struct cell*)page.my_page_start;
	now_cell += *start_cell_num;

	int cnt = 0;
	int raw_num = now_cell->int_data;
	int while_flag = 1;
	int current_raw_index = 0;
	int ret_value = 0;
	while (while_flag) {
		if ((now_cell->flag == RAW_NUM && now_cell->int_data != raw_num)) {
			while_flag = 0;
			continue;
		}
		if (now_cell->flag == TRANSPORT) {

			if (now_cell->int_data == -1) {
				while_flag = 0;
				ret_value = -1;
			}
			else {
				int new_page = now_cell->int_data;
				UnmapViewOfFile(page.start);
				openMyPage(a, new_page, &page);
				*start_page_num = new_page;
				*start_cell_num = 0;
				now_cell = (struct cell*)page.my_page_start;
			}
			continue;
		}
		cnt++;
		now_cell++;
		*start_cell_num += 1;
	}
	UnmapViewOfFile(page.start);
	return ret_value;
}



int changeRaw(struct FileMapping* a, int table_num, struct cell value[], int el_in_array, struct queryCondition conds[], int el_in_conds) {
	deleteDataFromTable(a, table_num, conds, el_in_conds);
	addRawToTable(a, table_num, value, el_in_array);
}