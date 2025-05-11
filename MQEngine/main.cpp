
#include "../FCT/headers.h"
#include "../FCT_IMGUI/headers.h"
#include "../FCT_IMGUI/FCTAPI.h"
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
    //TextureElement{"VertexCommandBuffer"},
    SamplerElement{"testSampler"}
};


template<typename IndexType = uint16_t>
static StaticMesh<IndexType>* createCube(Context* ctx, float size = 1.0f) {
    StaticMesh<IndexType>* mesh = new StaticMesh<IndexType>(ctx, vertexLayout);

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
    StaticMesh<>* cubeMesh;
    RHI::ConstBuffer* constBuffer;
    RHI::ConstBuffer* vertexConstBuffer;
    UniformBuffer* buffer;
    UniformBuffer* vertexBuffer;
    PassResource* passResource;
    int frameIndex;
    int maxFrameInFlight;
    float rotationAngle;
    Image* texture;
    PSampler sampler;
    float rotationAngleY;
    StaticMesh<uint32_t>* teapotMesh;
    GLFW_VK_ImGuiContext* imguiCtx;
    TraditionPipelineState* pso;
    DynamicMesh<uint32_t>* dynamicMesh;
    float animationTime;
    VertexContext* vertexCtx;
    Freetype_Font* emjFont;
    VertexPath* emjPath;
public:
    App(Runtime& rt) : rt(rt)
    {
        emjFont = rt.createFont();
        emjFont->create("C://Windows//Fonts//seguiemj.ttf");
        auto glyph = emjFont->toGlyptIndex(U'🔥');
        if (!glyph)
        {
            fout << "emj 读取失败" << std::endl;
        }
        emjPath = emjFont->translateGlyph(glyph);
        fout << "Hello, FCT!" << endl;
        wnd = rt.createWindow(1,1,800,600,"test");
        ctx = rt.createContext();
        ctx->create();
        wnd->enableDepthBuffer(Format::D32_SFLOAT_S8_UINT);
        wnd->bind(ctx);
        wnd->enableAutoViewport(Vec2(800,600));
        ctx->maxFrameInFlight(5);
        InitImgui();
        ImGui::StyleColorsLight();

        rt.postUiTask([this](void*)
        {
            },nullptr);
        Pass* pass = new TraditionRenderPass(ctx);
        pass->enableClear(ClearType::color | ClearType::depthStencil,Vec4(1,1,1,1));
        ctx->addPass("nomralObject",pass);
        pass->release();
        pass = new TraditionRenderPass(ctx);
        ctx->addPass("imguiPass",pass);
        pass->release();
        pass = nullptr;
        pass = new TraditionRenderPass(ctx);
        ctx->addPass("vertexPass",pass);
        pass->release();
        pass = nullptr;
        ctx->addPassDenpendency("nomralObject","imguiPass");
        ctx->addPassDenpendency("nomralObject","vertexPass");

        pass = new TraditionRenderPass(ctx);
        pass->enableClear(ClearType::color | ClearType::depthStencil,Vec4(1,1,1,1));

        vertexCtx = new VertexContext(ctx,wnd);
        vertexCtx->mvpUniformLayout(mvpUniform);

        ctx->bindOutput("nomralObject",wnd);
        ctx->bindOutput("vertexPass",wnd);
        ctx->bindOutput("imguiPass",wnd);
        imguiCtx = new GLFW_VK_ImGuiContext(
            static_cast<GLFW_Window*>(wnd),
            static_cast<VK_Context*>(ctx)
        );
        init();
        ctx->compilePasses();
        imguiCtx->attachPass("imguiPass");
        initDynamicMesh();
        fout << "初始化 完毕" << endl;
        wnd->enableAutoViewportForAllCurrentTargetToWndPass();
    }

    void initDynamicMesh()
    {
        dynamicMesh = new DynamicMesh<uint32_t>(ctx, vertexLayout);

        dynamicMesh->reserveVertices(100);
        dynamicMesh->reserveIndices(300);

        updateDynamicMesh(0.0f);

        dynamicMesh->create();

        animationTime = 0.0f;
    }
    void updateDynamicMesh(float time)
    {
        dynamicMesh->clear();

        const int gridSize = 10;
        const float size = 40.0f;
        const float halfSize = size * 0.5f;

        const float xOffset = 0.0f;
        const float zOffset = 0.0f;
        const float yOffset = -5.0f;

        for (int z = 0; z <= gridSize; z++) {
            for (int x = 0; x <= gridSize; x++) {
                float xPos = (float)x / gridSize * size - halfSize + xOffset;
                float zPos = (float)z / gridSize * size - halfSize + zOffset;

                float y = 5.0f * sin(xPos * 5.0f + time * 3.0f) *
                          cos(zPos * 5.0f + time * 2.0f) + yOffset;

                float r = 0.7f + 0.3f * sin(xPos + time);
                float g = 0.7f + 0.3f * cos(zPos + time * 0.7f);
                float b = 0.7f + 0.3f * sin(time * 0.5f);

                float u = (float)x / gridSize;
                float v = (float)z / gridSize;

                dynamicMesh->addVertex(Vec3(xPos, y, zPos), Vec4(r, g, b, 1.0f), Vec2(u, v));
            }
        }

        for (int z = 0; z < gridSize; z++) {
            for (int x = 0; x < gridSize; x++) {
                uint32_t topLeft = z * (gridSize + 1) + x;
                uint32_t topRight = topLeft + 1;
                uint32_t bottomLeft = (z + 1) * (gridSize + 1) + x;
                uint32_t bottomRight = bottomLeft + 1;

                dynamicMesh->addIndex(topLeft);
                dynamicMesh->addIndex(bottomLeft);
                dynamicMesh->addIndex(topRight);

                dynamicMesh->addIndex(topRight);
                dynamicMesh->addIndex(bottomLeft);
                dynamicMesh->addIndex(bottomRight);
            }
        }

        dynamicMesh->update();
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

        pso = new TraditionPipelineState();
        pso->vertexLayout = vertexLayout;
        pso->pixelLayout = pixelLayout;
        pso->resourceLayout = resourceLayout;
        pso->vertexShader = vs;
        pso->pixelShader = ps;

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

        vertexBuffer = new UniformBuffer(mvpUniform);
        vertexBuffer->setValue("modelMatrix",Mat4());
        vertexBuffer->setValue("viewMatrix", Mat4());
        vertexBuffer->setValue("projectionMatrix", Mat4());

        vertexConstBuffer = ctx->createConstBuffer();
        vertexConstBuffer->layout(mvpUniform);
        vertexConstBuffer->buffer(vertexBuffer);
        vertexConstBuffer->create();
        vertexConstBuffer->mapData();
        vertexConstBuffer->updataData();

        vertexCtx->addConstBuffer(vertexConstBuffer);
        vertexCtx->attachPass("vertexPass");
        vertexCtx->setScreen("screen")
            .setOrigin(Vec3(-1,-1,0))
            .setX(Vec3(2,0,0))
            .setY(Vec3(0,2,0))
            .setVertexCoordSize(Vec2(800,600))
            .setOriginVertexCoord(Vec2(0,0));

        passResource = ctx->createPassResource();
        passResource->addConstBuffer(constBuffer);
        passResource->bind(wnd);

        sampler = ctx->createSampler();
        sampler->setAnisotropic();
        sampler->create();

        texture = ctx->loadTexture("../../img.png");
        passResource->addTexture(texture,resourceLayout.findTexture("testTexture"));
        passResource->addSampler(sampler, resourceLayout.findSampler("testSampler"));
        passResource->create();
        teapotMesh = ctx->loadMesh("../../teapot.obj","teapot",vertexLayout);
    }

    ~App()
    {
        if (cubeMesh) {
            delete cubeMesh;
        }
        if (buffer) {
            delete buffer;
        }
        wnd->release();
        ctx->release();
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

        animationTime += deltaTime;

        updateDynamicMesh(animationTime);

        Mat4 ratation;
        ratation.rotateX(rotationAngle);
        ratation.rotateY(rotationAngleY);
        buffer->setValue("modelMatrix", ratation);
        constBuffer->updataData();
        vertexConstBuffer->updataData();
        wnd->title(ss.str());
        auto job = new TraditionRenderJob();
        job->addMesh(teapotMesh)
            .addMesh(cubeMesh)
            .addMesh(dynamicMesh)
            .setPassResource(passResource)
            .setPipelineState(pso);
        ctx->submit(job,"nomralObject");
        imguiCtx->addUi([]()
        {
            ImGui::Begin("FCT Debug");
            ImGui::Text("Hello, FCT!");
            ImGui::End();
        });
        VertexPath* path = new VertexPath();

        path->setColor(Vec4(0.2f, 0.6f, 1.0f, 0.8f));

        path->beginPath();

        path->moveTo(Vec2(100, 100));
        path->lineTo(Vec2(300, 100));
        path->lineTo(Vec2(300, 200));
        //path->lineTo(Vec2(100, 200));
        path->lineTo(Vec2(100, 100));

        path->endPath();

        path->beginPath();
        path->setColor(Vec4(1.0f, 0.3f, 0.3f, 1.0f));
        path->circle(Vec2(400, 150), 80);
        path->endPath();
        path->end();
        vertexCtx->clearPath("screen");
        vertexCtx->addPath("screen", path);
        vertexCtx->addPath("screen", emjPath);

        path->release();

        vertexCtx->submit();

        imguiCtx->submitJob();
        job->release();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        ctx->flush();
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
    _output_object(fout);
    return 0;
}

