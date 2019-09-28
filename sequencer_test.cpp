#include "sequencer.h"
#include "synthesizer.h"
#include <gtest/gtest.h>
#include <iostream>

using namespace std;
namespace {
class TestCtx : public Ctx {
public:
	TestCtx() : sum_(0) {}
	ptr<Value> note() const override { return ChainPool<Value>::instance().mk2<Const>(0.0); }
	ptr<Value> volume() const override { return ChainPool<Value>::instance().mk2<Const>(0.0); }
	float sampleRate() const override { return 1.0; }
	float& sum(Value*) override { return sum_; }
	float sum_;
};

class TestSynth : public SynthesizerIf
{
public:
	TestSynth()  {}
    void playNote(unsigned duration, float velocity, ptr<Value> note, ptr<Value> volume) override
	{
		duration_ = duration;
		velocity_ = velocity;
		note_ = note;
		volume_ = volume;
	}
	float duration_;
	float velocity_;
	ptr<Value> volume_;
	ptr<Value> note_;
};

TEST(Sequencer, basic)
{
	TestCtx ctx;
	ChainPool<Value>::Scope manager(20);
	ChainPool<Buffer<256>>::Scope bufferPool(10);
	TestSynth syn;
	Sequencer sequencer(syn, 240.0, 1000.0);
	sequencer.addTrack("/4%*.8cde=f'_'g_");
	EXPECT_TRUE(sequencer.parse());
	EXPECT_FLOAT_EQ(250, sequencer.timeToNext());
	EXPECT_FLOAT_EQ(200, syn.duration_);
	EXPECT_FLOAT_EQ(24.0, syn.note_->get(0,1,ctx)->buff[0]);

	EXPECT_TRUE(sequencer.parse());
	EXPECT_FLOAT_EQ(250, sequencer.timeToNext());
	EXPECT_FLOAT_EQ(200, syn.duration_);
	EXPECT_FLOAT_EQ(26.0, syn.note_->get(0,1,ctx)->buff[0]);

	EXPECT_TRUE(sequencer.parse());
	EXPECT_FLOAT_EQ(500, sequencer.timeToNext());
	EXPECT_FLOAT_EQ(400, syn.duration_);
	EXPECT_FLOAT_EQ(28.0, syn.note_->get(0,1,ctx)->buff[0]);

	EXPECT_TRUE(sequencer.parse());
	EXPECT_FLOAT_EQ(250, sequencer.timeToNext());
	EXPECT_FLOAT_EQ(100, syn.duration_);
	EXPECT_FLOAT_EQ(29.0, syn.note_->get(0,1,ctx)->buff[0]);

	EXPECT_TRUE(sequencer.parse());
	EXPECT_FLOAT_EQ(1000000, sequencer.timeToNext());
	EXPECT_FLOAT_EQ(200, syn.duration_);
	EXPECT_FLOAT_EQ(31.0, syn.note_->get(0,1,ctx)->buff[0]);
	EXPECT_TRUE(sequencer.ended());
}

TEST(Sequencer, macro)
{
	TestCtx ctx;
	ChainPool<Value>::Scope manager(20);
	ChainPool<Buffer<256>>::Scope bufferPool(10);
	TestSynth syn;
	Sequencer sequencer(syn, 240.0, 1000.0);
	sequencer.addTrack("/4Ax{cde}@@");
	EXPECT_TRUE(sequencer.parse());
	EXPECT_FLOAT_EQ(250, sequencer.timeToNext());
	EXPECT_FLOAT_EQ(200, syn.duration_);
	EXPECT_FLOAT_EQ(24.0, syn.note_->get(0,1,ctx)->buff[0]);

	EXPECT_TRUE(sequencer.parse());
	EXPECT_FLOAT_EQ(250, sequencer.timeToNext());
	EXPECT_FLOAT_EQ(200, syn.duration_);
	EXPECT_FLOAT_EQ(26.0, syn.note_->get(0,1,ctx)->buff[0]);

	EXPECT_TRUE(sequencer.parse());
	EXPECT_FLOAT_EQ(250, sequencer.timeToNext());
	EXPECT_FLOAT_EQ(200, syn.duration_);
	EXPECT_FLOAT_EQ(28.0, syn.note_->get(0,1,ctx)->buff[0]);

	EXPECT_TRUE(sequencer.parse());
	EXPECT_FLOAT_EQ(250, sequencer.timeToNext());
	EXPECT_FLOAT_EQ(200, syn.duration_);
	EXPECT_FLOAT_EQ(24.0, syn.note_->get(0,1,ctx)->buff[0]);

	EXPECT_TRUE(sequencer.parse());
	EXPECT_FLOAT_EQ(250, sequencer.timeToNext());
	EXPECT_FLOAT_EQ(200, syn.duration_);
	EXPECT_FLOAT_EQ(26.0, syn.note_->get(0,1,ctx)->buff[0]);

	EXPECT_TRUE(sequencer.parse());
	EXPECT_FLOAT_EQ(1000000, sequencer.timeToNext());
	EXPECT_FLOAT_EQ(200, syn.duration_);
	EXPECT_FLOAT_EQ(28.0, syn.note_->get(0,1,ctx)->buff[0]);
	EXPECT_TRUE(sequencer.ended());
}

TEST(Sequencer, lastHasDuration)
{
	TestCtx ctx;
	ChainPool<Value>::Scope manager(20);
	ChainPool<Buffer<256>>::Scope bufferPool(10);
	TestSynth syn;
	Sequencer sequencer(syn, 240.0, 1000.0);
	sequencer.addTrack("/2A{a''b'}@@");

	EXPECT_TRUE(sequencer.parse());
	EXPECT_FLOAT_EQ(33.0, syn.note_->get(0,1,ctx)->buff[0]); // 'a'
	EXPECT_FLOAT_EQ(125, sequencer.timeToNext());

	EXPECT_TRUE(sequencer.parse());
	EXPECT_FLOAT_EQ(35.0, syn.note_->get(0,1,ctx)->buff[0]); // 'b'
	EXPECT_FLOAT_EQ(250, sequencer.timeToNext());

	EXPECT_TRUE(sequencer.parse());
	EXPECT_FLOAT_EQ(33.0, syn.note_->get(0,1,ctx)->buff[0]); // 'a'
	EXPECT_FLOAT_EQ(125, sequencer.timeToNext());

	EXPECT_TRUE(sequencer.parse());
	EXPECT_FLOAT_EQ(35.0, syn.note_->get(0,1,ctx)->buff[0]); // 'b'
	EXPECT_FLOAT_EQ(1000000, sequencer.timeToNext());
	EXPECT_TRUE(sequencer.ended());
}

TEST(Sequencer, paranthesis0)
{
	TestCtx ctx;
	cout << "===============================================================" << endl;
	ChainPool<Value>::Scope manager(20);
	ChainPool<Buffer<256>>::Scope bufferPool(10);
	TestSynth syn;
	Sequencer sequencer(syn, 240.0, 1000.0);
	sequencer.addTrack("/4a2(_'b,cd,e)f");
	cout << "1--------------------------------------------------------------" << endl;
	EXPECT_TRUE(sequencer.parse());
	EXPECT_FLOAT_EQ(33.0, syn.note_->get(0,1,ctx)->buff[0]); // 'a2'
	EXPECT_FLOAT_EQ(250, sequencer.timeToNext());

	cout << "2--------------------------------------------------------------" << endl;
	EXPECT_TRUE(sequencer.parse());
	EXPECT_FLOAT_EQ(28.0, syn.note_->get(0,1,ctx)->buff[0]); // 'e'
	EXPECT_FLOAT_EQ(0  , sequencer.timeToNext());

	cout << "3--------------------------------------------------------------" << endl;
	EXPECT_TRUE(sequencer.parse());
	EXPECT_FLOAT_EQ(36.0, syn.note_->get(0,1,ctx)->buff[0]); // 'c'
	EXPECT_FLOAT_EQ(125, sequencer.timeToNext());
	cout << "4--------------------------------------------------------------" << endl;
	EXPECT_TRUE(sequencer.parse());
	EXPECT_FLOAT_EQ(35.0, syn.note_->get(0,1,ctx)->buff[0]); // 'b'
	EXPECT_FLOAT_EQ(125, sequencer.timeToNext());
	cout << "5--------------------------------------------------------------" << endl;
	EXPECT_TRUE(sequencer.parse());
	EXPECT_FLOAT_EQ(38.0, syn.note_->get(0,1,ctx)->buff[0]); // 'd'
	EXPECT_FLOAT_EQ(250, sequencer.timeToNext());
	cout << "6--------------------------------------------------------------" << endl;
	EXPECT_TRUE(sequencer.parse());
	EXPECT_FLOAT_EQ(29.0, syn.note_->get(0,1,ctx)->buff[0]); // 'f'
	EXPECT_FLOAT_EQ(1000000, sequencer.timeToNext());
	cout << "===============================================================" << endl;
	EXPECT_TRUE(sequencer.ended());
	EXPECT_FALSE(sequencer.parse());

}

TEST(Sequencer, paranthesis1)
{
	TestCtx ctx;
	cout << "===============================================================" << endl;
	ChainPool<Value>::Scope manager(20);
	ChainPool<Buffer<256>>::Scope bufferPool(10);
	TestSynth syn;
	Sequencer sequencer(syn, 240.0, 1000.0);
	sequencer.addTrack("/4(a,b,c)(d,e,f)");
	cout << "1--------------------------------------------------------------" << endl;
	EXPECT_TRUE(sequencer.parse());
	EXPECT_FLOAT_EQ(24.0, syn.note_->get(0,1,ctx)->buff[0]); // 'c'
	EXPECT_FLOAT_EQ(0  , sequencer.timeToNext());

	EXPECT_TRUE(sequencer.parse());
	EXPECT_FLOAT_EQ(35.0, syn.note_->get(0,1,ctx)->buff[0]); // 'b'
	EXPECT_FLOAT_EQ(0  , sequencer.timeToNext());

	EXPECT_TRUE(sequencer.parse());
	EXPECT_FLOAT_EQ(33.0, syn.note_->get(0,1,ctx)->buff[0]); // 'a'
	EXPECT_FLOAT_EQ(250, sequencer.timeToNext());

	EXPECT_TRUE(sequencer.parse());
	EXPECT_FLOAT_EQ(29.0, syn.note_->get(0,1,ctx)->buff[0]); // 'f'
	EXPECT_FLOAT_EQ(0  , sequencer.timeToNext());

	EXPECT_TRUE(sequencer.parse());
	EXPECT_FLOAT_EQ(28.0, syn.note_->get(0,1,ctx)->buff[0]); // 'e'
	EXPECT_FLOAT_EQ(0  , sequencer.timeToNext());

	EXPECT_TRUE(sequencer.parse());
	EXPECT_FLOAT_EQ(26.0, syn.note_->get(0,1,ctx)->buff[0]); // 'd'
	EXPECT_FLOAT_EQ(1000000, sequencer.timeToNext());

	EXPECT_TRUE(sequencer.ended());
	EXPECT_FALSE(sequencer.parse());
}
// 0
// [0;100[          buff [0;100[
// seq (0) -> 20    seq() -> 20
// syn [0;20[       syn[0;20[
// seq(20) -> 50    seq() -> 30
// syn [20; 50[     syn[20;50[
// seq(50) -> 110   seq() -> 60
// syn [50;100[     syn[50;100[
// PLAY [0; 100[    PLAY [0;100[
// [100;200[        buff [100;200[
// syn[100;110[     syn[0;10[
// seq(110) -> 140  seq() -> 30

}
