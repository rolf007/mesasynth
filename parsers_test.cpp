#include "parsers.h"
#include "buffer.h"
#include "value.h"
#include "macro.h"
#include <gtest/gtest.h>

using namespace std;
using namespace testing;
namespace {
 
#define CHECK(x) { SCOPED_TRACE(""); x; }

AssertionResult assertValue(const string expStr, const string gotStr, const ptr<Value>& exp, const ptr<Value>& got) {
	if (ptr<Oscillator> e = exp.cast<Oscillator>()) {
		if (ptr<Oscillator> g = got.cast<Oscillator>()) {
			AssertionResult freqEq = assertValue(expStr+"<Oscillator>.freq", gotStr+"<Oscillator>.freq", e->freq(), g->freq());
			if (!freqEq)
				return freqEq;
			return AssertionSuccess();
		}
	} else if (ptr<Envelope> e = exp.cast<Envelope>()) {
		if (ptr<Envelope> g = got.cast<Envelope>()) {
			return AssertionSuccess();
		}
	} else if (ptr<Adder> e = exp.cast<Adder>()) {
		if (ptr<Adder> g = got.cast<Adder>()) {
			AssertionResult lhsEq = assertValue(expStr+"<Adder>.lhs", gotStr+"<Adder>.lhs", e->lhs(), g->lhs());
			if (!lhsEq)
				return lhsEq;
			AssertionResult rhsEq = assertValue(expStr+"<Adder>.rhs", gotStr+"<Adder>.rhs", e->rhs(), g->rhs());
			if (!rhsEq)
				return rhsEq;
			return AssertionSuccess();
		}
	} else if (ptr<Const> e = exp.cast<Const>()) {
		if (ptr<Const> g = got.cast<Const>()) {
			if (e->value() == g->value()) // <-- this should be float-compare
				return AssertionSuccess();
			else {
				return AssertionFailure() << expStr+"<Const>.value" << " != " << gotStr+"<Const>.value" << ":" << e->value() << " != " << g->value();
			}
		}
	}

	return AssertionFailure() << expStr << " and " << gotStr << " are not the same type";
}

#define EXPECT_VALUE_EQ(exp, got) { EXPECT_PRED_FORMAT2(assertValue, exp, got); }

void testParserInt(const char* str, int exp, char end=0)
{
	const char* tmp = str;
	int got;
	EXPECT_TRUE(Parsers<const char*>::parseInt(tmp, got));
	EXPECT_EQ(end, *tmp);
	EXPECT_EQ(exp, got);
}

void testParserIntFail(const char* str)
{
	const char* tmp = str;
	int got;
	EXPECT_FALSE(Parsers<const char*>::parseInt(tmp, got));
	EXPECT_EQ(str, tmp);
}

void testParserFloat(const char* str, float exp, char end=0)
{
	const char* tmp = str;
	float got;
	EXPECT_TRUE(Parsers<const char*>::parseFloat(tmp, got));
	EXPECT_EQ(end, *tmp);
	EXPECT_FLOAT_EQ(exp, got);
}

void testParserFloatFail(const char* str)
{
	const char* tmp = str;
	float got;
	EXPECT_FALSE(Parsers<const char*>::parseFloat(tmp, got));
	EXPECT_EQ(str, tmp);
}

void testParserFloatOrFraction(const char* str, float exp, char end=0)
{
	const char* tmp = str;
	float got;
	EXPECT_TRUE(Parsers<const char*>::parseFloatOrFraction(tmp, got));
	EXPECT_EQ(end, *tmp);
	EXPECT_FLOAT_EQ(exp, got);
}

void testParserFloatOrFractionFail(const char* str)
{
	const char* tmp = str;
	float got;
	EXPECT_FALSE(Parsers<const char*>::parseFloatOrFraction(tmp, got));
	EXPECT_EQ(str, tmp);
}

void testParseDuration(const char* str, Parsers<const char*>::Modifier exp_modifier, float expValue, char end=0)
{
	const char* tmp = str;
	Parsers<const char*>::Modifier modifier;
	float value;
	EXPECT_TRUE(Parsers<const char*>::parseDuration(tmp, modifier, value));
	EXPECT_EQ(end, *tmp);
	EXPECT_EQ(exp_modifier, modifier);
	EXPECT_FLOAT_EQ(expValue, value);
}

void testParseDurationFail(const char* str)
{
	const char* tmp = str;
	Parsers<const char*>::Modifier modifier;
	float value;
	EXPECT_FALSE(Parsers<const char*>::parseDuration(tmp, modifier, value));
	EXPECT_EQ(str, tmp);
}

void testParserNote(const char* str, int oldNote, float expNote, char end=0)
{
	const char* tmp = str;
	float note;
	EXPECT_TRUE(Parsers<const char*>::parseNote(tmp, oldNote, note));
	EXPECT_EQ(end, *tmp);
	EXPECT_FLOAT_EQ(expNote, note);
}

void testParserNoteFail(const char* str, int oldNote)
{
	const char* tmp = str;
	float note;
	EXPECT_FALSE(Parsers<const char*>::parseNote(tmp, oldNote, note));
	EXPECT_EQ(str, tmp);
}
void testParserDefaultProperties(const char* str, Parsers<const char*>::Property expProperty, Parsers<const char*>::Modifier expModifier, float expValue, char end=0)
{
	const char* tmp = str;
	Parsers<const char*>::Property property;
	Parsers<const char*>::Modifier modifier;
	float value;
	EXPECT_TRUE(Parsers<const char*>::parseDefaultProperty(tmp, property, modifier, value));
	EXPECT_EQ(end, *tmp);
	EXPECT_EQ(expProperty, property);
	EXPECT_EQ(expModifier, modifier);
	EXPECT_FLOAT_EQ(expValue, value);
}

void testParserDefaultPropertiesFail(const char* str)
{
	const char* tmp = str;
	Parsers<const char*>::Property property;
	Parsers<const char*>::Modifier modifier;
	float value;
	EXPECT_FALSE(Parsers<const char*>::parseDefaultProperty(tmp, property, modifier, value));
	EXPECT_EQ(str, tmp);
}

class TestCtx : public Ctx {
	ValueInstance note() const override { return ValueInstance(ChainPool<Value>::instance().mk2<Const>(0.0)); }
	ValueInstance volume() const override { return ValueInstance(ChainPool<Value>::instance().mk2<Const>(0.0)); }
	float sampleRate() const override { return 1.0; }
};

void testParserNoteProperties(const char* str, Parsers<const char*>::Property expProperty, Parsers<const char*>::Modifier expModifier, float expValue, char end=0)
{
	TestCtx ctx;
	const char* tmp = str;
	Parsers<const char*>::Property property;
	Parsers<const char*>::Modifier modifier;
	ptr<Value> valuePtr;
	float value;
	EXPECT_TRUE(Parsers<const char*>::parseNoteProperties(tmp, property, modifier, valuePtr, value));
	EXPECT_EQ(end, *tmp);
	EXPECT_EQ(expProperty, property);
	EXPECT_EQ(expModifier, modifier);
	if (!valuePtr)
		EXPECT_FLOAT_EQ(expValue, value);
	else
		EXPECT_FLOAT_EQ(expValue, valuePtr->get(0,1,ctx,nullptr)->buff()[0]);
}

void testParserNoteProperties2(const char* str, Parsers<const char*>::Property expProperty, Parsers<const char*>::Modifier expModifier, ptr<Value> expValue, char end=0)
{
	TestCtx ctx;
	const char* tmp = str;
	Parsers<const char*>::Property property;
	Parsers<const char*>::Modifier modifier;
	ptr<Value> valuePtr;
	float value;
	EXPECT_TRUE(Parsers<const char*>::parseNoteProperties(tmp, property, modifier, valuePtr, value));
	EXPECT_EQ(end, *tmp);
	EXPECT_EQ(expProperty, property);
	EXPECT_EQ(expModifier, modifier);
	if (!valuePtr)
		cout << "ERrror" << endl;
	else {
		EXPECT_VALUE_EQ(expValue, valuePtr);
	}
}

void testParserNotePropertiesFail(const char* str)
{
	const char* tmp = str;
	Parsers<const char*>::Property property;
	Parsers<const char*>::Modifier modifier;
	ptr<Value> valuePtr;
	float value;
	EXPECT_FALSE(Parsers<const char*>::parseNoteProperties(tmp, property, modifier, valuePtr, value));
	EXPECT_EQ(str, tmp);
}

TEST(Parse, Int)
{
	CHECK(testParserInt("12", 12));
	CHECK(testParserInt("-34", -34));
	CHECK(testParserInt("0", 0));
	CHECK(testParserIntFail("."));
}

TEST(Parse, Float)
{
	CHECK(testParserFloat("12.34", 12.34));
	CHECK(testParserFloat(".34", 0.34));
	CHECK(testParserFloat("12.", 12.0));
	CHECK(testParserFloat("12/34", 12.0, '/'));
	CHECK(testParserFloatFail("."));
}

TEST(Parse, FloatOrFraction)
{
	CHECK(testParserFloatOrFraction("12.34", 12.34));
	CHECK(testParserFloatOrFraction("2/4", 0.5));
	CHECK(testParserFloatOrFraction("/4", 0.25));
	CHECK(testParserFloatOrFractionFail("4/."));
	CHECK(testParserFloatOrFractionFail("./7"));
	CHECK(testParserFloatOrFractionFail("."));
}


TEST(Parse, Duration)
{
	CHECK(testParseDuration("=", ParserEnums::Mul, 2.0));
	CHECK(testParseDuration("==", ParserEnums::Mul, 3.0));
	CHECK(testParseDuration(".", ParserEnums::Mul, 1.5));
	CHECK(testParseDuration("..", ParserEnums::Mul, 1.75));
	CHECK(testParseDuration("'", ParserEnums::Mul, 0.5));
	CHECK(testParseDuration("''", ParserEnums::Mul, 0.25));
	CHECK(testParseDuration("`", ParserEnums::Mul, 2.0/3.0));
	CHECK(testParseDuration("'`", ParserEnums::Mul, 1.0/3.0));
	CHECK(testParseDuration("*1", ParserEnums::Mul, 1.0));
	CHECK(testParseDuration("*2.1", ParserEnums::Mul, 2.1));
	CHECK(testParseDuration("$*2.1", ParserEnums::Mul, 2.1));
	CHECK(testParseDuration("*/4", ParserEnums::Mul, 0.25));
	CHECK(testParseDuration("$*/4", ParserEnums::Mul, 0.25));
	CHECK(testParseDuration("$+/4", ParserEnums::Add, 0.25));
	CHECK(testParseDuration("$+3/4", ParserEnums::Add, 0.75));
	CHECK(testParseDuration("$+0", ParserEnums::Add, 0.0));
	CHECK(testParseDuration("/4", ParserEnums::Set, 0.25));
	CHECK(testParseDuration("/1", ParserEnums::Set, 1.0));
	CHECK(testParseDuration("$3/4", ParserEnums::Set, 0.75));
	CHECK(testParseDuration("$/4", ParserEnums::Set, 0.25));
	CHECK(testParseDuration("$4", ParserEnums::Set, 4.0));
	CHECK(testParseDurationFail("x=="));
	CHECK(testParseDurationFail("+=="));
}

TEST(Parse, Note)
{
	CHECK(testParserNote("c", 48, 48));
	CHECK(testParserNote("c#", 48, 49));
	CHECK(testParserNote("d", 48, 50));
	CHECK(testParserNote("d#", 48, 51));
	CHECK(testParserNote("e", 48, 52));
	CHECK(testParserNote("f", 48, 53));
	CHECK(testParserNote("f#", 48, 42));
	CHECK(testParserNote("g", 48, 43));
	CHECK(testParserNote("g#", 48, 44));
	CHECK(testParserNote("a", 48, 45));
	CHECK(testParserNote("a#", 48, 46));
	CHECK(testParserNote("b", 48, 47));
	CHECK(testParserNote("c", 48, 48));
	CHECK(testParserNote("c1", 48, 12));
	CHECK(testParserNote("c#1", 48, 13));
	CHECK(testParserNote("c-1", 48, -12));
	CHECK(testParserNote("c#-1", 48, -11));
	CHECK(testParserNote("c.37#1", 48, 12.37));
	CHECK(testParserNote("c.74#1", 48, 12.74));
	CHECK(testParserNote("c.74#", 48, 48.74));
	CHECK(testParserNote("b.74#", 48, 47.74));
	CHECK(testParserNote("b<", 48, 46));
	CHECK(testParserNote("b.1<", 48, 46.9));
	CHECK(testParserNoteFail("i", 48));
}

TEST(Parse, DefaultProperties)
{
	CHECK(testParserDefaultProperties("%.8", ParserEnums::Legato, ParserEnums::Set, .8));
	CHECK(testParserDefaultProperties("^*4/5", ParserEnums::Velocity, ParserEnums::Mul, .8));
	CHECK(testParserDefaultProperties("&+0.4/0.5", ParserEnums::Volume, ParserEnums::Add, .8));
}

TEST(Parse, NoteProperties)
{
	ChainPool<Value>::Scope scopeValue(10);
	ChainPool<AudioBuffer>::Scope scopeBuffer(10);
	float osc = 0.0;
	float env = 92.0;
	CHECK(testParserNoteProperties("%.8", ParserEnums::Legato, ParserEnums::Set, .8));
	CHECK(testParserNoteProperties("^*4/5", ParserEnums::Velocity, ParserEnums::Mul, .8));
	CHECK(testParserNoteProperties("&+0.4/0.5", ParserEnums::Volume, ParserEnums::Add, .8));
	CHECK(testParserNoteProperties("?0.5", ParserEnums::Note, ParserEnums::Set, .5));
	CHECK(testParserNoteProperties("?*2/5", ParserEnums::Note, ParserEnums::Mul, .4));
	CHECK(testParserNoteProperties("?+5.", ParserEnums::Note, ParserEnums::Add, 5.0));
	CHECK(testParserNotePropertiesFail("%:5"));
	CHECK(testParserNotePropertiesFail("^:5"));
	CHECK(testParserNotePropertiesFail("%~5"));
	CHECK(testParserNotePropertiesFail("^~5"));
	CHECK(testParserNoteProperties("&:5", ParserEnums::Volume, ParserEnums::Set, env, '5'));
	CHECK(testParserNoteProperties("?:5", ParserEnums::Note, ParserEnums::Set, env, '5'));
	CHECK(testParserNoteProperties("&+:5", ParserEnums::Volume, ParserEnums::Add, env, '5'));
	CHECK(testParserNoteProperties("?+:5", ParserEnums::Note, ParserEnums::Add, env, '5'));
	CHECK(testParserNoteProperties("&*:5", ParserEnums::Volume, ParserEnums::Mul, env, '5'));
	CHECK(testParserNoteProperties("?*:5", ParserEnums::Note, ParserEnums::Mul, env, '5'));
	CHECK(testParserNoteProperties("&~5", ParserEnums::Volume, ParserEnums::Set, osc, '5'));
	CHECK(testParserNoteProperties("?~5", ParserEnums::Note, ParserEnums::Set, osc, '5'));
	CHECK(testParserNoteProperties("&+~5", ParserEnums::Volume, ParserEnums::Add, osc, '5'));
	CHECK(testParserNoteProperties("?+~5", ParserEnums::Note, ParserEnums::Add, osc, '5'));
	CHECK(testParserNoteProperties("&*~5", ParserEnums::Volume, ParserEnums::Mul, osc, '5'));
	CHECK(testParserNoteProperties("?*~5", ParserEnums::Note, ParserEnums::Mul, osc, '5'));
	CHECK(testParserNoteProperties(":5", ParserEnums::Note, ParserEnums::Add, env, '5'));
	CHECK(testParserNoteProperties("~5", ParserEnums::Note, ParserEnums::Add, osc, '5'));

	ptr<Value> freq0 = ChainPool<Value>::instance().mk2<Const>(440.0);
	ptr<Value> osc1 = ChainPool<Value>::instance().mk2<Oscillator>(freq0,.9);
	//Osc::mk(Num::mk(440), .9);
	CHECK(testParserNoteProperties2("~5", ParserEnums::Note, ParserEnums::Add, osc1, '5'));
}

TEST(Parse, WithMacro)
{
	float note;
	Ittr ittr("A{#}B{a}BA");
	EXPECT_TRUE(Parsers<Ittr>::parseNote(ittr, 48, note));
	EXPECT_FLOAT_EQ(46, note);
}

TEST(Parse, Envelope)
{
	ChainPool<Value>::Scope scopeValue(10);
	ChainPool<AudioBuffer>::Scope scopeBuffer(10);
	ptr<Envelope> env = ChainPool<Value>::instance().mk2<Envelope>();
	env->addSegment(0.5, 0.7);
	//CHECK(testParserEnvelope(manager, bufferPool, ":5", ParserEnums::Note, ParserEnums::Add, env, '5'));
}

}
