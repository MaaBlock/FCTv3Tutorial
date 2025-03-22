//
// Created by Administrator on 2025/3/16.
//
#include "../FCTAPI.h"
#include "Context.h"

namespace FCT {

    void Context::nextFrame() {
        m_nextFrame = true;
    }

    void Context::currentFlush() {
        m_currentFlush = true;
    }

    void Context::swapQueue() {
        std::swap(m_pushQueue,m_submitQueue);
    }

    void Context::submitThread() {
        while (m_ctxRunning) {
            compilePasses();
            submitPasses();
            if (m_flushWnd){
                m_flushWnd->swapBuffers();
            }
            FCT_WAIT_FOR(m_nextFrame);
        }
    }

    void Context::flush() {
        FCT_WAIT_FOR(m_currentFlush);
        swapQueue();
        nextFrame();
    }

    Context::Context() {
        m_ctxRunning = true;
        m_flushWnd = nullptr;
        m_nextFrame = false;
        m_currentFlush = true;
        m_submitQueue = &m_passQueue0;
        m_pushQueue = &m_passQueue1;
        m_submitThread = std::thread(&Context::submitThread, this);
    }

    Context::~Context() {
        m_ctxRunning = false;
        m_submitThread.join();
    }
}