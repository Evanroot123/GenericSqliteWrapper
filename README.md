HOW TO USE:

  1. Create Table Definition From Struct(in .h file), the struct doesn't have to be declared in the .h, it definitely wouldn't in a real project:
  
      typedef struct {
      double yomama;
      int x;
      char shiboopy[100];
    }test_data1;
    
    #define test_data1_TABLE(ENTRY)				\
		ENTRY(double, yomama, test_data1)	\
		ENTRY(int, x, test_data1)			\
		ENTRY(char, shiboopy, test_data1)
    
  2. Define Functions(in .c file):
  
    char_table(test_data1)
    
  3. Create Variable To Hold Functions and Call Init Function(.c file):
  
    TABLE(test_data1)* shiboopy1 = new_table(test_data1);
    
  4. Call Functions Through Generic Interface(.c file):
  
    print_statements(shiboopy1);
    create_table(shiboopy1);
    insert_data(shiboopy1, &notpoop1, 1);
    retrieve_data(shiboopy1);
    delete_data(shiboopy1, rows, num_rows);
    retrieve_data(shiboopy1);
