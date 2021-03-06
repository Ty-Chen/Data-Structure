#ifndef __C_LIST_H
#define __C_LIST_Htypedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long size_t;

/*本句宏定义实现了偏移量的计算*/
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

/**
 * container_of - cast a member of a structure out to the containing structure
 * @ptr:        the pointer to the member.
 * @type:       the type of the container struct this is embedded in.
 * @member:     the name of the member within the struct.
 *
 * 著名内核源码函数，经常见到，作用是根据type类型中member的位置获得结构的首地址
 * It takes three arguments – a pointer, type of the container,
 * and the name of the member the pointer refers to. 
 * The macro will then expand to a new address pointing 
 * to the container which accommodates the respective member. 
 */
#define container_of(ptr, type, member) ({                      \
	const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
	(type *)( (char *)__mptr - offsetof(type,member) );})

/* 
 * There are non-NULL pointers that will result in page faults
 * under normal circumstances, used to verify that nobody uses
 * non-initialized list entries
 */
#define LIST_POSITION1 ((void *) 0x00100100)
#define LIST_POSITION2 ((void *) 0x00200200)

/*链表指针：指向上一个和下一个*/
struct list_head {
	struct list_head *next, *prev;
};

#define LIST_HEAD_INIT(name) { &(name), &(name) }

/**/
#define LIST_HEAD(name) \
	struct list_head name = LIST_HEAD_INIT(name)

/**
 * 链表入口
 * list_entry - get the struct for this entry
 * @ptr:	the &struct list_head pointer.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_head within the struct.
 */
#define list_entry(ptr, type, member) \
	container_of(ptr, type, member)

/*链表初始化：头尾均是自身*/
static inline void INIT_LIST_HEAD(struct list_head *list)
{
	list->next = list;
    list->prev = list;
}

/**遍历链表：每次指向下一个，回到头则结束
 * list_for_each	-	iterate over list 
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_head within the struct.
 */
#define list_for_each(pos, head) for (pos = (head)->next; pos != (head); pos = pos->next)

/**反向遍历链表
 * list_for_each_r	-	iterate over list 
 *
 */
#define list_for_each_r(pos, head) for (pos = (head)->prev; pos != (head); pos = pos->prev)

/**
 * 添加链表表项
 * insert a new entry between two known consecutive entries.
 *
 * This is only for internal list manipulation where we konw
 * the prev/next entries already
 */
static inline void __list_add(struct list_head *new,
			     struct list_head *prev,
			     struct list_head *next)
{
	next->prev = new;
	new->next = next;
	new->prev = prev;
	prev->next = new;
}

/**
 * 添加新的链表项
 * list_add    -     add a new entry
 * @new: new entry to be added
 * @head: list head to add it after
 *
 * Insert a new entry after the specified head.
 * This is good for implenting stacks.
 */	
static inline void list_add(struct list_head *new, struct list_head *head)
{
	__list_add(new, head, head->next);
}

/**
 * 在尾部添加新链表项
 * list_add_tail    -     add a new entry
 * @new: new entry to be added
 * @head: list head to add it before
 *
 * Insert a new entry before the specified head.
 * This is good for implenting queue.
 */	
static inline void list_add_tail(struct list_head *new, struct list_head *head)
{
	__list_add(new, head->prev, head);
}

/*
 * Delete a list entry by making the prev/next entries
 * point to each other
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
 static inline void __list_del(struct list_head *prev, struct list_head *next)
 {
 	next->prev = prev;
	prev->next = next;
 }

/**
 * list_del    -    delete entry from list
 * @entry: the element to delete from the list
 * Note: list_empty on entry does not return true after this, the entry is
 * in an undefined state.
 */
static inline void list_del(struct list_head *entry)
{
	__list_del(entry->prev, entry->next);
	entry->next = LIST_POSITION1;
	entry->prev = LIST_POSITION2;
}

/* 
 * list_empty  - tests whether a list is empty
 * @head: the list to test
 */
static inline int list_empty(const struct list_head *head)
{
	return head->next == head;
}	

static inline void __list_splice(struct list_head *list, struct list_head *head)
{
	struct list_head *first = list->next;
	struct list_head *last = list->prev;
	struct list_head *at = head->next;
	first->prev = head;
	head->next = first;
	last->next = at;
	at->prev = last;
}

/**
 * list_splice    -    join two lists
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 */
static inline void list_splice(struct list_head *list, struct list_head *head)
{
	if (!list_empty(list))
		__list_splice(list, head);
}

#endif

