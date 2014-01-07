/************************************************************************
*
* Flood Project © (2008-201x)
* Licensed under the simplified BSD license. All rights reserved.
*
************************************************************************/

#pragma once

// NOTE: DOES NOT INCLUDE "API.H" BECAUSE ARRAY.H ASSUMES IT'S BEING INCLUDED BY API.H
#include <type_traits>

NAMESPACE_CORE_BEGIN

struct Allocator;

//-----------------------------------//

template <typename T, bool TrivialT>
class ArrayBase
{
public:
    void constructRange(T * data, size_t count);
    void moveRange(T * data, T * src, size_t count);
    void copyRange(T * data, T * src, size_t count);
    void destructRange(T * data, size_t count);
};

template <typename T>
class ArrayBase<T, true>
{
public:
    void constructRange(T * data, size_t count)
    {
#ifdef _DEBUG
        memset(data, 0, sizeof(T)* count);
#endif
    }

    void copyRange(T * data, T * src, size_t count)
    {
        memcpy(data, src, sizeof(T)* count);
    }

    void moveRange(T * data, T * src, size_t count)
    {
        memcpy(data, src, sizeof(T)* count);
    }

    void destructRange(T * data, size_t count)
    {
#ifdef _DEBUG
        memset(data, 0, sizeof(T)* count);
#endif
    }
};

template <typename T>
class ArrayBase<T, false>
{
public:
    void constructRange(T * data, size_t count)
    {
        for (size_t i = 0; i < count; ++i)
            new (data + i) T();
    }

    void copyRange(T * data, T * src, size_t count)
    {
        for (size_t i = 0; i < count; ++i)
            new (data + i) T(*(src + i));
    }

    void moveRange(T * data, T * src, size_t count)
    {
        for (size_t i = 0; i < count; ++i)
            new (data + i) T(std::move(*(src + i)));
    }

    void destructRange(T * data, size_t count)
    {
        for (size_t i = 0; i < count; ++i)
            data[i].~T();
    }
};

//-----------------------------------//

/**
*	A dynamic array container.
*/
template<typename T, bool TrivialT = std::is_pod<T>::value>
class Array : private ArrayBase<T, TrivialT>
{
public:

    /**
    *	Creates the array and allocates on the default global heap.
    */
    Array();

    /**
    *	Creates the array.
    *	@param a allocator serving any memory requests
    */
    Array(Allocator& a);

    /**
    *  Creates an array of n-elements which are initialized to a provided default value on the
    *  default global heap.
    *  @param count number of elements to size the array
    *  @param val default value for the elements in the array
    */
    Array(size_t count, const T& val);

    /**
    *	Creates an array of n-elements, allocated from a specified allocator, and initialized
    *	to a provided default value.
    *	@param a allocator serving memory requests
    *	@param count number of elements to size the array
    *	@param val default value for the elements in the array
    */
    Array(Allocator& a, size_t count, const T& val);

    /**
    *	Creates an array with the provided elements.
    *	@param elems initializer list of element values
    */
    Array(std::initializer_list<T> elems);

    /**
    *	Destructor.
    */
    ~Array();

    /**
    *	Copy constructor.
    *	@param other instance from which to copy
    */
    Array(const Array& other);

    /**
    *	Move constructor.
    *  @param other r-value reference to the source array
    */
    Array(Array&& other);

    /**
    *	Assignment operator.
    *	@param other reference to the source array
    */
    Array& operator=(const Array &other);

    /**
    *	Move-aware assignment operator.
    *	@param other r-value reference to the source array
    */
    Array& operator=(Array&& other);

    /**
    *	Indexing operator.
    *	@param i index of item
    */
    T &operator[](size_t i);
    const T &operator[](size_t i) const;

    /**
    *	Returns a pointer to the head of the array.
    */
    T * begin();
    T const * begin() const;
    T const * cbegin() const;

    /**
    *	Returns a pointer past the end of the array.
    */
    T * end();
    T const * end() const;
    T const * cend() const;

    /**
    *	Returns a pointer to the head of the array.
    */
    T * data();
    T const * data() const;

    /**
    *	Returns a reference to the first item.
    */
    T& front();
    T const & front() const;

    /**
    *	Returns a reference to the last item.
    */
    T& back();
    T const & back() const;

    /**
    *	Returns the number of elements.
    */
    size_t size() const;

    /**
    *	Returns the number of allocated elements.
    */
    size_t capacity() const;

    /**
    *	Returns size() != 0.
    */
    bool any() const;

    /**
    *	Returns size() == 0.
    */
    bool empty() const;

    /**
    *	Resizes the array to contain the specified number of elements.
    *	@param new_size number of elements
    */
    void resize(size_t new_size);

    /**
    *	Allocates the memory required to hold the provided number of elements.
    *	@param new_capacity number of elements
    */
    void reserve(size_t new_capacity);

    /**
    *	Sets the capacity equivalent to the size.
    */
    void trim();

    /**
    *	Sets the size to zero.
    */
    void clear();

    /**
    *	Appends the item to the back of the array.
    *	@param item item
    */
    void pushBack(T const & item);

