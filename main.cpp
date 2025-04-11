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
class App
{
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

        maxFrameInFlight = 2;
        presentFinshSemaphore = ctx->createSemaphore();
        presentFinshSemaphore->create();
        wnd->setPresentFinshSemaphore(presentFinshSemaphore);
        wnd->initRender();
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
        cmdPool = ctx->createCommandPool();
        cmdPool->create();
        cmdBuf = cmdPool->createCommandBuffer();
        cmdBuf->create();
        fence = ctx->createFence();
        semaphore = ctx->createSemaphore();
        fence->create();
        semaphore->create();
        cmdBuf->addWaitSemaphore(presentFinshSemaphore);
        cmdBuf->addSignalSemaphore(semaphore);
        wnd->addRenderFinshSemaphore(semaphore);
        cmdBuf->fence(fence);
    }
    void logicTick()
    {
        ctx->flush();
    }
    void submitTick()
    {
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
        fence->waitFor();
        fence->reset();
        ctx->compilePasses();
        ctx->submitPasses();
        ctx->swapBuffers();
    }
    void run()
    {
        while (wnd->isRunning())
        {
            logicTick();
        }
    }
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
    RHI::Fence* fence;
    RHI::Semaphore* semaphore;
    RHI::Semaphore* presentFinshSemaphore;
    RHI::VertexBuffer* vertexBuffer;
    VertexBuffer* cpuVertexBuffer;
    int frameIndex;
    int maxFrameInFlight;
};
int main(){
    Runtime rt;
    App app(rt);
    app.run();
    return 0;
}