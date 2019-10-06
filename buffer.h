#ifndef _BUFFER_H_
#define _BUFFER_H_

#include "refcnt.h"
#include <iostream>

template<unsigned SIZE>
class Buffer : public refcnt<Buffer<SIZE>> {
public:
	static const unsigned MaxSize = sizeof(Buffer);
	Buffer(unsigned size) : size_(size)
	{
		if (size > SIZE) {
			std::cout << "ERROR: Buffer: " << size << " > " << SIZE << "!" << std::endl;
			exit(-1);
		}
	}
	unsigned size() const { return size_; }
	unsigned size_;
	float buff[SIZE];
};
#endif
