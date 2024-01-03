#pragma once
#include "fileWork.h"

int initTable(struct FileMapping* a, struct table_data* table);

int deleteTable(struct FileMapping* a, int num);

int getTableData(struct FileMapping* a, int num, struct table_data* table);

int modifyTableData(struct FileMapping* a, int num, struct table_data* table);

int addRawToTable(struct FileMapping* a, int table_num, struct cell raw[], int el_in_array);

int deleteRawFromTable(struct FileMapping* a, int table_num, int raw_num);

int changeRaw(struct FileMapping* a, int table_num, struct cell value[], int el_in_array);

int selectTableData(struct FileMapping* a, int table_num, struct queryCondition conds[], int el_in_conds);

int getRawSize(struct FileMapping* a, int start_page_num, int start_cell_num);

int getRawFromTable(struct FileMapping* a, struct table_data* table, struct cell raw[], int raw_size);

int getValueFromRaw(struct cell raw[], int num);

int getValueFromTable(struct FileMapping* a, struct table_data* table, int n, int m, struct cell* value[]);

