#ifndef _VALUE_H_
#define _VALUE_H_

#include "refcnt.h"
#include <set>
#include <vector>

class Value;
template<unsigned S>
class Buffer;

class ValueInstance {
public:
	explicit ValueInstance(ptr<Value> value);
	ptr<Value> value_;
	ptr<Buffer<32>> data_;
};

class Ctx {
public:
	virtual ValueInstance note() const = 0;
	virtual ValueInstance volume() const = 0;
	virtual float sampleRate() const = 0;
};

class Value : public refcnt<Value> {
public:
	static const unsigned MaxSize;
	Value() {}
	virtual ~Value() = default;
	virtual ptr<Buffer<256>> get(unsigned sampleNr, unsigned len, Ctx& ctx, ptr<Buffer<32>> data) = 0;
	virtual unsigned size() const = 0;
};

class Oscillator : public Value {
public:
	class Data {
		float sum;
	};
	Oscillator(float freq, float amp);
	ptr<Buffer<256>> get(unsigned sampleNr, unsigned len, Ctx& ctx, ptr<Buffer<32>> data) override;
	virtual unsigned size() const override { return sizeof(Data); };
private:
	float freq_;
	float amp_;
};

class Envelope : public Value {
public:
	Envelope() {}
	ptr<Buffer<256>> get(unsigned sampleNr, unsigned len, Ctx& ctx, ptr<Buffer<32>> data) override;
	void addSegment(float duration, float value);
	std::vector<std::pair<float, float>> segments_;
	virtual unsigned size() const override { return 0; };
};

class Const : public Value {
public:
	Const(float value) : value_(value) {}
	ptr<Buffer<256>> get(unsigned sampleNr, unsigned len, Ctx& ctx, ptr<Buffer<32>> data) override;
	virtual unsigned size() const override { return 0; };
private:
	float value_;
};

class Adder : public Value {
public:
	Adder(ptr<Value> lhs, ptr<Value> rhs) : lhs_(lhs), rhs_(rhs) {}
	ptr<Buffer<256>> get(unsigned sampleNr, unsigned len, Ctx& ctx, ptr<Buffer<32>> data) override;
	virtual unsigned size() const override { return lhs_->size() + rhs_->size(); };
private:
	ptr<Value> lhs_;
	ptr<Value> rhs_;
};
#endif
