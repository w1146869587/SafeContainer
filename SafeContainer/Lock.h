#ifndef _Lock_H__
#define _Lock_H__

#include <map>
#if defined  _WIN32                                                         //Windows
#include <Windows.h>
#define MyMap_CLock_Mutex_t                 HANDLE
#define MyMap_CLock_Mutex_Init(_mutex)      (_mutex = CreateSemaphore(NULL,1,1,NULL))
#define MyMap_CLock_Mutex_Lock(_mutex)      (WaitForSingleObject(_mutex, INFINITE))
#define MyMap_CLock_Mutex_UnLock(_mutex)    (ReleaseSemaphore(_mutex,1,NULL))
#define MyMap_CLock_Mutex_Destroy(_mutex)   (CloseHandle(_mutex))
#define Declar_Typename						typename
#define Type_Typename

#elif defined __linux                                                       //Linux
#include <pthread.h>
#define MyMap_CLock_Mutex_t                 pthread_mutex_t
#define MyMap_CLock_Mutex_Init(_mutex)      (pthread_mutex_init(&_mutex, NULL))
#define MyMap_CLock_Mutex_Lock(_mutex)      (pthread_mutex_lock(&_mutex))
#define MyMap_CLock_Mutex_UnLock(_mutex)    (pthread_mutex_unlock(&_mutex))
#define MyMap_CLock_Mutex_Destroy(_mutex)   (pthread_mutex_destroy(&_mutex))
#define Declar_Typename 
#define Type_Typename                 typename
#endif

inline unsigned int PthreadSelf()
{
#ifdef _WIN32
	return GetCurrentThreadId();
#else
	return thread_self();
#endif
}

//lock
class CLock  
{  
public: 
	CLock(){MyMap_CLock_Mutex_Init(_mutex);}
	~CLock(){MyMap_CLock_Mutex_Destroy(_mutex);}

	void Lock() {MyMap_CLock_Mutex_Lock(_mutex);}
	void UnLock() {MyMap_CLock_Mutex_UnLock(_mutex);}


private:
	MyMap_CLock_Mutex_t _mutex;
};


class CReentrantLock
{
public:
	CReentrantLock()
	{
		_thread_lock_stats = new std::map<unsigned int, int>;
	}
	~CReentrantLock()
	{
		_mutex_stats.Lock();

		delete _thread_lock_stats;

		_thread_lock_stats = NULL;

		_mutex_stats.UnLock();
	}

	void TryLock()
	{
		bool _isneedusrlock = false;
		unsigned int _thread_id = PthreadSelf();

		_mutex_stats.Lock();

		if(_thread_lock_stats){

			std::map<unsigned int, int>::iterator _finditr = _thread_lock_stats->find(_thread_id);

			if(_finditr != _thread_lock_stats->end()){
				if(++(_finditr->second) <= 1){
					_finditr->second = 1;
					_isneedusrlock = true;
				}
			}else{
				_thread_lock_stats->insert(std::map<unsigned int, int>::value_type(_thread_id, 1));
				_isneedusrlock = true;
			}
		}

		_mutex_stats.UnLock();

		//将_mutex_usr.Lock放置于_mutex_stats释放后，防止死锁
		//函数中非成对出现的锁加上其他锁在多线程嵌套调用时一定会出现死锁
		if(_isneedusrlock){

			_mutex_usr.Lock();
		}
	}

	void TryUnLock()
	{
		bool _isneedusrunlock = false;
		unsigned int _thread_id = PthreadSelf();

		_mutex_stats.Lock();

		if(_thread_lock_stats){
			std::map<unsigned int, int>::iterator _finditr = _thread_lock_stats->find(_thread_id);

			if(_finditr != _thread_lock_stats->end()){
				if (--(_finditr->second)==0){
					_isneedusrunlock=true;
				}
			}else{
				_thread_lock_stats->insert(std::map<unsigned int, int>::value_type(_thread_id, 0));
			}
		}

		_mutex_stats.UnLock();

		if(_isneedusrunlock){

			_mutex_usr.UnLock();
		}
	}

	//强制解除当前线程所拥有的锁
	void Unlock()
	{
		bool _isneedusrunlock=false;
		unsigned int _thread_id = PthreadSelf();
		_mutex_stats.Lock();
		if(_thread_lock_stats){
			std::map<unsigned int, int>::iterator _finditr = _thread_lock_stats->find(_thread_id);

			if (_finditr!=_thread_lock_stats->end())
			{
				if (_finditr->second>0)
				{
					_isneedusrunlock=true;
					_finditr->second = 0 ;
				}
			}

		}else{
			int _new_lock_counts = 0;
			_thread_lock_stats->insert(Type_Typename std::map<unsigned int, int>::value_type(_thread_id, _new_lock_counts));
		}
		_mutex_stats.UnLock();

		if (_isneedusrunlock)
		{
			_mutex_usr.UnLock();
		}


	}

private:
	CLock _mutex_usr;

	CLock _mutex_stats;

	std::map<unsigned int, int>* _thread_lock_stats;
};


#endif // _Lock_H__