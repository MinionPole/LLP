#include <stdio.h>  
#include "fileWork.h"


int main(void) {                             

    char* filename = "file.db";
    HANDLE created = CreateFileA(filename, GENERIC_READ | GENERIC_WRITE, 0, NULL,
        CREATE_NEW,
        FILE_ATTRIBUTE_NORMAL, NULL);
    CloseHandle(created);
    
    struct FileMapping* myMypping = openMapping(filename);

    unsigned char* dataPtr = (unsigned char*)MapViewOfFile(myMypping->hMapping,
        FILE_MAP_ALL_ACCESS,
        0,
        0,
        myMypping->fsize);

    int x = *(dataPtr + 1);
    *(dataPtr + 1) = '6';

    x = *(dataPtr + 1);

    

    return 0;                       
}                                   