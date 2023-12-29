#pragma once
#define string_data_size_in_cell 32
enum cell_flag
{
	STRING_CONTINUE,        // показывает что данна€ €чейка хранит незаконченную строку и еЄ продолжение в следующей
	STRING_END,   // показывает что данна€ €чейка хранит конец строки.
	NOTE_END, // конец записи(строки в таблице)
	TABLE_START, // €чейка начала таблицы
	TRANSPORT,// €чейка перехода на следующую страницу данного типа
	EMPTY, // показывает что страница пуста€
	RAW_NUM, // хранит номер записи
	FREE_PAGE_DATA, // €чейка хранит информацию о пустой странице
	TEMPEST // пуста€ €чейка
};

enum type
{
	STRING_MYTYPE,
	INT_MYTYPE,
	DOUBLE_MYTYPE
};


struct cell {
	enum type type_of;
	union {
		int int_data;
		double double_data;
		char string_data[string_data_size_in_cell];
	};
	enum cell_flag flag;
};
enum table_flag
{
	TABLE_ACTIVE,
	TABLE_DELETED
};

struct table_data {
	int table_num;
	int start_page_num;
	int	last_page_num;
	int row_size;
	int rows_count;
	enum table_flag flag;
};

