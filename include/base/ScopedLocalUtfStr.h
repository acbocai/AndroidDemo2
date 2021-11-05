//
// Created by liuzhuangzhuang.das on 2021/11/05.
//

#ifndef MAIN_SCOPEDLOCALUTFSTR_H
#define MAIN_SCOPEDLOCALUTFSTR_H
#include <unistd.h>
#include <jni.h>
#include <string.h>

#if !defined(DISALLOW_COPY_AND_ASSIGN)
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName&) = delete;  \
    void operator=(const TypeName&) = delete
#endif

class ScopedLocalUtfStr {
public:
    ScopedLocalUtfStr(JNIEnv* env, jstring s) : env_(env), _jstr(s) {
        if (s == NULL) {
            _utf_chars = NULL;
        } else {
            _utf_chars = env->GetStringUTFChars(s, NULL);
        }
    }

    ~ScopedLocalUtfStr() {
        if (_utf_chars) {
            env_->ReleaseStringUTFChars(_jstr, _utf_chars);
        }
    }

    const char* c_str() const {
        return _utf_chars;
    }

    size_t size() const {
        return strlen(_utf_chars);
    }

    const char& operator[](size_t n) const {
        return _utf_chars[n];
    }

private:
    JNIEnv* const env_;
    const jstring _jstr;
    const char* _utf_chars;

    DISALLOW_COPY_AND_ASSIGN(ScopedLocalUtfStr);
};


#endif //MAIN_SCOPEDLOCALUTFSTR_H
