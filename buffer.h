#ifndef _BUFFER_H_
#define _BUFFER_H_

#include "refcnt.h"
#include <iostream>

template<typename T, unsigned SIZE>
class BufferMem : public refcnt<BufferMem<T, SIZE>> {
public:
	static const unsigned MaxSize = sizeof(BufferMem);
	BufferMem(unsigned size) : size_(size)
	{
		if (size > SIZE) {
			std::cout << "ERROR: BufferMem: " << size << " > " << SIZE << "!" << std::endl;
			exit(-1);
		}
	}
	unsigned size() const { return size_; }
	T* buff() { return buff_; }
	unsigned size_;
	T buff_[SIZE];
};

using AudioBuffer = BufferMem<float, 256>;

//using DataBuffer = BufferMem<uint8_t, 32>;
//using DataBuffer = ptr<BufferMem<uint8_t, 32>>;

class Buff {
public:
    template<typename POOL>
    static Buff mk(unsigned size) {
        auto b(POOL::instance().mk(size));
        return Buff(b, b->buff(), b->size());
    }
    const uint8_t* get() const { return mem_; }
    unsigned size() const { return size_; }
    uint8_t* get() { return mem_; }
private:
    Buff(ptr_base base, uint8_t* mem, unsigned size) : base_(base), mem_(mem), size_(size) {}
    ptr_base base_;
    uint8_t* mem_;
    unsigned size_;
};

using DataBuffer = Buff;
#endif
