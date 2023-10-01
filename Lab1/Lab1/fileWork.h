#pragma once
#include <windows.h>
struct FileMapping {
	HANDLE hFile;
	HANDLE hMapping;
	size_t fsize;
};

struct FileMapping* openMapping(char* fileName);


