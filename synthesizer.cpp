#include "synthesizer.h"
#include <math.h>
#include <iostream>
#include <string.h>

using namespace std;

Synthesizer::Synthesizer(int sampleRate) : sampleRate_(sampleRate), sample_nr(0), notePool_(10), bufferPool_(10)
{
	samples_ = 0;
	noteNum_ = 0;
}

void Synthesizer::playNote(float duration, float velocity, float note, float volume, unsigned noteValueId, unsigned volumeValueId)
{
	cout << "======================== play note " << note << ", duration = " << duration << endl;
	ptr<Note> newNote = notePool_.mk(note, duration, sample_nr, bufferPool_);
	notes__[noteNum_++] = newNote;
	noteNum_ &= 7;
}

void Synthesizer::generate(int16_t* begin, int16_t* end)
{
	memset(begin, 0, end-begin);
	int16_t* p = begin;
	while (p < end) {
		unsigned len = end-p;
		if (len>256) len = 256;
		for (ptr<Note> note : notes__) {
			if (!note) continue;
			ptr<BufferX<256>> buf;
			buf = note->get(sample_nr, len);
			for(unsigned i = 0; i < buf->size_; ++i) {
				p[i] += buf->buff[i]/2;
			}
		}
		sample_nr += len;
		p += len;
	}
}

Synthesizer::Note::Note(float note, float duration, unsigned start_nr, ChainPool<BufferX<256>>& bufferPool) : note_(note), duration_(duration), start_nr_(start_nr), bufferPool_(bufferPool)
{
}

Synthesizer::Note::~Note()
{
	cout << "note really terminated" << endl;
}
ptr<BufferX<256>> Synthesizer::Note::get(unsigned sample_nr, unsigned len)
{
	ptr<BufferX<256>> buff = bufferPool_.mk(len);
	float hz = pow(1.059463094, note_)*65.40639133; // frequency of c0 is 65.40639133
	float sampleRate_ = 44100;
	for(unsigned i = 0; i < buff->size_; ++i) {
		double time = (double)(sample_nr-start_nr_+i) / (double)sampleRate_;
		int AMPLITUDE = 3600-(sample_nr-start_nr_+i)/5;
		if (AMPLITUDE <0) AMPLITUDE = 0;
		buff->buff[i]  = (int16_t)(AMPLITUDE * sin(2.0f * M_PI * hz * time*1)*1.00);
		buff->buff[i] += (int16_t)(AMPLITUDE * sin(2.0f * M_PI * hz * time*2.01)*1.31/2);
		buff->buff[i] += (int16_t)(AMPLITUDE * sin(2.0f * M_PI * hz * time*3.01)*2/3);
		buff->buff[i] += (int16_t)(AMPLITUDE * sin(2.0f * M_PI * hz * time*4.01)*2.31/4);
		buff->buff[i] += (int16_t)(AMPLITUDE * sin(2.0f * M_PI * hz * time*5.01)*0.31/5);
	}
	return buff;
}

// from seq, we get: volume(&), note(?), freq(??), velocity(^), duration($)
// (~1f??;*~1f??*1.33;)*:.1>1:0.2>.5:$-.2>.5:.8>;

// o-params: a amplitude, f = freq, w = waveform, m = pwm, t = trig, s = sync
// trig options: never, first note, each note individual, each note shared


#if 0
parseO(const char*& str, valueId)
{
    // c$+~.2f2;        // c with unnamed oscillator, oscillator follows lifetime of note
    // x~.2f2;c$+x      // c with named oscillator, oscillator follows lifetime of x (for ever?)
    // ~a0.9f1.2w8p.2;  // oscillator with extra parameters
    const char* tmp;
    if (*tmp == '~')
}

parseE(const char*& str, valueId)
{
    // >durvalue value        : 5 ;
    // >durvalue >durvalue    : >5,5 >5,5     : >5,5       :5>5;
    // >durvalue durvalue     : 5,5 5,5 ;     : >5,0>0,5   :5>0:0>5; or :5>:>5;
    // >durvalue dur>durvalue : 5>5,5 5>5,5 ; : >5,0 >5,5  :5>0:5>5;
    // typical slide          :'>2; slide half of notes duration from 0 to 2 (' is short for &/2)
    // weird slide            :>4:^*3+3>; jump to 4, then slide to zero with velocity*3+3 as duration
    // ADSR                   :a>1:d>s:x>s:r>;

    const char* tmp;
    float amp;
    if (!parseFloatOrFrac(tmp, amp))
        return false;

    if (*tmp == ':')
}
#endif
