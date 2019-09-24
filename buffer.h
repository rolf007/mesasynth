#ifndef _BUFFER_H_
#define _BUFFER_H_

#include "refcnt.h"

template<unsigned SIZE>
class Buffer : public refcnt<Buffer<SIZE>> {
public:
	Buffer(unsigned size) : size_(size)
	{
		if (size > 256)
			size_ = 256;
	}
	unsigned size_;
	float buff[SIZE];
};
#endif
