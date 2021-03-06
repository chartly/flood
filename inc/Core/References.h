/************************************************************************
*
* Flood Project © (2008-201x)
* Licensed under the simplified BSD license. All rights reserved.
*
************************************************************************/

#pragma once

#include "Core/Concurrency.h"

//#define ENABLE_REFERENCES_DEBUG

#ifdef ENABLE_REFERENCES_DEBUG
#include <type_traits>
#endif

NAMESPACE_CORE_BEGIN

//-----------------------------------//

struct ReferenceCounted;

API_CORE int32 ReferenceGetCount(ReferenceCounted*);
API_CORE void ReferenceAdd(ReferenceCounted*);
API_CORE bool ReferenceRelease(ReferenceCounted*);

struct API_CORE NO_VTABLE ReferenceCounted
{
	ReferenceCounted() : references(0) {}
	ReferenceCounted(const ReferenceCounted&) : references(0) {}
	ReferenceCounted& operator=(const ReferenceCounted&) { return *this; }
	
	inline void addReference() { ReferenceAdd(this); }
	inline bool releaseReference() { return ReferenceRelease(this); }

	Atomic<uint32> references;
};

//-----------------------------------//

#ifdef ENABLE_REFERENCES_DEBUG

struct Class;
struct Object;

typedef Delegate2<Class*, Object*> DebugReferenceDelegate;
DebugReferenceDelegate& GetDebugReferences();

template<typename T>
void DebugReferencesObject(T* px, typename std::enable_if< std::is_base_of<Object, T>::value>::type* = 0)
{
	DebugReferenceDelegate& callback = GetDebugReferences();
	if( callback.empty() ) return;
	
	callback(px->getType(), px);
}

template<typename T>
void DebugReferencesObject(T* px, typename std::enable_if< !std::is_base_of<Object, T>::value>::type* = 0)
{
}

#define REFERENCES_DEBUG_CALLBACK() \
	DebugReferencesObject(px);

#else

#define REFERENCES_DEBUG_CALLBACK()

#endif

template<typename T> class RefPtr
{
public:

	RefPtr(): px(nullptr) { }

	RefPtr(T* p, bool add_ref = true) : px(p)
	{
		if( px != nullptr && add_ref )
		{
			px->addReference();
			REFERENCES_DEBUG_CALLBACK();
		}
	}

	template<typename U>
	RefPtr( RefPtr<U> const & rhs ) : px( rhs.get() )
	{
		if( px != nullptr )
		{
			px->addReference();
			REFERENCES_DEBUG_CALLBACK();
		}
	}

	RefPtr(RefPtr const & rhs): px(rhs.px)
	{
		if( px != nullptr )
		{
			px->addReference();
			REFERENCES_DEBUG_CALLBACK();
		}
	}

	~RefPtr()
	{
		if( px != nullptr && px->releaseReference() )
		{
			REFERENCES_DEBUG_CALLBACK();
			Deallocate(px);
		}
	}

	template<typename U>
	RefPtr& operator=(const RefPtr<U>& rhs)
	{
		RefPtr(rhs).swap(*this);
		return *this;
	}

#ifdef COMPILER_SUPPORTS_CXX11
	RefPtr(RefPtr&& rhs): px(rhs.px)
	{
	rhs.px = nullptr;
	}

	RefPtr& operator=(RefPtr&& rhs)
	{
		RefPtr(static_cast<RefPtr&&>(rhs)).swap(*this);
		return *this;
	}
#endif

	RefPtr& operator=(const RefPtr& rhs)
	{
		RefPtr(rhs).swap(*this);
		return *this;
	}

	RefPtr& operator=(T* rhs)
	{
		RefPtr(rhs).swap(*this);
		return *this;
	}

	void reset()
	{
		RefPtr().swap(*this);
	}

	void reset( T* rhs )
	{
		RefPtr(rhs).swap(*this);
	}

	T* get() const
	{
		return px;
	}

	T& operator*() const
	{
		assert( px != nullptr );
		return *px;
	}

	T* operator->() const
	{
		assert( px != nullptr );
		return px;
	}

	typedef T* RefPtr::*unspecified_bool_type;

	operator unspecified_bool_type() const // never throws
	{
		return px == nullptr ? false : &RefPtr::px;
	}

	void swap(RefPtr& rhs)
	{
		std::swap(px, rhs.px);
	}

public:

	T* px;
};

template<class T, class U> inline bool operator==(const RefPtr<T>& a, const RefPtr<U>& b)
{
	return a.get() == b.get();
}

template<class T, class U> inline bool operator!=(const RefPtr<T>& a, const RefPtr<U>& b)
{
	return a.get() != b.get();
}

template<class T, class U> inline bool operator==(const RefPtr<T>& a, U* b)
{
	return a.get() == b;
}

template<class T, class U> inline bool operator!=(const RefPtr<T>& a, U* b)
{
	return a.get() != b;
}

template<class T, class U> inline bool operator==(T* a, const RefPtr<U>& b)
{
	return a == b.get();
}

template<class T, class U> inline bool operator!=(T* a, const RefPtr<U>& b)
{
	return a != b.get();
}

template<class T> inline bool operator<(const RefPtr<T>& a, const RefPtr<T>& b)
{
	return std::less<T *>()(a.get(), b.get());
}

template<class T> void swap(const RefPtr<T>& lhs, const RefPtr<T>& rhs)
{
	lhs.swap(rhs);
}

template<class T, class U> RefPtr<T> RefCast(const RefPtr<U>& p)
{
	return static_cast<T*>(p.get());
}

//-----------------------------------//

NAMESPACE_CORE_END

#define TYPEDEF_INTRUSIVE_POINTER_FROM_TYPE( type ) \
	typedef RefPtr<type> type##Ptr;

#define FWD_DECL_INTRUSIVE(T) \
	NAMESPACE_CORE_BEGIN \
	class T; \
	TYPEDEF_INTRUSIVE_POINTER_FROM_TYPE( T ); \
	NAMESPACE_CORE_END
