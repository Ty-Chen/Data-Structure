#include <stdio.h>
#include <stdlib.h>

typedef sing_list s_list
{
	int val;
	sing_list *next;
}s_list;

s_list *add_node(s_list *loc, s_list *node)
{
	if (node != NULL)
	{
		node->next = loc->next;
		loc->next = node->next;		
	}

}

s_list *del_node(s_list *node)
{
	
}

int main()
{
	
}
