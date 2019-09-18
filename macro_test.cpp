#include <gtest/gtest.h>
#include "macro.h"

using namespace std;

void testMacro(string exp, const char* in)
{
    string out;
    for (Ittr ittr(in); *ittr != 0; ++ittr)
        out += *ittr;
	EXPECT_EQ(exp, out);
}

TEST(ParseMacro, basic)
{
	Ittr ittr("abcdef");
	EXPECT_EQ('a', *ittr);
	++ittr;
	EXPECT_EQ('b', *ittr);
	Ittr ittr2 = ittr;
	EXPECT_EQ('b', *ittr2);
}

TEST(ParseMacro, expand)
{
    testMacro("abcghidefjkl", "abcAx{def}ghiAx@jkl"); // simple
	testMacro( "abc.1234.hijkldegh", "Bx{Dx@1234Dx@}aCx{hijkl}Dx{.}Ax{bcBx@Cx@de}Ax@gh"); // recursive
	testMacro( "abc123d456e123123", "abAx{123}c@dB{456}@eAx@@"); // most recent
	testMacro( "abcdef456456123123", "abA{123}cd{456}ef@@A@@"); // anonymous macro
	testMacro( "ab123cd456e", "aA{123}bA@cA{456}dA@e"); // override macro
    testMacro("abcghi--jkl-:ab-", "abcAx{-}ghiAxAx@jklAx:abAx"); // any terminator
    testMacro("ag7", "aG{g}G7{g7}G7"); // shorter
    testMacro("ag", "aG{g}G7{g7}G"); // shorter
    testMacro("ag7", "aG7{g7}G{g}G7"); // shorter
    testMacro("ag", "aG7{g7}G{g}G"); // shorter
}


//EBNF:
//
//dur? (%scl ^scl &scl)* ( ([c-h] ((.finetune)?#)? octave? | _ ) dur? (+extdur)? (%scl ^scl &val $val)* )*
//
//scl is scalar
//val is scalar or function of time
//dur is absolute or relative duration
//
//example:
//V{~1,.2}/2%.8^1&1c.5#2'+'%*.5^*.5V:>'1       ':' is short for '$+:' (pitch add envelope)
