#include <iostream>
#include <windows.h>

int global = 0;
static int staticVal = 0;

class Test
{
public:
	int m_val = 0;
};

int test1()
{
	int tmp = 0;
	for (int i = 0; i < 1000000000; i++)
	{
		tmp += i;
	}
	return tmp;
}

void test2(Test *t)
{
	for (int i = 0; i < 1000000000; i++)
	{
		t->m_val += i;
	}
	return;
}

int test3()
{
	for (int i = 0; i < 1000000000; i++)
	{
		global += i;
	}
	return global;
}

int test4()
{
	for (int i = 0; i < 1000000000; i++)
	{
		staticVal += i;
	}
	return staticVal;
}

int main()
{
	Test test;
	unsigned long now, last;
	now = last = GetTickCount();
	test.m_val = test1();
	now = GetTickCount();
	std::cout << "test 1 val = " << test.m_val << ", now =  " << now 
		<< ", last = " << last << ", time cost = " << now - last << std::endl;

	test.m_val = 0;
	now = last = GetTickCount();
	test2(&test);
	now = GetTickCount();
	std::cout << "test 2 val = " << test.m_val << ", now =  " << now
		<< ", last = " << last << ", time cost = " << now - last << std::endl;

	test.m_val = 0;
	now = last = GetTickCount();
	test.m_val = test3();
	now = GetTickCount();
	std::cout << "test 3 val = " << test.m_val << ", now =  " << now
		<< ", last = " << last << ", time cost = " << now - last << std::endl;

	test.m_val = 0;
	now = last = GetTickCount();
	test.m_val = test4();
	now = GetTickCount();
	std::cout << "test 4 val = " << test.m_val << ", now =  " << now
		<< ", last = " << last << ", time cost = " << now - last << std::endl;

	getchar();
	return 0;
}