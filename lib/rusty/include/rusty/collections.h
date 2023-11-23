#pragma once

#include <unordered_map>
#include <unordered_set>
#include <memory>
#include "core.h"


namespace core {

template<typename T>
using Arc = std::shared_ptr<T>;


namespace collections {

template<typename K, typename V>
class HashMap
{
private:
    std::unordered_map<K, V> impl;

    HashMap(const std::unordered_map<K, V>& val): impl(val) {}
public:
    HashMap(): impl(std::unordered_map<K, V>()) {}
    static HashMap make() { return { std::unordered_map<K, V>() }; }

    typename std::unordered_map<K, V>::iterator begin() { return impl.begin(); }
    typename std::unordered_map<K, V>::const_iterator begin() const { return impl.cbegin(); }
    typename std::unordered_map<K, V>::const_iterator cbegin() const { return impl.cbegin(); }
    typename std::unordered_map<K, V>::iterator end() { return impl.end(); }
    typename std::unordered_map<K, V>::const_iterator end() const { return impl.cend(); }
    typename std::unordered_map<K, V>::const_iterator cend() const { return impl.cend(); }

    bool contains_key(const K& key) const {
        return impl.find(key) != impl.end();
    }
    Option<V> insert(const K& key, const V& val) {
        if (contains_key(key)) {
            impl[key] = val;
            return Option<V>::None();
        } else {
            auto old = impl[key];
            impl[key] = val;
            return Option<V>::Some(old);
        }
    }
    Option<const V*> get(const K& key) const {
        if (contains_key(key)) {
            return Option<const V*>::Some(&impl.at(key));
        } else {
            return Option<const V*>::None();
        }
    }
    Option<V*> get_mut(const K& key) {
        if (contains_key(key)) {
            return Option<V*>::Some(&impl[key]);
        } else {
            return Option<V*>::None();
        }
    }
    Option<V> remove(const K& key) {
        if (contains_key(key)) {
            V temp = impl[key];
            impl.erase(key);
            return Option<V>::Some(temp);
        } else {
            return Option<V>::None();
        }
    }
    void clear() { impl.clear(); }
    bool is_empty() const { return impl.empty(); }
    usize len() const { return impl.size(); }

    std::unordered_map<K, V>& inner() { return impl; }
};

template<typename T>
class HashSet
{
private:
    std::unordered_set<T> impl;

    HashSet(const std::unordered_set<T>& val): impl(val) {}
public:
    HashSet(): impl(std::unordered_set<T>()) {}
    static HashSet make() { return { std::unordered_set<T>() }; }

    typename std::unordered_set<T>::iterator begin() { return impl.begin(); }
    typename std::unordered_set<T>::const_iterator begin() const { return impl.cbegin(); }
    typename std::unordered_set<T>::const_iterator cbegin() const { return impl.cbegin(); }
    typename std::unordered_set<T>::iterator end() { return impl.end(); }
    typename std::unordered_set<T>::const_iterator end() const { return impl.cend(); }
    typename std::unordered_set<T>::const_iterator cend() const { return impl.cend(); }

    bool contains(const T& val) const {
        return impl.find(val) != impl.end();
    }
    void insert(const T& val) {
        impl.insert(val);
    }
    void remove(const T& val) {
        if (contains(val)) {
            impl.erase(val);
        }
    }
    void clear() { impl.clear(); }
    bool is_empty() const { return impl.empty(); }
    usize len() const { return impl.size(); }

    std::unordered_set<T>& inner() { return impl; }
};

}

}

