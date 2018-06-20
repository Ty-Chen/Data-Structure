#include <stdio.h>
#include <stdlib.h>

typedef struct s_list
{
	int val;
	struct s_list *next;
}s_list;


int init_list(s_list *node)
{
	node->next = NULL;
}


int add_node(s_list *loc, s_list *node)
{
	if (node != NULL)
	{
		node->next = loc->next;
		loc->next = node;		
	}

	return 0;
}



int del_node(s_list *node)
{
	if (node != NULL && node->next != NULL)
	{
		s_list *temp = node->next;
		node->next = temp->next;
		node->val = temp->val;
		temp = NULL;
		printf("node = %x\n", node);
	}
	else if (node->next == NULL)
	{
		//free(node);
		node->next = NULL;
		node->val = NULL;	
		node = NULL;
	
		printf("node now = %x\n", node);

	}

	return 0;
		
}


int main()
{
	s_list head;	
	init_list(&head);	
	head.val = 0;

	s_list node1;
	add_node(&head, &node1);
	node1.val = 1;

	printf("\n----------test for add------------\n");
	s_list *ptr = &head;
	while (ptr != NULL)
	{
		printf("node value = %d\n", ptr->val);
		ptr = ptr->next;
	}

	
	printf("\n----------test for delete------------\n");
	ptr = &head;
	
	del_node(&node1);
	while (ptr != NULL)
	{
		printf("node = %x\n", ptr);
		printf("node value = %d\n",  ptr->val);
		ptr = ptr->next;
	}	
	
	return 0;
}

