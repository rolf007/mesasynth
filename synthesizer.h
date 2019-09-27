#ifndef _SYNTHESIZER_H_
#define _SYNTHESIZER_H_

#include "chain_pool.h"
#include "buffer.h"
#include "value.h"
#include <cstdint>
#include <vector>

class SynthesizerIf {
public:
	virtual ~SynthesizerIf() = default;
    virtual void playNote(unsigned duration, float velocity, float note, float volume, ptr<Value> noteValuePtr, ptr<Value> volumeValuePtr) = 0;
};

class Piano : public Value {
public:
	Piano(){}
	ptr<Buffer<256>> get(unsigned sampleNr, unsigned len, const Ctx& ctx) override;
};

class Synthesizer : public SynthesizerIf {
public:
	class Note : public refcnt<Note>, public Ctx {
	public:
		Note(float note, unsigned duration, unsigned start_nr, ptr<Value> instrument);
		virtual ~Note();
		ptr<Buffer<256>> get(unsigned sample_nr, unsigned len);
	private:
		ptr<Value> instrument_;
		float note_;
		unsigned duration_;
		unsigned start_nr_;
		virtual float note() const { return note_; }
		virtual float sampleRate() const override { return 44100; }
	};
	ptr<Value> instrument0_;
	Synthesizer();
    void playNote(unsigned duration, float velocity, float note, float volume, ptr<Value> noteValuePtr, ptr<Value> volumeValuePtr) override;
	void generate(int16_t* begin, int16_t* end);
private:
	int sample_nr;
	ChainPool<Note>::Scope notePool_;
	ptr<Note> notes__[8];
	unsigned noteNum_;
};

#endif
