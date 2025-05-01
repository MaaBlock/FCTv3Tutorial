
#define _USE_MATH_DEFINES
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


const float CommandEnd = -1.0;
const float CommandMoveTo = 1.0;
const float CommandLineTo = 2.0;
const float CommandBezierCurveTo = 3.0;
const float CommandSetColor = 4.0;
const float CommandArcTo = 5.0;
const float CommandSetTransform = 6.0;
const float CommandBeginPath = 7.0;
const float CommandEndPath = 8.0;

class VertexPath
{
protected:
    std::vector<float> m_commandQueue;
    Vec2 m_currentPos;
    Vec2 m_pathStart;
    Vec2 m_minBounds;
    Vec2 m_maxBounds;
    bool m_hasPoints;

    enum CommandType {
        Command_MoveTo = 1,
        Command_LineTo = 2,
        Command_BezierCurveTo = 3,
        Command_SetColor = 4,
        Command_SetTransform = 5,
        Command_BeginPath = 6,
        Command_EndPath = 7,
        Command_End = 8,
        CommandArcTo = 9
    };

    void updateBounds(const Vec2& point) {
        if (!m_hasPoints) {
            m_minBounds = point;
            m_maxBounds = point;
            m_hasPoints = true;
        } else {
            m_minBounds.x = std::min(m_minBounds.x, point.x);
            m_minBounds.y = std::min(m_minBounds.y, point.y);
            m_maxBounds.x = std::max(m_maxBounds.x, point.x);
            m_maxBounds.y = std::max(m_maxBounds.y, point.y);
        }
    }

    void unwrapperArcTo(Vec2 center, float beginAngle, float endAngle, float q) {
        m_commandQueue.push_back(CommandArcTo);
        m_commandQueue.push_back(center.x);
        m_commandQueue.push_back(center.y);
        m_commandQueue.push_back(beginAngle);
        m_commandQueue.push_back(endAngle);
        m_commandQueue.push_back(q);

        float radius = m_currentPos.distance(center);

        float angles[] = {0, M_PI_2, M_PI, M_PI + M_PI_2}; // 0°, 90°, 180°, 270°

        for (float angle : angles) {
            if (beginAngle <= angle && angle <= endAngle) {
                Vec2 point(center.x + radius * cos(angle), center.y + radius * sin(angle));
                updateBounds(point);
            }
        }

        // 还需要考虑弧的起点和终点
        Vec2 startPoint(center.x + radius * cos(beginAngle), center.y + radius * sin(beginAngle));
        Vec2 endPoint(center.x + radius * cos(endAngle), center.y + radius * sin(endAngle));
        updateBounds(startPoint);
        updateBounds(endPoint);
    }

public:
    VertexPath() : m_hasPoints(false) {
        clear();
    }

