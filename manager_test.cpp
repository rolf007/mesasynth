#include "manager.h"
#include <gtest/gtest.h>

class Base {
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


TEST(Manager, basic)
{
	{
		Manager<Base, sizeof(BigChild)> manager;
		unsigned b0 = manager.mk<BigChild>(10);
		unsigned b1 = manager.mk<SmallChild>(20);
		unsigned b2 = manager.mk<BigChild>(30);
		unsigned b3 = manager.mk<SmallChild>(40);
		unsigned b4 = manager.mk<BigChild>(50);
		unsigned b5 = manager.mk<SmallChild>(60);
		EXPECT_EQ(nullptr, manager.get(b0));
		EXPECT_EQ(nullptr, manager.get(b1));
		EXPECT_NE(nullptr, manager.get(b2));
		EXPECT_NE(nullptr, manager.get(b3));
		EXPECT_NE(nullptr, manager.get(b4));
		EXPECT_NE(nullptr, manager.get(b5));
		EXPECT_EQ(1030, manager.get(b2)->foo());
		EXPECT_EQ(40, manager.get(b3)->foo());
		EXPECT_EQ(1050, manager.get(b4)->foo());
		EXPECT_EQ(60, manager.get(b5)->foo());
		EXPECT_EQ(2, Base::deletes);
		EXPECT_EQ(1, SmallChild::deletes);
		EXPECT_EQ(1, BigChild::deletes);
	}
	EXPECT_EQ(6, Base::deletes);
	EXPECT_EQ(3, SmallChild::deletes);
	EXPECT_EQ(3, BigChild::deletes);
}
