#include "FCT/headers.h"
using namespace FCT;
using namespace std;
static constexpr VertexLayout vertexLayout {
    VertexElement{VtxType::Position3f},
    VertexElement{VtxType::Color4f},
    VertexElement{VtxType::TexCoord2f}
};
static constexpr PixelLayout pixelLayout {
    VertexElement{VtxType::Position4f},
    VertexElement{VtxType::Color4f},
    VertexElement{VtxType::TexCoord2f}
};
static constexpr ConstLayout mvpUniform {
    "mvp",
    ConstElement(ConstType::ModelMatrix),
    ConstElement(ConstType::ViewMatrix),
    ConstElement(ConstType::ProjectionMatrix)
};
static constexpr ResourceLayout resourceLayout {
    TextureElement{"testTexture"},
    SamplerElement{"testSampler"}
};


template<typename IndexType = uint16_t>
static Mesh<IndexType>* createCube(Context* ctx, float size = 1.0f) {
    Mesh<IndexType>* mesh = new Mesh<IndexType>(ctx, vertexLayout);

    float halfSize = size * 0.5f;

    Vec3 vertices[8] = {
        Vec3(-halfSize, -halfSize, -halfSize),
        Vec3(halfSize, -halfSize, -halfSize),
        Vec3(halfSize, halfSize, -halfSize),
        Vec3(-halfSize, halfSize, -halfSize),
        Vec3(-halfSize, -halfSize, halfSize),
        Vec3(halfSize, -halfSize, halfSize),
        Vec3(halfSize, halfSize, halfSize),
        Vec3(-halfSize, halfSize, halfSize)
    };

    Vec4 colors[6] = {
        Vec4(1.0f, 0.0f, 0.0f, 1.0f), // 前面 (红色)
        Vec4(0.0f, 1.0f, 0.0f, 1.0f), // 后面 (绿色)
        Vec4(0.0f, 0.0f, 1.0f, 1.0f), // 上面 (蓝色)
        Vec4(1.0f, 1.0f, 0.0f, 1.0f), // 下面 (黄色)
        Vec4(1.0f, 0.0f, 1.0f, 1.0f), // 右面 (紫色)
        Vec4(0.0f, 1.0f, 1.0f, 1.0f)  // 左面 (青色)
    };

    Vec2 texCoords[4] = {
        Vec2(0.0f, 1.0f), // 左下
        Vec2(1.0f, 1.0f), // 右下
        Vec2(1.0f, 0.0f), // 右上
        Vec2(0.0f, 0.0f)  // 左上
    };

    mesh->addVertex(vertices[4], colors[0], texCoords[0]);
    mesh->addVertex(vertices[5], colors[0], texCoords[1]);
    mesh->addVertex(vertices[6], colors[0], texCoords[2]);
    mesh->addVertex(vertices[7], colors[0], texCoords[3]);

    mesh->addVertex(vertices[0], colors[1], texCoords[1]);
    mesh->addVertex(vertices[1], colors[1], texCoords[0]);
    mesh->addVertex(vertices[2], colors[1], texCoords[3]);
    mesh->addVertex(vertices[3], colors[1], texCoords[2]);

    mesh->addVertex(vertices[3], colors[2], texCoords[0]);
    mesh->addVertex(vertices[2], colors[2], texCoords[1]);
    mesh->addVertex(vertices[6], colors[2], texCoords[2]);
    mesh->addVertex(vertices[7], colors[2], texCoords[3]);

    mesh->addVertex(vertices[0], colors[3], texCoords[0]);
    mesh->addVertex(vertices[1], colors[3], texCoords[1]);
    mesh->addVertex(vertices[5], colors[3], texCoords[2]);
    mesh->addVertex(vertices[4], colors[3], texCoords[3]);

    mesh->addVertex(vertices[1], colors[4], texCoords[0]);
    mesh->addVertex(vertices[2], colors[4], texCoords[1]);
    mesh->addVertex(vertices[6], colors[4], texCoords[2]);
    mesh->addVertex(vertices[5], colors[4], texCoords[3]);

    mesh->addVertex(vertices[0], colors[5], texCoords[0]);
    mesh->addVertex(vertices[3], colors[5], texCoords[1]);
    mesh->addVertex(vertices[7], colors[5], texCoords[2]);
    mesh->addVertex(vertices[4], colors[5], texCoords[3]);

    mesh->setIndices({
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4,
        8, 9, 10, 10, 11, 8,
        12, 13, 14, 14, 15, 12,
        16, 17, 18, 18, 19, 16,
        20, 21, 22, 22, 23, 20
    });

    mesh->create();

    return mesh;
}
class AutoReviewport
{
public:
    AutoReviewport()
    {
        needReviewport = false;
        windowWidth = 800.0f;
        windowHeight = 600.0f;
        viewportWidth = 800.0f;
        viewportHeight = 600.0f;
        viewportOffsetX = 0.0f;
        viewportOffsetY = 0.0f;
    }
    void resize(int width, int height)
    {
        windowWidth = static_cast<float>(width);
        windowHeight = static_cast<float>(height);
        needReviewport = true;
    }
    void computeViewport()
    {
        float targetAspect = 4.0f / 3.0f;
        float windowAspect = windowWidth / windowHeight;

        if (windowAspect > targetAspect) {
            viewportHeight = windowHeight;
            viewportWidth = windowHeight * targetAspect;
            viewportOffsetX = (windowWidth - viewportWidth) / 2.0f;
            viewportOffsetY = 0.0f;
        } else {
            viewportWidth = windowWidth;
            viewportHeight = windowWidth / targetAspect;
            viewportOffsetX = 0.0f;
            viewportOffsetY = (windowHeight - viewportHeight) / 2.0f;
        }
    }
    void onRenderTick(RHI::CommandBuffer* cmdBuf)
    {
        if (needReviewport) {
            computeViewport();
            needReviewport = false;
        }
        cmdBuf->viewport(Vec2(viewportOffsetX, viewportOffsetY), Vec2(viewportOffsetX + viewportWidth, viewportOffsetY + viewportHeight));
        cmdBuf->scissor(Vec2(viewportOffsetX, viewportOffsetY), Vec2(viewportOffsetX + viewportWidth, viewportOffsetY + viewportHeight));
    }
private:
    bool needReviewport;
    float windowWidth, windowHeight;
    float viewportWidth, viewportHeight;
    float viewportOffsetX, viewportOffsetY;
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
    Mesh<>* cubeMesh;
    RHI::ConstBuffer* constBuffer;
    UniformBuffer* buffer;
    PassResource* passResource;
    int frameIndex;
    int maxFrameInFlight;
    float rotationAngle;
    Image* texture;
    RHI::Sampler* sampler;
    AutoReviewport autoReviewport;
    float rotationAngleY;
    Mesh<uint32_t>* teapotMesh;
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
        wnd->enableDepthBuffer(Format::D32_SFLOAT_S8_UINT);
        wnd->getCallBack()->addResizeCallback([this](Window* wnd,int width, int height)
        {
            autoReviewport.resize(width, height);
        });
        ctx->maxFrameInFlight(5);
        init();
    }
    void init()
    {
        rotationAngle = 0.0f;
        rotationAngleY = 0.0f;
        vs = ctx->createVertexShader();
        vs->addUniform(mvpUniform);
        vs->pixelLayout(pixelLayout);
        vs->addLayout(0,vertexLayout);
        vs->resourceLayout(resourceLayout);
        vs->code(R"(
ShaderOut main(ShaderIn vsIn) {
    ShaderOut vsOut;

    float4 worldPos = mul(modelMatrix, float4(vsIn.pos, 1.0f));
    float4 viewPos = mul(viewMatrix, worldPos);
    float4 clipPos = mul(projectionMatrix, viewPos);

    vsOut.pos = clipPos;
    vsOut.color = vsIn.color;
    vsOut.texcoord = vsIn.texcoord;

    return vsOut;
}
)");
        vs->create();

        ps = ctx->createPixelShader();
        ps->addUniform(mvpUniform);
        ps->pixelLayout(pixelLayout);
        ps->resourceLayout(resourceLayout);
        ps->code(R"(
ShaderOut main(ShaderIn psIn) {
    ShaderOut psOut;
    float4 texColor = testTexture.Sample(testSampler, psIn.texcoord);
    psOut.target0 = texColor * psIn.color;
    return psOut;
}
)");
        ps->create();

        cubeMesh = createCube(ctx, 1.0f);

        pass = ctx->createPass();
        pass->bindTarget(0,wnd->getCurrentTarget()->targetImage());
        pass->depthStencil(wnd->getCurrentTarget()->depthStencilBuffer());
        passGroup = ctx->createPassGroup();
        passGroup->addPass(pass);
        passGroup->create();
        pipeline = ctx->createTraditionPipeline();
        pipeline->pixelLayout(pixelLayout);
        pipeline->vertexLayout(vertexLayout);
        pipeline->resourceLayout(resourceLayout);
        pipeline->bindPass(pass);
        pipeline->addResources(vs);
        pipeline->addResources(ps);
        pipeline->create();

        Mat4 view = Mat4::LookAt(Vec3(20,20,-20), Vec3(0,0,0), Vec3(0,0,1));
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

        ctx->allocBaseCommandBuffers(wnd);

        sampler = ctx->createSampler();
        sampler->setAnisotropic();
        sampler->create();

        texture = ctx->loadTexture("../img.png");
        passResource->addTexture(texture,resourceLayout.findTexture("testTexture"));
        passResource->addSampler(sampler, resourceLayout.findSampler("testSampler"));
        passResource->create();
        teapotMesh = ctx->loadMesh("../teapot.obj","teapot",vertexLayout);
    }

    ~App()
    {
        if (cubeMesh) {
            delete cubeMesh;
        }
        if (buffer) {
            delete buffer;
        }
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
        rotationAngleY += deltaTime * rotationV * 0.7f;
        wnd->title(ss.str());
        ctx->flush();
    }
    void submitTick()
    {
        Mat4 ratation;
        ratation.rotateX(rotationAngle);
        ratation.rotateY(rotationAngleY);
        buffer->setValue("modelMatrix", ratation);
        constBuffer->updataData();
        auto cmdBuf = ctx->getCmdBuf(wnd, 0);
        cmdBuf->reset();
        cmdBuf->begin();

        autoReviewport.onRenderTick(cmdBuf);
        cmdBuf->bindPipieline(pipeline);
        passGroup->beginSubmit(cmdBuf);
        passResource->bind(cmdBuf);


        teapotMesh->bind(cmdBuf);
        teapotMesh->draw(cmdBuf, 1);
        /*
        cubeMesh->bind(cmdBuf);
        cubeMesh->draw(cmdBuf, 1);
*/
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