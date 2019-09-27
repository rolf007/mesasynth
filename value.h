#ifndef _VALUE_H_
#define _VALUE_H_

#include "refcnt.h"
#include "buffer.h"
#include "chain_pool.h"

class Ctx {
public:
	virtual float note() const = 0;
	virtual float sampleRate() const = 0;
};

class Value : public refcnt<Value> {
public:
	Value() {}
	~Value() = default;
	virtual ptr<Buffer<256>> get(unsigned sampleNr, unsigned len, const Ctx& ctx) = 0;
};

class Oscillator : public Value {
public:
	Oscillator(float freq, float amp);
	ptr<Buffer<256>> get(unsigned sampleNr, unsigned len, const Ctx& ctx) override;
private:
	float freq_;
	float amp_;
};

class Envelope : public Value {
public:
	Envelope() {}
	ptr<Buffer<256>> get(unsigned sampleNr, unsigned len, const Ctx& ctx) override;
	void addSegment(float duration, float value) {}
	unsigned a,b;
};

class Const : public Value {
public:
	Const(float value) : value_(value) {}
	ptr<Buffer<256>> get(unsigned sampleNr, unsigned len, const Ctx& ctx) override;
private:
	float value_;
};
#endif
