#include "lsd.h"
#include <Windows.h>
#include <iostream>

lsd::lsd(IListenner *listener)
{
	m_table = 0;
	registerListenner(listener);
	__timeWatch();
}

lsd::~lsd()
{
	m_table = 0;
}

void lsd::registerListenner(IListenner *l)
{
	std::cout << "register listener" << std::endl;
	listenners.push_back(l);
}

void lsd::removeListenner(IListenner *l)
{
	std::cout << "remove listener" << std::endl;
	std::list<IListenner*>::iterator it;
	for (it = listenners.begin(); it != listenners.end(); it++)
	{
		if (*it == l)
		{
			listenners.remove(l);
			break;
		}
	}
}

void lsd::notify()
{
	std::list<IListenner*>::iterator it;
	for (it = listenners.begin(); it != listenners.end(); it++)
	{
		(*it)->onTecherComming(m_table);
	}
}

void lsd::__timeWatch()
{
Start:

	time_t start_time = GetTickCount();
	time_t now_time = start_time;

	while (now_time - start_time <= 1000)
	{
		now_time = GetTickCount();
	}

	notify();

	goto Start;

}