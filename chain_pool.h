#ifndef _CHAIN_POOL_H_
#define _CHAIN_POOL_H_
#include "refcnt.h"
#include <iostream>
#include <string.h>

template<typename T>
class ChainPool {
public:
	struct F{F* next;};
	ChainPool(unsigned maxEntries) : freePool_(nullptr), maxEntries_(maxEntries) {
		mem_ = new unsigned char[maxEntries_*sizeof(T)];
		memset(mem_, 0, maxEntries_*sizeof(T));
        static_assert(sizeof(T) >= sizeof(void*));
		for (unsigned i = 0; i < maxEntries_-1; ++i) {
			((F*)(((T*)mem_) + i))->next = (F*)(((T*)mem_) + i + 1);
		}
		((F*)(((T*)mem_) + maxEntries_ - 1))->next = nullptr;
		freePool_ = (F*)mem_;
	}
	~ChainPool()
	{
		F* f = freePool_;
		unsigned num = 0;
		while (f) {
			++num;
			f = f->next;
		}
		if (num != maxEntries_)
			std::cout << "ChainPool leak: " << num << "/" << maxEntries_ << " free" << std::endl;
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
			std::cout << "ERROR chainPool out of memory" << std::endl;
			return nullptr;
		}
		freePool_ = f->next;
		ptr<T> t(new(f)T(args...));
		t->chainPool_ = this;
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
	F* freePool_;
};

template<typename C>
void refcnt<C>::destroy() {
	chainPool_->release((C*)this);
}

#endif
