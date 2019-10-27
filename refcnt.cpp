#include "refcnt.h"
#include "chain_pool.h"
#include <iostream>
#include "stdlib.h"

using namespace std;

void null_ptr_error(char const* name)
{
	cerr << "recfnt[" << name << "]: NULL pointer execption";
	int* p = 0; *p = 0;
	exit(0);
}

void ref_cnt_destruct_error(char const* name)
{
	cerr << "ref count has been increased within destructor of refcnt_base " << name;
	exit(0);
}

void refcnt::destroy()
{
	chainPoolBase_->release(this);
}
