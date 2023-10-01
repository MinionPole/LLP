enum cell_flag 
{	
	STRING_CONTINUE,        // ���������� ��� ������ ������ ������ ������������� ������ � � ����������� � ���������
	STRING_END,   // ���������� ��� ������ ������ ������ ����� ������.
	NOTE_END, // ����� ������(������ � �������)
	TABLE_START, // ������ ������ �������
	TABLE_END,// ������ ����� �������
	TRASNSPORT// ������ �������� �� ��������� �������� ������� ����
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