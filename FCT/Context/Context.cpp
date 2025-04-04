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
        std::vector<char> readFile(const std::string& filename) {
            std::ifstream file(filename, std::ios::ate | std::ios::binary);

            if (!file.is_open()) {
                throw std::runtime_error("failed to open file!");
            }

            size_t fileSize = (size_t) file.tellg();
            std::vector<char> buffer(fileSize);

            file.seekg(0);
            file.read(buffer.data(), fileSize);

            file.close();
            return buffer;
        }
        struct TestState
        {
            bool isInited = false;
            Context* ctx;
            Window* wnd;
            RHI::Pass* pass;
            RHI::PassGroup* passGroup;
            RHI::PixelShader* ps;
            RHI::VertexShader* vs;
            RHI::RasterizationPipeline* pipeline;
            RHI::CommandPool* cmdPool;
            RHI::CommandBuffer* cmdBuf;
            RHI::Fence* fence;
            RHI::Semaphore* semaphore;
            void init(Context* ctx)
            {
                this->ctx = ctx;
                isInited = true;
                wnd = ctx->flushWindow();
                pass = ctx->createPass();
                pass->bindTarget(0,wnd->getCurrentTarget()->targetImage());
                //pass->addTargets(wnd->getCurrentTarget()->getTargetImages());
                passGroup = ctx->createPassGroup();
                passGroup->addPass(pass);
                passGroup->create();
                ps = ctx->createPixelShader();
                vs = ctx->createVertexShader();
                fout << std::filesystem::current_path().string() << std::endl;
                vs->code(readFile("../FCT/Shader/tmpVert.spv"));
                ps->code(readFile("../FCT/Shader/tmpFrag.spv"));
                vs->create();
                ps->create();
                pipeline = ctx->createTraditionPipeline();
                pipeline->addResources(vs);
                pipeline->addResources(ps);
                pipeline->bindPass(pass);
                pipeline->create();
                cmdPool = ctx->createCommandPool();
                cmdPool->create();
                cmdBuf = cmdPool->createCommandBuffer();
                cmdBuf->create();
                fence = ctx->createFence();
                semaphore = ctx->createSemaphore();
                fence->create();
                semaphore->create();
                cmdBuf->addWaitSemaphore(wnd->getImageAvailableSemaphore());
                cmdBuf->addSignalSemaphore(semaphore);
                wnd->addRenderFinshSemaphore(semaphore);
                cmdBuf->fence(fence);
            }
            void tick(Context* ctx)
            {
                if (!isInited) {
                    init(ctx);
                }
                cmdBuf->reset();
                cmdBuf->begin();
                cmdBuf->viewport(Vec2(0,0),Vec2(800,600));
                cmdBuf->scissor(Vec2(0,0),Vec2(800,600));
                cmdBuf->bindPipieline(pipeline);
                passGroup->beginSubmit(cmdBuf);
                cmdBuf->draw(0,0,3,1);
                passGroup->endSubmit(cmdBuf);
                cmdBuf->end();
                cmdBuf->submit();
                fence->waitFor();
                fence->reset();
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