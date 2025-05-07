
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
    RHI::ConstBuffer* vertexConstBuffer;
    UniformBuffer* buffer;
    UniformBuffer* vertexBuffer;
    PassResource* passResource;
    float rotationAngle;
    Image* texture;
    PSampler sampler;
    AutoReviewport autoReviewport;
    float rotationAngleY;
    StaticMesh<uint32_t>* teapotMesh;
    TraditionPipelineState* pso;
    DynamicMesh<uint32_t>* dynamicMesh;
    float animationTime;
    RHI::ConstBuffer* teapotConstBuffer;
    UniformBuffer* teapotBuffer;
    PassResource* teapotPassResource;
    PassResource* meshPassResource;

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
public:
    App(Runtime& rt) : rt(rt)
    {
        cameraPosition = Vec3(0, 15, 30);
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
        static_cast<GLFW_Window*>(wnd)->postUiTask([this](void*)
        {
            /*
            if (glfwRawMouseMotionSupported())
            {
                glfwSetInputMode(static_cast<GLFWwindow*>(static_cast<GLFW_Window*>(wnd)->getWindow()),
                             GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
            }*/
        });
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
        init();
        ctx->compilePasses();
        initDynamicMesh();
        autoReviewport.addPass("vertexPass");
        autoReviewport.addPass("nomralObject");
    }
    void handleKeyboardInput(float deltaTime)
    {
        if (!mouseControlEnabled) return;

        Vec3 front;
        front.x = cos(cameraYaw * 3.14159f / 180.0f) * cos(cameraPitch * 3.14159f / 180.0f);
        front.y = sin(cameraPitch * 3.14159f / 180.0f);
        front.z = sin(cameraYaw * 3.14159f / 180.0f) * cos(cameraPitch * 3.14159f / 180.0f);
        front = front.normalize();

        Vec3 right = front.cross(cameraUp).normalize();

        float cameraSpeed = 20.0f * deltaTime;

        if (keyState[GLFW_KEY_W])
            cameraPosition = cameraPosition + front * cameraSpeed;
        if (keyState[GLFW_KEY_S])
            cameraPosition = cameraPosition - front * cameraSpeed;

        if (keyState[GLFW_KEY_A])
            cameraPosition = cameraPosition - right * cameraSpeed;
        if (keyState[GLFW_KEY_D])
            cameraPosition = cameraPosition + right * cameraSpeed;

        if (keyState[GLFW_KEY_Q])
            cameraPosition.y -=  cameraSpeed;
        if (keyState[GLFW_KEY_E])
            cameraPosition.y +=  cameraSpeed;

        updateCamera();
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

        teapotBuffer->setValue("viewMatrix", view);
        buffer->setValue("viewMatrix", view);
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

        pso = new TraditionPipelineState();
        pso->vertexLayout = vertexLayout;
        pso->pixelLayout = pixelLayout;
        pso->resourceLayout = resourceLayout;
        pso->vertexShader = vs;
        pso->pixelShader = ps;

        Mat4 view = Mat4::LookAt(Vec3(0,15,30), Vec3(0,0,0), Vec3(0,-1,0));
        Mat4 proj = Mat4::Perspective(45.0f, 800.0f / 600.0f, 0.1f, 100.0);

        teapotBuffer = new UniformBuffer(mvpUniform);
        teapotBuffer->setValue("modelMatrix", Mat4::Scale(0.01,0.01,0.01));
        teapotBuffer->setValue("viewMatrix", view);
        teapotBuffer->setValue("projectionMatrix", proj);

        teapotConstBuffer = ctx->createConstBuffer();
        teapotConstBuffer->layout(mvpUniform);
        teapotConstBuffer->buffer(teapotBuffer);
        teapotConstBuffer->create();
        teapotConstBuffer->mapData();

        teapotPassResource = ctx->createPassResource();
        teapotPassResource->addConstBuffer(teapotConstBuffer);
        teapotPassResource->bind(wnd);

        buffer = new UniformBuffer(mvpUniform);
        buffer->setValue("modelMatrix", Mat4());
        buffer->setValue("viewMatrix", view);
        buffer->setValue("projectionMatrix", proj);

        constBuffer = ctx->createConstBuffer();
        constBuffer->layout(mvpUniform);
        constBuffer->buffer(buffer);
        constBuffer->create();
        constBuffer->mapData();

        meshPassResource = ctx->createPassResource();
        meshPassResource->addConstBuffer(constBuffer);
        meshPassResource->bind(wnd);

        ctx->allocBaseCommandBuffers(wnd);

        sampler = ctx->createSampler();
        sampler->setAnisotropic();
        sampler->create();

        texture = ctx->loadTexture("./res/img.png");

        teapotPassResource->addTexture(texture, resourceLayout.findTexture("testTexture"));
        teapotPassResource->addSampler(sampler, resourceLayout.findSampler("testSampler"));
        teapotPassResource->create();

        meshPassResource->addTexture(texture, resourceLayout.findTexture("testTexture"));
        meshPassResource->addSampler(sampler, resourceLayout.findSampler("testSampler"));
        meshPassResource->create();

        teapotMesh = ctx->loadMesh("./res/teapot.obj", "teapot", vertexLayout);
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
        handleKeyboardInput(deltaTime);

        std::stringstream ss;
        ss << "FCT Demo - Instant FPS: " << std::fixed << std::setprecision(1) << instantFPS;
        float rotationV = 90.0f; //90度/s
        rotationAngle += deltaTime * rotationV;
        rotationAngleY += deltaTime * rotationV * 0.7f;

        animationTime += deltaTime;

        autoReviewport.submit();
        updateDynamicMesh(animationTime);

        Mat4 rotation;
        rotation.rotateX(rotationAngle);
        rotation.rotateY(rotationAngleY);
        rotation *= Mat4::Scale(0.25,0.25,0.25);
        teapotBuffer->setValue("modelMatrix", rotation);

        teapotConstBuffer->updataData();
        constBuffer->updataData();

        wnd->title(ss.str());

        auto teapotJob = new TraditionRenderJob();
        teapotJob->addMesh(teapotMesh)
            .setPassResource(teapotPassResource)
            .setPipelineState(pso);
        ctx->submit(teapotJob, "nomralObject");
        teapotJob->release();

        auto meshJob = new TraditionRenderJob();
        meshJob->addMesh(dynamicMesh)
            .setPassResource(meshPassResource)
            .setPipelineState(pso);
        ctx->submit(meshJob, "nomralObject");
        meshJob->release();

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