#pragma once
#include <windows.h>
#include "structures.h"
#define list_size 2048
#define windows_page_size 65536
#define free_block_start 2
#define table_block_start 1
#define cellsOnList list_size/sizeof(struct cell) - 1
#define tableStructOnList (cellsOnList) * sizeof(struct cell) / sizeof(struct table_data)

struct FileMapping {
	HANDLE hFile;
	HANDLE hMapping;
	size_t lowSize;
	size_t highSize;
};

struct map_file_of_view {
	char* start;
	char* my_page_start;
};

struct FileMapping* openMapping(char* fileName);

void initFile(char* fileName);

void updateMapping(struct FileMapping* oldMaping);

void freeMapping(struct FileMapping* Maping);
void addNewListToFileFromFile(HANDLE hFile);
void addNewListToFileFromMapping(struct FileMapping* Maping);

int registerFreePage(struct FileMapping* Maping, int num);
int getFreePage(struct FileMapping* Maping);

int getTransportCell(struct map_file_of_view* page);
int setTransportCell(struct FileMapping* a, int page_num, int value);

int pageCompresser(struct FileMapping* Maping, int page_num);

void openMyPage(struct FileMapping* a, int num, struct map_file_of_view* ret);
unsigned char* setMyPageFree(struct FileMapping* a, int num);

int countFreeCellsOnPage(struct FileMapping* a, int page_num);

int get_first_free_cell(struct FileMapping* Maping, int page_num);

int countCellsInRaw(struct cell raw[], int el_in_array);

// указываем заполнить начина€ с какого-то номера €чейки, такой-то страницы в файле наши данные
int fillerDataToPage(struct FileMapping* Maping, int page_num, int cell_num, struct cell raw[], int el_in_array);