    /**
    *	Removes the last item from the array.
    */
    void popBack();

    /**
    *	Inserts a sequence of items into the array at the specified position.
    *	@param pos position to insert items
    *	@param first iterator to the first item in the source sequence
    *	@param last iterator to the last item in the source sequence
    */
    template <typename Iter>
    void insert(T * pos, Iter first, Iter last);

    /**
    *	Removes the specified element. Returns the pointer provided to it.
    *	@param item pointer to the item's location within the array
    */
    T const * remove(T const * item);

    /**
    *	Returns the allocator serving the array's memory requests.
    */
    Allocator * allocator();
    Allocator const * allocator() const;

private:
    void setCapacity(size_t new_capacity);
    void grow(size_t min_capacity = 0);

private:
    Allocator *_allocator;
    size_t _size;
    size_t _capacity;
    T *_data;
};

//-----------------------------------//

template <typename T, bool TrivialT>
Array<T, TrivialT>::Array()
: _allocator(AllocatorGetHeap())
, _size(0)
, _capacity(0)
, _data(0)
{}

//-----------------------------------//

template <typename T, bool TrivialT>
Array<T, TrivialT>::Array(Allocator& allocator)
: _allocator(&allocator)
, _size(0)
, _capacity(0)
, _data(0)
{}

//-----------------------------------//

template <typename T, bool TrivialT>
Array<T, TrivialT>::Array(size_t count, const T& val)
: _allocator(AllocatorGetHeap())
, _size(0)
, _capacity(0)
, _data(0)
{
    reserve(count);
    for (size_t i = 0; i < count; ++i)
        pushBack(val);
}

//-----------------------------------//

template <typename T, bool TrivialT>
Array<T, TrivialT>::Array(Allocator& a, size_t count, const T& val)
: _allocator(&a)
, _size(0)
, _capacity(0)
, _data(0)
{
    reserve(count);
    for (size_t i = 0; i < count; ++i)
        pushBack(val);
}

//-----------------------------------//

template <typename T, bool TrivialT>
Array<T, TrivialT>::Array(std::initializer_list<T> elems)
: _allocator(AllocatorGetHeap())
{
    reserve(elems.size());
    for (auto e : elems)
        pushBack(e);
}

//-----------------------------------//

template <typename T, bool TrivialT>
Array<T, TrivialT>::Array(const Array& other)
: _allocator(other._allocator)
, _size(0)
, _capacity(0)
, _data(0)
{
    const size_t n = other._size;
    setCapacity(n);
    copyRange(_data, other._data, n);
    _size = n;
}

//-----------------------------------//

template <typename T, bool TrivialT>
Array<T, TrivialT>::Array(Array&& other)
: _allocator(other._allocator)
, _size(other._size)
, _capacity(other._capacity)
, _data(other._data)
{
    other._allocator = nullptr;
    other._size = 0;
    other._capacity = 0;
    other._data = nullptr;
}

//-----------------------------------//

template <typename T, bool TrivialT>
Array<T, TrivialT>::~Array()
{
    destructRange(_data, _size);
    AllocatorDeallocate(_data);
}

//-----------------------------------//

template <typename T, bool TrivialT>
auto Array<T, TrivialT>::operator=(const Array& other) -> Array&
{
    if (this != &other)
    {
        const size_t n = other._size;
        resize(n);
        copyRange(_data, other._data, n);
    }

    return *this;
}

//-----------------------------------//

template <typename T, bool TrivialT>
auto Array<T, TrivialT>::operator=(Array&& other) -> Array&
{
    destructRange(_data, _size);
    AllocatorDeallocate(_data);

    _allocator = other._allocator;
    _size = other._size;
    _capacity = other._capacity;
    _data = other._data;

    other._allocator = nullptr;
    other._size = 0;
    other._capacity = 0;
    other._data = nullptr;

    return *this;
}

//-----------------------------------//

template <typename T, bool TrivialT>
T& Array<T, TrivialT>::operator[](size_t i)
{
    return _data[i];
}

//-----------------------------------//

template <typename T, bool TrivialT>
const T& Array<T, TrivialT>::operator[](size_t i) const
{
    return _data[i];
}

//-----------------------------------//

template <typename T, bool TrivialT>
T * Array<T, TrivialT>::begin()
{
    return _data;
}

//-----------------------------------//

template <typename T, bool TrivialT>
T const * Array<T, TrivialT>::begin() const
{
    return _data;
}

//-----------------------------------//

template <typename T, bool TrivialT>
T const * Array<T, TrivialT>::cbegin() const
{
    return _data;
}

//-----------------------------------//

template <typename T, bool TrivialT>
T * Array<T, TrivialT>::end()
{
    return _data + _size;
}

//-----------------------------------//

template <typename T, bool TrivialT>
T const * Array<T, TrivialT>::end() const
{
    return _data + _size;
}

//-----------------------------------//

template <typename T, bool TrivialT>
T const * Array<T, TrivialT>::cend() const
{
    return _data + _size;
}

//-----------------------------------//

template <typename T, bool TrivialT>
T * Array<T, TrivialT>::data()
{
    return _data;
}

//-----------------------------------//

