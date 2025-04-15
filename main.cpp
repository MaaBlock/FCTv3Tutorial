#include "FCT/headers.h"
using namespace FCT;
using namespace std;
static constexpr VertexLayout vertexLayout {
    VertexElement{VtxType::Position2f},
    VertexElement{VtxType::Color4f},
};
static constexpr PixelLayout pixelLayout {
    VertexElement{VtxType::Position4f},
    VertexElement{VtxType::Color4f},
};
static constexpr UniformLayout mvpUniform {
    "mvp",

    UniformElement(UniformType::ModelMatrix),
    UniformElement(UniformType::ViewMatrix),
    UniformElement(UniformType::ProjectionMatrix)
    /*
    UniformElement(UniformType::Mat3,"modelMatrix"),
    UniformElement(UniformType::Mat3,"viewMatrix"),
    UniformElement(UniformType::Mat3,"projectionMatrix")
*/
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
private:
    Window* wnd;
    Context* ctx;
    Runtime& rt;
    VertexShader* vs;
    RHI::Pass* pass;
    RHI::PassGroup* passGroup;
    PixelShader* ps;
    RHI::RasterizationPipeline* pipeline;
    RHI::IndexBuffer* indexBuffer;
    RHI::VertexBuffer* vertexBuffer;
    VertexBuffer* cpuVertexBuffer;
    RHI::ConstBuffer* constBuffer;
    UniformBuffer* buffer;
    PassResource* passResource;
    int frameIndex;
    int maxFrameInFlight;
    float rotationAngle;
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
        ctx->maxFrameInFlight(5);
        init();
    }
    void init()
    {
        rotationAngle = 0.0f;
        vs = ctx->createVertexShader();
        vs->addUniform(mvpUniform);
        vs->pixelLayout(pixelLayout);
        vs->addLayout(0,vertexLayout);
        vs->code(R"(
ShaderOut main(ShaderIn vsIn) {
    ShaderOut vsOut;

    float4 worldPos = mul(modelMatrix,float4(vsIn.pos,0.0f, 1.0f));
    float4 viewPos = mul(viewMatrix,worldPos);
    float4 clipPos = mul(projectionMatrix,viewPos);

    vsOut.pos = clipPos;
    vsOut.color = vsIn.color;

    return vsOut;
}
)");
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
        pipeline->addResources(vs);
        pipeline->create();

        Mat4 view = Mat4::LookAt(Vec3(2,2,-2), Vec3(0,0,0), Vec3(0,0,1));
        Mat4 proj = Mat4::Perspective(45.0f, 800.0f / 600.0f, 0.1f, 100.0);;

        buffer = new UniformBuffer(mvpUniform);
        buffer->setValue("modelMatrix",Mat4());
        buffer->setValue("viewMatrix", view);
        buffer->setValue("projectionMatrix", proj);

        Mat4 debug_model = buffer->getValue<Mat4>("modelMatrix");
        Mat4 debug_view = buffer->getValue<Mat4>("viewMatrix");
        Mat4 debug_proj = buffer->getValue<Mat4>("projectionMatrix");

        fout << debug_model << endl;
        fout << debug_view << endl;
        fout << debug_proj << endl;

        constBuffer = ctx->createConstBuffer();
        constBuffer->layout(mvpUniform);
        constBuffer->buffer(buffer);
        constBuffer->create();
        constBuffer->mapData();

        passResource = ctx->createPassResource();
        passResource->addConstBuffer(constBuffer);
        passResource->bind(wnd);
        passResource->pipeline(pipeline);
        passResource->create();

        ctx->allocBaseCommandBuffers(wnd);
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
        float rotationV = 90.0f; //90度/s
        rotationAngle += deltaTime * rotationV;
        wnd->title(ss.str());
        ctx->flush();
    }
    void submitTick()
    {
        Mat4 ratation;
        ratation.rotateZ(rotationAngle);
        buffer->setValue("modelMatrix", ratation);
        constBuffer->updataData();
        auto cmdBuf = ctx->getCmdBuf(wnd, 0);
        cmdBuf->reset();
        cmdBuf->begin();
        cmdBuf->viewport(Vec2(0,0),Vec2(800,600));
        cmdBuf->scissor(Vec2(0,0),Vec2(800,600));
        cmdBuf->bindPipieline(pipeline);
        passGroup->beginSubmit(cmdBuf);
        passResource->bind(cmdBuf);
        vertexBuffer->bind(cmdBuf);
        indexBuffer->bind(cmdBuf);
        cmdBuf->drawIndex(0,0,6,1);
        passGroup->endSubmit(cmdBuf);
        cmdBuf->end();
        cmdBuf->submit();
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
};
int main(){
    Runtime rt;
    App app(rt);
    app.run();
    return 0;
}