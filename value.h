#ifndef _VALUE_H_
#define _VALUE_H_

#include "refcnt.h"
#include <set>
#include <vector>
#include "buffer.h"

class Value;

class ValueInstance {
public:
	explicit ValueInstance(ptr<Value> value);
	ptr<Value> value_;
	ptr<DataBuffer> data_;
};

class Ctx {
public:
	virtual ValueInstance note() const = 0;
	virtual ValueInstance volume() const = 0;
	virtual float sampleRate() const = 0;
};

class Value : public refcnt {
public:
	static const unsigned MaxSize;
	Value() {}
	virtual ~Value() = default;
	virtual ptr<AudioBuffer> get(unsigned sampleNr, unsigned len, Ctx& ctx, ptr<DataBuffer> data) = 0;
	virtual unsigned size() const = 0;
};

class Oscillator : public Value {
public:
	class Data {
		float sum;
	};
	Oscillator(ptr<Value> freq, float amp);
	ptr<AudioBuffer> get(unsigned sampleNr, unsigned len, Ctx& ctx, ptr<DataBuffer> data) override;
	virtual unsigned size() const override { return sizeof(Data); };
	ptr<Value> freq() const { return freq_; }
private:
	ptr<Value> freq_;
	float amp_;
};

class Envelope : public Value {
public:
	Envelope() {}
	ptr<AudioBuffer> get(unsigned sampleNr, unsigned len, Ctx& ctx, ptr<DataBuffer> data) override;
	void addSegment(float duration, float value);
	std::vector<std::pair<float, float>> segments_;
	virtual unsigned size() const override { return 0; };
};

class Const : public Value {
public:
	Const(float value) : value_(value) {}
	ptr<AudioBuffer> get(unsigned sampleNr, unsigned len, Ctx& ctx, ptr<DataBuffer> data) override;
	virtual unsigned size() const override { return 0; };
	float value() const { return value_; }
private:
	float value_;
};

class Adder : public Value {
public:
	Adder(ptr<Value> lhs, ptr<Value> rhs) : lhs_(lhs), rhs_(rhs) {}
	ptr<AudioBuffer> get(unsigned sampleNr, unsigned len, Ctx& ctx, ptr<DataBuffer> data) override;
	virtual unsigned size() const override { return lhs_->size() + rhs_->size(); };
	ptr<Value> lhs() const { return lhs_; }
	ptr<Value> rhs() const { return rhs_; }
private:
	ptr<Value> lhs_;
	ptr<Value> rhs_;
};
#endif
