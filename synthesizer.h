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
    virtual void playNote(float duration, float velocity, float note, float volume, ptr<Value> noteValuePtr, ptr<Value> volumeValuePtr) = 0;
	virtual void generate(int16_t* begin, int16_t* end) = 0;
};

class Synthesizer : public SynthesizerIf {
public:
	class Note : public refcnt<Note> {
	public:
		Note(float note, float duration, unsigned start_nr);
		virtual ~Note();
		ptr<Buffer<256>> get(unsigned sample_nr, unsigned len);
	private:
		float note_;
		float duration_;
		unsigned start_nr_;
	};
	Synthesizer(int sampleRate);
    void playNote(float duration, float velocity, float note, float volume, ptr<Value> noteValuePtr, ptr<Value> volumeValuePtr) override;
	void generate(int16_t* begin, int16_t* end) override;
private:
	int sampleRate_;
	int sample_nr;
	unsigned samples_;
	ChainPool<Note>::Scope notePool_;
	ptr<Note> notes__[8];
	unsigned noteNum_;
};

#endif
