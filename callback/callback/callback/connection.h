#pragma once
#include "lsd.h"

class PeerConnection : public IListenner
{
	lsd *m_lsd;
	int  m_int;

public :
	PeerConnection();
	~PeerConnection();

	void onNotify(int value);

private:

	void __update(int val);
};