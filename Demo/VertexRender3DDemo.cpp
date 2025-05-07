
#include "../FCT/FCTAPI.h"
#include "./AutoReviewport.h"
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
class App
{
private:
    Window* wnd;
    Context* ctx;
    Runtime& rt;
    VertexShader* vs;
    PixelShader* ps;
    RHI::ConstBuffer* constBuffer;
    UniformBuffer* buffer;
    PassResource* passResource;
    Image* texture;
    PSampler sampler;
    AutoReviewport autoReviewport;
    TraditionPipelineState* pso;

    StaticMesh<uint32_t>* floorMesh;
    StaticMesh<uint32_t>* wallMeshes[4];

    bool keyState[255] = {};
    Vec3 cameraPosition;
    Vec3 cameraTarget;
    Vec3 cameraUp;
    float cameraYaw;
    float cameraPitch;
    bool firstMouse;
    int lastMouseX;
    int lastMouseY;
    float mouseSensitivity;
    bool mouseControlEnabled;

    float currentViewScale = 1.0f;

    VertexContext* vertexCtx;
    Freetype_Font* emjFont;
    Freetype_Font* font;
    VertexPath* emjPath;
    VertexPath* goodPath;
    UniformBuffer* vertexBuffer;

    RHI::ConstBuffer* vertexConstBuffer;
public:
    void initVertex()
    {
        vertexCtx = new VertexContext(ctx,wnd);
        vertexCtx->mvpUniformLayout(mvpUniform);


        Mat4 proj = Mat4::Perspective(45.0f, 800.0f / 600.0f, 0.1f, 100.0);
        vertexBuffer = new UniformBuffer(mvpUniform);
        vertexBuffer->setValue("modelMatrix",Mat4());
        vertexBuffer->setValue("viewMatrix", Mat4());
        vertexBuffer->setValue("projectionMatrix", proj);

        vertexConstBuffer = ctx->createConstBuffer();
        vertexConstBuffer->layout(mvpUniform);
        vertexConstBuffer->buffer(vertexBuffer);
        vertexConstBuffer->create();
        vertexConstBuffer->mapData();
        vertexConstBuffer->updataData();

        vertexCtx->addConstBuffer(vertexConstBuffer);
        vertexCtx->attachPass("vertexPass");

        const float roomSize = 9.9f;
        const float halfSize = roomSize / 2.0f;
        const float wallHeight = 3.0f;

        vertexCtx->setScreen("frontWallScreen")
            .setOrigin(Vec3(-halfSize, 0, -halfSize))
            .setX(Vec3(roomSize, 0, 0))
            .setY(Vec3(0, wallHeight, 0))
            .setVertexCoordSize(Vec2(800, 600))
            .setOriginVertexCoord(Vec2(-400, -300));

        vertexCtx->setScreen("backWallScreen")
            .setOrigin(Vec3(-halfSize, 0, halfSize))
            .setX(Vec3(roomSize, 0, 0))
            .setY(Vec3(0, wallHeight, 0))
            .setVertexCoordSize(Vec2(800, 600))
            .setOriginVertexCoord(Vec2(-400, -300));
    }
    App(Runtime& rt) : rt(rt)
    {
        emjFont = rt.createFont();
        emjFont->create("./res/seguiemj.ttf");
        auto glyph = emjFont->toGlyptIndex(U'🥹');
        if (!glyph)
        {
            fout << "emj 读取失败" << std::endl;
        }
        emjPath = emjFont->translateGlyph(glyph);

        font = rt.createFont();
        font->create("C:\\Windows\\Fonts\\simsun.ttc");
        glyph = font->toGlyptIndex(U'好');
        if (!glyph)
        {
            fout << "好 读取失败" << std::endl;
        }
        goodPath = font->translateGlyph(glyph);

        cameraPosition = Vec3(0, 1.8, 0);
        cameraTarget = Vec3(0, 0, 0);
        cameraUp = Vec3(0, -1, 0);
        cameraYaw = -90.0f;
        cameraPitch = -26.57f;
        firstMouse = true;
        lastMouseX = 400;
        lastMouseY = 300;
        mouseSensitivity = 0.1f;
        mouseControlEnabled = true;
        wnd = rt.createWindow(1,1,800,600,"test");
        ctx = rt.createContext();
        std::function<void()> submitTick = std::bind(&App::submitTick,this);
        ctx->submitTicker(submitTick);
        ctx->create();
        wnd->enableDepthBuffer(Format::D32_SFLOAT_S8_UINT);
        wnd->bind(ctx);
        wnd->getCallBack()->addResizeCallback([this](Window* wnd,int width, int height)
        {
            autoReviewport.resize(width, height);
        });
        wnd->getCallBack()->addKeyUpCallback([this](Window* wnd, int key)
        {
            keyState[key] = false;
        });
        wnd->getCallBack()->addKeyDownCallback([this](Window* wnd, int key)
        {
            keyState[key] = true;

            if (key == GLFW_KEY_SPACE)
            {
                mouseControlEnabled = !mouseControlEnabled;
            }
        });
        wnd->getCallBack()->addLButtonUpCallback([this](Window* wnd, int x, int y)
        {
            mouseControlEnabled = !mouseControlEnabled;
        });
        wnd->getCallBack()->addMouseWheelCallback([this](Window* wnd, int dealt)
{
    static float viewScale = 1.0f;
    const float scaleStep = 0.1f;

    viewScale += dealt * scaleStep;

    if (viewScale < 0.1f) viewScale = 0.1f;

    currentViewScale = viewScale;

});
        ctx->maxFrameInFlight(5);
        autoReviewport.context(ctx);

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

        ctx->bindOutput("nomralObject",wnd);
        ctx->bindOutput("vertexPass",wnd);
        ctx->bindOutput("imguiPass",wnd);
        initVertex();
        init();
        ctx->compilePasses();
        autoReviewport.addPass("vertexPass");
        autoReviewport.addPass("nomralObject");
    }
    void handleMouseInput()
    {
        if (!mouseControlEnabled) return;

        int windowWidth = wnd->getWidth();
        int windowHeight = wnd->getHeight();

        auto cursorPos = wnd->getCursorPos();
        int xpos = cursorPos.x;
        int ypos = cursorPos.y;

        if (firstMouse)
        {
            lastMouseX = xpos;
            lastMouseY = ypos;
            firstMouse = false;
            return;
        }

        float xoffset = static_cast<float>(xpos - lastMouseX);
        float yoffset = static_cast<float>(lastMouseY - ypos);


        lastMouseX = xpos;
        lastMouseY = ypos;

        xoffset *= mouseSensitivity;
        yoffset *= mouseSensitivity;

        cameraYaw -= xoffset;
        cameraPitch += yoffset;

        if (cameraPitch > 89.0f)
            cameraPitch = 89.0f;
        if (cameraPitch < -89.0f)
            cameraPitch = -89.0f;

       static_cast<GLFW_Window*>(wnd)->postUiTask([this,windowWidth,windowHeight](void*)
       {
           wnd->setCursorPos(windowWidth / 2, windowHeight / 2);
       },nullptr,false);
       lastMouseX = windowWidth / 2;
       lastMouseY = windowHeight / 2;

        updateCamera();
    }
    void updateCamera()
    {
        Vec3 front;
        front.x = cos(cameraYaw * 3.14159f / 180.0f) * cos(cameraPitch * 3.14159f / 180.0f);
        front.y = sin(cameraPitch * 3.14159f / 180.0f);
        front.z = sin(cameraYaw * 3.14159f / 180.0f) * cos(cameraPitch * 3.14159f / 180.0f);
        front = front.normalize();

        cameraTarget = cameraPosition + front;

        Mat4 view = Mat4::LookAt(cameraPosition, cameraTarget, cameraUp);

        buffer->setValue("viewMatrix", view);
        vertexBuffer->setValue("viewMatrix", view);
    }
void init()
    {
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

        pso = new TraditionPipelineState();
        pso->vertexLayout = vertexLayout;
        pso->pixelLayout = pixelLayout;
        pso->resourceLayout = resourceLayout;
        pso->vertexShader = vs;
        pso->pixelShader = ps;


        Mat4 proj = Mat4::Perspective(45.0f, 800.0f / 600.0f, 0.1f, 100.0);

        buffer = new UniformBuffer(mvpUniform);
        buffer->setValue("modelMatrix", Mat4());
        buffer->setValue("viewMatrix", Mat4::LookAt(cameraPosition, cameraTarget, cameraUp));
        buffer->setValue("projectionMatrix", proj);

        constBuffer = ctx->createConstBuffer();
        constBuffer->layout(mvpUniform);
        constBuffer->buffer(buffer);
        constBuffer->create();
        constBuffer->mapData();

        cameraPosition = Vec3(0, 1.7f, 0);
        updateCamera();
        passResource = ctx->createPassResource();
        passResource->addConstBuffer(constBuffer);
        passResource->bind(wnd);

        ctx->allocBaseCommandBuffers(wnd);

        sampler = ctx->createSampler();
        sampler->setAnisotropic();
        sampler->create();

        texture = ctx->loadTexture("./res/img.png");

        passResource->addTexture(texture, resourceLayout.findTexture("testTexture"));
        passResource->addSampler(sampler, resourceLayout.findSampler("testSampler"));
        passResource->create();

        createRoom();
    }

