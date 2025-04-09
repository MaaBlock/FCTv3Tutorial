#include "FCT/headers.h"
using namespace FCT;
using namespace std;
static constexpr VertexLayout vertexLayout {
    VertexElement{ElementType::Color4f},
    VertexElement{ElementType::Position2f},
};
static constexpr PixelLayout pixelLayout {
    vertexLayout
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
        cpuVertexBuffer = new VertexBuffer(vertexLayout);
        cpuVertexBuffer->emplaceBack(
        Vec4(1,0,0,1),
        Vec2(0.0f,-0.5f)
        );
        cpuVertexBuffer->emplaceBack(
        Vec4(0,1,0,1),
        Vec2(0.5f,0.5f)
        );
        cpuVertexBuffer->emplaceBack(
        Vec4(0,0,1,1),
        Vec2(-0.5f,0.5f)
        );

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
        /*
        vs = ctx->createVertexShader();
        vs->pixelLayout(pixelLayout);
        vs->code(R"(
static const float2 positions[3] = {
    float2(0.0, -0.5),
    float2(0.5, 0.5),
    float2(-0.5, 0.5)
};

static const float3 colors[3] = {
    float3(1.0, 0.0, 0.0),
    float3(0.0, 1.0, 0.0),
    float3(0.0, 0.0, 1.0)
};

ShaderOut main(ShaderIn In) {
    ShaderOut Out;
    Out.color = float4(colors[ In.vertexID], 1.0);
    Out.pos = positions[In.vertexID];
    return Out;
}
)");
        vs->create();*/
        pipeline = ctx->createTraditionPipeline();
        pipeline->pixelLayout(pixelLayout);
        pipeline->vertexLayout(vertexLayout);
        //pipeline->addResources(vs);
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
        cmdBuf->draw(0,0,3,1);
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
    RHI::PixelShader* ps;
    //RHI::VertexShader* vs;
    RHI::RasterizationPipeline* pipeline;
    RHI::CommandPool* cmdPool;
    RHI::CommandBuffer* cmdBuf;
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