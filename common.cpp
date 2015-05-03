#include "common.hpp"


AppLogger* AppLogger::GetInstance()
{
	static AppLogger inst;
	return &inst;
}

CSock::CSock(char* ip ,int port)
{
	mFd = -1;
	memset(&mAddr,0,sizeof(mAddr));
	mAddr.sin_family = AF_INET;
	mAddr.sin_port = htons(port);
	mAddr.sin_addr.s_addr = (ip)?inet_addr(ip):htonl(INADDR_ANY);
}
int CSock::Create()
{
	mFd = socket(PF_INET, SOCK_STREAM, 0);
	if(mFd>=0)
	{
		int ret = bind(mFd, (struct sockaddr *) &mAddr, sizeof(mAddr));
		if(ret<0)
		{
			close(mFd);
			mFd = -1;
		}

	}
	return mFd;
}

int CSock::Listen(int sz)
{
	return ::listen(mFd,sz);
    
}

int CSock::Accept(CSock& a)
{
	struct sockaddr_in Addr;
	socklen_t addr_size=sizeof(Addr);
	int newSocket = accept(mFd, (struct sockaddr *) &Addr, &addr_size);
	if(newSocket>=0)
	{
		a.mAddr = Addr;
		a.mFd = newSocket;
	}

	return newSocket;

}

int CSock::Connect(const CSock& a)
{
	socklen_t addr_size=sizeof(a.mAddr);
	return connect(mFd, (struct sockaddr *) &a.mAddr, addr_size);

}
int CSock::Send(const string& str)
{
	int len = str.length();
	int ret = send(mFd,str.c_str(),len,0);
	return ret;
}

int CSock::Recv(string& str)
{
	char buf[100];
	int ret;
	while((ret = recv(mFd, buf, sizeof(buf)-1, 0))>0)
	{
		buf[ret]=0;
		str.append(buf);
		if( ret ==0 ||
			buf[ret-1]=='\n'          
			)
		{
			break;
		}
	}
	return ret;
}

CSock::~CSock()
{
	if(mFd>=0)close(mFd);
}