#ifndef _SYNTHESIZER_H_
#define _SYNTHESIZER_H_

#include "chain_pool.h"
#include "buffer.h"
#include <cstdint>
#include <vector>

template<unsigned SIZE>
class BufferX : public refcnt<BufferX<SIZE>> {
public:
	BufferX(unsigned size) : size_(size)
	{
		if (size > 256)
			size_ = 256;
	}
	unsigned size_;
	float buff[SIZE];
};

class SynthesizerIf {
public:
	virtual ~SynthesizerIf() = default;
    virtual void playNote(float duration, float velocity, float note, float volume, unsigned noteValueId, unsigned volumeValueId) = 0;
	virtual void generate(int16_t* begin, int16_t* end) = 0;
};

class Synthesizer : public SynthesizerIf {
public:
	class Note : public refcnt<Note> {
	public:
		Note(float note, float duration, unsigned start_nr, ChainPool<BufferX<256>>& bufferPool);
		virtual ~Note();
		ptr<BufferX<256>> get(unsigned sample_nr, unsigned len);
	private:
		float note_;
		float duration_;
		unsigned start_nr_;
		ChainPool<BufferX<256>>& bufferPool_;
	};
	Synthesizer(int sampleRate);
    void playNote(float duration, float velocity, float note, float volume, unsigned noteValueId, unsigned volumeValueId) override;
	void generate(int16_t* begin, int16_t* end) override;
private:
	int sampleRate_;
	int sample_nr;
	unsigned samples_;
	ChainPool<Note> notePool_;
	ptr<Note> notes__[8];
	unsigned noteNum_;
	ChainPool<BufferX<256>> bufferPool_;
};

#endif
