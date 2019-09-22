#include "sequencer.h"
#include "synthesizer.h"
#include <gtest/gtest.h>
#include <iostream>

using namespace std;

class TestSynth : public SynthesizerIf
{
public:
	TestSynth(Manager<Value, sizeof(Envelope)>& valueManager) : valueManager_(valueManager) {}
    virtual void playNote(float duration, float velocity, float note, float volume, unsigned noteValueId, unsigned volumeValueId)
	{
		duration_ = duration;
		velocity_ = velocity;
		note_ = note;
		volume_ = volume;
	}
	void generate(int16_t* begin, int16_t* end) override {}
	Manager<Value, sizeof(Envelope)>& valueManager_;
	float duration_;
	float velocity_;
	float volume_;
	float note_;
};

TEST(Sequencer, basic)
{
	Manager<Value, sizeof(Envelope)> manager;
	TestSynth syn(manager);
	Sequencer sequencer(manager, syn);
	sequencer.addTrack("/4%*.8cde=f'_'g_");
	EXPECT_TRUE(sequencer.parse());
	EXPECT_FLOAT_EQ(.250, sequencer.timeToNext());
	EXPECT_FLOAT_EQ(.200, syn.duration_);
	EXPECT_FLOAT_EQ(24.0, syn.note_);

	EXPECT_TRUE(sequencer.parse());
	EXPECT_FLOAT_EQ(.250, sequencer.timeToNext());
	EXPECT_FLOAT_EQ(.200, syn.duration_);
	EXPECT_FLOAT_EQ(26.0, syn.note_);

	EXPECT_TRUE(sequencer.parse());
	EXPECT_FLOAT_EQ(.500, sequencer.timeToNext());
	EXPECT_FLOAT_EQ(.400, syn.duration_);
	EXPECT_FLOAT_EQ(28.0, syn.note_);

	EXPECT_TRUE(sequencer.parse());
	EXPECT_FLOAT_EQ(.250, sequencer.timeToNext());
	EXPECT_FLOAT_EQ(.100, syn.duration_);
	EXPECT_FLOAT_EQ(29.0, syn.note_);

	EXPECT_TRUE(sequencer.parse());
	EXPECT_FLOAT_EQ(1000, sequencer.timeToNext());
	EXPECT_FLOAT_EQ(.200, syn.duration_);
	EXPECT_FLOAT_EQ(31.0, syn.note_);
	EXPECT_TRUE(sequencer.ended());
}

TEST(Sequencer, macro)
{
	Manager<Value, sizeof(Envelope)> manager;
	TestSynth syn(manager);
	Sequencer sequencer(manager, syn);
	sequencer.addTrack("/4Ax{cde}@@");
	EXPECT_TRUE(sequencer.parse());
	EXPECT_FLOAT_EQ(.250, sequencer.timeToNext());
	EXPECT_FLOAT_EQ(.200, syn.duration_);
	EXPECT_FLOAT_EQ(24.0, syn.note_);

	EXPECT_TRUE(sequencer.parse());
	EXPECT_FLOAT_EQ(.250, sequencer.timeToNext());
	EXPECT_FLOAT_EQ(.200, syn.duration_);
	EXPECT_FLOAT_EQ(26.0, syn.note_);

	EXPECT_TRUE(sequencer.parse());
	EXPECT_FLOAT_EQ(.250, sequencer.timeToNext());
	EXPECT_FLOAT_EQ(.200, syn.duration_);
	EXPECT_FLOAT_EQ(28.0, syn.note_);

	EXPECT_TRUE(sequencer.parse());
	EXPECT_FLOAT_EQ(.250, sequencer.timeToNext());
	EXPECT_FLOAT_EQ(.200, syn.duration_);
	EXPECT_FLOAT_EQ(24.0, syn.note_);

	EXPECT_TRUE(sequencer.parse());
	EXPECT_FLOAT_EQ(.250, sequencer.timeToNext());
	EXPECT_FLOAT_EQ(.200, syn.duration_);
	EXPECT_FLOAT_EQ(26.0, syn.note_);

	EXPECT_TRUE(sequencer.parse());
	EXPECT_FLOAT_EQ(1000, sequencer.timeToNext());
	EXPECT_FLOAT_EQ(.200, syn.duration_);
	EXPECT_FLOAT_EQ(28.0, syn.note_);
	EXPECT_TRUE(sequencer.ended());
}

