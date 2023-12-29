#include "fileWork.h"

struct FileMapping* openMapping(char* fileName) {
	HANDLE hFile = CreateFileA(fileName, GENERIC_READ | GENERIC_WRITE, 0, NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		return NULL;
	}

	DWORD dwhighSize = 0;
	DWORD dwLowSize = GetFileSize(hFile, dwhighSize);
	if (dwLowSize == INVALID_FILE_SIZE) {
		CloseHandle(hFile);
		return NULL;
	}


	HANDLE hMapping = CreateFileMapping(hFile, NULL, PAGE_READWRITE,
		0, 0, NULL);

	if (hMapping == NULL) {
		CloseHandle(hFile);
		return NULL;
	}

	struct FileMapping* mapping = (struct FileMapping*)malloc(sizeof(struct FileMapping));
	if (mapping == NULL) {
		CloseHandle(hMapping);
		CloseHandle(hFile);
		return NULL;
	}

	mapping->hFile = hFile;
	mapping->hMapping = hMapping;
	mapping->lowSize = (size_t)dwLowSize;
	mapping->highSize = (size_t)dwhighSize;

	return mapping;
}

void updateMapping(struct FileMapping* oldMaping) {

	DWORD dwhighSize = 0;
	DWORD dwLowSize = GetFileSize(oldMaping->hFile, dwhighSize);
	if (dwLowSize == INVALID_FILE_SIZE) {
		CloseHandle(oldMaping->hFile);
		return NULL;
	}

	HANDLE hMapping = CreateFileMapping(oldMaping->hFile, NULL, PAGE_READWRITE,
		0, 0, NULL);

	if (hMapping == NULL) {
		CloseHandle(oldMaping);
		return NULL;
	}

	CloseHandle(oldMaping->hMapping);

	oldMaping->hMapping = hMapping;
	oldMaping->lowSize = (size_t)dwLowSize;
	oldMaping->highSize = (size_t)dwhighSize;
}

void freeMapping(struct FileMapping* Maping) {
	CloseHandle(Maping->hFile);
	CloseHandle(Maping->hMapping);
	free(Maping);
}


void initFile(char* fileName) {
	HANDLE created = CreateFileA(fileName, GENERIC_READ | GENERIC_WRITE, 0, NULL,
		CREATE_NEW,
		FILE_ATTRIBUTE_NORMAL, NULL);
	addNewListToFileFromFile(created);
	addNewListToFileFromFile(created);
	CloseHandle(created);

	struct FileMapping* myMapping = openMapping(fileName);
	setMyPageFree(myMapping, 1);
	setMyPageFree(myMapping, 2);
	freeMapping(myMapping);

}

void openMyPage(struct FileMapping* a, int num, struct map_file_of_view* ret) {
	size_t start = (num - 1) * list_size / windows_page_size;
	size_t delta = (num - 1) * list_size - start * windows_page_size;
	ret->start = MapViewOfFile(a->hMapping, FILE_MAP_ALL_ACCESS, 0, start * windows_page_size, delta + list_size);
	DWORD temp = GetLastError();
	ret->my_page_start = ret->start + delta;
	// доделать старшее слово
}

int countFreeCellsOnPage(struct FileMapping* a, int page_num) {
	struct map_file_of_view page;
	openMyPage(a, page_num, &page);
	int ret = 0;
	struct cell* now = (struct cell*)page.my_page_start;
	enum cell_flag previous_flag = -1;
	for (int i = 0; i < cellsOnList - 1; i++) {
		if (now->flag == TEMPEST && previous_flag == STRING_CONTINUE) {
			ret = -1;
			break;
		}
		if (now->flag == TEMPEST ) {
			ret++;
		}
		now++;
	}
	UnmapViewOfFile(page.start);
	return ret;
}


int getTransportCell(struct map_file_of_view* page) {
	struct cell* ret = (struct cell*)page->my_page_start;
	ret += cellsOnList;
	return ret->int_data;
}

int setTransportCell(struct FileMapping* a, int page_num, int value) {
	struct map_file_of_view page;
	openMyPage(a, page_num, &page);
	struct cell* ret = (struct cell*)page.my_page_start;
	ret += cellsOnList;
	ret->int_data = value;
	UnmapViewOfFile(page.start);
	return 0;
}


