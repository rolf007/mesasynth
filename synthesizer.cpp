#include "synthesizer.h"
#include <math.h>
#include <iostream>
#include <string.h>

using namespace std;

Synthesizer::Synthesizer(int sampleRate) : sampleRate_(sampleRate), sample_nr(0)
{
	hz_ = 440.0;
	samples_ = 0;
}

void Synthesizer::playNote(float duration, float velocity, float note, float volume, unsigned noteValueId, unsigned volumeValueId)
{
	cout << "======================== play note " << note << ", duration = " << duration << endl;
	hz_ = pow(1.059463094, note)*65.40639133; // frequency of c0 is 65.40639133
	notes_.push_back(Note(note, duration));
	cout << hz_ << endl;
}

void Synthesizer::generate(int16_t* begin, int16_t* end)
{
	memset(begin, 0, end-begin);
	int16_t buff[8192];
	for (Note& note : notes_) {
		note.generate(buff, buff+(end-begin));
		for(int16_t* p = begin; p != end; ++p) {
			*p += buff[p-begin]/4;
		}
	}
}

Synthesizer::Note::Note(float note, float duration) : note_(note), duration_(duration)
{
	sample_nr = 0;
}

void Synthesizer::Note::generate(int16_t* begin, int16_t* end)
{
	float hz_ = pow(1.059463094, note_)*65.40639133; // frequency of c0 is 65.40639133
	float sampleRate_ = 44100;
	for(int16_t* p = begin; p != end; ++p, ++sample_nr) {
		double time = (double)sample_nr / (double)sampleRate_;
		int AMPLITUDE = 3600-sample_nr/5;
		if (AMPLITUDE <0) AMPLITUDE = 0;
		*p  = (int16_t)(AMPLITUDE * sin(2.0f * M_PI * hz_ * time*1)*1.00);
		*p += (int16_t)(AMPLITUDE * sin(2.0f * M_PI * hz_ * time*2.01)*1.31/2);
		*p += (int16_t)(AMPLITUDE * sin(2.0f * M_PI * hz_ * time*3.01)*2/3);
		*p += (int16_t)(AMPLITUDE * sin(2.0f * M_PI * hz_ * time*4.01)*2.31/4);
		*p += (int16_t)(AMPLITUDE * sin(2.0f * M_PI * hz_ * time*5.01)*0.31/5);
		//*p += (int16_t)(AMPLITUDE * sin(2.0f * M_PI * (hz_+1.005) * time));
	}
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
