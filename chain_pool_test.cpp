#include "chain_pool.h"
#include <gtest/gtest.h>

using namespace std;

class Element : public refcnt<Element> {
public:
	Element(unsigned a, unsigned b) :a(a), b(b) {}
	long long a,b;
	char c;
};


TEST(ChainPool, basic)
{
	{
		ChainPool<Element> chainPool(4);
		ptr<Element> elem0 = chainPool.mk(7, 8);
	}
}

TEST(ChainPool, alloc_all)
{
	ChainPool<Element> chainPool(4);
	ptr<Element> elem0 = chainPool.mk(1, 2);
	ptr<Element> elem1 = chainPool.mk(3, 4);
	ptr<Element> elem2 = chainPool.mk(5, 6);
	ptr<Element> elem3 = chainPool.mk(7, 8);
	EXPECT_EQ(1, elem0->a);
	EXPECT_EQ(3, elem1->a);
	EXPECT_EQ(5, elem2->a);
	EXPECT_EQ(7, elem3->a);
	EXPECT_EQ(nullptr, chainPool.mk(9, 10));
}

TEST(ChainPool, mixed)
{
	ChainPool<Element> chainPool(4);
	ptr<Element> elem0 = chainPool.mk(1, 2);
	ptr<Element> elem1 = chainPool.mk(3, 4);
	ptr<Element> elem2 = chainPool.mk(5, 6);
	elem1 = nullptr;
	ptr<Element> elem3 = chainPool.mk(7, 8);
	ptr<Element> elem4 = chainPool.mk(9, 10);
	EXPECT_EQ(nullptr, chainPool.mk(11, 12));
	elem2 = nullptr;
	ptr<Element> elem5 = chainPool.mk(13, 14);
	EXPECT_EQ(nullptr, chainPool.mk(11, 12));
}
