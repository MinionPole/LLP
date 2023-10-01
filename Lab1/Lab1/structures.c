enum cell_flag 
{	
	STRING_CONTINUE,        // показывает что данна€ €чейка хранит незаконченную строку и еЄ продолжение в следующей
	STRING_END,   // показывает что данна€ €чейка хранит конец строки.
	NOTE_END, // конец записи(строки в таблице)
	TABLE_START, // €чейка начала таблицы
	TABLE_END,// €чейка конца таблицы
	TRASNSPORT// €чейка перехода на следующую страницу данного типа
};

enum type
{
	STRING,
	INT, 
	DOUBLE
};

struct cell {
	enum type type_of;
	union {
		int int_data;
		double double_data;
		char string_data[20];
	};
	enum cell_flag flag;
};

struct table_data {
	int table_num;
	int page_num;
};

struct free_list_data {
	int table_num;
};