#include "value.h"
#include "chain_pool.h"
#include "buffer.h"
#include <math.h>

using namespace std;

const unsigned Value::MaxSize = sizeof(Adder);

Oscillator::Oscillator(float freq, float amp) : freq_(freq), amp_(amp)
{
}

ptr<Buffer<256>> Oscillator::get(unsigned sampleNr, unsigned len, Ctx& ctx)
{
	ptr<Buffer<256>> buff = ChainPool<Buffer<256>>::instance().mk(len);
	float hz = freq_;
	for(unsigned i = 0; i < buff->size_; ++i) {
		double time = (double)(sampleNr+i) / (double)ctx.sampleRate();
		buff->buff[i]  = amp_ * sin(2.0f * M_PI * hz * time);
	}
	return buff;
}

ptr<Buffer<256>> Envelope::get(unsigned sampleNr, unsigned len, Ctx& ctx)
{
	ptr<Buffer<256>> buff = ChainPool<Buffer<256>>::instance().mk(len);
	float& sum = ctx.sum(this);
	for(unsigned i = 0; i < buff->size(); ++i) {
		double time = (double)(sampleNr+i) / (double)ctx.sampleRate();
		if (time < segments_[0].first)
			buff->buff[i] = time/segments_[0].first*segments_[0].second;
		else if (time < segments_[1].first + segments_[0].first)
			buff->buff[i] = (time-segments_[0].first)/segments_[1].first*(segments_[1].second - segments_[0].second) + segments_[0].second;
		else if (time < segments_[2].first + segments_[1].first + segments_[0].first)
			buff->buff[i] = (time-segments_[0].first-segments_[1].first)/segments_[2].first*(segments_[2].second - segments_[1].second) + segments_[1].second;
	}
	return buff;
}

void Envelope::addSegment(float duration, float value)
{
	segments_.push_back(pair<float, float>(duration, value));
}

ptr<Buffer<256>> Const::get(unsigned sampleNr, unsigned len, Ctx& ctx)
{
	ptr<Buffer<256>> buff = ChainPool<Buffer<256>>::instance().mk(len);
	for (unsigned i = 0; i < buff->size(); ++i)
		buff->buff[i] = value_;
	return buff;
}

ptr<Buffer<256>> Adder::get(unsigned sampleNr, unsigned len, Ctx& ctx)
{
	ptr<Buffer<256>> buff = ChainPool<Buffer<256>>::instance().mk(len);
	ptr<Buffer<256>> l = lhs_->get(sampleNr, len, ctx);
	ptr<Buffer<256>> r = rhs_->get(sampleNr, len, ctx);
	for (unsigned i = 0; i < buff->size(); ++i) {
		buff->buff[i] = l->buff[i] + r->buff[i];
	}
	return buff;
}
