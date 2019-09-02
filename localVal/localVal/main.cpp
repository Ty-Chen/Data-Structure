#include <iostream>
#include <windows.h>

#define TEST_COUNT 3000000000

int global = 0;
static int staticVal = 0;

class Test
{
public:
	int m_val = 0;
};

void test1(Test *t)
{
	for (int i = 0; i < TEST_COUNT; i++)
	{
		t->m_val += i;
	}
	return;
}

int test2()
{
	for (int i = 0; i < TEST_COUNT; i++)
	{
		global += i;
	}
	return global;
}

int test3()
{
	for (int i = 0; i < TEST_COUNT; i++)
	{
		staticVal += i;
	}
	return staticVal;
}

int test4()
{
	int tmp = 0;
	for (int i = 0; i < TEST_COUNT; i++)
	{
		tmp += i;
	}
	return tmp;
}

int test5(int val)
{
	for (int i = 0; i < TEST_COUNT; i++)
	{
		val += i;
	}
	return val;
}

int main()
{
	Test test;
	unsigned long now, last;

	test.m_val = 0;
	now = last = GetTickCount();
	test1(&test);
	now = GetTickCount();
	std::cout << "test 1 val = " << test.m_val << ", now =  " << now
		<< ", last = " << last << ", time cost = " << now - last << std::endl;

	test.m_val = 0;
	now = last = GetTickCount();
	test.m_val = test2();
	now = GetTickCount();
	std::cout << "test 2 val = " << test.m_val << ", now =  " << now
		<< ", last = " << last << ", time cost = " << now - last << std::endl;

	test.m_val = 0;
	now = last = GetTickCount();
	test.m_val = test3();
	now = GetTickCount();
	std::cout << "test 3 val = " << test.m_val << ", now =  " << now
		<< ", last = " << last << ", time cost = " << now - last << std::endl;

	now = last = GetTickCount();
	test.m_val = test4();
	now = GetTickCount();
	std::cout << "test 4 val = " << test.m_val << ", now =  " << now
		<< ", last = " << last << ", time cost = " << now - last << std::endl;

	test.m_val = 0;
	int tmp = 0;
	now = last = GetTickCount();
	test.m_val = test5(tmp);
	now = GetTickCount();
	std::cout << "test 5 val = " << test.m_val << ", now =  " << now
		<< ", last = " << last << ", time cost = " << now - last << std::endl;

	test.m_val = 0;
	now = last = GetTickCount();
	tmp = 0;
	test.m_val = test5(tmp);
	now = GetTickCount();
	std::cout << "test 6 val = " << test.m_val << ", now =  " << now
		<< ", last = " << last << ", time cost = " << now - last << std::endl;

	getchar();
	return 0;
}