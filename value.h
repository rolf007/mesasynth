#ifndef _VALUE_H_
#define _VALUE_H_

#include "refcnt.h"
#include <set>
#include <vector>

class Value;
template<unsigned S>
class Buffer;

class Ctx {
public:
	virtual ptr<Value> note() const = 0;
	virtual ptr<Value> volume() const = 0;
	virtual float sampleRate() const = 0;
	virtual float& sum(Value*) = 0;
};

class Value : public refcnt<Value> {
public:
	static const unsigned MaxSize;
	Value() {}
	~Value() = default;
	virtual ptr<Buffer<256>> get(unsigned sampleNr, unsigned len, Ctx& ctx) = 0;
};

class Oscillator : public Value {
public:
	Oscillator(float freq, float amp);
	ptr<Buffer<256>> get(unsigned sampleNr, unsigned len, Ctx& ctx) override;
private:
	float freq_;
	float amp_;
};

class Envelope : public Value {
public:
	Envelope() {}
	ptr<Buffer<256>> get(unsigned sampleNr, unsigned len, Ctx& ctx) override;
	void addSegment(float duration, float value);
	std::vector<std::pair<float, float>> segments_;
};

class Const : public Value {
public:
	Const(float value) : value_(value) {}
	ptr<Buffer<256>> get(unsigned sampleNr, unsigned len, Ctx& ctx) override;
private:
	float value_;
};

class Adder : public Value {
public:
	Adder(ptr<Value> lhs, ptr<Value> rhs) : lhs_(lhs), rhs_(rhs) {}
	ptr<Buffer<256>> get(unsigned sampleNr, unsigned len, Ctx& ctx) override;
private:
	ptr<Value> lhs_;
	ptr<Value> rhs_;
};
#endif
