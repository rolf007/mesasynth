#ifndef _BUFFER_H_
#define _BUFFER_H_

#include "refcnt.h"
#include <iostream>

template<typename T>
class BufferMem : public refcnt {
public:
	explicit BufferMem(unsigned size) : size_(size)
	{
		//if (size > SIZE) {
		//	std::cout << "ERROR: BufferMem: " << size << " > " << SIZE << "!" << std::endl;
		//	exit(-1);
		//}
	}
	unsigned size() const { return size_; }
	T* buff() { return buff_; }
	unsigned size_;
	T buff_[0];
};

class AudioBuffer : public BufferMem<float>
{
public:
	static const unsigned MaxSize = sizeof(BufferMem<float>) + sizeof(float)*256;
	explicit AudioBuffer(unsigned size) : BufferMem<float>(size) {}
};

class DataBuffer : public BufferMem<uint8_t>
{
public:
	static const unsigned MaxSize = sizeof(BufferMem<uint8_t>) + sizeof(uint8_t)*32;
	explicit DataBuffer(unsigned size) : BufferMem<uint8_t>(size) {}
};

#endif
