//
// Created by liuzhuangzhuang on 2021/11/05.
//

#include <jni.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unordered_set>
#include <sys/system_properties.h>
#include <fcntl.h>
#include <future>
#include <unistd.h>
#include <map>
#include "base/ThreadPool.h"

#include "base/ThreadUtil.h"
#include "base/ScopedGuard.h"
#include "base/ScopedLocalUtfStr.h"
#include "base/unique_fd.h"

#define LOG_TAG "LibEntry"
#include "base/log.h"

#if defined(ENABLE_BACKTRACER)
#include "backtrace/CallStack.h"
#endif

#if defined(ENABLE_JSON_LIB)
#include "json/json.h"
#endif

#define LIKELY( exp )       (__builtin_expect( (exp) != 0, true  ))
#define UNLIKELY( exp )     (__builtin_expect( (exp) != 0, false ))

#ifndef PAGE_SIZE
#define PAGE_SIZE 4096
#endif
#define PAGE_START(x) ((x) & PAGE_MASK)
#define PAGE_OFFSET(x) ((x) & ~PAGE_MASK)
#define PAGE_END(x) PAGE_START((x) + (PAGE_SIZE-1))

typedef union {
    JNIEnv* env;
    void* venv;
} UnionJNIEnvToVoid;

using ClassFuncArray = std::map<std::string, std::unordered_set<std::string>>;

jclass ThrowableClass;
jmethodID ThrowableConstructor;
jmethodID PrintStackTrace;

static JavaVM* javaVm;
static const char* sPackageName = nullptr;

static std::unordered_set<std::string> sJavaTraces;
static ThreadPool* sThreadPool;

#define BEGIN_ENV(JVM) \
    JNIEnv* env; \
    int status = JVM->GetEnv((void **) &env, JNI_VERSION_1_6); \
    bool attached = false; \
    if (status != JNI_OK) { \
        status = JVM->AttachCurrentThread(&env, NULL); \
        if (status < 0) { \
            return; \
        } \
        attached = true; \
    }

#define END_ENV(JVM) \
    if (attached) { \
        JVM->DetachCurrentThread(); \
    }

#if defined(ENABLE_LOGGING) && 0
static void printJavaStackTrace() {
    if (ThrowableClass && ThrowableConstructor && PrintStackTrace) {
        BEGIN_ENV(javaVm)
        {
            jstring tmpStr = env->NewStringUTF("INTEREST-JAVA");
            jobject instance = env->NewObject(ThrowableClass, ThrowableConstructor, tmpStr);
            env->CallVoidMethod(instance, PrintStackTrace);

            env->DeleteLocalRef(instance);
            env->DeleteLocalRef(tmpStr);
        } END_ENV(javaVm)
    }
}
#endif

static inline int getPlatformVersion() {
    static int platformVersion = 0;
    if (platformVersion == 0) {
        char platformVersionChar[PROP_VALUE_MAX];
        __system_property_get("ro.build.version.sdk", platformVersionChar);
        platformVersion = atoi(platformVersionChar);
    }
    return platformVersion;
}

std::string getProcessName() {
    std::unique_ptr<unique_fd> file(new unique_fd(
            TEMP_FAILURE_RETRY(open("/proc/self/cmdline",
                                    O_RDONLY|O_CLOEXEC))));
    int fd = file->get();
    struct stat _stat;
    std::string content;
    ssize_t n = 0;
    char buf[BUFSIZ] __attribute__((__uninitialized__));
    if (fstat(fd, &_stat) != -1 && _stat.st_size > 0) {
        content.reserve(_stat.st_size);
    }

    while ((n = TEMP_FAILURE_RETRY(read(fd, buf, BUFSIZ))) > 0) {
        content.append(buf, n);
    }
    return content;
}