int openTableEl(struct map_file_of_view* page, struct map_file_of_view* page_copy, int num) {

	if (num > tableStructOnList) {
		return -1;
	}
	page_copy->my_page_start = page->my_page_start + sizeof(struct table_data) * num;
	return 0;
}

unsigned char* setMyPageFree(struct FileMapping* a, int num) {
	struct map_file_of_view page;
	openMyPage(a, num, &page);
	struct cell* empty = (struct cell*)page.my_page_start;
	for (int i = 0; i < cellsOnList; i++) {
		empty->flag = TEMPEST;
		empty++;
	}

	empty->flag = TRANSPORT;
	empty->int_data = -1;
	UnmapViewOfFile(page.start);
}

unsigned char* setTableStartOnPage(struct FileMapping* a, int num) {
	struct map_file_of_view page;
	openMyPage(a, num, &page);
	struct cell* now_cell = (struct cell*)page.my_page_start;
	//now_cell->flag = TABLE_START;

	now_cell += cellsOnList;
	now_cell->flag = TRANSPORT;
	now_cell->int_data = -1;
	UnmapViewOfFile(page.start);
}


int registerFreePage(struct FileMapping* Maping, int num) {
	struct map_file_of_view page;
	openMyPage(Maping, free_block_start, &page);
	struct cell* now = (struct cell*)page.my_page_start;
	while (1) {
		if (now->flag == TRANSPORT) {
			if (now->int_data == -1) {
				struct cell* deleted_cell = (struct cell*)page.my_page_start;
				deleted_cell->flag = TEMPEST;
				int temp_num = deleted_cell->int_data;
				now->type_of = INT_MYTYPE;
				now->int_data = temp_num;
			}
			int temp = now->int_data;
			UnmapViewOfFile(page.start);
			openMyPage(Maping, temp, &page);
			now = (struct cell*)page.my_page_start;
			continue;
		}
		if (now->flag != FREE_PAGE_DATA) {
			now->flag = FREE_PAGE_DATA;
			now->int_data = num;
			now->type_of = INT_MYTYPE;
			break;
		}
		now += 1;
	}

	UnmapViewOfFile(page.start);
}



void addNewListToFileFromFile(HANDLE hFile) {
	char newList[list_size];
	memset(newList, '0', sizeof(newList));

	LPDWORD written = 0;
	SetFilePointer(hFile, 0, 0, FILE_END);
	WriteFile(hFile, &newList, sizeof(char) * list_size, written, NULL);
}


void addNewListFromMapping(struct FileMapping* Maping) {
	addNewListToFileFromFile(Maping->hFile);
	updateMapping(Maping);
}
void addNewRegisteredListFromMapping(struct FileMapping* Maping) {
	addNewListFromMapping(Maping);
	int new_list_num = getListCountFromMaping(Maping);
	registerFreePage(Maping, new_list_num);
	setMyPageFree(Maping, new_list_num);
}


int getListCountFromMaping(struct FileMapping* Maping) {
	return Maping->lowSize / list_size;
}


int getFreePage(struct FileMapping* Maping){
	struct map_file_of_view page;
	openMyPage(Maping, free_block_start, &page);
	struct cell* now = (struct cell*)page.my_page_start;
	while (1) {
		if (now->flag == TRANSPORT) {
			if (now->int_data == -1) {
				addNewListFromMapping(Maping);
				int new_list_num = getListCountFromMaping(Maping);
				setMyPageFree(Maping, new_list_num);
				return new_list_num;
			}
			else {
				int temp = now->int_data;
				UnmapViewOfFile(page.start);
				openMyPage(Maping, temp, &page);
				now = (struct cell*)page.my_page_start;
				continue;
			}
		}
		if (now->flag == FREE_PAGE_DATA) {
			now->flag = TEMPEST;
			return now->int_data;
		}
		now += 1;
	}
	UnmapViewOfFile(page.start);
	return 0;
}


