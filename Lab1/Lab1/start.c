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

    for (int i = 1; i <= 5; i++) {
        int new_table_size = abs(rand()) % 3 + 1;
        struct table_data temp_table_data = {.table_num=i, .row_size= new_table_size };
        initTable(myMapping, &temp_table_data);
    }
    
    //int p1 = getFreePage(myMapping);


    //deleteTable(myMapping, 3);
    //int p2 = getFreePage(myMapping);

    int sizesRaw[6] = {0, 0, 0, 0, 0, 0 };
    int startRaw[6] = {1, 1, 1, 1, 1, 1};
    for (int z = 0; z < 1000; z++) {

        for (int i = 0; i < 500; i++) {

            int num = abs(rand()) % 5 + 1;

            struct table_data table_info = { .table_num = num };
            int status1 = getTableData(myMapping, num, &table_info);

            struct cell* raw_to_table = malloc(sizeof(struct cell) * (table_info.row_size + 1)); // 1
            //struct cell raw_to_table[3];

            raw_to_table[0].flag = RAW_NUM;
            raw_to_table[0].type_of = INT_MYTYPE;
            raw_to_table[0].int_data = ++sizesRaw[num];

            for (int j = 1; j <= table_info.row_size; j++) {
                raw_to_table[j].type_of = INT_MYTYPE;
                int chislo = rand() % 1000 + 1;
                raw_to_table[j].int_data = chislo;
            }
            addRawToTable(myMapping, num, raw_to_table, table_info.row_size + 1);
            free(raw_to_table);
        }



       /*int cntT = 0;
       for (int i = 0; i < 20; i++) {
            int num = abs(rand()) % 5 + 1;
            if (startRaw[num] <= sizesRaw[num]) {

                deleteDataFromTable(myMapping, 1, cond1, 1);
                cntT++;
            }
       }
       */


       if (z % 10 == 0) {
           // для хранения времени выполнения кода


           /*for (int i = 0; i < 20; i++) {
               int num = rand() % sizesRaw[1] + 1;
               struct cell raw_to_table[4];
               raw_to_table[0].flag = RAW_NUM;
               raw_to_table[0].type_of = INT_MYTYPE;
               raw_to_table[0].int_data = num;
               for (int j = 1; j <= 3; j++) {
                   raw_to_table[j].type_of = INT_MYTYPE;
                   int chislo = rand() % 10000 + 1000;
                   raw_to_table[j].int_data = chislo;
               }
               changeRaw(myMapping, 1, raw_to_table, 4);
           }
           */



           double time_spent = 0.0;
           clock_t begin = clock();

           struct queryCondition cond1;
           cond1.int_data = 700;
           cond1.sign = '>';
           cond1.stolbec_num = 0;
           cond1.type_of = INT_MYTYPE;

           int temp = selectTableData(myMapping, 1, &cond1, 1);

           clock_t end = clock();
           time_spent += (double)(end - begin) / CLOCKS_PER_SEC;
           printf("%f\n", time_spent);


           //
           //printf("\n\n\n%d   %d   %d\n", sizesRaw[1], startRaw[1], sizesRaw[1] - startRaw[1] + 1);

                     


           
           //printf("%d  ", temp);
           //printf("\n\n\n%d   %d   %d\n", sizesRaw[1], startRaw[1], sizesRaw[1] - startRaw[1] + 1);
           //printf("%d  ", sizesRaw[1]);



           

           

       }



        
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


    return 0;                       
}                                   