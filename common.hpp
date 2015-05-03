
#ifndef _COMMON_HPP
#define _COMMON_HPP

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <arpa/inet.h>
#include <errno.h>
#include <iostream>
#include <pthread.h>
#include <memory>
#include <signal.h>
#include <sys/types.h> 
#include <sys/ipc.h>
#include <sys/msg.h>
#include <mqueue.h>
#include <set>
#include <fstream>
using namespace std;

#ifdef _DEBUG
	#define LOG(x) AppLogger::GetInstance()->Dump(x)
#else
	#define LOG(x)
#endif

class AppLogger
{
	ofstream mFile;
	AppLogger(){try{mFile.open("/opt/OracleServer/log.txt",ios::app);}catch(...){}}
	~AppLogger(){if(mFile.is_open()){mFile.close();}}
public:
	static AppLogger* GetInstance();
	void Dump(const char* str){if(mFile.is_open()){mFile<<str<<endl;}}

};


class CSock
{
	struct sockaddr_in mAddr;
	int mFd;
public:
	CSock(char* ip = NULL,int port=0);
	int Create();
	int Listen(int sz=100);
	int Accept(CSock&);
	int Connect(const CSock&);
	int Send(const string& str);
	int Recv(string&);
	~CSock();

};



class CThread
{
	pthread_t mTid;
	static void* tFunc(void* dat){((CThread*)dat)->OnStart();}
public:

	CThread():mTid(-1),mLoop(0){}
	virtual ~CThread(){}
	int Start(){if(mTid!=-1) return -1; mLoop =1;return pthread_create(&mTid, NULL, tFunc, this);}
	void Cancel(){if(mTid == -1)return;mLoop=0;pthread_join(mTid,NULL);mTid = -1;}
protected:
	int mLoop;
	virtual void OnStart()=0;

};

#endif //_COMMON_HPP