    void createRoom()
    {
        const float roomSize = 10.0f;
        const float halfSize = roomSize / 2.0f;
        const float wallHeight = 3.0f;

        floorMesh = new StaticMesh<uint32_t>(ctx, vertexLayout);

        floorMesh->addVertex(Vec3(-halfSize, 0, -halfSize), Vec4(0.8f, 0.8f, 0.8f, 1.0f), Vec2(0, 0));
        floorMesh->addVertex(Vec3(halfSize, 0, -halfSize), Vec4(0.8f, 0.8f, 0.8f, 1.0f), Vec2(1, 0));
        floorMesh->addVertex(Vec3(halfSize, 0, halfSize), Vec4(0.8f, 0.8f, 0.8f, 1.0f), Vec2(1, 1));
        floorMesh->addVertex(Vec3(-halfSize, 0, halfSize), Vec4(0.8f, 0.8f, 0.8f, 1.0f), Vec2(0, 1));

        std::vector<uint32_t> floorIndices = {
            0, 1, 2,
            0, 2, 3
        };
        floorMesh->setIndices(floorIndices);

        floorMesh->create();

        floorMesh->create();

        Vec4 wallColors[4] = {
            Vec4(1.0f, 0.8f, 0.8f, 1.0f),
            Vec4(0.8f, 1.0f, 0.8f, 1.0f),
            Vec4(0.8f, 0.8f, 1.0f, 1.0f),
            Vec4(1.0f, 1.0f, 0.8f, 1.0f)
        };

        struct WallDef {
            Vec3 p1, p2, p3, p4;
        };

        WallDef wallDefs[4] = {
            {
                Vec3(-halfSize, 0, -halfSize),
                Vec3(halfSize, 0, -halfSize),
                Vec3(halfSize, wallHeight, -halfSize),
                Vec3(-halfSize, wallHeight, -halfSize)
            },
            {
                Vec3(halfSize, 0, -halfSize),
                Vec3(halfSize, 0, halfSize),
                Vec3(halfSize, wallHeight, halfSize),
                Vec3(halfSize, wallHeight, -halfSize)
            },
            {
                Vec3(halfSize, 0, halfSize),
                Vec3(-halfSize, 0, halfSize),
                Vec3(-halfSize, wallHeight, halfSize),
                Vec3(halfSize, wallHeight, halfSize)
            },
            {
                Vec3(-halfSize, 0, halfSize),
                Vec3(-halfSize, 0, -halfSize),
                Vec3(-halfSize, wallHeight, -halfSize),
                Vec3(-halfSize, wallHeight, halfSize)
            }
        };

        for (int i = 0; i < 4; i++) {
            wallMeshes[i] = new StaticMesh<uint32_t>(ctx, vertexLayout);

            wallMeshes[i]->addVertex(wallDefs[i].p1, wallColors[i], Vec2(0, 0));
            wallMeshes[i]->addVertex(wallDefs[i].p2, wallColors[i], Vec2(1, 0));
            wallMeshes[i]->addVertex(wallDefs[i].p3, wallColors[i], Vec2(1, 1));
            wallMeshes[i]->addVertex(wallDefs[i].p4, wallColors[i], Vec2(0, 1));

            std::vector<uint32_t> wallIndices = {
                0, 1, 2,
                0, 2, 3
            };
            wallMeshes[i]->setIndices(wallIndices);

            wallMeshes[i]->create();
        }
    }
    ~App()
    {
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

        handleMouseInput();

        std::stringstream ss;
        ss << "FCT Demo - Instant FPS: " << std::fixed << std::setprecision(1) << instantFPS;

        autoReviewport.submit();

        constBuffer->updataData();
        vertexConstBuffer->updataData();

        wnd->title(ss.str());

        auto floorJob = new TraditionRenderJob();
        floorJob->addMesh(floorMesh)
            .setPassResource(passResource)
            .setPipelineState(pso);
        ctx->submit(floorJob, "nomralObject");
        floorJob->release();

        auto wallJob = new TraditionRenderJob();
        for (int i = 0; i < 4; i++) {
            wallJob->addMesh(wallMeshes[i])
                .setPassResource(passResource)
                .setPipelineState(pso);
        }
        ctx->submit(wallJob, "nomralObject");
        wallJob->release();
        vertexCtx->clearPath("frontWallScreen");
        vertexCtx->addPath("frontWallScreen", emjPath);
        vertexCtx->clearPath("backWallScreen");
        vertexCtx->addPath("backWallScreen", goodPath);

        vertexCtx->submit();
        std::this_thread::yield();
        ctx->flush();
    }
    void submitTick()
    {
        auto cmdBuf = ctx->getCmdBuf(wnd, 0);
        cmdBuf->reset();
        cmdBuf->begin();
        ctx->excutePasses(cmdBuf);
        cmdBuf->end();
        cmdBuf->submit();
        std::this_thread::yield();
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
    _output_object(fout);
    return 0;
}