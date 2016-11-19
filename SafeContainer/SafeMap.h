#ifndef _SafeMap_H__
#define _SafeMap_H__
#include <map>
#include "Lock.h"


template<class K, class V>
class safe_map;

template<class K, class V>
class safe_map_iterator;

//safe_map_iterator:迭代器继承标准迭代器,减少运算符和一些函数的重载
template<class K, class V>
class safe_map_iterator: public Type_Typename std::map<K, V>::iterator
{
public:
	safe_map_iterator();
	//************************************  
	// 函数名称: safe_map_iterator
	// 函数说明： 重载拷贝构造，方便拷贝时就Lock,释放时就UnLock；
	// 作 成 者：smallErLang  
	// 作成日期：2016/04/22
	// 返 回 值:   
	// 参    数: const safe_map_iterator & val_
	//************************************
	safe_map_iterator(const safe_map_iterator& val_);
	~safe_map_iterator();

	//************************************  
	// 函数名称: operator=
	// 函数说明： 赋值运算会隐藏父类赋值函数，不会继承，需要重载
	// 作 成 者：smallErLang  
	// 作成日期：2016/04/22
	// 返 回 值: safe_map_iterator&  
	// 参    数: const safe_map_iterator & val_
	//************************************
	safe_map_iterator& operator=(const safe_map_iterator& val_);

private:
	safe_map_iterator& operator=(const Declar_Typename std::map<K, V>::iterator& val_);

private:
	CReentrantLock* _mutex_stats;

	friend safe_map<K, V>;
};

template<class K, class V>
safe_map_iterator<K, V>& safe_map_iterator<K, V>::operator=( const safe_map_iterator<K, V>& val_ )
{

	if (_mutex_stats)
	{
		//已经获取过锁，需要释放
		_mutex_stats->TryUnLock();
	}

	_mutex_stats = val_._mutex_stats;

	if(_mutex_stats){

		_mutex_stats->TryLock();
	}

	this->std::map<K, V>::iterator::operator=(val_);

	return *this;
}

template<class K, class V>
safe_map_iterator<K, V>::safe_map_iterator( const safe_map_iterator& val_ )
{
	_mutex_stats = val_._mutex_stats;

	if(_mutex_stats){

		_mutex_stats->TryLock();
	}

	this->std::map<K, V>::iterator::operator=(val_);
}

template<class K, class V>
safe_map_iterator<K, V>& safe_map_iterator<K, V>::operator=( const Declar_Typename std::map<K, V>::iterator& val_ )
{
	this->std::map<K, V>::iterator::operator=(val_);

	return *this;
}

//************************************  
// 函数名称: ~safe_map_iterator
// 函数说明： 迭代器赋值和拷贝构造时Lock，析构时需要释放锁
// 作 成 者：smallErLang  
// 作成日期：2016/04/22
// 返 回 值:   
//************************************
template<class K, class V>
safe_map_iterator<K, V>::~safe_map_iterator()
{
	if(_mutex_stats){

		_mutex_stats->TryUnLock();
	}

	_mutex_stats = NULL;
}

template<class K, class V>
safe_map_iterator<K, V>::safe_map_iterator()
{
	_mutex_stats = NULL;
}

//mymap
/************************************************************************/
/* 
由于iterator默认构造函数没有TryLock，所以在返回该类型临时变量时需要TryLock一次，
临时变量析构时TryUnLock一次；将返回的值进行拷贝构造或者拷贝赋值会TryLock一次，使用
完后析构TryUnLock一次。达到成对锁和释放锁的目的。
*/
/************************************************************************/
template<class K, class V>
class safe_map: private std::map<K, V>
{
public:
	safe_map();
	safe_map(const safe_map& val_);
	~safe_map();

	safe_map& operator=(const safe_map& val_);

	typedef Declar_Typename safe_map_iterator<K, V> iterator;

	void insert_s(const K& key_, const V& val_);
	void erase_s(const K& key_);
	iterator erase_s(iterator& itr_);
	iterator find_s(const K& key_);

	iterator begin_s();
	iterator end_s();

	void clear_s();

	//V& operator[](const K& key_);


	unsigned int size_s();

private:
	CReentrantLock _mutex_stats;
};

//template<class K, class V>
//V& safe_map<K, V>::operator[](const K& key_)
//{
//	V* val_;
//	_mutex_stats.TryLock();
//	val_=&std::map[key_];
//	_mutex_stats.TryUnLock();
//	return *val_;
//}

template<class K, class V>
void safe_map<K, V>::clear_s()
{
	_mutex_stats.TryLock();
	std::map::clear();

	_mutex_stats.Unlock();
}

template<class K, class V>
unsigned int safe_map<K, V>::size_s()
{
	unsigned int _size = 0;

	_mutex_stats.TryLock();

	_size = this->size();

	_mutex_stats.TryUnLock();

	return _size;
}

template<class K, class V>
Declar_Typename safe_map<K, V>::iterator safe_map<K, V>::end_s()
{
	safe_map<K, V>::iterator _ret;

	_ret._mutex_stats = &_mutex_stats;

	_mutex_stats.TryLock();

	_ret = this->end();

	return _ret;
}

template<class K, class V>
Declar_Typename safe_map<K, V>::iterator safe_map<K, V>::begin_s()
{
	safe_map<K, V>::iterator _ret;

	_ret._mutex_stats = &_mutex_stats;

	_mutex_stats.TryLock();

	_ret = this->begin();

	return _ret;
}

template<class K, class V>
Declar_Typename safe_map<K, V>::iterator safe_map<K, V>::find_s( const K& key_ )
{
	safe_map<K, V>::iterator _ret;

	_ret._mutex_stats = &_mutex_stats;

	_mutex_stats.TryLock();

	_ret = this->find(key_);

	return _ret;
}

template<class K, class V>
Declar_Typename safe_map<K, V>::iterator safe_map<K, V>::erase_s( iterator& itr_ )
{
	_mutex_stats.TryLock();

	this->erase(itr_++);

	_mutex_stats.TryUnLock();

	return itr_;
}

template<class K, class V>
void safe_map<K, V>::erase_s( const K& key_ )
{
	_mutex_stats.TryLock();

	this->erase(key_);

	_mutex_stats.TryUnLock();

}

template<class K, class V>
void safe_map<K, V>::insert_s( const K& key_, const V& val_ )
{
	_mutex_stats.TryLock();

	this->insert(std::map<K, V>::value_type(key_, val_));

	_mutex_stats.TryUnLock();

	return ;
}

template<class K, class V>
safe_map<K, V>& safe_map<K, V>::operator=( const safe_map& val_ )
{
	val_._mutex_stats.TryLock();

	this->std::map<K, V>::operator =(val_);

	val_._mutex_stats.TryUnLock();

	return *this;
}

template<class K, class V>
safe_map<K, V>::~safe_map()
{

}

template<class K, class V>
safe_map<K, V>::safe_map( const safe_map<K, V>& val_ )
{
	val_._mutex_stats.TryLock();

	this->std::map<K, V>::operator =(val_);

	val_._mutex_stats.TryUnLock();
}

template<class K, class V>
safe_map<K, V>::safe_map()
{

}



#endif // _SafeMap_H__
