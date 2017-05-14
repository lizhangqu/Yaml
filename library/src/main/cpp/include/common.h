//
// Copyright RIME Developers
// Distributed under the BSD License
//
// 2011-03-14 GONG Chen <chen.sst@gmail.com>
//
#ifndef COMMON_H_
#define COMMON_H_

#include <functional>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <utility>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
#include <iostream>

#define TAG "YAML"

#ifdef ANDROID

#include <android/log.h>

#define ALOGE(fmt, ...) __android_log_print(ANDROID_LOG_ERROR, TAG, fmt, ##__VA_ARGS__)
#define ALOGI(fmt, ...) __android_log_print(ANDROID_LOG_INFO, TAG, fmt, ##__VA_ARGS__)
#define ALOGD(fmt, ...) __android_log_print(ANDROID_LOG_DEBUG, TAG, fmt, ##__VA_ARGS__)
#define ALOGW(fmt, ...) __android_log_print(ANDROID_LOG_WARN, TAG, fmt, ##__VA_ARGS__)
#else
#define ALOGE printf
#define ALOGI printf
#define ALOGD printf
#define ALOGW printf
#endif

namespace yaml {

    template<class Key, class T>
    using hash_map = std::unordered_map<Key, T>;
    template<class T>
    using hash_set = std::unordered_set<T>;

    template<class T>
    using the = std::unique_ptr<T>;
    template<class T>
    using an = std::shared_ptr<T>;
    template<class T>
    using of = an<T>;
    template<class T>
    using weak = std::weak_ptr<T>;

    template<class X, class Y>
    inline an<X> As(const an<Y> &ptr) {
        return std::dynamic_pointer_cast<X>(ptr);
    }

    template<class X, class Y>
    inline bool Is(const an<Y> &ptr) {
        return bool(As<X, Y>(ptr));
    }

    template<class T, class... Args>
    inline an<T> New(Args &&... args) {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }


    inline bool starts_with(const std::string &s1, const std::string &s2) {
        return s2.size() <= s1.size() && s1.compare(0, s2.size(), s2) == 0;
    }

}  // namespace yaml

#endif  // COMMON_H_
