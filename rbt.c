/*
 * This file implements red black tree structure and functions
 */
#include "stdio.h"
#include "stdlib.h"

typedef enum ColorType {RED, BLACK} ColorType;

typedef struct rbnode{
	int key;
	rbnode *left;
	rbnode *right;
	rbnode *p;
	ColorType color;
}rbnode;

typedef struct rb_root{
	rbnode *root;
	rbnode *nil;
}rb_root;

rb_root *rbt_init()
{
	rb_root *T;
}
	
