#include "chain_pool.h"
#include <gtest/gtest.h>

using namespace std;

class Element {
public:
	Element(unsigned a, unsigned b) :a(a), b(b) {}
	long long a,b;
	char c;
};


TEST(ChainPool, basic)
{
	ChainPool<Element, 4> chainPool;
	Element* elem0 = chainPool.mk(7, 8);
	chainPool.release(elem0);
}

TEST(ChainPool, alloc_all)
{
	cout << "=====" << endl;
	ChainPool<Element, 4> chainPool;
	Element* elem0 = chainPool.mk(1, 2);
	Element* elem1 = chainPool.mk(3, 4);
	Element* elem2 = chainPool.mk(5, 6);
	Element* elem3 = chainPool.mk(7, 8);
	cout << "ptr: " << elem0 << endl;
	cout << "ptr: " << elem1 << endl;
	cout << "ptr: " << elem2 << endl;
	cout << "ptr: " << elem3 << endl;
	EXPECT_EQ(1, elem0->a);
	EXPECT_EQ(3, elem1->a);
	EXPECT_EQ(5, elem2->a);
	EXPECT_EQ(7, elem3->a);
	EXPECT_EQ(nullptr, chainPool.mk(9, 10));
	cout << "=====" << endl;
}

TEST(ChainPool, mixed)
{
	ChainPool<Element, 4> chainPool;
	Element* elem0 = chainPool.mk(1, 2);
	Element* elem1 = chainPool.mk(3, 4);
	Element* elem2 = chainPool.mk(5, 6);
	chainPool.release(elem1);
	Element* elem3 = chainPool.mk(7, 8);
	Element* elem4 = chainPool.mk(9, 10);
	EXPECT_EQ(nullptr, chainPool.mk(11, 12));
	chainPool.release(elem2);
	Element* elem5 = chainPool.mk(13, 14);
	EXPECT_EQ(nullptr, chainPool.mk(11, 12));
}
