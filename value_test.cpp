#include "value.h"
#include "buffer.h"
#include "refcnt.h"
#include "chain_pool.h"
#include <gtest/gtest.h>
#include <iostream>

using namespace std;

namespace {
class TestCtx : public Ctx {
public:
	TestCtx() {}
	ValueInstance note() const override { return ValueInstance(ChainPool<Value>::instance().mk2<Const>(0.0)); }
	ValueInstance volume() const override { return ValueInstance(ChainPool<Value>::instance().mk2<Const>(0.0)); }
	float sampleRate() const override { return 14080; }
};

class ExpBuff {
public:
	ExpBuff() {
	}
	ExpBuff(unsigned size, const char* str) {
		buff = ChainPool<AudioBuffer>::instance().mk(size);
		unsigned char* p = (unsigned char*)buff->buff();
		unsigned chars = buff->size()*sizeof(float);
		for (unsigned i = 0; i < chars; i+=3) {
			char c0 = *str++;
			char c1 = *str++;
			char c2 = *str++;
			char c3 = *str++;
			unsigned char v0 = ((c0 - ' ') << 2) + ((c1 - ' ') >> 4);
			unsigned char v1 = ((c1 - ' ') << 4) + ((c2 - ' ') >> 2);
			unsigned char v2 = ((c2 - ' ') << 6) + ((c3 - ' ') >> 0);
			p[i] = v0;
			if (i+1 < chars) p[i+1] = v1;
			if (i+2 < chars) p[i+2] = v2;
		}
	}
	ptr<AudioBuffer> buff;
};

bool operator==(const ExpBuff& buff0, const ptr<AudioBuffer>& buff1)
{
	if (buff0.buff->size() != buff1->size())
		return false;
	for (unsigned i = 0; i < buff1->size(); ++i)
		if (buff0.buff->buff()[i] != buff1->buff()[i])
			return false;
	return true;
}

static ptr<AudioBuffer> gGotBuff;
std::ostream& operator<<(std::ostream& os, const ExpBuff& expBuff)
{
	os << endl;
    static constexpr unsigned height=22;
    static constexpr unsigned width=100;
    float min = 10000.0;
    float max = -10000.0;
	ptr<AudioBuffer> buff2 = gGotBuff;
	ptr<AudioBuffer> buff = expBuff.buff;
    for (unsigned i = 0; i < buff->size(); ++i) {
        if (buff->buff()[i] > max) max = buff->buff()[i];
        if (buff->buff()[i] < min) min = buff->buff()[i];
    }
    for (unsigned i = 0; i < buff2->size(); ++i) {
        if (buff2->buff()[i] > max) max = buff2->buff()[i];
        if (buff2->buff()[i] < min) min = buff2->buff()[i];
    }
    unsigned maxSize = buff->size() > buff2->size() ? buff->size() : buff2->size();
    char screen[height*width];
    memset(screen, ' ', height*width);
    for (unsigned x = 0; x < width; ++x) {
        unsigned i = x*maxSize/width;
        if (i < buff->size()) {
            unsigned y = (max-buff->buff()[i])*(height-1)/(max-min);
            screen[x+y*width] = '.';
        }
        if (i < buff2->size()) {
            unsigned y2 = (max-buff2->buff()[i])*(height-1)/(max-min);
            screen[x+y2*width] = 'o';
        }
    }
    for (unsigned y = 0; y < height; ++y) {
        os << setprecision(2) << setw(7) << setfill(' ') << fixed << max-y*(max-min)/height;
        for (unsigned x = 0; x < width; ++x) {
            os << screen[x+y*width];
        }
        os << endl;
    }
    os << ". expected size = " << buff->size() << endl;
    os << "o got size =      " << buff2->size() << endl;
	gGotBuff = nullptr;
	return os;
}

ostream& uuOut(ostream& os, const char& c)
{
	if (c == '"') os << "\\\"";
	else if (c == '\\') os << "\\\\";
	else os << c;
	return os;
}
std::ostream& operator<<(std::ostream& os, const ptr<AudioBuffer>& buff)
{
	os << "see above" << endl;
	os << "\"";
	unsigned char* p = (unsigned char*)(buff->buff());
	unsigned chars = buff->size()*sizeof(float);
	for (unsigned i = 0; i < chars; i+=3) {
		unsigned char v0 = p[i];
		unsigned char v1 = i+1 < chars ? p[i+1] : 0;
		unsigned char v2 = i+2 < chars ? p[i+2] : 0;
		char c0 =                  ((v0 >> 2)&63) + ' ';
		char c1 = ((v0 << 4)&63) + ((v1 >> 4)&63) + ' ';
		char c2 = ((v1 << 2)&63) + ((v2 >> 6)&63) + ' ';
		char c3 = ((v2 << 0)&63) + ' ';
		uuOut(os, c0);
		uuOut(os, c1);
		uuOut(os, c2);
		uuOut(os, c3);

	}
	os << "\"";
	gGotBuff = buff;
	return os;
}

TEST(Value, envelope)
{
	TestCtx ctx;
	ChainPool<Value>::Scope valuePool(10);
	ChainPool<AudioBuffer>::Scope bufferPool(10);
	Envelope::Segs segs;
	//1>4:2>4:1>;
	segs.add(.001,4);
	segs.add(.002,4);
	segs.add(.001,1);
	ptr<Envelope> env = ChainPool<Value>::instance().mk2<Envelope>(segs);

	ExpBuff exp(64, "     %UTD3Y==!$_BRY:/UUTD3]TT;4_BR[:/Z*+_C]==!% Z*(C0'31-4#__T= BRY:0!==;$\"BBWY   \" 0   @$   (!   \" 0   @$   (!   \" 0   @$   (!   \" 0   @$   (!   \" 0   @$   (!   \" 0   @$   (!   \" 0   @$   (!   \" 0   @$   (!   \" 0   @$   (!   \" 0.FB=4   &A &%U:0\"^Z3$!&%S] 7G0Q0'71(T\",+A9 I(L(0';1]3^DB]H_TT6_/P( I#\\PNH@_  \" /P  @#\\  ( _  \" /P  @#\\  ( _  \" /P  ");
	ptr<AudioBuffer> got = env->get(0, 64, ctx,nullptr);
	EXPECT_EQ(exp, got);
	EXPECT_EQ(4.0, got->buff()[20]);
	EXPECT_EQ(1.0, got->buff()[60]);
}

TEST(Value, empty_envelope)
{
	TestCtx ctx;
	ChainPool<Value>::Scope valuePool(10);
	ChainPool<AudioBuffer>::Scope bufferPool(10);
	Envelope::Segs segs;
	ptr<Envelope> env = ChainPool<Value>::instance().mk2<Envelope>(segs);

	ExpBuff exp(64, "                                                                                                                                                                                                                                                                                                                                                        ");
	ptr<AudioBuffer> got = env->get(0, 64, ctx,nullptr);
	EXPECT_EQ(exp, got);
	EXPECT_EQ(0.0, got->buff()[20]);
	EXPECT_EQ(0.0, got->buff()[60]);
}

TEST(Value, non_continous_envelope)
{
	TestCtx ctx;
	ChainPool<Value>::Scope valuePool(10);
	ChainPool<AudioBuffer>::Scope bufferPool(10);
	Envelope::Segs segs;
	// :>4:1>4:>3:1>3:>2;
	segs.add(0,4);
	segs.add(0.001,4);
	segs.add(0,3);
	segs.add(0.001,3);
	segs.add(0,2);
	ptr<Envelope> env = ChainPool<Value>::instance().mk2<Envelope>(segs);

	ExpBuff exp(64, "  \" 0   @$   (!   \" 0   @$   (!   \" 0   @$   (!   \" 0   @$   (!   \" 0   @$   (!   ! 0   0$   $!   ! 0   0$   $!   ! 0   0$   $!   ! 0   0$   $!   ! 0   0$    !     0    $    !     0    $    !     0    $    !     0    $    !     0    $    !     0    $    !     0    $    !     0    $    !     0    $    !     0    $    !     0    $    !     0   ");
	ptr<AudioBuffer> got = env->get(0, 64, ctx,nullptr);
	EXPECT_EQ(exp, got);
	EXPECT_EQ(4.0, got->buff()[10]);
	EXPECT_EQ(3.0, got->buff()[20]);
	EXPECT_EQ(2.0, got->buff()[60]);
}

TEST(Value, oscillator)
{
	TestCtx ctx;
	ChainPool<Value>::Scope valuePool(10);
	ChainPool<AudioBuffer>::Scope bufferPool(10);
	ptr<Value> freq0   = ChainPool<Value>::instance().mk2<Const>(440.0);
	ptr<Oscillator> osc = ChainPool<Value>::instance().mk2<Oscillator>(freq0, 0.5);
	ExpBuff exp(64, "     ,+%QST5[T,^VCF./O,$M3XQV]0^7H/L/KX4^SX    _OA3[/EZ#[#XQV]0^\\P2U/MHYCCX5[T,^PL7'/3(QC23\"Q<>]%>]#OMHYCK[S!+6^,=O4OEZ#[+Z^%/N^    O[X4^[Y>@^R^,=O4OO,$M;[:.8Z^%>]#OL+%Q[TR,0VEPL7'/17O0S[:.8X^\\P2U/C';U#Y>@^P^OA3[/@   #^^%/L^7H/L/C';U#[S!+4^VCF./A7O0S[\"Q<<]RLE3)<+%Q[T5[T.^VCF.OO,$M;XQV]2^7H/LOKX4^[X   \"_OA3[OEZ#[+XQV]2^\\P2UOMHYCKX5[T.^PL7'O0  ");
	ptr<AudioBuffer> got = osc->get(0, 64, ctx,nullptr);
	EXPECT_EQ(exp, got);
}
}

class Segs {
public:
	Segs& add(float, float) { return *this; }
};
class Env {
public:
	Env(Segs& segs) : segs_(segs) {}
	Segs segs_;
};
TEST(Value, experimental_env_construction)
{
	Segs segs;
	segs.add(8,8).add(7,7);
	Env env(segs);
}
