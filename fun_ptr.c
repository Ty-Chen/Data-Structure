#include <stdio.h>
#include <stdlib.h>
#include "string.h"

typedef void (*callback) (void);
typedef struct cmd
{
	char name[10];
	callback cb;
}cmd;


void hello()
{
	printf("hello\n");
}

void hi()
{
	printf("hi\n");
}

static void cmd_hello(void)
{
	hello();
}

static void cmd_hi(void)
{
     hi();
}
 
static void cmd_exit(void)
{
    exit(0);
}

cmd cmd_array[] = 
{
	{"hello", 	cmd_hello},
	{"hi", 		cmd_hi},
	{"exit", 	cmd_exit}
};


cmd *find_cmd(const char *val)
{
	int i = 0;
	for ( ; i < sizeof(cmd_array) / sizeof(cmd_array[0]) ; i++)
	{
		if (! strcmp(val, cmd_array[i].name))
		{
			return &cmd_array[i];
		}
	}
	return 0;
}

int main()
{
	char val[20] = {};
	cmd *cmd_ptr;
	
	while(1)
	{
		gets(val);
		cmd_ptr = find_cmd(val);
			
		if(cmd_ptr)
		{
			cmd_ptr->cb();
		}
		else
		{
			printf("no cmd\n");
		}
	}
	return 0;
}
