#ifndef _SYNTHESIZER_H_
#define _SYNTHESIZER_H_

#include <cstdint>
#include <vector>

class SynthesizerIf {
public:
	virtual ~SynthesizerIf() = default;
    virtual void playNote(float duration, float velocity, float note, float volume, unsigned noteValueId, unsigned volumeValueId) = 0;
	virtual void generate(int16_t* begin, int16_t* end) = 0;
};

class Synthesizer : public SynthesizerIf {
public:
	class Note {
	public:
		Note(float note, float duration);
		void generate(int16_t* begin, int16_t* end);
	private:
		float note_;
		float duration_;
		unsigned sample_nr;
	};
	Synthesizer(int sampleRate);
    void playNote(float duration, float velocity, float note, float volume, unsigned noteValueId, unsigned volumeValueId) override;
	void generate(int16_t* begin, int16_t* end) override;
private:
	int sampleRate_;
	int sample_nr;
	float hz_;
	unsigned samples_;
	std::vector<Note> notes_;
};

#endif
