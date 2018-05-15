#include <stdio.h>
#include <stdlib.h>
#include "list.h"

typedef struct data_list
{
	int  test_int_data;
	char test_char_data; 
	struct list_head head;
}data_list;


int main()
{
	//LIST_HEAD(list);
	data_list dataHead;
	dataHead.test_int_data = 123;
	dataHead.test_char_data = 'a';

	/* We test offsetof here*/
	printf("Firstly, we test the offset marco\n");
	printf("offset of test_int_data = %lu\n", offsetof(data_list, test_int_data));
	printf("offset of test_char_data = %lu\n", offsetof(data_list, test_char_data));
	printf("offset of list_head = %lu\n", offsetof(data_list, head));

	/**/
	
	printf("\ncomplete a simple list\n");
	/* We test list here*/
	
	//getchar();
}
