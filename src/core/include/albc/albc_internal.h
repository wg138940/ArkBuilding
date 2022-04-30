﻿#pragma once
#ifndef ALBC_INTERNAL_H
#define ALBC_INTERNAL_H
#include "albc_common.h"
#include <new>
#include <cstddef>

#if !defined(__cplusplus)
#   error "Use 'calbc.h' instead of 'albc.h' for C code."
#endif

#if __cplusplus >= 201703L
#   define ALBC_NODISCARD [[nodiscard]]
#   define ALBC_MAYBE_UNUSED [[maybe_unused]]
#else
#   define ALBC_NODISCARD
#   define ALBC_MAYBE_UNUSED
#endif

#ifndef ALBC_E_PTR
#   define ALBC_E_PTR AlbcException **e_ptr = nullptr
#endif

#ifdef ALBC_EXPORTS
#   define ALBC_API ALBC_MAYBE_UNUSED ALBC_EXPORT
#   define ALBC_API_CLASS ALBC_MAYBE_UNUSED ALBC_EXPORT_CLASS
#   define ALBC_API_MEMBER ALBC_MAYBE_UNUSED ALBC_EXPORT_MEMBER
#else
#   define ALBC_API ALBC_MAYBE_UNUSED ALBC_IMPORT
#   define ALBC_API_CLASS ALBC_MAYBE_UNUSED ALBC_IMPORT_CLASS
#   define ALBC_API_MEMBER ALBC_MAYBE_UNUSED ALBC_IMPORT_MEMBER
#endif

#define ALBC_PIMPL                                                                                                     \
  public:                                                                                                              \
    class Impl;                                                                                                        \
    Impl *impl_;                                                                                                       \
    ALBC_NODISCARD ALBC_MAYBE_UNUSED const Impl* impl() const noexcept { return impl_; }                               \

#define ALBC_MEM_DELEGATE                                                                                              \
  public:                                                                                                              \
        void* operator new(std::size_t size, std::nothrow_t) noexcept { return albc::malloc(size); }                   \
        void* operator new[](std::size_t size, std::nothrow_t) noexcept { return albc::malloc(size); }                 \
        void* operator new(std::size_t size) noexcept { return albc::malloc(size); }                                   \
        void* operator new[](std::size_t size) noexcept { return albc::malloc(size); }                                 \
        void operator delete(void* ptr) noexcept { albc::free(ptr); }                                                  \
        void operator delete[](void* ptr) noexcept { albc::free(ptr); }

namespace albc
{
template <typename T>
using RefPtr = T*;

ALBC_NODISCARD ALBC_API void* malloc(std::size_t size) noexcept;
ALBC_API void free(void* ptr) noexcept;
ALBC_NODISCARD ALBC_API void* realloc(void* ptr, std::size_t size) noexcept;

template <typename T>
class FwdIterator
{
  public:
    explicit FwdIterator(T *ptr) noexcept : ptr(ptr)  {}
    FwdIterator(const FwdIterator &other) noexcept : ptr(other.ptr) {}
    ~FwdIterator() noexcept = default;
    FwdIterator &operator=(const FwdIterator &other) noexcept { if (this != &other) ptr = other.ptr; return *this; }
    FwdIterator &operator++() noexcept { ++ptr; return *this; }
    const FwdIterator operator++(int) noexcept { FwdIterator tmp(*this); ++ptr; return tmp; }
    bool operator==(const FwdIterator &other) const noexcept { return ptr == other.ptr; }
    bool operator!=(const FwdIterator &other) const noexcept { return ptr != other.ptr; }
    T& operator*() const noexcept { return *ptr; }
    T* operator->() const noexcept { return ptr; }
    explicit operator bool() const noexcept { return ptr != nullptr; }
    explicit operator T*() const noexcept { return ptr; }

  private:
    T* ptr;
};

class ALBC_API_CLASS ICollectionBase
{
  public:
    ALBC_NODISCARD ALBC_API_MEMBER virtual int GetCount() const noexcept = 0;
    ALBC_API_MEMBER virtual void ForEach(AlbcForEachCallback callback, void *user_data) const noexcept = 0;
    ALBC_API_MEMBER virtual ~ICollectionBase() noexcept = default;

    ALBC_MEM_DELEGATE
};

template<typename T, template <class> typename TIter = FwdIterator>
class ALBC_API_CLASS ICollection : public ICollectionBase
{
  public:
    using const_iterator = TIter<const T>;
    using iterator = TIter<T>;
    using value_type = T;

    ALBC_NODISCARD ALBC_API_MEMBER virtual const_iterator begin() const noexcept = 0;
    ALBC_NODISCARD ALBC_API_MEMBER virtual const_iterator end() const noexcept = 0;
    ALBC_NODISCARD ALBC_API_MEMBER virtual iterator begin() noexcept = 0;
    ALBC_NODISCARD ALBC_API_MEMBER virtual iterator end() noexcept = 0;
    ALBC_NODISCARD ALBC_API_MEMBER int GetCount() const noexcept override
    {
        int count = 0;
        for (auto it = begin(); it != end(); ++it)
            ++count;
        return count;
    }
    void ForEach(AlbcForEachCallback callback, void *user_data) const noexcept override
    {
        int i = 0;
        for (auto it = begin(); it != end(); ++it, ++i)
            callback(i, &*it, user_data);
    }
    ALBC_MEM_DELEGATE
};

/**
 * 该字符串类是对STL字符串的简单封装，提供了一些常用的操作，为了避免ABI问题。
 * API规范中，字符串入参统一使用UTF8 const char*，出参统一使用UTF8 String。
 */
class ALBC_API_CLASS String
{
  public:
    ALBC_API_MEMBER explicit String() noexcept;
    ALBC_API_MEMBER explicit String(const char *str) noexcept;
    ALBC_API_MEMBER String(const String &str) noexcept;
    ALBC_API_MEMBER String(String &&str) noexcept;
    ALBC_API_MEMBER ~String() noexcept;
    
    ALBC_API_MEMBER String &operator=(const char *str) noexcept;
    ALBC_API_MEMBER String &operator=(const String &str) noexcept;
    ALBC_API_MEMBER String &operator=(String &&str) noexcept;
    ALBC_NODISCARD ALBC_API_MEMBER const char *c_str() const noexcept;
    ALBC_NODISCARD ALBC_API_MEMBER std::size_t size() const noexcept;

    ALBC_PIMPL
    ALBC_MEM_DELEGATE
};

}
#endif //ALBC_INTERNAL_H