    void clear() {
        m_commandQueue.clear();
        m_hasPoints = false;
        m_minBounds = Vec2(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
        m_maxBounds = Vec2(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest());
    }

    void beginPath(float sign = 1.0f) {
        m_commandQueue.push_back(Command_BeginPath);
        m_commandQueue.push_back(sign);
    }

    void endPath() {
        m_commandQueue.push_back(Command_EndPath);
    }

    void moveTo(Vec2 to) {
        m_commandQueue.push_back(Command_MoveTo);
        m_commandQueue.push_back(to.x);
        m_commandQueue.push_back(to.y);
        m_currentPos = to;
        m_pathStart = to;
        updateBounds(to);
    }

    void lineTo(Vec2 to) {
        m_commandQueue.push_back(Command_LineTo);
        m_commandQueue.push_back(to.x);
        m_commandQueue.push_back(to.y);
        m_currentPos = to;
        updateBounds(to);
    }

    void fillRect(Vec2 pos, Vec2 size) {
        moveTo(pos);
        lineTo(Vec2(pos.x, pos.y + size.y));
        lineTo(Vec2(pos.x + size.x, pos.y + size.y));
        lineTo(Vec2(pos.x + size.x, pos.y));
        lineTo(pos);
    }

    void arcTo(Vec2 center, float beginAngle, float endAngle) {
        beginAngle = fmod(beginAngle, 2 * M_PI);
        endAngle = fmod(endAngle, 2 * M_PI);
        if (beginAngle < 0) {
            beginAngle += 2 * M_PI;
            if (endAngle) {
                endAngle += 2 * M_PI;
                endAngle = fmod(endAngle, 2 * M_PI);
            } else {
                endAngle += 2 * M_PI;
            }
        }
        if (endAngle < 0) {
            if (beginAngle) {
                beginAngle += 2 * M_PI;
                beginAngle = fmod(beginAngle, 2 * M_PI);
            } else {
                beginAngle += 2 * M_PI;
            }
            endAngle += 2 * M_PI;
        }

        if (abs(endAngle - beginAngle) < 1e-6 || abs(endAngle - beginAngle) >= 2 * M_PI - 1e-6) {
            for (int q = 1; q <= 4; ++q) {
                unwrapperArcTo(center, (q - 1) * M_PI_2, q * M_PI_2, q);
            }
            return;
        }

        bool isClockwise = endAngle < beginAngle;

        if (isClockwise) {
            if (endAngle > beginAngle)
                endAngle -= 2 * M_PI;

            for (int q = 4; q >= 1; --q) {
                float quadrantStart = q * M_PI_2;
                float quadrantEnd = (q - 1) * M_PI_2;

                if (beginAngle > quadrantEnd && endAngle < quadrantStart) {
                    float arcStart = std::min(beginAngle, quadrantStart);
                    float arcEnd = std::max(endAngle, quadrantEnd);
                    unwrapperArcTo(center, arcStart, arcEnd, q);
                }

                if (endAngle >= quadrantEnd)
                    break;
            }
        } else {
            if (endAngle < beginAngle)
                endAngle += 2 * M_PI;

            for (int q = 1; q <= 4; ++q) {
                float quadrantStart = (q - 1) * M_PI_2;
                float quadrantEnd = q * M_PI_2;

                if (beginAngle < quadrantEnd && endAngle > quadrantStart) {
                    float arcStart = std::max(beginAngle, quadrantStart);
                    float arcEnd = std::min(endAngle, quadrantEnd);
                    unwrapperArcTo(center, arcStart, arcEnd, q);
                }

                if (endAngle <= quadrantEnd)
                    break;
            }
        }
    }

    void curveTo(Vec2 control, Vec2 to) {
        m_commandQueue.push_back(Command_BezierCurveTo);
        m_commandQueue.push_back(control.x);
        m_commandQueue.push_back(control.y);
        m_commandQueue.push_back(to.x);
        m_commandQueue.push_back(to.y);

        updateBounds(m_currentPos);
        updateBounds(control);
        updateBounds(to);

        m_currentPos = to;
    }

    void curveTo(Vec2 control1, Vec2 control2, Vec2 to) {
        Vec2 start = m_currentPos;

        Vec2 q1 = start + (control1 - start) * 2.0f / 3.0f;
        Vec2 q2 = to + (control2 - to) * 2.0f / 3.0f;
        Vec2 m = (q1 + q2) * 0.5f;

        m_commandQueue.push_back(Command_BezierCurveTo);
        m_commandQueue.push_back(q1.x);
        m_commandQueue.push_back(q1.y);
        m_commandQueue.push_back(m.x);
        m_commandQueue.push_back(m.y);

        m_commandQueue.push_back(Command_BezierCurveTo);
        m_commandQueue.push_back(q2.x);
        m_commandQueue.push_back(q2.y);
        m_commandQueue.push_back(to.x);
        m_commandQueue.push_back(to.y);

        updateBounds(start);
        updateBounds(control1);
        updateBounds(control2);
        updateBounds(to);

        m_currentPos = to;
    }

    void circle(Vec2 center, float radius) {
        moveTo(Vec2(center.x + radius, center.y));
        arcTo(center, 0, 2 * M_PI);

        updateBounds(Vec2(center.x - radius, center.y - radius));
        updateBounds(Vec2(center.x + radius, center.y + radius));
    }

    void setColor(Vec4 color) {
        m_commandQueue.push_back(Command_SetColor);
        m_commandQueue.push_back(color.x);
        m_commandQueue.push_back(color.y);
        m_commandQueue.push_back(color.z);
        m_commandQueue.push_back(color.w);
    }

    void setTransform(const Mat4 &transform) {
        m_commandQueue.push_back(Command_SetTransform);
        for (int i = 0; i < 16; i++) {
            m_commandQueue.push_back(transform.m[i]);
        }
    }

    void closePath() {
        lineTo(m_pathStart);
    }

    Vec4 getBoundingRect() const {
        if (!m_hasPoints) {
            return Vec4(0, 0, 0, 0);
        }
        return Vec4(m_minBounds.x, m_minBounds.y,
                   m_maxBounds.x - m_minBounds.x,
                   m_maxBounds.y - m_minBounds.y);
    }

    const std::vector<float>& getCommandQueue() const {
        return m_commandQueue;
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
    Mesh<>* cubeMesh;
    RHI::ConstBuffer* constBuffer;
    UniformBuffer* buffer;
    PassResource* passResource;
    int frameIndex;
    int maxFrameInFlight;
    float rotationAngle;
    Image* texture;
    PSampler sampler;
    AutoReviewport autoReviewport;
    float rotationAngleY;
    Mesh<uint32_t>* teapotMesh;
    GLFW_VK_ImGuiContext* imguiCtx;
    TraditionPipelineState* pso;
public:
    App(Runtime& rt) : rt(rt)
    {
        fout << "Hello, FCT!" << endl;
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
        ctx->maxFrameInFlight(5);

        InitImgui();
        ImGui::StyleColorsLight();

        rt.postUiTask([this](void*)
        {
            },nullptr);
        Pass* pass = new ::TraditionRenderPass(ctx);
        pass->enableClear(ClearType::color | ClearType::depthStencil,Vec4(1,1,1,1));
        ctx->addPass("nomralObject",pass);
        pass->release();
        pass = new ::TraditionRenderPass(ctx);
        ctx->addPass("imguiPass",pass);
        pass->release();
        ctx->addPassDenpendency("nomralObject","imguiPass");
        ctx->bindOutput("nomralObject",wnd);
        ctx->bindOutput("imguiPass",wnd);
        ctx->compilePasses();
        imguiCtx = new GLFW_VK_ImGuiContext(
            static_cast<GLFW_Window*>(wnd),
            static_cast<VK_Context*>(ctx)
        );
        imguiCtx->attachPass("imguiPass");
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

        passResource = ctx->createPassResource();
        passResource->addConstBuffer(constBuffer);
        passResource->bind(wnd);

        ctx->allocBaseCommandBuffers(wnd);

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
        Mat4 ratation;
        ratation.rotateX(rotationAngle);
        ratation.rotateY(rotationAngleY);
        buffer->setValue("modelMatrix", ratation);
        constBuffer->updataData();
        wnd->title(ss.str());
        auto job = new TraditionRenderJob();
        job->addMesh(teapotMesh)
            .addMesh(cubeMesh)
            .setPassResource(passResource)
            .setPipelineState(pso);
        ctx->submit(job,"nomralObject");
        imguiCtx->addUi([]()
        {
            ImGui::Begin("FCT Debug");
            ImGui::Text("Hello, FCT!");
            ImGui::End();
        });
        imguiCtx->submitJob();
        job->release();
        ctx->flush();
    }
    void submitTick()
    {
        auto cmdBuf = ctx->getCmdBuf(wnd, 0);
        cmdBuf->reset();
        cmdBuf->begin();
        autoReviewport.onRenderTick(cmdBuf);
        ctx->excutePasses(cmdBuf);
        cmdBuf->end();
        cmdBuf->submit();
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