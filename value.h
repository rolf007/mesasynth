#ifndef _VALUE_H_
#define _VALUE_H_

#include "refcnt.h"
#include "buffer.h"

class Value : public refcnt<Value> {
public:
	Value() {}
	~Value() = default;
	virtual ptr<Buffer<256>> get(unsigned sampleNr, unsigned len) = 0;
};

class Oscillator : public Value {
public:
	Oscillator() {}
	virtual ptr<Buffer<256>> get(unsigned sampleNr, unsigned len) override { ptr<Buffer<256>> buff = ChainPool<Buffer<256>>::instance().mk(len); buff->buff[0] = 91.0; return buff; }
};

class Envelope : public Value {
public:
	Envelope() {}
	virtual ptr<Buffer<256>> get(unsigned sampleNr, unsigned len) override { ptr<Buffer<256>> buff = ChainPool<Buffer<256>>::instance().mk(len); buff->buff[0] = 92.0; return buff; }
	void addSegment(float duration, float value) {}
	unsigned a,b;
};

class Const : public Value {
public:
	Const(float value) : value_(value) {}
	virtual ptr<Buffer<256>> get(unsigned sampleNr, unsigned len) override { ptr<Buffer<256>> buff = ChainPool<Buffer<256>>::instance().mk(len); buff->buff[0] = value_; return buff; }
private:
	float value_;
};
#endif
