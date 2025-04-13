#include "FCT/headers.h"
using namespace FCT;
using namespace std;
static constexpr VertexLayout vertexLayout {
    VertexElement{VtxType::Position2f},
    VertexElement{VtxType::Color4f},
};
static constexpr PixelLayout pixelLayout {
    vertexLayout
};
static constexpr UniformLayout mvpUniform {
    "mvp",
    UniformElement(UniformType::ModelMatrix),
    UniformElement(UniformType::ViewMatrix),
    UniformElement(UniformType::ProjectionMatrix)
};

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
struct FrameResource
{
    /*
     *提供alloc 函数，来在初始化阶段指定需要 几个，如renderFinishedSemaphores
     *提供获取指定序号的renderFinishedSemaphores和cmdB uf
     */
    RHI::Semaphore* imageAvailableSemaphore;
    std::vector<RHI::Semaphore*> renderFinishedSemaphores;
    std::vector<RHI::Fence*> presentCompleteFences;
    RHI::CommandPool* cmdPool;
    std::vector<RHI::CommandBuffer*> cmdBufs;
    Context* ctx;
    void init(Context* ctx)
    {
        this->ctx = ctx;

        cmdPool = ctx->createCommandPool();
        cmdPool->create();

        imageAvailableSemaphore = ctx->createSemaphore();
        imageAvailableSemaphore->create();
    }
    void allocCommandBuffers(uint32_t additionalCount)
    {
        uint32_t currentCount = cmdBufs.size();

        cmdBufs.resize(currentCount + additionalCount);

        for (uint32_t i = currentCount; i < cmdBufs.size(); i++) {
            cmdBufs[i] = cmdPool->createCommandBuffer();
            cmdBufs[i]->create();
        }
    }


    void allocPresentCompleteFences(uint32_t additionalCount)
    {
        uint32_t currentCount = presentCompleteFences.size();

        presentCompleteFences.resize(currentCount + additionalCount);

        for (uint32_t i = currentCount; i < presentCompleteFences.size(); i++) {
            presentCompleteFences[i] = ctx->createFence();
            presentCompleteFences[i]->createSignaled();
        }
    }

    void allocRenderFinshSemaphores(uint32_t additionalCount)
    {
        uint32_t currentCount = renderFinishedSemaphores.size();

        renderFinishedSemaphores.resize(currentCount + additionalCount);

        for (uint32_t i = currentCount; i < renderFinishedSemaphores.size(); i++) {
            renderFinishedSemaphores[i] = ctx->createSemaphore();
            renderFinishedSemaphores[i]->create();
        }
    }
    uint32_t allocBaseCommandBuffers()
    {
        size_t cmdBufIndex = cmdBufs.size();
        size_t fenceIndex = presentCompleteFences.size();
        size_t semaphoreIndex = renderFinishedSemaphores.size();
        allocCommandBuffers(1);
        allocRenderFinshSemaphores(1);
        allocPresentCompleteFences(1);
        auto cmdBuf = cmdBufs[cmdBufIndex];
        auto fence = presentCompleteFences[fenceIndex];
        auto semaphore = renderFinishedSemaphores[semaphoreIndex];
        cmdBuf->addWaitSemaphore(imageAvailableSemaphore);
        cmdBuf->addSignalSemaphore(semaphore);
        cmdBuf->fence(fence);
        return cmdBufIndex;
    }

    RHI::Fence* getPresentCompleteFence(uint32_t index)
    {
        if (index < presentCompleteFences.size()) {
            return presentCompleteFences[index];
        }
        return nullptr;
    }

    RHI::Semaphore* getRenderFinishedSemaphore(uint32_t index)
    {
        if (index < renderFinishedSemaphores.size()) {
            return renderFinishedSemaphores[index];
        }
        return nullptr;
    }

    RHI::Semaphore* getImageAvailableSemaphore()
    {
        return imageAvailableSemaphore;
    }

    RHI::CommandPool* getCmdPool()
    {
        return cmdPool;
    }

