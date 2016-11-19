#ifndef _SafeVector_H__
#define _SafeVector_H__
#include <vector>
#include "Lock.h"




//////////////////////////////////////////////////////////////////////////
template<typename V>
class safe_vector;

template<typename V>
class safe_vector_iterator:public std::vector<V>::iterator
{
public:
	safe_vector_iterator();
	safe_vector_iterator(const safe_vector_iterator &ite);
	safe_vector_iterator& operator=(const safe_vector_iterator &ite);


	~safe_vector_iterator();

private:
	safe_vector_iterator& operator=(const typename std::vector<V>::iterator &ite);

	CReentrantLock* _mutex_stats;
	friend safe_vector<V>;
};

template<typename V>
safe_vector_iterator<V>& safe_vector_iterator<V>::operator=(const typename std::vector<V>::iterator &ite)
{
	this->std::vector<V>::iterator::operator=(ite);
	return *this;
}

template<typename V>
safe_vector_iterator<V>::~safe_vector_iterator()
{
	if (_mutex_stats)
	{
		_mutex_stats->TryUnLock();
	}
	_mutex_stats=NULL;
}

template<typename V>
safe_vector_iterator<V>& safe_vector_iterator<V>::operator=(const safe_vector_iterator &ite)
{
	if (this->_mutex_stats)
	{
		this->_mutex_stats->TryUnLock();
	}
	_mutex_stats=ite._mutex_stats;
	if (_mutex_stats)
	{
		_mutex_stats->TryLock();
	}
	this->std::vector<V>::iterator::operator=(ite);
	return *this;
}

template<typename V>
safe_vector_iterator<V>::safe_vector_iterator(const safe_vector_iterator &ite)
{


	_mutex_stats=ite._mutex_stats;
	if (_mutex_stats)
	{
		_mutex_stats->TryLock();
	}
	this->std::vector<V>::iterator::operator=(ite);
}

template<typename V>
safe_vector_iterator<V>::safe_vector_iterator():_mutex_stats(NULL)
{

}

//////////////////////////////////////////////////////////////////////////
template<typename V>
class safe_vector:private std::vector<V>
{
public:
	typedef typename std::vector<V>::size_type size_type;
	typedef typename std::vector<V>::value_type value_type;
	typedef typename safe_vector_iterator<V> iterator;

	safe_vector();
	safe_vector(const safe_vector& val_);
	~safe_vector();

	safe_vector& operator=(const safe_vector& val_);

	typedef typename safe_vector_iterator<V> iterator;

	void insert_s(const iterator& key_);
	void push_back_s(const value_type& key_);
	void pop_front_s();
	iterator erase_s(iterator& itr_);
	iterator find_s(const V& key_);

	iterator begin_s();
	iterator end_s();

	size_type size_s();
	void clear_s();

	//V& operator[](size_t idx);

	

protected:
private:
	CReentrantLock _mutex_stats;
};

//template<typename V>
//V& safe_vector<V>::operator[](size_t idx)
//{
//	V* val_;
//	_mutex_stats.TryLock();
//	if (idx>=0 && idx<std::map::size())
//	{
//		val_=&std::map[idx];
//	}
//	_mutex_stats.TryUnLock();
//	return *val;
//}

template<typename V>
void safe_vector<V>::pop_front_s()
{
	_mutex_stats.TryLock();

	this->pop_front();

	_mutex_stats.TryUnLock();
}

template<typename V>
void safe_vector<V>::push_back_s(const value_type& val_)
{
	_mutex_stats.TryLock();

	this->push_back(val_);

	_mutex_stats.TryUnLock();
}

template<typename V>
void safe_vector<V>::clear_s()
{
	_mutex_stats.TryLock();

	this->std::vector<V>::clear();

	//当前线程的所有迭代器均已失效，故强制解锁
	_mutex_stats.Unlock();
}

template<typename V>
typename safe_vector<V>::size_type safe_vector<V>::size_s()
{
	safe_vector<V>::size_type _size=0;
	_mutex_stats.TryLock();

	_size=this->std::vector<V>::size();

	_mutex_stats.TryUnLock();
	return _size;
}

template<typename V>
typename safe_vector<V>::iterator safe_vector<V>::end_s()
{
	safe_vector<V>::iterator ite;
	ite._mutex_stats=&_mutex_stats;
	_mutex_stats.TryLock();

	ite=this->end();

	return ite;

}

template<typename V>
typename safe_vector<V>::iterator safe_vector<V>::begin_s()
{
	safe_vector<V>::iterator ite;
	ite._mutex_stats=&_mutex_stats;
	_mutex_stats.TryLock();

	ite=this->begin();

	return ite;
}

template<typename V>
typename safe_vector<V>::iterator safe_vector<V>::find_s(const V& val_)
{
	safe_vector<V>::iterator ite;
	ite._mutex_stats=&_mutex_stats;
	_mutex_stats.TryLock();

	ite=this->find(val_);

	return ite;
}

template<typename V>
typename safe_vector<V>::iterator safe_vector<V>::erase_s(typename safe_vector<V>::iterator& itr_)
{
	_mutex_stats.TryLock();

	itr=this->erase(itr_);

	_mutex_stats.TryUnLock();
	return itr_;
}

template<typename V>
void safe_vector<V>::insert_s(const iterator &ite)
{
	_mutex_stats.TryLock();

	this->insert(itr_);

	_mutex_stats.TryUnLock();

}

template<typename V>
typename safe_vector<V>& safe_vector<V>::operator=(const typename safe_vector<V>& val_)
{
	_mutex_stats.TryLock();

	this->std::vector<V>::operator=(val);

	_mutex_stats.TryUnLock();

	return *this;
}

template<typename V>
safe_vector<V>::~safe_vector()
{

}

template<typename V>
safe_vector<V>::safe_vector(const typename safe_vector<V>& val_)
{
	_mutex_stats.TryLock();

	this->std::vector<V>::operator=(val);

	_mutex_stats.TryUnLock();

	return *this;
}

template<typename V>
safe_vector<V>::safe_vector()
{

}



#endif // _SafeVector_H__