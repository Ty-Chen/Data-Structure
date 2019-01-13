#include <iostream>

void test1()
{
	char msg[] = { 0, 0, 0, 1, 5 };
	char choke[] = { 0, 0, 0, 1, 0 };

	int cmp1 = strcmp(msg, choke);
	int cmp2 = strcmp(choke, msg);
	std::cout << cmp1 << std::endl;
	std::cout << cmp2 << std::endl;

}

void test2()
{
	char msg[] = { 0, 0, 0, 1, 5 };
	char interest[] = { 0, 0, 0, 1, 2 };
	char unintrest[] = { 0, 0, 0, 1, 3 };

	char tmp = *(msg + 4);
	char tmp2 = *(interest + 4);
	char tmp3 = *(unintrest + 4);

	if (tmp == '\x5')
	{ 
		std::cout << "tmp equal" << std::endl;
	}

	if (tmp2 == '\x2')
	{
		std::cout << "tmp2 equal" << std::endl;
	}

	if (tmp3 == '\x3')
	{
		std::cout << "tmp3 equal" << std::endl;
	}
}

int main()
{
  test1();
  test2();
  getchar();
}
