/************************************************************************
*
* Flood Project © (2008-201x)
* Licensed under the simplified BSD license. All rights reserved.
*
************************************************************************/

#pragma once

#include "Core/Array.h"

NAMESPACE_CORE_BEGIN

/**
 *	HashMap implementation which uses a generic uint64 as the hashed key type and contains
 *	objects or PODs via Arrays. If you want to key by a string, hash it via an external hash
 *	method to uint64 and then use this hash to key into the hash map.
 */
template<typename T> struct HashMap
{
public:
    /**
     *	Data structure describing each entry into the hash map.
     */
    struct Entry
    {
        uint64 key;
        size_t next;
        T value;
    };

public:
    /**
     *  Default constructor which forwards to global heap allocator.
     */
    HashMap();

    /**
     *  Constructor taking a reference to an allocator.
     *  @param a reference to allocator
     */
    HashMap(Allocator &a);

    /**
     *	A const pointer to the first Entry inside the hash map. The traversal
     *	of Entries is unordered. Intended for compatibility with STL-compatible
     *	constructs and algorithms.
     */
    Entry const * begin() const;

    /**
     *	A const pointer to the end of the Array of Entry structures.
     */
    Entry const * end() const;

    /**
     *	The number of elements in the hash map.
     */
    size_t size() const;

    /**
     *	Size equals zero.
     */
    bool empty() const;

    /**
     *	Allocates a capacity for a number of elements.
     *  @param size number of elements
     */
    void reserve(size_t size);

    /**
     *	Resize to zero elements.
     */
    void clear();

    /**
     *	A const reference to the value associated with the hash key. The user must
     *	provide a default value to return in the event that the key does not exist
     *	within the hash map.
     *	@param key integer hash value used as a key to look up an element
     *	@param default_value a default value that is returned if key does not exist
     */
    T const & get(uint64 key, T const & default_value) const;

    /**
     *	Inserts the provided value at key.
     *	@param key integer hash value used as a key to look up an element
     *	@param value the value to be stored within the hash map
     */
    void set(uint64 key, T const & value);

    /**
     *	True if the key exists in the hash map.
     *	@param key integer hash value used as a look up key
     */
    bool has(uint64 key) const;

    /**
     *	Removes the element designated by key.
     *	@param key integer hash value used as a look up key
     */
    void remove(uint64 key);

private:
    const static size_t END_OF_LIST;
    const static float MAX_LOAD_FACTOR;
    struct FindResult
    {
        size_t hash_i;
        size_t data_prev;
        size_t data_i;
    };

    bool full() const;
    void rehash(size_t new_size);
    size_t addEntry(uint64 key);
    size_t make(uint64 key);
    void insert(uint64 key, const T &value);
    void erase(FindResult const & fr);
    void grow();

    FindResult find(uint64 key) const;
    FindResult find(Entry const * e) const;
    size_t findOrFail(uint64 key) const;
    size_t findOrMake(uint64 key);
    void findAndErase(uint64 key);

private:
    Array<size_t> _hash;
    Array<Entry> _data;
};

//-----------------------------------//

template <typename T>
const size_t HashMap<T>::END_OF_LIST = SIZE_MAX;

//-----------------------------------//

template <typename T>
const float HashMap<T>::MAX_LOAD_FACTOR = 0.7f;

//-----------------------------------//

template <typename T>
HashMap<T>::HashMap(Allocator &a)
    : _hash(a)
    , _data(a)
{}

//-----------------------------------//

template <typename T>
typename HashMap<T>::Entry const * HashMap<T>::begin() const
{
    return _data.begin();
}

//-----------------------------------//

template<typename T>
typename HashMap<T>::Entry const * HashMap<T>::end() const
{
    return _data.end();
}

//-----------------------------------//

template <typename T>
size_t HashMap<T>::size() const
{
    return _data.size();
}

//-----------------------------------//

template <typename T>
bool HashMap<T>::empty() const
{
    return size() == 0;
}

//-----------------------------------//

template <typename T>
void HashMap<T>::reserve(size_t size)
{
    rehash(size);
}

//-----------------------------------//

template <typename T>
void HashMap<T>::clear()
{
    _hash.clear();
    _data.clear();
}

//-----------------------------------//

template <typename T>
T const & HashMap<T>::get(uint64 key, T const & default_value) const
{
    const size_t i = findOrFail(key);
    return i == END_OF_LIST ? default_value : _data[i].value;
}

//-----------------------------------//

template <typename T>
void HashMap<T>::set(uint64 key, T const & value)
{
    if (_hash.size() == 0)
        grow();

    const size_t i = findOrMake(key);
    _data[i].value = value;
    if (full())
        grow();
}

//-----------------------------------//

template <typename T>
bool HashMap<T>::has(uint64 key) const
{
    return findOrFail(key) != END_OF_LIST;
}

//-----------------------------------//

template <typename T>
void HashMap<T>::remove(uint64 key)
{
    findAndErase(key);
}