int fillerDataToPage(struct FileMapping* Maping, int page_num, int cell_num, struct cell raw[], int el_in_array) {
	int raw_size = el_in_array;
	int now_page_num = page_num;
	struct map_file_of_view page;
	openMyPage(Maping, page_num, &page);
	struct cell* now_cell = (struct cell*)page.my_page_start;
	now_cell += cell_num;
	int now_cell_num = cell_num;
	int counter = 0;

	while (counter < raw_size) {
		for (; now_cell_num < cellsOnList && counter < raw_size;now_cell_num++) {
			now_cell->flag = raw[counter].flag;
			now_cell->double_data = raw[counter].double_data;
			now_cell->int_data = raw[counter].int_data;
			for(int j = 0; j < string_data_size_in_cell;j++)
				now_cell->string_data[j] = raw[counter].string_data[j];
			now_cell->type_of = raw[counter].type_of;
			counter++;
			now_cell++;
		}
		if (counter >= raw_size)
			break;
		// сейчас в page хранится номер следующей страницы, будущая третья
		int next_page_num = getFreePage(Maping); // будущая вторая
		int second_page = getTransportCell(&page);
		setTransportCell(Maping, now_page_num, next_page_num);
		UnmapViewOfFile(page.start);


		setTransportCell(Maping, next_page_num, second_page);
		openMyPage(Maping, next_page_num, &page);
		now_page_num = next_page_num;
		now_cell = (struct cell*)page.my_page_start;
		now_cell_num = 0;
	}
	UnmapViewOfFile(page.start);
	return now_page_num;
}

int get_first_free_cell(struct FileMapping* Maping, int page_num) {
	struct map_file_of_view page;
	openMyPage(Maping, page_num, &page);
	struct cell* now_cell = (struct cell*)page.my_page_start;
	for (int i = 0; i < cellsOnList; i++) {
		if (now_cell->flag == TEMPEST)
			return i;
		now_cell++;
	}
	UnmapViewOfFile(page.start);
	return -1;
}

int countCellsInRaw(struct cell raw[], int el_in_array) {
	int n = el_in_array;
	int string_mode = 0; // 0 выключен, 0 включен
	int ret = 0;
	for (int i = 0; i < n; i++) {
		if (string_mode == 0) {
			if (raw[i].type_of == INT_MYTYPE || raw[i].type_of == DOUBLE_MYTYPE || raw[i].flag == STRING_END || raw[i].flag == RAW_NUM)
				ret++;
			if (raw->flag == STRING_CONTINUE)
				string_mode = 1;
		}
		else {
			if (raw[i].type_of == INT_MYTYPE || raw[i].type_of == DOUBLE_MYTYPE) {
				ret = -1;
				break;
			}
			if (raw[i].flag == STRING_END) {
				ret++;
				string_mode = 0;
			}
		}
	}
	return ret;
}

int pageCompresser(struct FileMapping* Maping, int page_num) {
	struct cell arr[cellsOnList];
	int cnt = 0;
	struct map_file_of_view page;
	openMyPage(Maping, page_num, &page);
	struct cell* now_cell = (struct cell*)page.my_page_start;
	for (int i = 0; i < cellsOnList; i++) {
		if (now_cell->flag != TEMPEST) {
			arr[cnt].flag = now_cell->flag;
			arr[cnt].double_data = now_cell->double_data;
			arr[cnt].int_data = now_cell->int_data;
			arr[cnt].type_of = now_cell->type_of;
			for(int j = 0;j < string_data_size_in_cell;j++)
				arr[cnt].string_data[j] = now_cell->string_data[j];
			cnt++;
		}
		now_cell++;
	}
	now_cell = (struct cell*)page.my_page_start;
	for (int i = 0; i < cellsOnList; i++) {
		if (i < cnt) {
			now_cell->flag = arr[i].flag;
			now_cell->double_data =arr[i].double_data;
			now_cell->int_data = arr[i].int_data;
			now_cell->type_of = arr[i].type_of;
			for (int j = 0; j < string_data_size_in_cell; j++)
				now_cell->string_data[j] = arr[i].string_data[j];
		}
		else {
			now_cell->flag = TEMPEST;
		}
	}
	UnmapViewOfFile(page.start);
	return cnt;
}