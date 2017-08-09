#define DATABASE		"db_test"

//======== BEGIN HELPER FUNCTIONS ========

void remove_last_comma_between_paren(char* input);

//======== END HELPER FUNCTIONS ========

//======== BEGIN SQL TYPE LOOKUP MACROS ========

#define SQL_LOOKUP_TABLE(x) DB_TYPE_FOR_##x
#define DB_TYPE_FOR_double  "REAL"
#define DB_TYPE_FOR_int     "INTEGER"
#define DB_TYPE_FOR_char    "TEXT"
#define DB_TYPE_FOR_uint8_t	"BLOB"

//======== END SQL TYPE LOOKUP MACROS ========

//======== BEGIN HELPER MACROS ========

#define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))

//======== END HELPER MACROS ========

//======== BEGIN TABLE EXPANSION MACROS ========

#define EXPAND_AS_CREATE_STATEMENT(type, element, struct_name) # element " " SQL_LOOKUP_TABLE(type) ", "
#define EXPAND_AS_INSERT_STATEMENT(type, element, struct_name) # element ", "
#define EXPAND_AS_QUESTION_MARKS(type, element, struct_name) "?,"
#define EXPAND_AS_TYPE(type, element, struct_name) type,
#define EXPAND_AS_NAME(type, element, struct_name) # element,
#define EXPAND_AS_STRING_TYPE(type, element, struct_name) # type ,
#define EXPAND_AS_COUNTING_STRUCT(type, element, struct_name) uint8_t element;
#define EXPAND_AS_OFFSET(type, element, struct_name) OFFSETOF(struct_name, element),

//======== END TABLE EXPANSION MACROS ========

//======== BEGIN TABLE DEFINITON MACROS ========

#define test_data1_TABLE(ENTRY)				\
		ENTRY(double, yomama, test_data1)	\
		ENTRY(int, x, test_data1)			\
		ENTRY(char, shiboopy, test_data1)
		
#define test_data2_TABLE(ENTRY)				\
		ENTRY(int, y, test_data2)			\
		ENTRY(char, shuriken, test_data2)	\
		ENTRY(double, narwhal, test_data2)

#define shidanky_TABLE(ENTRY)			\
	ENTRY(int, z, shidanky)				\
	ENTRY(char, meatwagon, shidanky)	\
	ENTRY(uint8_t, epc, shidanky)		\
	ENTRY(char, wagonmeat, shidanky)	\
	ENTRY(double, doop, shidanky)
		
//======== END TABLE DEFINITON MACROS ========

//======== BEGIN TABLE FUNCTION MACROS ========

