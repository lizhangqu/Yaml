//
// Created by 李樟取 on 2017/5/13.
//

#ifndef YAML_YAML_CPP_H
#define YAML_YAML_CPP_H

#include "jni.h"
#include "iostream"

#ifndef NELEM
# define NELEM(x) ((int) (sizeof(x) / sizeof((x)[0])))
#endif

#define TAG "YAML"

#ifdef ANDROID

#include <android/log.h>

#define ALOGE(fmt, ...) __android_log_print(ANDROID_LOG_ERROR, TAG, fmt, ##__VA_ARGS__)
#else
#define ALOGE printf
#endif

#ifndef CLASSNAME
#define CLASSNAME "io/github/lizhangqu/yaml/Yaml"
#endif


#endif //YAML_YAML_CPP_H
