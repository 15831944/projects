#ifndef SCOPEDPTR_H 
#define SCOPEDPTR_H
#pragma once

#include <assert.h>
template<class C>
class scoped_ptr
{
public:
	typedef C element_type;

	explicit scoped_ptr(C* p = NULL):ptr_(p){}
	~scoped_ptr()
	{
		enum{type_must_be_complete = sizeof(C)};
		delete ptr_;
	}

public:
	// operator

	C& operator*() const
	{
		assert(ptr_ != NULL);
		return *ptr_;
	}

	C* operator->() const
	{
		assert(ptr_ != NULL);
		return ptr_;
	}

	bool operator==(C* p) const {return ptr_ == p;}
	bool operator!=(C* p) const {return ptr_ != p;}

public:
	//Type conversion
	operator bool() const
	{
		if(!ptr_)
			return false;
		return true;
	}

public:
	//function

	C* get() const {return ptr_;}

	void swap(scoped_ptr &p2)
	{
		C* tmp = ptr_;
		ptr_ = p2.ptr_;
		p2.ptr_ = tmp;
	}

	//C* release()
	//{
	//	C* retVal = ptr_;
	//	ptr_ = NULL;
	//	return retVal;
	//}

	void reset(C *p = NULL)
	{
		if(p != ptr_)
		{
			enum{type_must_be_complete = sizeof(C)};
			delete ptr_;
			ptr_ = p;
		}
	}

private:
	C* ptr_;

	scoped_ptr(const scoped_ptr&); 
	scoped_ptr& operator=(const scoped_ptr&); 

	template<class C2> bool operator==(scoped_ptr<C2> const& p2) const;
	template<class C2> bool operator!=(scoped_ptr<C2> const& p2) const;
	template<class C2> bool operator==(scoped_ptr<C2> const& p2);
	template<class C2> bool operator!=(scoped_ptr<C2> const& p2);
};

template <class C>
void swap(scoped_ptr<C>& p1, scoped_ptr<C>& p2)
{
	p1.swap(p2)
}

template <class C>
bool operator==(C* p1, const scoped_ptr<C>& p2)
{
	return p2 == p1;
}

template <class C>
bool operator!=(C* p1, const scoped_ptr<C>& p2)
{
	return p2 != p1;
}

template <class C>
class scoped_array
{
public:
	typedef C element_type;

	explicit scoped_array(C* p = NULL): array_(p){}
	~scoped_array()
	{
		enum{type_must_be_complete = sizeof(C)};
		delete[] array_;
	}

public:
	void reset(C* p = NULL)
	{
		if(p != array_)
		{
			enum{type_must_be_complete = sizeof(C)};
			delete[] array_;
			array_ = p;
		}
	}

	void swap(scoped_array &other)
	{
		C* tmp = array_;
		array_ = other.array_;
		other.array_ = tmp;
	}

	C* get() const
	{
		return array_;
	}

public:
	operator bool()
	{
		if(array_)
			return true;
		return false;
	}

	C& operator[](ptrdiff_t i) const
	{
		assert(i >= 0);
		assert(array_ != NULL);
		return array_[i];
	}

	bool operator==(C* p) const{return array_ == p;}
	bool operator!=(C* p) const{return array_ != p;}

private:
	C* array_;

	scoped_array(const scoped_array&);
	scoped_array& operator==(const scoped_array&);

	template<class C2> bool operator==(scoped_array<C2> const& p2) const;
	template<class C2> bool operator!=(scoped_array<C2> const& p2) const;
};

template <class C>
void swap(scoped_array<C>& p1, scoped_array<C>& p2)
{
	p2.swap(p1);
}

template <class C>
bool operator==(C* p1, const scoped_array<C>& p2)
{
	return p1 == p2.get();
}

template <class C>
bool operator!=(C* p1, const scoped_array<C>& p2)
{
	return p1 != p2.get();
}


class ScopedPtrMallocFree {
public:
	inline void operator()(void* x) const {
		free(x);
	}
};



template<class C, class FreeProc = ScopedPtrMallocFree>
class scoped_ptr_malloc
{
public:
	// The element type
	typedef C element_type;  

	explicit scoped_ptr_malloc(C* p = NULL):ptr_(p){}
	~scoped_ptr_malloc()
	{
		if(ptr_ != NULL)
		{
			FreeProc free_proc;
			free_proc(ptr_);
		}
	}

public:
	void reset(C* p = NULL)
	{
		if(ptr_ != p)
		{
			FreeProc free_proc;
			free_proc(ptr_);
			ptr_ = p;
		}
	}

	void swap(scoped_ptr_malloc &other)
	{
		C* tmp = ptr_;
		ptr_ = other.ptr_;
		other.ptr_ = tmp;
	}

	C* get() const
	{
		return ptr_;
	}

public:
	C* operator->() const
	{
		assert(ptr_ != NULL);
		return ptr_;
	}

	C& operator*() const
	{
		assert(ptr_ != NULL);
		return *ptr_;
	}

	operator bool()
	{
		if(ptr_)
			return true;
		return false;
	}

	bool operator==(C* p) const{return ptr_ == p;}
	bool operator!=(C* p) const{return ptr_ != p;}
private:
	C* ptr_;

	scoped_ptr_malloc(const scoped_ptr_malloc&);
	scoped_ptr_malloc& operator==(const scoped_ptr_malloc&);


	template <class C2, class GP>
	bool operator==(scoped_ptr_malloc<C2, GP> const&) const;
	template <class C2, class GP>
	bool operator!=(scoped_ptr_malloc<C2, GP> const&) const;
};

template<class C, class FP> inline
void swap(scoped_ptr_malloc<C, FP>& a, scoped_ptr_malloc<C, FP>& b) {
	a.swap(b);
}

template<class C, class FP> inline
bool operator==(C* p, const scoped_ptr_malloc<C, FP>& b) {
	return p == b.get();
}

template<class C, class FP> inline
bool operator!=(C* p, const scoped_ptr_malloc<C, FP>& b) {
	return p != b.get();
}

#endif