#include "common.hpp"

void server_exit(char* msg)
{
	static char str[1024];
	snprintf(str,sizeof(str),"%s\nerr code:%d\n",msg,errno);
	LOG(str);
	exit(1);
}



class CleanThread: public CThread
{
	set<CThread*> mSet;
	mqd_t mQfd;
	void OnStart();
	const char* mQName;
	CleanThread():mQfd(-1),mQName("/CleanThreadQ"){}

public:
	void Create(const char* name=NULL);
	void Destroy();
	void Clean(CThread*);
	void Add(CThread* t){mSet.insert(t);}
	static CleanThread* GetInstance();
};

CleanThread* CleanThread::GetInstance()
{
	static CleanThread inst;
	return &inst;
}

void CleanThread::Create(const char* name)
{
	if(mQfd!=-1)return;
	struct mq_attr attr;
	attr.mq_maxmsg = 10;
	attr.mq_msgsize = sizeof(CThread**);
	attr.mq_flags = 0;
	mQfd = mq_open(mQName,O_RDWR|O_CREAT/*|O_NONBLOCK*/,0666,&attr);
	if(mQfd==-1)
	{
		server_exit("msg q creation failed");
	}
	Start();

}

void CleanThread::Destroy()
{
	if(mQfd==-1)return;
	while(!mSet.empty())
	{
		LOG("CleanThread::Destroy iterating");
		set<CThread*>::iterator it;
		it = mSet.begin();
		(*it)->Cancel();
		delete (*it);
		mSet.erase(it);
		LOG("CleanThread::Destroy erasing");
	}
	Cancel();
	mq_close(mQfd);
	mq_unlink(mQName);
	mQfd = -1;
	LOG("CleanThread::Destroy exit");
}

void CleanThread::OnStart()
{
	if(mQfd==-1)return;
	while(mLoop)
	{
		set<CThread*>::iterator it;
		CThread* t;
		struct   timespec tm;
		clock_gettime(CLOCK_REALTIME, &tm);
		tm.tv_sec += 1;
		if(mq_timedreceive(mQfd, (char*)&t, sizeof(t), 0,&tm)<0)
		{
			sleep(1);
			continue;
		}
		it = mSet.find(t);
		if(it!=mSet.end())
		{
			mSet.erase(it);
			delete (t);
		}

	}
}
void CleanThread::Clean(CThread* t)
{
	if(mQfd!=-1)
	{
		mq_send(mQfd,(const char*)&t,sizeof(t),0);
		LOG("CleanThread::Clean del req sent");
	}
}

class ServerThread: public CThread
{
	auto_ptr<CSock> client;
	void OnStart();

public:
	~ServerThread(){LOG("~serverthread");}
	ServerThread(CSock* s):client(s){}
};

void ServerThread::OnStart()
{
	while(mLoop)
	{
		string s;
		if(client->Recv(s)<=0)break;
		LOG(s.c_str());
		if(s.compare("GETMESSAGE\n")==0)
		{
			string ret;
			for(int i=0;i<50;++i)
			{
				ret+=(char)('A'+(rand()%26));
			}
			ret+='\n';
			client->Send(ret);
		}
		else if(s.compare("BYE\n")==0)
		{
			break;
		}
		else
		{
			client->Send("BadRequest\n");
		}
	}

	if(mLoop)CleanThread::GetInstance()->Clean(this);
	LOG("ServerThread::OnStart quit");
}

CSock* gServ=NULL;
void shutDown()
{
	LOG("shutDown");
	CleanThread::GetInstance()->Destroy();
	if(gServ)delete gServ;
	LOG("shut down done!!!");
}

void signal_callback_handler(int signum)
{
	if(SIGTERM==signum)
	{
		LOG("SIGTERM");
		shutDown();
		exit(1);

	}
}


int main(int argc,char* argv[])
{
	daemon(0,0);
	int port = 8787;
	int c;
	opterr = 0;
	while((c=getopt(argc,argv,"p:"))!=-1)
	{
		if(c=='p')
		{
			port = atoi(optarg);
			LOG(optarg);
			break;
		}
	}

	signal(SIGPIPE, signal_callback_handler);
	signal(SIGTERM, signal_callback_handler);

	 gServ = new CSock(NULL,port);
	if(gServ->Create()<0)
		server_exit("socket crreate failed");
	if(gServ->Listen()!=0)
		server_exit("listen failed");
	CleanThread::GetInstance()->Create();
	while(1)
	{
		CSock* client=new CSock();
		if(gServ->Accept(*client)>=0)
		{
			LOG("connected");
			ServerThread* t = new ServerThread(client);
			CleanThread::GetInstance()->Add(t);
			t->Start();
		}//accept
		else
		{
			delete client;
		}
	}
	return 0;
}
