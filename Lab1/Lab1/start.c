#include <stdio.h>  
#include <stdlib.h>  
#include "fileWork.h"
#include "tablework.h"
#include <time.h>

int main(void) {                             

    char* filename = "file.db";
    
    initFile(filename);

    /*HANDLE created = CreateFileA(filename, GENERIC_READ | GENERIC_WRITE, 0, NULL,
        CREATE_NEW,
        FILE_ATTRIBUTE_NORMAL, NULL);
    CloseHandle(created);
    */
    SYSTEM_INFO s;
    GetSystemInfo(&s);
    struct FileMapping* myMapping = openMapping(filename);





    /*unsigned char* dataPtr = (unsigned char*)MapViewOfFile(myMypping->hMapping,
        FILE_MAP_ALL_ACCESS,
        0,
        0,
        myMypping->fsize);
        

    int x = *(dataPtr + 10);
    *(dataPtr + 1) = '6';

    x = *(dataPtr + 1);
    */
    
    /*
    int ttt = cellsOnList;
    addNewRegisteredListFromMapping(myMapping);
    addNewRegisteredListFromMapping(myMapping);
    addNewRegisteredListFromMapping(myMapping);

    int p1 = getFreePage(myMapping);
    int p2 = getFreePage(myMapping);
    int p3 = getFreePage(myMapping);
    int p4 = getFreePage(myMapping);
    */

    for (int i = 1; i <= 3; i++) {
        struct table_data temp_table_data = {.table_num=i, .row_size= i };
        initTable(myMapping, &temp_table_data);
    }
    //int p1 = getFreePage(myMapping);


    //deleteTable(myMapping, 3);
    //int p2 = getFreePage(myMapping);



    for (int z = 0; z < 40; z++) {
        // для хранения времени выполнения кода
        double time_spent = 0.0;
        clock_t begin = clock();

        for (int i = 0; i < 500; i++) {
            int num = abs(rand()) % 3 + 1;

            struct table_data table_info = { .table_num = num };
            int status1 = getTableData(myMapping, num, &table_info);

            struct cell* raw_to_table = malloc(sizeof(struct cell) * (num + 1)); // 1

            raw_to_table[0].flag = RAW_NUM;
            raw_to_table[0].type_of = INT_MYTYPE;
            raw_to_table[0].int_data = table_info.rows_count + 1;

            for (int j = 1; j <= table_info.row_size; j++) {
                raw_to_table[j].type_of = INT_MYTYPE;
                int chislo = rand() % 1000 + 1;
                raw_to_table[j].int_data = chislo;
            }
            addRawToTable(myMapping, num, raw_to_table, num + 1);

            free(raw_to_table);
        }
        clock_t end = clock();

        time_spent += (double)(end - begin) / CLOCKS_PER_SEC;

        printf("The elapsed time is %f seconds\n", time_spent);
    }


    /*struct table_data table1_info = {.table_num = 1};
    int status1 = getTableData(myMapping, 1, &table1_info);


    struct table_data table2_info = { .table_num = 2 };
    int status2 = getTableData(myMapping, 2, &table2_info);
    
    struct cell raw1_to_table1[2]; // 1
    raw1_to_table1[0].flag = RAW_NUM;
    raw1_to_table1[0].type_of = INT_MYTYPE;
    raw1_to_table1[0].int_data = 1;

    raw1_to_table1[1].type_of = INT_MYTYPE;
    raw1_to_table1[1].int_data = 1;

    struct cell raw2_to_table1[2]; // 2

    raw2_to_table1[0].flag = RAW_NUM;
    raw2_to_table1[0].type_of = INT_MYTYPE;
    raw2_to_table1[0].int_data = 2;

    raw2_to_table1[1].type_of = INT_MYTYPE;
    raw2_to_table1[1].int_data = 2;

    addRawToTable(myMapping, 1, &raw1_to_table1, 2);
    addRawToTable(myMapping, 1, &raw2_to_table1, 2);



    struct cell raw1_to_table2[3]; // "aaaa", 7
    raw1_to_table2[0].flag = RAW_NUM;
    raw1_to_table2[0].type_of = INT_MYTYPE;
    raw1_to_table2[0].int_data = 1;

    raw1_to_table2[1].flag = STRING_END;
    raw1_to_table2[1].type_of = STRING_MYTYPE;
    for (int iii = 0; iii < 4; iii++)
        raw1_to_table2[1].string_data[iii] = 'a';

    raw1_to_table2[2].type_of = INT_MYTYPE;
    raw1_to_table2[2].int_data = 7;


    struct cell raw2_to_table2[3]; // 2.0, 3.0
    raw2_to_table2[0].flag = RAW_NUM;
    raw2_to_table2[0].type_of = INT_MYTYPE;
    raw2_to_table2[0].int_data = 2;

    raw2_to_table2[1].type_of = DOUBLE_MYTYPE;
    raw2_to_table2[1].double_data = 2.0;

    raw2_to_table2[2].type_of = DOUBLE_MYTYPE;
    raw2_to_table2[2].double_data = 3.0;




    struct cell raw3_to_table2[3]; // "ccc", "bbbbbbbbbbbbbbbbbbbbbbb"

    addRawToTable(myMapping, 2, raw1_to_table2, 3);
    addRawToTable(myMapping, 2, raw2_to_table2, 3);
       */
    deleteRawFromTable(myMapping, 1, 2);
    deleteRawFromTable(myMapping, 2, 1);

    return 0;                       
}                                   