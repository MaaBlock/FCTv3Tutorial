#ifndef ANDROIDGLINVESTIGATIONS_ANDROIDOUT_H
#define ANDROIDGLINVESTIGATIONS_ANDROIDOUT_H

#include <android/log.h>
#include <sstream>

extern std::ostream aout;
extern std::ostream aerr;

class AndroidOut: public std::stringbuf {
public:
    AndroidOut(const char* logTag) : m_logTag(logTag){

    }
protected:
    virtual int sync() override {
        __android_log_print(ANDROID_LOG_DEBUG, m_logTag, "%s", str().c_str());
        str("");
        return 0;
    }

private:
    const char* m_logTag;
};

class AndroidErr: public std::stringbuf {
public:
    AndroidErr(const char* kLogTag) : m_logTag(kLogTag){}
protected:
    virtual int sync() override {
        __android_log_print(ANDROID_LOG_ERROR, m_logTag, "%s", str().c_str());
        str("");
        return 0;
    }

private:
    const char* m_logTag;
};

#endif //ANDROIDGLINVESTIGATIONS_ANDROIDOUT_H