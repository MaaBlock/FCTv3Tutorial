
#include "../FCT/FCTAPI.h"
#include "./AutoReviewport.h"
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

    bool keyState[512] = {};
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
    float logViewScale = 0.0f;

    VertexContext* vertexCtx;
    Freetype_Font* emjFont;
    Freetype_Font* font;
    VertexPath* emjPath;
    VertexPath* goodPath;
    UniformBuffer* vertexBuffer;

    RHI::ConstBuffer* vertexConstBuffer;

    float pathX = 0,pathY = 0,pathZ = 0;

    GLFW_VK_ImGuiContext* imguiCtx;
public:
    void initVertex()
    {
        vertexCtx = new VertexContext(ctx,wnd);
        vertexCtx->mvpUniformLayout(mvpUniform);


        Mat4 proj = Mat4::Perspective(45.0f, 800.0f / 600.0f, 0.1f, 100.0);
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

        const float roomSize = 9.9f;
        const float halfSize = roomSize / 2.0f;
        const float wallHeight = 3.0f;

        vertexCtx->setScreen("screen")
            .setOrigin(Vec3(-1.0, 1.0, 0))
            .setX(Vec3(2, 0, 0))
            .setY(Vec3(0, -2, 0))
            .setVertexCoordSize(Vec2(800, 600))
            .setOriginVertexCoord(Vec2(-400, -300));

    }
    void setupRenderGraph()
{
    Pass* pass = new TraditionRenderPass(ctx);
    pass->enableClear(ClearType::color | ClearType::depthStencil, Vec4(0.1f, 0.1f, 0.1f, 1.0f));
    ctx->addPass("GBufferPass", pass);
    pass->release();

    pass = new TraditionRenderPass(ctx);
    pass->enableClear(ClearType::depthStencil, Vec4(1, 1, 1, 1));
    ctx->addPass("ShadowMapPass", pass);
    pass->release();

    pass = new TraditionRenderPass(ctx);
    ctx->addPass("AmbientOcclusionPass", pass);
    pass->release();

    pass = new TraditionRenderPass(ctx);
    ctx->addPass("AtmosphericScatteringPass", pass);
    pass->release();

    pass = new TraditionRenderPass(ctx);
    ctx->addPass("LightingPass", pass);
    pass->release();

    pass = new TraditionRenderPass(ctx);
    ctx->addPass("VolumetricLightingPass", pass);
    pass->release();

    pass = new TraditionRenderPass(ctx);
    ctx->addPass("ReflectionPass", pass);
    pass->release();

    pass = new TraditionRenderPass(ctx);
    ctx->addPass("BloomPass", pass);
    pass->release();

    pass = new TraditionRenderPass(ctx);
    ctx->addPass("TonemappingPass", pass);
    pass->release();

    pass = new TraditionRenderPass(ctx);
    ctx->addPass("MotionBlurPass", pass);
    pass->release();

    pass = new TraditionRenderPass(ctx);
    ctx->addPass("AntiAliasingPass", pass);
    pass->release();

    pass = new TraditionRenderPass(ctx);
    ctx->addPass("PostEffectsPass", pass);
    pass->release();

    pass = new TraditionRenderPass(ctx);
    ctx->addPass("UIRenderPass", pass);
    pass->release();

    pass = new TraditionRenderPass(ctx);
    ctx->addPass("DebugInfoPass", pass);
    pass->release();

    pass = new TraditionRenderPass(ctx);
    ctx->addPass("FinalCompositePass", pass);
    pass->release();

    ctx->bindOutputImage("GBufferPass", "albedo", 0);
    ctx->bindOutputImage("GBufferPass", "normal", 1);
    ctx->bindOutputImage("GBufferPass", "position", 2);
    ctx->bindOutputImage("GBufferPass", "depth", 3);

    ctx->bindOutputImage("ShadowMapPass", "shadowMap", 0);
    ctx->bindOutputImage("AmbientOcclusionPass", "ao", 0);
    ctx->bindOutputImage("AtmosphericScatteringPass", "scattering", 0);
    ctx->bindOutputImage("LightingPass", "lighting", 0);
    ctx->bindOutputImage("VolumetricLightingPass", "volumetricLighting", 0);
    ctx->bindOutputImage("ReflectionPass", "reflection", 0);
    ctx->bindOutputImage("BloomPass", "bloom", 0);
    ctx->bindOutputImage("TonemappingPass", "tonemapped", 0);
    ctx->bindOutputImage("MotionBlurPass", "motionBlur", 0);
    ctx->bindOutputImage("AntiAliasingPass", "antiAliased", 0);
    ctx->bindOutputImage("PostEffectsPass", "postEffects", 0);
    ctx->bindOutputImage("UIRenderPass", "ui", 0);
    ctx->bindOutputImage("DebugInfoPass", "debugInfo", 0);
    ctx->bindOutputImage("FinalCompositePass", "final", 0);

    ctx->addPassDenpendency("ShadowMapPass", "GBufferPass");
    ctx->addPassDenpendency("GBufferPass", "AmbientOcclusionPass");
    ctx->addPassDenpendency("GBufferPass", "AtmosphericScatteringPass");

    ctx->addPassDenpendency("AmbientOcclusionPass", "LightingPass");
    ctx->addPassDenpendency("GBufferPass", "LightingPass");
    ctx->addPassDenpendency("ShadowMapPass", "LightingPass");
    ctx->addPassDenpendency("AtmosphericScatteringPass", "LightingPass");

    ctx->addPassDenpendency("LightingPass", "VolumetricLightingPass");
    ctx->addPassDenpendency("ShadowMapPass", "VolumetricLightingPass");

    ctx->addPassDenpendency("LightingPass", "ReflectionPass");
    ctx->addPassDenpendency("GBufferPass", "ReflectionPass");

    ctx->addPassDenpendency("ReflectionPass", "BloomPass");
    ctx->addPassDenpendency("VolumetricLightingPass", "BloomPass");

    ctx->addPassDenpendency("BloomPass", "TonemappingPass");
    ctx->addPassDenpendency("ReflectionPass", "TonemappingPass");

    ctx->addPassDenpendency("TonemappingPass", "MotionBlurPass");
    ctx->addPassDenpendency("GBufferPass", "MotionBlurPass");

    ctx->addPassDenpendency("MotionBlurPass", "AntiAliasingPass");
    ctx->addPassDenpendency("AntiAliasingPass", "PostEffectsPass");
    ctx->addPassDenpendency("PostEffectsPass", "UIRenderPass");
    ctx->addPassDenpendency("UIRenderPass", "DebugInfoPass");
    ctx->addPassDenpendency("DebugInfoPass", "FinalCompositePass");

    ctx->addPassDenpendency("FinalCompositePass", "vertexPass");
    ctx->addPassDenpendency("vertexPass", "imguiPass");
    ctx->addPassDenpendency("nomralObject", "vertexPass");

    ctx->bindOutput("FinalCompositePass", wnd);
}
    App(Runtime& rt) : rt(rt)
    {
        for (int i = 0 ;i < 256;++i)
        {
            keyState[i] = false;
        }
        emjFont = rt.createFont();
        emjFont->create("C://Windows//Fonts//seguiemj.ttf");
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
        });
        wnd->getCallBack()->addMouseWheelCallback([this](Window* wnd, int dealt)
{
    const float logScaleStep = 0.1f;
    logViewScale += dealt * logScaleStep;

    if (logViewScale < -1.0f) logViewScale = -1.0f;

    float viewScale = std::pow(10.0f, logViewScale);

    currentViewScale = viewScale;

});
        ctx->maxFrameInFlight(5);

        InitImgui();
        setupImGuiStyle();

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

        pass = new TraditionRenderPass(ctx);
        pass->enableClear(ClearType::color | ClearType::depthStencil,Vec4(1,1,1,1));

        ctx->bindOutput("nomralObject",wnd);
        ctx->bindOutput("vertexPass",wnd);
        ctx->bindOutput("imguiPass",wnd);

        imguiCtx = new GLFW_VK_ImGuiContext(
            static_cast<GLFW_Window*>(wnd),
            static_cast<VK_Context*>(ctx)
        );
        initVertex();
        init();
        setupRenderGraph();
        ctx->compilePasses();
        imguiCtx->attachPass("imguiPass");
        autoReviewport.addPass("vertexPass");
        autoReviewport.addPass("nomralObject");
        ImGuiIO& io = ImGui::GetIO();
        ctx->currentGraph()->printExecutionOrder();
        ctx->currentGraph()->printResourceInfo();


    }
    void setupImGuiStyle()
    {
        ImGuiIO& io = ImGui::GetIO();

        ImFont* font = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\msyh.ttc", 18.0f, nullptr,
            io.Fonts->GetGlyphRangesChineseFull());

        if (font == nullptr) {
            font = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\simsun.ttc", 18.0f, nullptr,
                io.Fonts->GetGlyphRangesChineseFull());
        }

        if (font == nullptr) {
            fout << "警告：无法加载中文字体，ImGui界面可能无法正确显示中文" << std::endl;
        } else
        {
            io.FontDefault = font;
        }

        io.Fonts->Build();

        ImGui::StyleColorsLight();
        ImGuiStyle& style = ImGui::GetStyle();

        style.WindowRounding = 5.0f;
        style.FrameRounding = 3.0f;
        style.ScrollbarRounding = 3.0f;

        style.Colors[ImGuiCol_WindowBg] = ImVec4(0.95f, 0.95f, 0.95f, 0.95f);
        style.Colors[ImGuiCol_Header] = ImVec4(0.7f, 0.8f, 0.9f, 0.5f);
        style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.8f, 0.9f, 1.0f, 0.6f);
        style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.8f, 0.9f, 1.0f, 0.8f);

        style.ItemSpacing = ImVec2(8, 6);
        style.FramePadding = ImVec2(6, 4);
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

        texture = ctx->loadTexture("../../img.png");

        passResource->addTexture(texture, resourceLayout.findTexture("testTexture"));
        passResource->addSampler(sampler, resourceLayout.findSampler("testSampler"));
        passResource->create();
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
        std::stringstream ss;
        ss << "FCT Demo - Instant FPS: " << std::fixed << std::setprecision(1) << instantFPS;
        autoReviewport.submit();
        wnd->title(ss.str());
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