#define char_table(macro_name)	\
	typedef struct {			\
		macro_name##_TABLE(EXPAND_AS_COUNTING_STRUCT)	\
	} macro_name##size_struct_t;			\
								\
	char* type_table##macro_name[] = {		\
		macro_name##_TABLE(EXPAND_AS_STRING_TYPE)	\
	};							\
								\
	char* name_table##macro_name[] = {		\
		macro_name##_TABLE(EXPAND_AS_NAME)	\
	};							\
								\
	int offset_table##macro_name[] = {		\
		macro_name##_TABLE(EXPAND_AS_OFFSET)			\
	};							\
								\
	char* macro_name##_create_stmt = "CREATE TABLE " # macro_name " (id INTEGER PRIMARY KEY, " macro_name##_TABLE(EXPAND_AS_CREATE_STATEMENT)##");";	\
	char* macro_name##_insert_stmt = "INSERT INTO " # macro_name " (" macro_name##_TABLE(EXPAND_AS_INSERT_STATEMENT) ") VALUES(" macro_name##_TABLE(EXPAND_AS_QUESTION_MARKS) ");";	\
	char* macro_name##_retrieve_stmt = "SELECT * FROM " # macro_name ";";	\
	char* macro_name##_delete_stmt = "DELETE FROM " # macro_name " WHERE id = ?;";	\
								\
	typedef struct 	\
	{							\
		void (*print_statements)();	\
		void (*create_table)();	\
		void (*insert_data)(macro_name*, int);	\
		void (*retrieve_data)();	\
		void (*delete_data)(int*, int);	\
	}	_table_functions_##macro_name;		\
								\
	void initialize_stuff_##macro_name()	\
	{							\
		remove_last_comma_between_paren(macro_name##_create_stmt);	\
		remove_last_comma_between_paren(macro_name##_insert_stmt);	\
	}							\
								\
	void print_statements_##macro_name()	\
	{							\
		printf("Create: %s\n", macro_name##_create_stmt);		\
		printf("Insert: %s\n", macro_name##_insert_stmt);		\
		printf("Retrieve: %s\n", macro_name##_retrieve_stmt);	\
	}							\
								\
	void create_table_##macro_name()	\
	{							\
		sqlite3* db_connection;			\
		sqlite3_stmt* db_statement;		\
		sqlite3_open(DATABASE, &db_connection);		\
		if (sqlite3_exec(db_connection, macro_name##_create_stmt, NULL, NULL, NULL))		\
		{						\
			printf("Table creation failed\n");		\
			return;				\
		}						\
		printf("Table created successfully\n");		\
	}							\
								\
	void insert_data_##macro_name(macro_name* data, int array_size)	\
	{							\
		sqlite3* db_connection;										\
		sqlite3_stmt* db_statement;									\
		sqlite3_open(DATABASE, &db_connection);						\
		sqlite3_exec(db_connection, "BEGIN", NULL, NULL, NULL);		\
																	\
		if (sqlite3_prepare_v2(db_connection, macro_name##_insert_stmt, strlen(macro_name##_insert_stmt), &db_statement, NULL) != SQLITE_OK)	\
		{															\
			printf("Table prepare statement failed\n");				\
			return;													\
		}															\
													\
		for (int i = 0; i < array_size; i++)		\
		{											\
			for (int j = 0; j < sizeof(macro_name##size_struct_t); j++)	\
			{						\
				if (strcmp(type_table##macro_name[j], "int") == 0)		\
				{                                   \
					sqlite3_bind_int(db_statement, j + 1, *(int*)((int)data + offset_table##macro_name[j]) );	\
				}                                   \
				else if (strcmp(type_table##macro_name[j], "char") == 0)      \
				{                                   \
					sqlite3_bind_text(db_statement, j + 1, (char*)((int)data + offset_table##macro_name[j]), strlen((char*)((int)data + offset_table##macro_name[j])), NULL);	\
				}                                   \
				else if (strcmp(type_table##macro_name[j], "double") == 0)    \
				{                                   \
					sqlite3_bind_double(db_statement, j + 1, *(double*)((int)data + offset_table##macro_name[j]) );	\
				}                                   \
				else if (strcmp(type_table##macro_name[j], "uint8_t") == 0)		\
				{									\
					int num_bytes = 0;				\
					if (j == (sizeof(macro_name##size_struct_t) - 1))	\
					{								\
						num_bytes = sizeof(macro_name##size_struct_t) - offset_table##macro_name[j];	\
					}								\
					else							\
					{								\
						int before_bytes = 0, after_bytes = 0;	\
						before_bytes = offset_table##macro_name[j];	\
						after_bytes = sizeof(macro_name##size_struct_t) - offset_table##macro_name[j + 1];	\
						num_bytes = sizeof(macro_name##size_struct_t) - before_bytes - after_bytes;		\
					}								\
					sqlite3_bind_blob(db_statement, j + 1, (void*)((int)data + offset_table##macro_name[j]), num_bytes, NULL);	\
				}									\
				else								\
				{									\
					printf("found nothing\n");      \
				}									\
			}										\
													\
			if (sqlite3_step(db_statement) != SQLITE_DONE)		\
			{												\
				printf("Insert Step failed\n");				\
				break;										\
			}												\
															\
			sqlite3_reset(db_statement);					\
															\
			sqlite3_finalize(db_statement);					\
			sqlite3_exec(db_connection, "COMMIT", NULL, NULL, NULL);	\
			sqlite3_close(db_connection);					\
			printf("Data deleted correctly, maybe!\n");	\
		}											\
	}												\
													\
	void retrieve_data_##macro_name()				\
	{												\
		sqlite3* db_connection;						\
		sqlite3_stmt* db_statement;					\
		sqlite3_open(DATABASE, &db_connection);		\
													\
		if (sqlite3_prepare_v2(db_connection, macro_name##_retrieve_stmt, strlen(macro_name##_retrieve_stmt), &db_statement, NULL) != SQLITE_OK)	\
		{											\
			printf("Table select statement failed\n");	\
			return;									\
		}											\
													\
		int macro_name##id;							\
		int row_return = 0;							\
													\
		printf("Outputting data:\n");				\
		while(1)									\
		{											\
			row_return = sqlite3_step(db_statement);	\
													\
			if (row_return == SQLITE_ROW)			\
			{										\
				macro_name##id = sqlite3_column_int(db_statement, 0);	\
				printf("id: %d\t", macro_name##id);	\
														\
				for (int j = 0; j < sizeof(macro_name##size_struct_t); j++)	\
				{						\
					if (strcmp(type_table##macro_name[j], "int") == 0)		\
					{                                   \
						printf("%s: %d\t", name_table##macro_name[j], sqlite3_column_int(db_statement, j + 1));	\
					}                                   \
					else if (strcmp(type_table##macro_name[j], "char") == 0)      \
					{                                   \
						printf("%s: %s\t", name_table##macro_name[j], sqlite3_column_text(db_statement, j + 1));	\
					}                                   \
					else if (strcmp(type_table##macro_name[j], "double") == 0)    \
					{                                   \
						printf("%s: %f\t", name_table##macro_name[j], sqlite3_column_double(db_statement, j + 1));	\
					}                                   \
					else if (strcmp(type_table##macro_name[j], "uint8_t") == 0)	\
					{									\
						const void* dank = sqlite3_column_blob(db_statement, j + 1);	\
						int bytes = sqlite3_column_bytes(db_statement, j + 1);	\
						printf("%s: ", name_table##macro_name[j]);				\
						for (int q = 0; q < bytes; q++)	\
						{								\
							printf("%u", ((uint8_t*)dank)[q]);		\
						}								\
						printf("\t");					\
					}									\
					else								\
					{									\
						printf("found nothing");		\
					}									\
				}										\
				printf("\n");							\
			}											\
			else if (row_return == SQLITE_DONE)			\
			{											\
				printf("Retrieve finished\n");			\
				break;									\
			}											\
			else										\
			{											\
				printf("Retrieve failed\n");			\
				break;									\
			}											\
		}												\
		                                                \
		sqlite3_finalize(db_statement);                 \
		sqlite3_close(db_connection);                   \
	}													\
														\
	void delete_data_##macro_name(int* row_ids, int num_ids)	\
	{													\
		sqlite3* db_connection;							\
		sqlite3_stmt* db_statement;						\
		sqlite3_open(DATABASE, &db_connection);			\
		sqlite3_exec(db_connection, "BEGIN", NULL, NULL, NULL);		\
														\
		if (sqlite3_prepare_v2(db_connection, macro_name##_delete_stmt, strlen(macro_name##_delete_stmt), &db_statement, NULL) != SQLITE_OK)	\
		{															\
			printf("Table prepare statement failed: %s\n", sqlite3_errmsg(db_connection));				\
			return;													\
		}															\
														\
		for (int i = 0; i < num_ids; i++)				\
		{												\
			sqlite3_bind_int(db_statement, 1, row_ids[i] );	\
			if (sqlite3_step(db_statement) != SQLITE_DONE)	\
			{											\
				printf("Delete Step failed\n");			\
				break;									\
			}											\
			sqlite3_reset(db_statement);				\
		}												\
														\
		sqlite3_finalize(db_statement);					\
		sqlite3_exec(db_connection, "COMMIT", NULL, NULL, NULL);	\
		sqlite3_close(db_connection);					\
		printf("Data deleted correctly, maybe!\n");		\
	}													\
	                                                    \
	_table_functions_##macro_name _table_funcs_##macro_name = {		\
		&print_statements_##macro_name,					\
		&create_table_##macro_name,						\
		&insert_data_##macro_name,						\
		&retrieve_data_##macro_name,					\
		&delete_data_##macro_name,						\
	};													\
														\
	_table_functions_##macro_name* new_table_##macro_name()	\
	{													\
		initialize_stuff_##macro_name();				\
		return &_table_funcs_##macro_name;				\
	}

//======== END TABLE FUNCTION MACROS ========
	
//======== BEGIN GENERIC INTERFACE MACROS ========

#define TABLE(macro_name)	\
	_table_functions_##macro_name
	
#define new_table(macro_name)	\
	new_table_##macro_name()
	
#define print_statements(collection)	\
	collection->print_statements()
	
#define create_table(collection)	\
	collection->create_table()
	
#define insert_data(collection, data, count)	\
	collection->insert_data(data, count)
	
#define retrieve_data(collection)	\
	collection->retrieve_data()
	
#define delete_data(collection, row_ids, num_rows)	\
	collection->delete_data(row_ids, num_rows)
	
//======== END GENERIC INTERFACE MACROS ========

//======== BEGIN HELPER FUNCTION IMPLEMENTATIONS ========

void remove_last_comma_between_paren(char* input)
{
	int* array = (int*)malloc(sizeof(int));
	size_t array_size = 0;
	
	int position = -1;
	for (int i = 0; i < strlen(input); i++)
	{
		if (input[i] == ',')
		{
			position = i;
		}
		else if (input[i] == ')')
		{
			if (position > -1)
			{
				array[array_size] = position;
				position = -1;
				array_size++;
				array = (int*)realloc(array, (array_size + 1) * sizeof(int));
			}
		}
	}
	
	if (array_size)
	{
		for (int i = array_size - 1; i > -1; i--)
		{
			memmove( &input[array[i]], &input[array[i] + 1], strlen(input) - array[i]);
		}
	}
	
	free(array);
}

//======== END HELPER FUNCTION IMPLEMENTATIONS ========