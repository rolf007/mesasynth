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
	Piano(){}
	ptr<Buffer<256>> get(unsigned sampleNr, unsigned len, Ctx& ctx) override;
};

class Synthesizer : public SynthesizerIf {
public:
	class Note : public refcnt<Note>, public Ctx {
	public:
		static const unsigned MaxSize;
		Note(ptr<Value> note, ptr<Value> volume, unsigned duration, unsigned start_nr, ptr<Value> instrument);
		virtual ~Note();
		ptr<Buffer<256>> get(unsigned sample_nr, unsigned len);
	private:
		ptr<Value> instrument_;
		ptr<Value> note_;
		ptr<Value> volume_;
		unsigned duration_;
		unsigned start_nr_;
		virtual ptr<Value> note() const override { return note_; }
		virtual ptr<Value> volume() const override { return volume_; }
		virtual float sampleRate() const override { return 44100; }
		virtual float& sum(Value* v) override { return sumMap_[v]; }
		std::map<Value*, float> sumMap_;
	};
	ptr<Value> instrument0_;
	Synthesizer();
    void playNote(unsigned duration, float velocity, ptr<Value> note, ptr<Value> volume) override;
	void generate(int16_t* begin, int16_t* end);
private:
	int sample_nr;
	ChainPool<Note>::Scope notePool_;
	ptr<Note> notes__[8];
	unsigned noteNum_;
};

#endif
