#include "FCT/headers.h"
using namespace FCT;
using namespace std;
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
    RHI::Pass* pass;
    RHI::PassGroup* passGroup;
    RHI::PixelShader* ps;
    RHI::VertexShader* vs;
    RHI::RasterizationPipeline* pipeline;
    RHI::CommandPool* cmdPool;
    RHI::CommandBuffer* cmdBuf;
    RHI::Fence* fence;
    RHI::Semaphore* semaphore;
    RHI::Semaphore* presentFinshSemaphore;
    int frameIndex;
    int maxFrameInFlight;
};
int main(){
    Runtime rt;
    App app(rt);
    app.run();
    return 0;
}