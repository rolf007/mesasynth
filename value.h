#ifndef _VALUE_H_
#define _VALUE_H_

class Value {
public:
	~Value() = default;
	virtual float get() = 0;
};

class Oscillator : public Value {
public:
	Oscillator(){}
	virtual float get() override { return 91.0; }
};

class Envelope : public Value {
public:
	Envelope(){}
	virtual float get() override { return 92.0; }
	unsigned a,b;
};

class Const : public Value {
public:
	Const(float value) : value_(value) {}
	virtual float get() override { return value_; }
private:
	float value_;
};
#endif
