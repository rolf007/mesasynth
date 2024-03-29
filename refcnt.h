#ifndef _REFCNT_H__
#define _REFCNT_H__
#include <typeinfo>

class ChainPoolBase;

void null_ptr_error(char const* name);
void ref_cnt_destruct_error(char const* name);

class refcnt {
	mutable unsigned int cnt;
	template<typename C>
	friend class ptr;
protected:
	refcnt() : cnt(0) {}
	refcnt(const refcnt&) : cnt(0) {}
	refcnt& operator=(refcnt const&) { return *this; }
	virtual ~refcnt() { if (cnt > 1) ref_cnt_destruct_error(typeid(this).name()); }
public:
	void destroy();
	unsigned int __cnt() const { return cnt; }
	ChainPoolBase* chainPoolBase_;
};

template<class C>
class ptr {
	C* p;
	void check_null() const { if (!p) null_ptr_error(typeid(p).name()); }
	void release(C* p) {
		if (p) {
			if (p->C::cnt == 1)
				p->destroy();
			else
				--(p->C::cnt);
		}
	}

	void inc(C* p) { if (p) p->C::cnt++; }
public:
	ptr(C* p = 0) : p(p) { inc(p); }
	ptr(const ptr& p2) : p(p2.p) { inc(p); }
	virtual ~ptr() { release(p); }
	ptr& operator=(const ptr& p2)
	{
		if (this != &p2) {
			release(p);
			p = p2.p;
			inc(p);
		}
		return *this;
	}
	/// allow a flat pointer to mutate into any compatible pointer
	template<class C2> operator ptr<C2>() const
	{
		return ptr<C2>(p);
	}
	/** A dynamic cast, that returns NULL if the object pointed to is not
	 * compatible with the requested class.
	 */
	template<class C2> ptr<C2> cast() const
	{
		return ptr<C2>(dynamic_cast<C2*>(p));
	}
	template<class C2> ptr<C2> scast() const { return ptr<C2>(static_cast<C2*>(p)); }
	C* operator->() const { check_null(); return p; }
	const C& operator*() const { check_null(); return *p; }
	operator const void*() const { return p; }
	bool operator==(const ptr& b) const { return p == b.p; }
	bool operator!=(const ptr& b) const { return p != b.p; }
	C*	 rawPtr() const { return p;}
};

#endif
