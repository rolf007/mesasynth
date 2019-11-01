#include "value.h"
#include "chain_pool.h"
#include "buffer.h"
#include <math.h>

using namespace std;

const unsigned Value::MaxSize = sizeof(Adder);

ValueInstance::ValueInstance(ptr<Value> value) : value_(value), data_(value_ ? ChainPool<DataBuffer>::instance().mk(32) : nullptr)
{
}

ptr<Oscillator> Oscillator::mk(ptr<Value> freq, float amp)
{
	return ChainPool<Value>::instance().mk2<Oscillator>(freq, amp);
}

Oscillator::Oscillator(ptr<Value> freq, float amp) : freq_(freq), amp_(amp)
{
}

ptr<AudioBuffer> Oscillator::get(unsigned sampleNr, unsigned len, Ctx& ctx, ptr<DataBuffer> data)
{
	ptr<AudioBuffer> buff = ChainPool<AudioBuffer>::instance().mk(len);
	ptr<AudioBuffer> freq = freq_->get(sampleNr, len, ctx, data);
	float hz = freq->buff()[0];
	for(unsigned i = 0; i < buff->size_; ++i) {
		double time = (double)(sampleNr+i) / (double)ctx.sampleRate();
		buff->buff()[i]  = amp_ * sin(2.0f * M_PI * hz * time);
	}
	return buff;
}

ptr<AudioBuffer> Envelope::get(unsigned sampleNr, unsigned len, Ctx& ctx, ptr<DataBuffer> data)
{
	ptr<AudioBuffer> buff = ChainPool<AudioBuffer>::instance().mk(len);
	float oldVal = 0.0;
	float val= 0.0;
	float dt = 0.0;
	unsigned j = 0;
	float duration = 0.0;
	bool ended = false;
	for(unsigned i = 0; i < buff->size(); ++i) {
		double time = (double)(sampleNr+i) / (double)ctx.sampleRate();
		while (!ended && time-dt >= duration) {
			if (j == segments_.size()) {
				duration = 1;
				ended = true;
				oldVal = val;
			} else {
				dt += duration;
				duration = segments_[j].first;
				oldVal = val;
				val = segments_[j].second;
			}
			++j;
		}
		buff->buff()[i] = (time-dt)/duration*(val - oldVal) + oldVal;
	}
	return buff;
}

void Envelope::addSegment(float duration, float value)
{
	segments_.push_back(pair<float, float>(duration, value));
}

ptr<AudioBuffer> Const::get(unsigned sampleNr, unsigned len, Ctx& ctx, ptr<DataBuffer> data)
{
	ptr<AudioBuffer> buff = ChainPool<AudioBuffer>::instance().mk(len);
	for (unsigned i = 0; i < buff->size(); ++i)
		buff->buff()[i] = value_;
	return buff;
}

ptr<Const> Const::mk(float value)
{
	return ChainPool<Value>::instance().mk2<Const>(value);
}

ptr<AudioBuffer> Adder::get(unsigned sampleNr, unsigned len, Ctx& ctx, ptr<DataBuffer> data)
{
	ptr<AudioBuffer> buff = ChainPool<AudioBuffer>::instance().mk(len);
	ptr<AudioBuffer> l = lhs_->get(sampleNr, len, ctx, data);
	ptr<AudioBuffer> r = rhs_->get(sampleNr, len, ctx, data);
	for (unsigned i = 0; i < buff->size(); ++i) {
		buff->buff()[i] = l->buff()[i] + r->buff()[i];
	}
	return buff;
}
