#ifndef _SYNTHESIZER_H_
#define _SYNTHESIZER_H_

#include "chain_pool.h"
#include "buffer.h"
#include "value.h"
#include <cstdint>
#include <map>

class SynthesizerIf {
public:
	virtual ~SynthesizerIf() = default;
    virtual void playNote(unsigned duration, float velocity, ptr<Value> note, ptr<Value> volume) = 0;
};

class Piano : public Value {
public:
	class Data {
		float sum;
	};
	Piano(){}
	ptr<AudioBuffer> get(unsigned sampleNr, unsigned len, Ctx& ctx, DataBuffer data) override;
	virtual unsigned size() const override { return sizeof(Data); };
};



class Note : public refcnt<Note>, public Ctx {
public:
	static const unsigned MaxSize;
	Note(ptr<Value> note, ptr<Value> volume, unsigned duration, unsigned start_nr, ptr<Value> instrument);
	virtual ~Note();
	ptr<AudioBuffer> get(unsigned sample_nr, unsigned len);
private:
	using ValueInstanceMap = std::map<char, ValueInstance>;
	ValueInstanceMap noteInstances_; // key = 0 is the actual instrument, '?' is note, '&' is volume
	unsigned duration_;
	unsigned start_nr_;
	virtual ValueInstance note() const override;
	virtual ValueInstance volume() const override;
	virtual float sampleRate() const override { return 44100; }
};

class Instrument {
public:
	std::map<char, ptr<Value>> types_; // key = 0 is the actual instrument
	std::map<char, ValueInstance> instrInstances_;
	ptr<Note> notes__[8];

};

class Synthesizer : public SynthesizerIf {
public:
	Synthesizer();
    void playNote(unsigned duration, float velocity, ptr<Value> note, ptr<Value> volume) override;
	void generate(int16_t* begin, int16_t* end);
private:
	int sample_nr;
	ChainPool<Note>::Scope notePoolScope_;
	ChainPool<BufferMem<uint8_t, 32>>::Scope dataPoolScope_;
	Instrument instrument0_;
	unsigned noteNum_;
};

#endif