ClassFuncArray parseJsonFile(JNIEnv* env, jstring jpath) {

    ClassFuncArray ret;

#if defined(ENABLE_JSON_LIB)

    if (jpath == nullptr) {
        ALOGD("parseJsonFile failed for jpath NULL");
        return ret;
    }

    /*1.BEGIN open json file*******/
    ScopedLocalUtfStr path(env, jpath);
    std::unique_ptr<unique_fd> jsonFile(new unique_fd(open(path.c_str(), O_RDONLY)));
    int fd = jsonFile->get();

    /*1.END open json file*******/

    if (fd <= 0) {
        ALOGD("parseJsonFile failed for invalid fd");
        return ret;
    }

    struct stat _stat;
    if (fstat(fd, &_stat) != 0) {
        ALOGE("parseJsonFile fstat failed");
        return ret;
    }
    size_t mapSize = PAGE_END(_stat.st_size);

    void* const mapAddr = mmap(NULL, mapSize, PROT_READ, MAP_SHARED, fd, 0);

    /*2.BEGIN parse json file********/
    Json::Reader reader;
    Json::Value root;
    if (!reader.parse((const char*)mapAddr, (const char*)((long)mapAddr + _stat.st_size), root)) {
        ALOGE("parseJsonFile failed for invalid file");
        return ret;
    }

    /*2.END parse json file********/

    /*3.BEGIN check json********/

    if (root["enable"].empty()) {
        ALOGE("json doesn't have \"tag\" node!");
        return ret;
    }

    if (root["package"].empty()) {
        ALOGE("json doesn't hava \"package\" node");
        return ret;
    }

    const Json::Value packageArray = root["package"];

#if defined(ENABLE_LOGGING)
    std::string DumpStrPkgs("[ ");
    for (Json::Value::iterator pkgIt = packageArray.begin(); pkgIt != packageArray.end(); ++pkgIt) {
        std::string package = pkgIt->asString();
        DumpStrPkgs.append(package + " ");
    }
    DumpStrPkgs.append("]");
    ALOGD("json parse packages: %s", DumpStrPkgs.c_str());
#endif

    if (root["objs"].empty()) {
        ALOGE("json doesn't have valid \"objs\" node");
        return ret;
    }
    const Json::Value javaNode = root["objs"]["java"];
    if (javaNode.empty()) {
        ALOGE("json doesn't have valid \"java\" node");
        return ret;
    }
    /*3.END check json********/

    const Json::Value::Members javaClasses = javaNode.getMemberNames();

    for (std::vector<std::string>::const_iterator jcls = javaClasses.begin(); jcls != javaClasses.end(); ++jcls) {
        //std::string className = *it;
        const Json::Value methodArray = javaNode[*jcls];

        for (Json::Value::iterator jmid = methodArray.begin(); jmid != methodArray.end(); ++jmid) {
            std::string method = jmid->asString();
            if (ret[*jcls].insert(method).second) {
                ALOGD("insert javaMethod: %s.%s", jcls->c_str(), method.c_str());
                std::string tmp = *jcls + "." + method;
            } else {
                ALOGW("config javaMethod: %s.%s!!! Already inserted?",
                      jcls->c_str(), method.c_str());
            }
        }
    }

    ALOGD("parseConfigFile success");
#else
    ALOGW("parseConfigFile not supported");
#endif
    return ret;
}

