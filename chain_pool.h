#ifndef _CHAIN_POOL_H_
#define _CHAIN_POOL_H_
#include "refcnt.h"
#include <iostream>
#include <string.h>

template<typename T>
class ChainPool {
	static ChainPool<T>* inst;
public:
	class Scope {
		ChainPool<T> inst;
	public:
		Scope(unsigned maxEntries) :
			inst(maxEntries)
		{
			if (ChainPool<T>::inst)
				std::cout << "ERROR: " << typeid(ChainPool<T>).name() << " already constructed" << std::endl;
			ChainPool<T>::inst = &inst;
		}
		Scope(const Scope&) = delete;
		~Scope()
		{
			ChainPool<T>::inst = nullptr;
		}
	};
	static ChainPool<T>& instance()
	{
		if (inst == nullptr) {
			std::cout << "ERROR: " << typeid(ChainPool<T>).name() << " not constructed" << std::endl;
			exit(-1);
		}
		return *inst;
	}
	struct F{F* next;};
	friend class Scope;
private:
	ChainPool(unsigned maxEntries) : freePool_(nullptr), maxEntries_(maxEntries), maxSize_(T::MaxSize) {
		mem_ = new unsigned char[maxEntries_*maxSize_];
		memset(mem_, 0, maxEntries_*maxSize_);
        if (maxSize_ < sizeof(void*))
			std::cout << "ERROR: ChainPool entries not big enough to hold a pointer" << std::endl;
		for (unsigned i = 0; i < maxEntries_-1; ++i) {
			unsigned char** x = (unsigned char**)(mem_+i*maxSize_);
			*x = mem_+(i+1)*maxSize_;
		}
		unsigned char** x = (unsigned char**)(mem_+(maxEntries-1)*maxSize_);
		*x = nullptr;
		freePool_ = (F*)mem_;
	}
public:
	~ChainPool()
	{
		F* f = freePool_;
		unsigned num = 0;
		while (f) {
			++num;
			f = f->next;
		}
		if (num != maxEntries_)
			std::cout << "ERROR: ChainPool leak: " << num << "/" << maxEntries_ << " free" << std::endl;
		delete []mem_;
	}
	void dump()
	{
		for (unsigned i = 0; i < maxEntries_*sizeof(T); ++i) {
			if ((i%sizeof(T)) == 0)
				std::cout << std::endl;
			if ((i%8) == 0)
				std::cout << std::endl;
			std::cout << " " << std::hex << (unsigned)(mem_[i]);
		}
	}
	template<typename ...Args>
	ptr<T> mk(Args&&... args)
	{
		F* f = freePool_;
		if (!f) {
			std::cout << "ERROR mk() " << typeid(ChainPool<T>).name() << " out of memory" << std::endl;
			return nullptr;
		}
		freePool_ = f->next;
		ptr<T> t(new(f)T(args...));
		return t;
	}
	template<typename T2, typename ...Args>
	ptr<T2> mk2(Args&&... args)
	{
		F* f = freePool_;
		if (!f) {
			std::cout << "ERROR mk2 " << typeid(ChainPool<T>).name() << " out of memory" << std::endl;
			exit(-1);
			return nullptr;
		}
		if (sizeof(T2) > maxSize_) {
			std::cout << "ERROR mk2 " << typeid(ChainPool<T>).name() << " (" << sizeof(T2) << ") larger than maxSize (" << maxSize_ << ")" << std::endl;
			exit(-1);
		}
		freePool_ = f->next;
		ptr<T2> t(new(f)T2(args...));
		return t;
	}
	void release(T* t)
	{
        t->~T();
		F* f = (F*)t;
		f->next = freePool_;
		freePool_ = f;
	}
private:
	unsigned char* mem_;
	unsigned maxEntries_;
	unsigned maxSize_;
	F* freePool_;
};

template<typename T>
ChainPool<T>* ChainPool<T>::inst = nullptr;

template<typename C>
void refcnt<C>::destroy() {
	ChainPool<C>::instance().release((C*)this);
}

#endif
