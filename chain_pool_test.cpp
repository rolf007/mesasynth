#include "chain_pool.h"
#include <gtest/gtest.h>

using namespace std;
namespace {
class Element : public refcnt<Element> {
public:
	Element(unsigned a, unsigned b) :a(a), b(b) {}
	long long a,b;
	char c;
};


TEST(ChainPool, basic)
{
	{
		ChainPool<Element>::Scope scope(4);
		ptr<Element> elem0 = ChainPool<Element>::instance().mk(7, 8);
	}
}

TEST(ChainPool, alloc_all)
{
	ChainPool<Element>::Scope scope(4);
	ptr<Element> elem0 = ChainPool<Element>::instance().mk(1, 2);
	ptr<Element> elem1 = ChainPool<Element>::instance().mk(3, 4);
	ptr<Element> elem2 = ChainPool<Element>::instance().mk(5, 6);
	ptr<Element> elem3 = ChainPool<Element>::instance().mk(7, 8);
	EXPECT_EQ(1, elem0->a);
	EXPECT_EQ(3, elem1->a);
	EXPECT_EQ(5, elem2->a);
	EXPECT_EQ(7, elem3->a);
	EXPECT_EQ(nullptr, ChainPool<Element>::instance().mk(9, 10));
}

TEST(ChainPool, mixed)
{
	ChainPool<Element>::Scope scope(4);
	ptr<Element> elem0 = ChainPool<Element>::instance().mk(1, 2);
	ptr<Element> elem1 = ChainPool<Element>::instance().mk(3, 4);
	ptr<Element> elem2 = ChainPool<Element>::instance().mk(5, 6);
	elem1 = nullptr;
	ptr<Element> elem3 = ChainPool<Element>::instance().mk(7, 8);
	ptr<Element> elem4 = ChainPool<Element>::instance().mk(9, 10);
	EXPECT_EQ(nullptr, ChainPool<Element>::instance().mk(11, 12));
	elem2 = nullptr;
	ptr<Element> elem5 = ChainPool<Element>::instance().mk(13, 14);
	EXPECT_EQ(nullptr, ChainPool<Element>::instance().mk(11, 12));
}

class Base : public refcnt<Base> {
public:
	static unsigned deletes;
    virtual ~Base() { ++deletes; }
    virtual unsigned foo() = 0;
};

class BigChild : public Base {
public:
	static unsigned deletes;
	BigChild(unsigned i) : i(i) {}
    virtual ~BigChild() { ++deletes; }
    virtual unsigned foo() override { return 1000 + i; }
    int a,b;
	unsigned i;
};

class SmallChild : public Base {
public:
	static unsigned deletes;
	SmallChild(unsigned i) : i(i) {}
    virtual ~SmallChild() { ++deletes; }
    virtual unsigned foo() override { return i; }
    int c;
	unsigned i;
};

unsigned Base::deletes = 0;
unsigned SmallChild::deletes = 0;
unsigned BigChild::deletes = 0;


TEST(ChainPool, polymorphism)
{
	ChainPool<Base>::Scope scope(10, sizeof(BigChild));
	Base::deletes = 0;
	SmallChild::deletes = 0;
	BigChild::deletes = 0;
	{
		ptr<Base> b0 = ChainPool<Base>::instance().mk2<BigChild>(10);
		ptr<Base> b1 = ChainPool<Base>::instance().mk2<SmallChild>(20);
		ptr<Base> b2 = ChainPool<Base>::instance().mk2<BigChild>(30);
		ptr<Base> b3 = ChainPool<Base>::instance().mk2<SmallChild>(40);
		ptr<Base> b4 = ChainPool<Base>::instance().mk2<BigChild>(50);
		ptr<Base> b5 = ChainPool<Base>::instance().mk2<SmallChild>(60);
		EXPECT_EQ(0, Base::deletes);
		EXPECT_EQ(0, SmallChild::deletes);
		EXPECT_EQ(0, BigChild::deletes);
	}
	EXPECT_EQ(6, Base::deletes);
	EXPECT_EQ(3, SmallChild::deletes);
	EXPECT_EQ(3, BigChild::deletes);
}
}
