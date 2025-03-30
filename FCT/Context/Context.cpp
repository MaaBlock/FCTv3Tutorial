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
    namespace test
    {
        struct TestState
        {
            bool isInited = false;
            Context* ctx;
            Window* wnd;
            RHI::Pass* pass;
            RHI::PassGroup* passGroup;
            void init(Context* ctx)
            {
                thio->ctx = ctx;
                isInited = true;
                wnd = ctx->flushWindow();
                pass = ctx->createPass();
                passGroup = ctx->createPassGroup();
            }
            void tick(Context* ctx)
            {
                if (!isInited) {
                    init(ctx);
                }
            }
        } state;
    }
    void Context::submitThread() {
        while (m_ctxRunning) {
            FCT_WAIT_FOR(m_nextFrame);
            test::state.tick(this);
            compilePasses();
            submitPasses();
            if (m_flushWnd){
                m_flushWnd->swapBuffers();
            }
            currentFlush();
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