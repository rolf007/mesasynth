#include "synthesizer.h"
#include <math.h>
#include <iostream>
#include <string.h>

using namespace std;

Synthesizer::Synthesizer() : sample_nr(0), notePool_(10), instrument0_(ChainPool<Value>::instance().mk2<Piano>())
{
	noteNum_ = 0;
}

void Synthesizer::playNote(unsigned duration, float velocity, float note, float volume, ptr<Value> noteValuePtr, ptr<Value> volumeValuePtr)
{
	cout << "======================== play note " << note << ", duration = " << duration << endl;
	ptr<Note> newNote = ChainPool<Note>::instance().mk(note, duration, sample_nr, instrument0_);
	notes__[noteNum_++] = newNote;
	noteNum_ %= 8;
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
			ptr<Buffer<256>> buf;
			buf = note->get(sample_nr, len);
			for(unsigned i = 0; i < buf->size_; ++i) {
				p[i] += buf->buff[i]/2;
			}
		}
		sample_nr += len;
		p += len;
	}
}

ptr<Buffer<256>> Piano::get(unsigned sampleNr, unsigned len, const Ctx& ctx)
{
	ptr<Buffer<256>> buff = ChainPool<Buffer<256>>::instance().mk(len);
	float hz = pow(1.059463094, ctx.note())*65.40639133; // frequency of c0 is 65.40639133
	for(unsigned i = 0; i < buff->size_; ++i) {
		double time = (double)(sampleNr+i) / (double)ctx.sampleRate();
		int AMPLITUDE = 3600-(sampleNr+i)/5;
		if (AMPLITUDE <0) AMPLITUDE = 0;
		buff->buff[i]  = (int16_t)(AMPLITUDE * sin(2.0f * M_PI * hz * time*1)*1.00);
		buff->buff[i] += (int16_t)(AMPLITUDE * sin(2.0f * M_PI * hz * time*2.01)*1.31/2);
		buff->buff[i] += (int16_t)(AMPLITUDE * sin(2.0f * M_PI * hz * time*3.01)*2/3);
		buff->buff[i] += (int16_t)(AMPLITUDE * sin(2.0f * M_PI * hz * time*4.01)*2.31/4);
		buff->buff[i] += (int16_t)(AMPLITUDE * sin(2.0f * M_PI * hz * time*5.01)*0.31/5);
	}
	return buff;
}

Synthesizer::Note::Note(float note, unsigned duration, unsigned start_nr, ptr<Value> instrument) : note_(note), duration_(duration), start_nr_(start_nr), instrument_(instrument)
{
}

Synthesizer::Note::~Note()
{
	cout << "note really terminated" << endl;
}


ptr<Buffer<256>> Synthesizer::Note::get(unsigned sample_nr, unsigned len)
{
	return instrument_->get(sample_nr-start_nr_, len, *this);
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
