//
// Created by admin on 2021/11/05.
//

#ifndef MAIN_THREADUTIL_H
#define MAIN_THREADUTIL_H

enum {
    THREAD_JOINED = 1,
};

class ThreadUtil {
public:
    static ThreadUtil* self() {
        if (sHaveTLS.load(std::memory_order_acquire)) {
            special:
            ThreadUtil* threadMark = (ThreadUtil*)pthread_getspecific(sTLS);
            if (threadMark) {
                return threadMark;
            }
            return new ThreadUtil();
        }

        pthread_mutex_lock(&sTLSMutex);

        if (!sHaveTLS.load(std::memory_order_relaxed)) {
            int pthread_key_value = pthread_key_create(&sTLS, threadDestroy);
            if (pthread_key_value != 0) {
                pthread_mutex_unlock(&sTLSMutex);
                return nullptr;
            }

            sHaveTLS.store(true, std::memory_order_release);
        }
        pthread_mutex_unlock(&sTLSMutex);
        goto special;
    }

    bool hasMask(uint32_t flag) {
        return mMask & flag;
    }

    void addMask(uint32_t flag) {
        mMask |= flag;
    }

    void removeMask(uint32_t flag) {
        mMask &= ~flag;
    }

private:
    ThreadUtil() : mMask(0) {
        pthread_setspecific(sTLS, this);
    }

    ~ThreadUtil() {
    }

    static void threadDestroy(void* tm) {
        ThreadUtil* mark = (ThreadUtil*)tm;
        delete mark;
    }


    uint32_t mMask;

    static pthread_key_t sTLS;
    static pthread_mutex_t sTLSMutex;
    static std::atomic<bool> sHaveTLS;

    ThreadUtil(ThreadUtil&) = delete;
    ThreadUtil(ThreadUtil&&) = delete;
};

pthread_mutex_t ThreadUtil::sTLSMutex = PTHREAD_MUTEX_INITIALIZER;
std::atomic<bool> ThreadUtil::sHaveTLS = false;
pthread_key_t ThreadUtil::sTLS = 0;

#endif //MAIN_THREADUTIL_H
