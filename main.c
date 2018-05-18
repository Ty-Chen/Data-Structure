#include <stdio.h>
#include <stdlib.h>
#include "list.h"

typedef struct data_list
{
	int  test_int_data;
	char test_char_data; 
	struct list_head list;
}data_list;


int main()
{	
	data_list data;
	data.test_int_data = 123;
	data.test_char_data = 'a';
	
	/* We test offsetof here*/
	printf("\n-------We test the offset marco-------\n");
	printf("offset of test_int_data = %lu\n", offsetof(data_list, test_int_data));
	printf("offset of test_char_data = %lu\n", offsetof(data_list, test_char_data));
	printf("offset of list_head = %lu\n", offsetof(data_list, list));

	/*We test container_of here*/
	printf("\n-------We test the container_of marco-------\n");
	data_list *ptr;
	ptr = container_of(&(data.test_int_data), data_list, test_int_data);
	printf("We define a new pointer ptr to struct data_list\n");
	printf("After container_of, the values are as follow\n");
	printf("ptr->test_int_data = %d\n", ptr->test_int_data);
	printf("ptr->test_char_data = %c\n", ptr->test_char_data); 

	/* We test list here*/	
	printf("\n-------We test the list-------\n");

	struct list_head head;
	INIT_LIST_HEAD(&head);	
	
	list_add( &data.list, &head);
	
	data_list data2;
	data2.test_int_data = 456;
	data2.test_char_data = 'b';
	list_add( &data2.list, &head);

	printf("Test list_for_each\n");
	
	struct list_head *pos;
	
	list_for_each( pos, &head)
	{
		data_list *plist = list_entry(pos, data_list, list);
		printf("\nint_data = %d\n", plist->test_int_data);
		printf("char_data = %c\n", plist->test_char_data);
	}
	
	//getchar();
}
