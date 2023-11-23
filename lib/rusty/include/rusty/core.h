#pragma once

#include <algorithm>
#include <variant>
#include <vector>
#include <string>
#include <memory>
#include <cstdlib>
#include <cstdint>
#include <cstdio>


using i8  = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;
using u8  = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
using f32 = float;
using f64 = double;
using usize = size_t;
using isize = i64;
struct Unit {};

#define loop while(true)
#define panic(...) printf(__VA_ARGS__); exit(1)

#ifdef RUSTY_SYNTAX
#define str const char
#define let auto
#define fn auto
#endif


namespace core {

template<typename T, typename E>
class Result;

template<typename T>
class Option
{
private:
    enum { VSome, VNone, } tag;
    std::variant<T, Unit> data;

    Option(T val): tag(Option::VSome), data(val) {}
    Option():      tag(Option::VNone), data(Unit{})  {}

public:
    using Some_t = T;

    static Option Some(T val) { return Option(val); }
    static Option None() { return Option(); }

    bool is_some() const { return tag == Option::VSome; }
    bool is_none() const { return tag == Option::VNone; }

    T unwrap()                     { if (is_some()) { return std::get<T>(data); } else { panic("called 'Option::unwrap' on a 'None' value"); } }
    T unwrap_or(const T& fallback) { if (is_some()) { return std::get<T>(data); } else { return fallback; } }
    T expect(const char* msg)      { if (is_some()) { return std::get<T>(data); } else { panic(msg); } }

    template<typename E>
    Option<T> ok_or(E err) { if (is_some()) { return Result<T, E>::Ok(std::get<T>(data)); } else { return Result<T, E>::Err(err); } }

    template<typename U>
    Option<U> map(U(*func)(T)) { if (is_some()) { return Option<U>::Some(func(std::get<T>(data))); } else { return Option<U>::None(); } }
    template<typename U>
    Option<U> and_then(Option<U>(*func)(T)) { if (is_some()) { return func(std::get<T>(data)); } else { return Option<U>::None(); } }

    Option<const T*> as_ref() const { if (is_some()) { return Option<const T*>::Some(&std::get<T>(data)); } else { return Option<const T*>::None(); } }
    Option<T*> as_mut() { if (is_some()) { return Option<T*>::Some(&std::get<T>(data)); } else { return Option<T*>::None(); } }

    const T* if_let() const { if (is_some()) { return &std::get<T>(data); } else { return nullptr; } }
    T* if_let_mut() { if (is_some()) { return &std::get<T>(data); } else { return nullptr; } }
};


template<typename T, typename E>
class Result
{
private:
    enum { VOk, VErr, } tag;
    std::variant<T, E> data;

    Result(T val):       tag(Result::VOk),  data(val) {}
    Result(E err, Unit): tag(Result::VErr), data(err) {}

public:
    using Ok_t = T;
    using Err_t = E;

    static Result Ok(T val) { return Result(val); }
    static Result Err(E err) { return Result(err, {}); }

    bool is_ok()  const { return tag == Result::VOk; }
    bool is_err() const { return tag == Result::VErr; }

    T unwrap()                     { if (is_ok())  { return std::get<T>(data); } else { panic("called 'Result::unwrap' on an 'Err' value: %s", std::get<E>(data).to_string()); } }
    T unwrap_or(const T& fallback) { if (is_ok())  { return std::get<T>(data); } else { return fallback; } }
    T expect(const char* msg)      { if (is_ok())  { return std::get<T>(data); } else { panic(msg); } }
    T unwrap_err()                 { if (is_err()) { return std::get<E>(data); } else { panic("called 'Result::unwrap_err' on an 'Ok' value"); } }

    Option<T> ok()  { if (is_ok())  { return Option<T>::Some(std::get<T>(data)); } else { return Option<T>::None(); } }
    Option<E> err() { if (is_err()) { return Option<E>::Some(std::get<E>(data)); } else { return Option<E>::None(); } }

    template<typename U>
    Result<U, E> map(U(*func)(T)) { if (is_ok()) { return Result<U, E>::Ok(func(std::get<T>(data))); } else { return Result<U, E>::Err(std::get<E>(data)); } }
    template<typename F>
    Result<T, F> map_err(F(*func)(E)) { if (is_ok()) { return Result<T, F>::Ok(std::get<T>(data)); } else { return Result<T, F>::Err(func(std::get<E>(data))); } }
    template<typename U>
    Result<U, E> and_then(Result<U, E>(*func)(T)) { if (is_ok()) { return func(std::get<T>(data)); } else { return Result<U, E>::Err(std::get<E>(data)); } }

    const T* if_let() const { if (is_ok()) { return &std::get<T>(data); } else { return nullptr; } }
    T* if_let_mut() { if (is_ok()) { return &std::get<T>(data); } else { return nullptr; } }
};

#define prop_opt(opt, expr) if (auto inner = opt.if_let()) { expr; } else { return opt; }
#define prop_res(res, expr) if (auto inner = res.if_let()) { expr; } else { return res; }


template<typename T>
using Box = std::unique_ptr<T>;

using String = std::string;

template<typename T>
class Vec
{
private:
    std::vector<T> impl;

public:
    Vec(): impl(std::vector<T>()) {}
    Vec(const std::vector<T>& val): impl(val) {}
    static Vec make() { return { std::vector<T>() }; }
    static Vec with_capacity(usize cap) { auto res = make(); res.reserve(cap); return res; }

    T& operator[](usize index) { return impl[index]; }
    const T& operator[](usize index) const { return impl[index]; }

    typename std::vector<T>::iterator begin() { return impl.begin(); }
    typename std::vector<T>::const_iterator begin() const { return impl.cbegin(); }
    typename std::vector<T>::const_iterator cbegin() const { return impl.cbegin(); }
    typename std::vector<T>::iterator end() { return impl.end(); }
    typename std::vector<T>::const_iterator end() const { return impl.cend(); }
    typename std::vector<T>::const_iterator cend() const { return impl.cend(); }

    void reserve(usize cap) {
        impl.reserve(cap);
    }
    void push(T val) {
        impl.push_back(val);
    }
    Option<const T*> get(usize index) const {
        if (index < impl.size()) {
            return Option<const T*>::Some(&impl[index]);
        } else {
            return Option<const T*>::None();
        }
    }
    Option<T*> get_mut(usize index) {
        if (index < impl.size()) {
            return Option<T*>::Some(&impl[index]);
        } else {
            return Option<T*>::None();
        }
    }
    Option<const T*> first() const {
        return get(0);
    }
    Option<T*> first_mut() {
        return get_mut(0);
    }
    Option<const T*> last() const {
        return get(len() - 1);
    }
    Option<T*> last_mut() {
        return get_mut(len() - 1);
    }
    Option<T> pop() {
        if (impl.empty()) {
            return Option<T>::None();
        } else {
            T temp = impl.back();
            impl.pop_back();
            return Option<T>::Some(temp);
        }
    }
    Option<T> remove(usize index) {
        if (impl.empty()) {
            return Option<T>::None();
        } else {
            T temp = impl[index];
            impl.erase(impl.begin() + index);
            return Option<T>::Some(temp);
        }
    }
    void clear() { impl.clear(); }
    bool is_empty() const { return impl.empty(); }
    usize len() const { return impl.size(); }
    void sort() { std::sort(impl.begin(), impl.end()); }
    void sort_by(bool(*func)(const T, const T)) { std::sort(impl.begin(), impl.end(), func); }
    void sort_by(bool(*func)(const T&, const T&)) { std::sort(impl.begin(), impl.end(), func); }

    std::vector<T>& inner() { return impl; }
};

}