TEST(Sequencer, lastHasDuration)
{
	Manager<Value, sizeof(Envelope)> manager;
	TestSynth syn(manager);
	Sequencer sequencer(manager, syn);
	sequencer.addTrack("/2A{a''b'}@@");

	EXPECT_TRUE(sequencer.parse());
	EXPECT_FLOAT_EQ(33.0, syn.note_); // 'a'
	EXPECT_FLOAT_EQ(.125, sequencer.timeToNext());

	EXPECT_TRUE(sequencer.parse());
	EXPECT_FLOAT_EQ(35.0, syn.note_); // 'b'
	EXPECT_FLOAT_EQ(.250, sequencer.timeToNext());

	EXPECT_TRUE(sequencer.parse());
	EXPECT_FLOAT_EQ(33.0, syn.note_); // 'a'
	EXPECT_FLOAT_EQ(.125, sequencer.timeToNext());

	EXPECT_TRUE(sequencer.parse());
	EXPECT_FLOAT_EQ(35.0, syn.note_); // 'b'
	EXPECT_FLOAT_EQ(1000, sequencer.timeToNext());
	EXPECT_TRUE(sequencer.ended());
}

TEST(Sequencer, paranthesis0)
{
	cout << "===============================================================" << endl;
	Manager<Value, sizeof(Envelope)> manager;
	TestSynth syn(manager);
	Sequencer sequencer(manager, syn);
	sequencer.addTrack("/4a2(_'b,cd,e)f");
	cout << "1--------------------------------------------------------------" << endl;
	EXPECT_TRUE(sequencer.parse());
	EXPECT_FLOAT_EQ(33.0, syn.note_); // 'a2'
	EXPECT_FLOAT_EQ(.250, sequencer.timeToNext());

	cout << "2--------------------------------------------------------------" << endl;
	EXPECT_TRUE(sequencer.parse());
	EXPECT_FLOAT_EQ(28.0, syn.note_); // 'e'
	EXPECT_FLOAT_EQ(.0  , sequencer.timeToNext());

	cout << "3--------------------------------------------------------------" << endl;
	EXPECT_TRUE(sequencer.parse());
	EXPECT_FLOAT_EQ(36.0, syn.note_); // 'c'
	EXPECT_FLOAT_EQ(.125, sequencer.timeToNext());
	cout << "4--------------------------------------------------------------" << endl;
	EXPECT_TRUE(sequencer.parse());
	EXPECT_FLOAT_EQ(35.0, syn.note_); // 'b'
	EXPECT_FLOAT_EQ(.125, sequencer.timeToNext());
	cout << "5--------------------------------------------------------------" << endl;
	EXPECT_TRUE(sequencer.parse());
	EXPECT_FLOAT_EQ(38.0, syn.note_); // 'd'
	EXPECT_FLOAT_EQ(.250, sequencer.timeToNext());
	cout << "6--------------------------------------------------------------" << endl;
	EXPECT_TRUE(sequencer.parse());
	EXPECT_FLOAT_EQ(29.0, syn.note_); // 'f'
	EXPECT_FLOAT_EQ(1000, sequencer.timeToNext());
	cout << "===============================================================" << endl;
	EXPECT_TRUE(sequencer.ended());
	EXPECT_FALSE(sequencer.parse());

}

TEST(Sequencer, paranthesis1)
{
	cout << "===============================================================" << endl;
	Manager<Value, sizeof(Envelope)> manager;
	TestSynth syn(manager);
	Sequencer sequencer(manager, syn);
	sequencer.addTrack("/4(a,b,c)(d,e,f)");
	cout << "1--------------------------------------------------------------" << endl;
	EXPECT_TRUE(sequencer.parse());
	EXPECT_FLOAT_EQ(24.0, syn.note_); // 'c'
	EXPECT_FLOAT_EQ(.0  , sequencer.timeToNext());

	EXPECT_TRUE(sequencer.parse());
	EXPECT_FLOAT_EQ(35.0, syn.note_); // 'b'
	EXPECT_FLOAT_EQ(.0  , sequencer.timeToNext());

	EXPECT_TRUE(sequencer.parse());
	EXPECT_FLOAT_EQ(33.0, syn.note_); // 'a'
	EXPECT_FLOAT_EQ(.250, sequencer.timeToNext());

	EXPECT_TRUE(sequencer.parse());
	EXPECT_FLOAT_EQ(29.0, syn.note_); // 'f'
	EXPECT_FLOAT_EQ(.0  , sequencer.timeToNext());

	EXPECT_TRUE(sequencer.parse());
	EXPECT_FLOAT_EQ(28.0, syn.note_); // 'e'
	EXPECT_FLOAT_EQ(.0  , sequencer.timeToNext());

	EXPECT_TRUE(sequencer.parse());
	EXPECT_FLOAT_EQ(26.0, syn.note_); // 'd'
	EXPECT_FLOAT_EQ(1000, sequencer.timeToNext());

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

