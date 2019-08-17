#pragma once
#include <list>

class IListenner
{
public:
	virtual void onTecherComming(int value) = 0;
};

class INotifier
{
public:
	virtual void registerListenner(IListenner *l) = 0;
	virtual void removeListenner(IListenner *l) = 0;
	virtual void notify() = 0;
};

class lsd : public INotifier
{
	int m_table;
	std::list<IListenner*> listenners;

public:

	lsd(IListenner *listener);

	~lsd();

	void registerListenner(IListenner *l);

	void removeListenner(IListenner *l);

	void notify();

	void __timeWatch();
};
