//
// Created by 李樟取 on 2017/5/13.
//

#include <sstream>
#include "yaml_native.h"
#include "yaml.h"

jstring list(JNIEnv *env, jobject thiz, jstring value) {
    yaml::Yaml yaml;
    yaml.SetBool("flag", true);
    yaml.SetString("str", "just a test");
    yaml.SetInt("num", 9);
    std::ostringstream ostringstream;
    yaml.SaveToStream(ostringstream);
    return env->NewStringUTF(ostringstream.str().c_str());
}

static const JNINativeMethod sMethods[] = {
        {
                const_cast<char *>("list"),
                const_cast<char *>("(Ljava/lang/String;)Ljava/lang/String;"),
                reinterpret_cast<void *>(list)
        },

};

int registerNativeMethods(JNIEnv *env, const char *className, const JNINativeMethod *methods,
                          const int numMethods) {
    jclass clazz = env->FindClass(className);
    if (!clazz) {
        ALOGE("Native registration unable to find class '%s'\n", className);
        return JNI_FALSE;
    }
    if (env->RegisterNatives(clazz, methods, numMethods) != 0) {
        ALOGE("RegisterNatives failed for '%s'\n", className);
        env->DeleteLocalRef(clazz);
        return JNI_FALSE;
    }
    env->DeleteLocalRef(clazz);
    return JNI_TRUE;
}

jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env;
    if (vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }
    registerNativeMethods(env, CLASSNAME, sMethods, NELEM(sMethods));
    return JNI_VERSION_1_6;
}
