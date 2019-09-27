#include "value.h"
#include <math.h>

using namespace std;

Oscillator::Oscillator(float freq, float amp) : freq_(freq), amp_(amp)
{
}

ptr<Buffer<256>> Oscillator::get(unsigned sampleNr, unsigned len, const Ctx& ctx)
{
	ptr<Buffer<256>> buff = ChainPool<Buffer<256>>::instance().mk(len);
	float hz = freq_;//pow(1.059463094, ctx.note())*65.40639133; // frequency of c0 is 65.40639133
	for(unsigned i = 0; i < buff->size_; ++i) {
		double time = (double)(sampleNr+i) / (double)ctx.sampleRate();
		buff->buff[i]  = amp_ * sin(2.0f * M_PI * hz * time);
	}
	return buff;
}

ptr<Buffer<256>> Envelope::get(unsigned sampleNr, unsigned len, const Ctx& ctx)
{
	ptr<Buffer<256>> buff = ChainPool<Buffer<256>>::instance().mk(len);
	buff->buff[0] = 92.0;
	return buff;
}

ptr<Buffer<256>> Const::get(unsigned sampleNr, unsigned len, const Ctx& ctx)
{
	ptr<Buffer<256>> buff = ChainPool<Buffer<256>>::instance().mk(len);
	buff->buff[0] = value_;
	return buff;
}
