#include "parsers.h"
#include "value.h"
#include "manager.h"
#include "macro.h"
#include <gtest/gtest.h>

using namespace std;
 
#define CHECK(x) { SCOPED_TRACE(""); x; }

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

void testParserNoteProperties(Manager<Value, sizeof(Envelope)>& manager, const char* str, Parsers<const char*>::Property expProperty, Parsers<const char*>::Modifier expModifier, float expValue, char end=0)
{
	const char* tmp = str;
	Parsers<const char*>::Property property;
	Parsers<const char*>::Modifier modifier;
	unsigned valueId;
	float value;
	EXPECT_TRUE(Parsers<const char*>::parseNoteProperties(manager, tmp, property, modifier, valueId, value));
	EXPECT_EQ(end, *tmp);
	EXPECT_EQ(expProperty, property);
	EXPECT_EQ(expModifier, modifier);
	if (valueId == -1)
		EXPECT_FLOAT_EQ(expValue, value);
	else
		EXPECT_FLOAT_EQ(expValue, manager.get(valueId)->get());
}

void testParserNotePropertiesFail(Manager<Value, sizeof(Envelope)>& manager, const char* str)
{
	const char* tmp = str;
	Parsers<const char*>::Property property;
	Parsers<const char*>::Modifier modifier;
	unsigned valueId;
	float value;
	EXPECT_FALSE(Parsers<const char*>::parseNoteProperties(manager, tmp, property, modifier, valueId, value));
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
	float osc = 91.0;
	float env = 92.0;
	Manager<Value, sizeof(Envelope)> manager;
	CHECK(testParserNoteProperties(manager, "%.8", ParserEnums::Legato, ParserEnums::Set, .8));
	CHECK(testParserNoteProperties(manager, "^*4/5", ParserEnums::Velocity, ParserEnums::Mul, .8));
	CHECK(testParserNoteProperties(manager, "&+0.4/0.5", ParserEnums::Volume, ParserEnums::Add, .8));
	CHECK(testParserNoteProperties(manager, "?0.5", ParserEnums::Note, ParserEnums::Set, .5));
	CHECK(testParserNoteProperties(manager, "?*2/5", ParserEnums::Note, ParserEnums::Mul, .4));
	CHECK(testParserNoteProperties(manager, "?+5.", ParserEnums::Note, ParserEnums::Add, 5.0));
	CHECK(testParserNotePropertiesFail(manager, "%:5"));
	CHECK(testParserNotePropertiesFail(manager, "^:5"));
	CHECK(testParserNotePropertiesFail(manager, "%~5"));
	CHECK(testParserNotePropertiesFail(manager, "^~5"));
	CHECK(testParserNoteProperties(manager, "&:5", ParserEnums::Volume, ParserEnums::Set, env, '5'));
	CHECK(testParserNoteProperties(manager, "?:5", ParserEnums::Note, ParserEnums::Set, env, '5'));
	CHECK(testParserNoteProperties(manager, "&~5", ParserEnums::Volume, ParserEnums::Set, osc, '5'));
	CHECK(testParserNoteProperties(manager, "?~5", ParserEnums::Note, ParserEnums::Set, osc, '5'));
	CHECK(testParserNoteProperties(manager, ":5", ParserEnums::Note, ParserEnums::Add, env, '5'));
	CHECK(testParserNoteProperties(manager, "~5", ParserEnums::Note, ParserEnums::Add, osc, '5'));
}

TEST(Parse, WithMacro)
{
	float note;
	Ittr ittr("A{#}B{a}BA");
	EXPECT_TRUE(Parsers<Ittr>::parseNote(ittr, 48, note));
	EXPECT_FLOAT_EQ(46, note);
}
