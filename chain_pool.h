#ifndef _CHAIN_POOL_H_
#define _CHAIN_POOL_H_
#include <iostream>
#include <iomanip>
#include <string.h>

template<typename T, unsigned MAX_ENTRIES>
class ChainPool {
public:
	struct F{F* next;};
	ChainPool() : freePool_(nullptr) {
		memset(mem_, 0, MAX_ENTRIES*sizeof(T));
        static_assert(sizeof(T) >= sizeof(void*));
		for (unsigned i = 0; i < MAX_ENTRIES-1; ++i) {
			((F*)(((T*)mem_) + i))->next = (F*)(((T*)mem_) + i + 1);
		}
		((F*)(((T*)mem_) + MAX_ENTRIES - 1))->next = nullptr;
		freePool_ = (F*)mem_;
	}
	void dump()
	{
		for (unsigned i = 0; i < MAX_ENTRIES*sizeof(T); ++i) {
			if ((i%sizeof(T)) == 0)
				std::cout << std::endl;
			if ((i%8) == 0)
				std::cout << std::endl;
			std::cout << " " << std::hex << (unsigned)(mem_[i]);
		}
	}
	template<typename ...Args>
	T* mk(Args&&... args)
	{
		F* f = freePool_;
		if (!f) {
			std::cout << "ERROR chainPool out of memory" << std::endl;
			return nullptr;
		}
		freePool_ = f->next;
		T* t = new(f)T(args...);
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
	unsigned char mem_[MAX_ENTRIES*sizeof(T)];
	F* freePool_;

};

#endif