template <typename T, bool TrivialT>
T const * Array<T, TrivialT>::data() const
{
    return _data;
}

//-----------------------------------//

template <typename T, bool TrivialT>
T& Array<T, TrivialT>::front()
{
    return _data[0];
}

//-----------------------------------//

template <typename T, bool TrivialT>
T const & Array<T, TrivialT>::front() const
{
    return _data[0];
}

//-----------------------------------//

template <typename T, bool TrivialT>
T& Array<T, TrivialT>::back()
{
    return _data[_size - 1];
}

//-----------------------------------//

template <typename T, bool TrivialT>
T const & Array<T, TrivialT>::back() const
{
    return _data[_size - 1];
}

//-----------------------------------//

template <typename T, bool TrivialT>
size_t Array<T, TrivialT>::size() const
{
    return _size;
}

//-----------------------------------//

template <typename T, bool TrivialT>
size_t Array<T, TrivialT>::capacity() const
{
    return _capacity;
}

//-----------------------------------//

template <typename T, bool TrivialT>
bool Array<T, TrivialT>::any() const
{
    return _size != 0;
}

//-----------------------------------//

template <typename T, bool TrivialT>
bool Array<T, TrivialT>::empty() const
{
    return _size == 0;
}

//-----------------------------------//

template <typename T, bool TrivialT>
void Array<T, TrivialT>::resize(size_t new_size)
{
    if (new_size > _capacity)
        grow(new_size);
    else if (new_size > _size)
        constructRange(_data + _size, new_size - _size);
    else
        destructRange(_data + new_size, _size - new_size);

    _size = new_size;
}

//-----------------------------------//

template <typename T, bool TrivialT>
void Array<T, TrivialT>::reserve(size_t new_capacity)
{
    if (new_capacity > _capacity)
        setCapacity(new_capacity);
}

//-----------------------------------//

template <typename T, bool TrivialT>
void Array<T, TrivialT>::trim()
{
    setCapacity(_size);
}

//-----------------------------------//

template <typename T, bool TrivialT>
void Array<T, TrivialT>::clear()
{
    resize(0);
}

//-----------------------------------//

template <typename T, bool TrivialT>
void Array<T, TrivialT>::pushBack(T const & item)
{
    if (_size + 1 > _capacity)
        grow();
    _data[_size++] = item;
}

//-----------------------------------//

template <typename T, bool TrivialT>
void Array<T, TrivialT>::popBack()
{
    destructRange(_data + _size - 1, 1);
    _size--;
}

//-----------------------------------//

template <typename T, bool TrivialT>
template <typename Iter>
void Array<T, TrivialT>::insert(T * pos, Iter first, Iter last)
{
    if (pos < begin() || pos > end() || (last < first))
        return;

    size_t offset_ = (pos - begin());
    size_t newSize_ = size() + (last - first);
    auto newData_ = (byte*)AllocatorAllocate(_allocator, newSize_ * sizeof(T), alignof(T));

    moveRange((T*)newData_, _data, offset_);

    for (auto it = first; it != last; ++it)
        new (&newData_[sizeof(T)* (offset_ + (it - first))]) T(*it);

    moveRange((T*)(newData_ + (offset_ + (last - first)) * sizeof(T)), _data + offset_, size() - offset_);

    AllocatorDeallocate(_data);
    _data = (T*)newData_;
    _size = newSize_;
    _capacity = newSize_;
}

//-----------------------------------//

template <typename T, bool TrivialT>
T const * Array<T, TrivialT>::remove(T const * item)
{
    assert(item >= begin() || item < end());

    for (size_t i = (item - begin()); i < size() - 1; ++i)
        _data[i] = _data[i + 1];
    popBack();

    return item;
}

//-----------------------------------//

template <typename T, bool TrivialT>
Allocator * Array<T, TrivialT>::allocator()
{
    return _allocator;
}

//-----------------------------------//

template <typename T, bool TrivialT>
Allocator const * Array<T, TrivialT>::allocator() const
{
    return _allocator;
}

//-----------------------------------//

template <typename T, bool TrivialT>
void Array<T, TrivialT>::setCapacity(size_t new_capacity)
{
    if (new_capacity == _capacity)
        return;

    if (new_capacity < _size)
        resize(new_capacity);

    T *new_data = 0;
    if (new_capacity > 0) {
        auto sizeBytes = new_capacity * sizeof(T);
        auto alignment = (size_t)4;

#if _DEBUG
        sizeBytes += 16;
#endif
        new_data = (T*)AllocatorAllocate(_allocator, sizeBytes, alignment);

        moveRange(new_data, _data, _size);
        constructRange(new_data + _size, new_capacity - _size);
    }

    destructRange(_data, _size);
    AllocatorDeallocate(_data);
    _data = new_data;
    _capacity = new_capacity;
}

//-----------------------------------//

template <typename T, bool TrivialT>
void Array<T, TrivialT>::grow(size_t min_capacity)
{
    size_t new_capacity = (_capacity * 2) + 8;
    if (new_capacity < min_capacity)
        new_capacity = min_capacity;
    setCapacity(new_capacity);
}

//-----------------------------------//

NAMESPACE_CORE_END