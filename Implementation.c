/*
PROGRAM NOTES:
	- This program abuses the OFFSETOF macro in several ways. While unlikely to 
	ever be a problem, if for some reason the structs are not packed in memory 
	then undefined behavior is possible/likely/guaranteed/rip.
*/

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "sqlite3.h"
#include "SqliteGenericHeader.h"

typedef struct {
	double yomama;
	int x;
	char shiboopy[100];
}test_data1;

typedef struct {
	int y;
	char shuriken[100];
	double narwhal;
}test_data2;

typedef struct {
	int z;
	char meatwagon[128];
	uint8_t epc[48];
	char wagonmeat[69];
	double doop;
}shidanky;

//======== BEGIN IMPLEMENTATION MACROS ========

char_table(test_data1)
char_table(test_data2)
char_table(shidanky)

//======== END IMPLEMENTATION MACROS ========

int main()
{
	test_data1 notpoop1;
	notpoop1.yomama = 7.7;
	notpoop1.x = 14;
	strcpy(notpoop1.shiboopy, "hello there");

	test_data2 notpoop2;
	notpoop2.y = 8;
	strcpy(notpoop2.shuriken, "nathan is a weeaboo");
	notpoop2.narwhal = 6.66;
	
	shidanky notpoop3;
	notpoop3.z = 9;
	strcpy(notpoop3.meatwagon, "nathan is a meatwagon");
	for (int i = 0; i < 48; i++)
	{
		notpoop3.epc[i] = i;
	}
	strcpy(notpoop3.wagonmeat, "nathan is a wagonmeat");
	notpoop3.doop = 6.9;
	
	TABLE(test_data1)* shiboopy1 = new_table(test_data1);
	TABLE(test_data2)* shiboopy2 = new_table(test_data2);
	TABLE(shidanky)* shiboopy3 = new_table(shidanky);
	
	print_statements(shiboopy1);
	print_statements(shiboopy2);
	print_statements(shiboopy3);
	create_table(shiboopy1);
	create_table(shiboopy2);
	create_table(shiboopy3);
	insert_data(shiboopy1, &notpoop1, 1);
	insert_data(shiboopy2, &notpoop2, 1);
	insert_data(shiboopy3, &notpoop3, 1);
	retrieve_data(shiboopy1);
	retrieve_data(shiboopy2);
	retrieve_data(shiboopy3);
	int num_rows = 1;
	int* rows = (int*)malloc(sizeof(int) * num_rows);
	rows[0] = 1;
	delete_data(shiboopy1, rows, num_rows);
	delete_data(shiboopy2, rows, num_rows);
	delete_data(shiboopy3, rows, num_rows);
	retrieve_data(shiboopy1);
	retrieve_data(shiboopy2);
	retrieve_data(shiboopy3);
	
	return 0;
}
