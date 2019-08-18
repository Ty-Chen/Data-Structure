#include "control.h"
#include <iostream>

PeerConnection::PeerConnection()
{
	m_lsd = new lsd(this);
}

PeerConnection::~PeerConnection()
{
	delete m_lsd;
}

void PeerConnection::__update(int val)
{
	if (!val)
	{ 
		m_int++;
		std::cout << "callback work, m_int = " << m_int << " second" << std::endl;
	}
}

void PeerConnection::onNotify(int value)
{
	__update(value);
}
