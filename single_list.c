#include <stdio.h>
#include <stdlib.h>

typedef struct s_list
{
	int val;
	s_list *next;
}s_list;

s_list *add_node(s_list *loc, s_list *node)
{
	if (node != NULL)
	{
		node->next = loc->next;
		loc->next = node;		
	}

}

s_list *del_node(s_list *node)
{
	if (node != NULL && node->next != NULL)
	{
		s_list *temp = node->next;
		node->next = temp->next;
		node->val = temp->val;
		temp = NULL;
	}
	else
		node = NULL;
}

int main()
{
	
}