    RHI::CommandBuffer* getCmdBuf(uint32_t index)
    {
        if (index < cmdBufs.size()) {
            return cmdBufs[index];
        }
        return nullptr;
    }
};
struct FrameManager {
    uint32_t m_maxFrameInFlight;
    Context* ctx;
    std::map<Window*, std::vector<FrameResource>> m_frameResources;
    size_t m_frameIndex = 0;
    std::vector<Window*> m_bindedWindows;
    void context(Context* ctx)
    {
        this->ctx = ctx;
    }
    RHI::CommandBuffer* getCmdBuf(Window* wnd,uint32_t index)
    {
        auto it = m_frameResources.find(wnd);
        if (it!= m_frameResources.end()) {
            return it->second[m_frameIndex].getCmdBuf(index);
        }
        return nullptr;
    }
    uint32_t allocBaseCommandBuffers(Window* wnd)
    {
        auto it = m_frameResources.find(wnd);
        if (it != m_frameResources.end()) {
            std::vector<uint32_t> indices;

            auto fenceIndex = m_frameResources[wnd][0].presentCompleteFences.size();

            for (auto& frame : m_frameResources[wnd]) {
                uint32_t index = frame.allocBaseCommandBuffers();
                indices.push_back(index);
            }

            m_frameResources[wnd][m_frameIndex].presentCompleteFences[fenceIndex]->reset();

            if (!indices.empty()) {
                return indices[0];
            }
        }
        return UINT32_MAX;
    }
    void flush()
    {
        for (auto &[wnd, frameResources] : m_frameResources)
        {
            wnd->clearRenderFinshSemaphores();
            for (auto semaphore : frameResources[m_frameIndex].renderFinishedSemaphores)
            {
                wnd->addRenderFinshSemaphore(semaphore);
            }
        }
        m_frameIndex = (m_frameIndex + 1) % m_maxFrameInFlight;
        for (auto &[wnd, frameResources] : m_frameResources)
        {
            wnd->clearRenderFinshFences();
            for (auto fence : frameResources[m_frameIndex].presentCompleteFences)
            {
                wnd->addRenderFinshFence(fence);
            }
            wnd->setPresentFinshSemaphore(frameResources[m_frameIndex].imageAvailableSemaphore);
        }
        ctx->swapBuffers();
    }
    void bindWindow(Window* wnd)
    {
        m_bindedWindows.push_back(wnd);
    }
    void maxFrameInFlight(uint32_t maxFrameInFlight)
    {
        m_maxFrameInFlight = maxFrameInFlight;
    }
    void create()
    {
        m_frameIndex = 0;

        for (auto wnd : m_bindedWindows)
        {
            std::vector<FrameResource> frameResources(m_maxFrameInFlight);
            for (auto& resource : frameResources) {
                resource.init(ctx);
            }
            wnd->setPresentFinshSemaphore(frameResources[m_frameIndex].imageAvailableSemaphore);
            wnd->initRender();
            m_frameResources[wnd] = std::move(frameResources);
        }
    }
};
class App
{
private:
    Window* wnd;
    Context* ctx;
    Runtime& rt;
    VertexShader* vs;
    RHI::Pass* pass;
    RHI::PassGroup* passGroup;
    PixelShader* ps;
    RHI::RasterizationPipeline* pipeline;
    RHI::CommandPool* cmdPool;
    RHI::CommandBuffer* cmdBuf;
    RHI::IndexBuffer* indexBuffer;
    /*
    RHI::Fence* fence;
    RHI::Semaphore* semaphore;
    RHI::Semaphore* presentFinshSemaphore;*/
    RHI::VertexBuffer* vertexBuffer;
    VertexBuffer* cpuVertexBuffer;
    int frameIndex;
    int maxFrameInFlight;
    FrameManager frameManager;
public:
    App(Runtime& rt) : rt(rt)
    {
        fout << "Hello, FCT!" << endl;
        wnd = rt.createWindow(1,1,800,600,"test");
        ctx = rt.createContext();
        std::function<void()> submitTick = std::bind(&App::submitTick,this);
        ctx->submitTicker(submitTick);
        ctx->create();
        wnd->bind(ctx);
        init();
    }
    void init()
    {
        frameManager.context(ctx);
        frameManager.maxFrameInFlight(3);
        frameManager.bindWindow(wnd);
        frameManager.create();

        vs = ctx->createVertexShader();
        vs->addUniform(mvpUniform);
        vs->pixelLayout(pixelLayout);
        vs->addLayout(0,vertexLayout);
        vs->create();

        ps = ctx->createPixelShader();
        ps->addUniform(mvpUniform);
        ps->pixelLayout(pixelLayout);
        ps->create();

        cpuVertexBuffer = new VertexBuffer(vertexLayout);
        cpuVertexBuffer->emplaceBack(
            Vec2(-0.5f, -0.5f),
            Vec4(1.0f, 0.0f, 0.0f, 1.0f)
        );
        cpuVertexBuffer->emplaceBack(
            Vec2(0.5f, -0.5f),
            Vec4(0.0f, 1.0f, 0.0f, 1.0f)
        );
        cpuVertexBuffer->emplaceBack(
            Vec2(0.5f, 0.5f),
            Vec4(0.0f, 0.0f, 1.0f, 1.0f)
        );
        cpuVertexBuffer->emplaceBack(
            Vec2(-0.5f, 0.5f),
            Vec4(1.0f, 1.0f, 1.0f, 1.0f)
        );
        std::vector<uint16_t> indices = {
            0, 1, 2, 2, 3, 0
        };
        indexBuffer = ctx->createIndexBuffer();
        indexBuffer->indexBuffer(indices);
        indexBuffer->create();
        indexBuffer->updataBuffer();
        vertexBuffer = ctx->createVertexBuffer();
        vertexBuffer->vertexBuffer(cpuVertexBuffer);
        vertexBuffer->create();
        vertexBuffer->updataBuffer();

        pass = ctx->createPass();
        pass->bindTarget(0,wnd->getCurrentTarget()->targetImage());
        passGroup = ctx->createPassGroup();
        passGroup->addPass(pass);
        passGroup->create();
        pipeline = ctx->createTraditionPipeline();
        pipeline->pixelLayout(pixelLayout);
        pipeline->vertexLayout(vertexLayout);
        pipeline->bindPass(pass);
        pipeline->create();

        pipeline->addResources(vs);
        frameManager.allocBaseCommandBuffers(wnd);

    }
    void logicTick()
    {
        static auto lastFrameTime = std::chrono::high_resolution_clock::now();

        auto currentTime = std::chrono::high_resolution_clock::now();

        float deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - lastFrameTime).count() / 1000000.0f;

        float instantFPS = 1.0f / deltaTime;

        lastFrameTime = currentTime;

        std::stringstream ss;
        ss << "FCT Demo - Instant FPS: " << std::fixed << std::setprecision(1) << instantFPS;
        wnd->title(ss.str());

        ctx->flush();
    }
    void submitTick()
    {
        auto cmdBuf = frameManager.getCmdBuf(wnd, 0);
        cmdBuf->reset();
        cmdBuf->begin();
        cmdBuf->viewport(Vec2(0,0),Vec2(800,600));
        cmdBuf->scissor(Vec2(0,0),Vec2(800,600));
        cmdBuf->bindPipieline(pipeline);
        passGroup->beginSubmit(cmdBuf);
        vertexBuffer->bind(cmdBuf);
        indexBuffer->bind(cmdBuf);
        cmdBuf->drawIndex(0,0,6,1);
        passGroup->endSubmit(cmdBuf);
        cmdBuf->end();
        cmdBuf->submit();

        ctx->compilePasses();
        ctx->submitPasses();

        frameManager.flush();
    }
    void run()
    {
        while (wnd->isRunning())
        {
            logicTick();
        }
    }
};
int main(){
    Runtime rt;
    App app(rt);
    app.run();
    return 0;
}