static bool testJsonMethods(JNIEnv* env, ClassFuncArray funcArray) {

    jclass _class = env->FindClass("java/lang/Class");
    jmethodID getDeclaredMethods = env->GetMethodID(_class, "getDeclaredMethods", "()[Ljava/lang/reflect/Method;");

    jclass methodClass = env->FindClass("java/lang/reflect/Method");
    jmethodID getMethodName = env->GetMethodID(methodClass, "getName", "()Ljava/lang/String;");

#if defined(ENABLE_LOGGING)
    jmethodID getClassName = env->GetMethodID(_class, "getName", "()Ljava/lang/String;");
    jmethodID getParameterTypes = env->GetMethodID(methodClass, "getParameterTypes",
                                                   "()[Ljava/lang/Class;");
#endif

    for(std::map<std::string, std::unordered_set<std::string>>::iterator it = funcArray.begin(); it != funcArray.end(); ++it) {
        std::string className = it->first;
        std::unordered_set<std::string> funcs = it->second;

        std::string descriptor(className);
        std::replace(descriptor.begin(), descriptor.end(), '.', '/');

        jclass hclass = env->FindClass(descriptor.c_str());
        jobjectArray methodArray = (jobjectArray) env->CallObjectMethod(hclass, getDeclaredMethods);
        jsize arrayLen = env->GetArrayLength(methodArray);

        for (jsize i = 0; i < arrayLen; i++) {
            jobject jmethod = env->GetObjectArrayElement(methodArray, i);
            jstring jname = (jstring) env->CallObjectMethod(jmethod, getMethodName);

            const char* name = env->GetStringUTFChars(jname, 0);

            if (funcs.count(name)) {
#if defined(ENABLE_LOGGING)
                jobjectArray paramsArray = (jobjectArray) env->CallObjectMethod(jmethod, getParameterTypes);
                jsize paramsLen = env->GetArrayLength(paramsArray);
                std::string params;

                for (jsize j = 0; j < paramsLen; j++) {
                    jobject jparam = env->GetObjectArrayElement(paramsArray, j);
                    jstring jparamTypeName = (jstring) env->CallObjectMethod(jparam, getClassName);
                    const char* paramTypeName = env->GetStringUTFChars(jparamTypeName, 0);

                    params.append(paramTypeName);
                    if (j != paramsLen -1) {
                        params.append(", ");
                    }

                    env->ReleaseStringUTFChars(jparamTypeName, paramTypeName);
                    env->DeleteLocalRef(jparamTypeName);
                    env->DeleteLocalRef(jparam);
                }

                ALOGD("testJsonMethods %s(%s)", name, params.c_str());

                env->DeleteLocalRef(paramsArray);
#endif
            }

            env->ReleaseStringUTFChars(jname, name);
            env->DeleteLocalRef(jmethod);
            env->DeleteLocalRef(jname);
        }

        env->DeleteLocalRef(methodArray);
        env->DeleteLocalRef(hclass);
    }

    env->DeleteLocalRef(_class);
    env->DeleteLocalRef(methodClass);
    ALOGI("testJsonMethods done!");
    return true;
}

static void nativeTestJson(JNIEnv* env, jclass, jstring jpath) {
    ClassFuncArray funcArray = parseJsonFile(env, jpath);
    testJsonMethods(env, funcArray);
}

static void nativeTestUnw(JNIEnv* env, jclass) {
    CallStack::log("nativeTestUnw");
}

static const char *classPathName = "com/whulzz/demo/DemoApplication";

static JNINativeMethod methods[] = {
        {"testJson", "(Ljava/lang/String;)V", (void *) nativeTestJson},
        {"testUnw", "()V", (void*) nativeTestUnw}
};

static int registerNativeMethods(JNIEnv* env, const char* className,
                                 const JNINativeMethod* gMethods, int numMethods)
{
    jclass clazz = env->FindClass(className);
    if (clazz == NULL) {
        ALOGE("Native registration unable to find class '%s'", className);
        return JNI_FALSE;
    }
    if (env->RegisterNatives(clazz, gMethods, numMethods) < 0) {
        ALOGE("RegisterNatives failed for '%s'", className);
        return JNI_FALSE;
    }

    return JNI_TRUE;
}

static int registerNatives(JNIEnv* env)
{
    if (!registerNativeMethods(env, classPathName,
                               methods, sizeof(methods) / sizeof(methods[0]))) {
        return JNI_FALSE;
    }

    return JNI_TRUE;
}

jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    UnionJNIEnvToVoid uenv;
    uenv.venv = NULL;
    jint result = -1;
    JNIEnv* env = NULL;

    ALOGD("JNI_OnLoad");
    javaVm = vm;

    if (vm->GetEnv(&uenv.venv, JNI_VERSION_1_6) != JNI_OK) {
        ALOGE("ERROR: GetEnv failed");
        goto bail;
    }
    env = uenv.env;

    if (!registerNatives(env)) {
        goto bail;
    }

    result = JNI_VERSION_1_6;

bail:
    return result;
}