//-----------------------------------//

template<typename T>
bool HashMap<T>::full() const
{
    return _data.size() >= _hash.size() * MAX_LOAD_FACTOR;
}

//-----------------------------------//

template<typename T>
void HashMap<T>::rehash(size_t new_size)
{
    HashMap<T> nh(*_hash.allocator());
    nh._hash.resize(new_size);
    nh._data.reserve(_data.size());
    for (size_t i = 0; i < new_size; ++i)
        nh._hash[i] = END_OF_LIST;
    for (size_t i = 0; i < _data.size(); ++i)
    {
        const typename HashMap<T>::Entry &e = _data[i];
        nh.insert(e.key, e.value);
    }

    HashMap<T> empty(*_hash.allocator());
    clear();
    memcpy(this, &nh, sizeof(HashMap<T>));
    memcpy(&nh, &empty, sizeof(HashMap<T>));
}

//-----------------------------------//

template<typename T>
size_t HashMap<T>::addEntry(uint64 key)
{
    typename HashMap<T>::Entry e;
    e.key = key;
    e.next = END_OF_LIST;
    size_t ei = _data.size();
    _data.pushBack(e);
    return ei;
}

//-----------------------------------//

template<typename T>
size_t HashMap<T>::make(uint64 key)
{
    const FindResult fr = find(key);
    const size_t i = addEntry(key);

    if (fr.data_prev == END_OF_LIST)
        _hash[fr.hash_i] = i;
    else
        _data[fr.data_prev].next = i;

    _data[i].next = fr.data_i;
    return i;
}

//-----------------------------------//

template<typename T>
void HashMap<T>::insert(uint64 key, const T &value)
{
    if (_hash.size() == 0)
        grow();

    const size_t i = make(key);
    _data[i].value = value;
    if (full())
        grow();
}

//-----------------------------------//

template<typename T>
void HashMap<T>::erase(const FindResult &fr)
{
    if (fr.data_prev == END_OF_LIST)
        _hash[fr.hash_i] = _data[fr.data_i].next;
    else
        _data[fr.data_prev].next = _data[fr.data_i].next;

    if (fr.data_i == _data.size() - 1)
    {
        _data.popBack();
        return;
    }

    _data[fr.data_i] = _data[_data.size() - 1];
    FindResult last = find(_data[fr.data_i].key);

    if (last.data_prev != END_OF_LIST)
        _data[last.data_prev].next = fr.data_i;
    else
        _hash[last.hash_i] = fr.data_i;
}

//-----------------------------------//

template<typename T>
void HashMap<T>::grow()
{
    const size_t new_size = _data.size() * 2 + 10;
    rehash(new_size);
}

//-----------------------------------//

template<typename T>
typename HashMap<T>::FindResult HashMap<T>::find(uint64 key) const
{
    FindResult fr;
    fr.hash_i = END_OF_LIST;
    fr.data_prev = END_OF_LIST;
    fr.data_i = END_OF_LIST;

    if (_hash.size() == 0)
        return fr;

    fr.hash_i = key % _hash.size();
    fr.data_i = _hash[fr.hash_i];
    while (fr.data_i != END_OF_LIST)
    {
        if (_data[fr.data_i].key == key)
            return fr;
        fr.data_prev = fr.data_i;
        fr.data_i = _data[fr.data_i].next;
    }
    return fr;
}

//-----------------------------------//

template<typename T>
typename HashMap<T>::FindResult HashMap<T>::find(const typename HashMap<T>::Entry *e) const
{
    FindResult fr;
    fr.hash_i = END_OF_LIST;
    fr.data_prev = END_OF_LIST;
    fr.data_i = END_OF_LIST;

    if (_hash.size() == 0)
        return fr;

    fr.hash_i = e->key % _hash.size();
    fr.data_i = _hash[fr.hash_i];
    while (fr.data_i != END_OF_LIST)
    {
        if (&_data[fr.data_i] == e)
            return fr;
        fr.data_prev = fr.data_i;
        fr.data_i = _data[fr.data_i].next;
    }
    return fr;
}

//-----------------------------------//

template<typename T>
size_t HashMap<T>::findOrFail(uint64 key) const
{
    return find(key).data_i;
}

template<typename T>
size_t HashMap<T>::findOrMake(uint64 key)
{
    const FindResult fr = find(key);
    if (fr.data_i != END_OF_LIST)
        return fr.data_i;

    size_t i = addEntry(key);
    if (fr.data_prev == END_OF_LIST)
        _hash[fr.hash_i] = i;
    else
        _data[fr.data_prev].next = i;
    return i;
}

//-----------------------------------//

template <typename T>
void HashMap<T>::findAndErase(uint64 key)
{
    const FindResult fr = find(key);
    if (fr.data_i != END_OF_LIST)
        erase(fr);
}

//-----------------------------------//

NAMESPACE_CORE_END
