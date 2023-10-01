#include "fileWork.h"

struct FileMapping* openMapping(char* fileName) {
	HANDLE hFile = CreateFileA(fileName, GENERIC_READ | GENERIC_WRITE, 0, NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		int aaa = 2;
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
	mapping->fsize = (size_t)dwLowSize;

	return mapping;
}

