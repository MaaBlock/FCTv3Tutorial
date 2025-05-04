
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



constexpr float VertexCommand_End = -1.0;
constexpr float VertexCommand_MoveTo = 1.0;
constexpr float VertexCommand_LineTo = 2.0;
constexpr float VertexCommand_BezierCurveTo = 3.0;
constexpr float VertexCommand_SetColor = 4.0;
constexpr float VertexCommand_ArcTo = 6.0;
constexpr float VertexCommand_SetTransform = 5.0;
constexpr float VertexCommand_BeginPath = 7.0;
constexpr float VertexCommand_EndPath = 8.0;
class VertexPath : public RefCount
{
protected:
    std::vector<float> m_commandQueue;
    Vec2 m_currentPos;
    Vec2 m_pathStart;
    Vec2 m_minBounds;
    Vec2 m_maxBounds;
    bool m_hasPoints;

    enum CommandType {
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
    void end()
    {
        m_commandQueue.push_back(VertexCommand_End);
    }
    void unwrapperArcTo(Vec2 center, float beginAngle, float endAngle, float q) {
        m_commandQueue.push_back(VertexCommand_ArcTo);
        m_commandQueue.push_back(center.x);
        m_commandQueue.push_back(center.y);
        m_commandQueue.push_back(beginAngle);
        m_commandQueue.push_back(endAngle);
        m_commandQueue.push_back(q);

        float radius = m_currentPos.distance(center);

        float angles[] = {0, M_PI_2, M_PI, M_PI + M_PI_2};

        for (float angle : angles) {
            if (beginAngle <= angle && angle <= endAngle) {
                Vec2 point(center.x + radius * cos(angle), center.y + radius * sin(angle));
                updateBounds(point);
            }
        }

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
        m_commandQueue.push_back(VertexCommand_BeginPath);
        m_commandQueue.push_back(sign);
    }

    void endPath() {
        m_commandQueue.push_back(VertexCommand_EndPath);
    }

    void moveTo(Vec2 to) {
        m_commandQueue.push_back(VertexCommand_MoveTo);
        m_commandQueue.push_back(to.x);
        m_commandQueue.push_back(to.y);
        m_currentPos = to;
        m_pathStart = to;
        updateBounds(to);
    }

    void lineTo(Vec2 to) {
        m_commandQueue.push_back(VertexCommand_LineTo);
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
        m_commandQueue.push_back(VertexCommand_BezierCurveTo);
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

        m_commandQueue.push_back(VertexCommand_BezierCurveTo);
        m_commandQueue.push_back(q1.x);
        m_commandQueue.push_back(q1.y);
        m_commandQueue.push_back(m.x);
        m_commandQueue.push_back(m.y);

        m_commandQueue.push_back(VertexCommand_BezierCurveTo);
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
        m_commandQueue.push_back(VertexCommand_SetColor);
        m_commandQueue.push_back(color.x);
        m_commandQueue.push_back(color.y);
        m_commandQueue.push_back(color.z);
        m_commandQueue.push_back(color.w);
    }

    void setTransform(const Mat4 &transform) {
        m_commandQueue.push_back(VertexCommand_SetTransform);
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
struct VertexScreenInfo {
    Vec4 origin;
    Vec4 XDir;
    Vec4 YDir;
    Vec4 originVertexCoord;
    Vec4 VertexCoordSize;
};
struct VertexScreen {
public:
    VertexScreen(std::vector<VertexScreenInfo>& infos, size_t infoIndex)
        : m_infos(infos), m_infoIndex(infoIndex)
    {
    }

    void pushPath(VertexPath* path)
    {
        path->addRef();
        m_paths.push_back(path);
    }

    void clearPaths()
    {
        for (auto& path : m_paths) {
            path->release();
        }
        m_paths.clear();
    }

    VertexScreen& setX(Vec3 dir)
    {
        m_infos[m_infoIndex].XDir = Vec4(dir);
        return *this;
    }

    VertexScreen& setY(Vec3 dir)
    {
        m_infos[m_infoIndex].YDir = Vec4(dir);
        return *this;
    }

    VertexScreen& setOrigin(Vec3 origin)
    {
        m_infos[m_infoIndex].origin = Vec4(origin);
        return *this;
    }

    VertexScreen& setOriginVertexCoord(Vec2 origin)
    {
        m_infos[m_infoIndex].originVertexCoord = Vec4(origin);
        return *this;
    }

    VertexScreen& setVertexCoordSize(Vec2 size)
    {
        m_infos[m_infoIndex].VertexCoordSize = Vec4(size);
        return *this;
    }

    const VertexScreenInfo& getInfo() const
    {
        return m_infos[m_infoIndex];
    }

    const std::vector<VertexPath*>& getPaths() const
    {
        return m_paths;
    }
protected:
    std::vector<VertexScreenInfo>& m_infos;
    size_t m_infoIndex;
    std::vector<VertexPath*> m_paths;
};
using VertexSubmitJobCallBack = std::function<void()>;
class VertexSubmitJob : public SubmitJob
{
protected:
    VertexSubmitJobCallBack m_callBack;
public:
    VertexSubmitJob(VertexSubmitJobCallBack callBack)
    {
        m_callBack = callBack;
    }
    virtual void submit(RHI::CommandBuffer* cmdBuffers) override
    {
        m_callBack();
    }
};
class VertexContext
{
protected:
    Context* m_ctx;
    std::string m_passName;
    std::pair<ConstLayout,ConstElement> m_viewMatrix;
    std::pair<ConstLayout,ConstElement> m_projectionMatrix;
    PassResource* m_passResource;
    std::vector<VertexScreen> m_screens;
    std::map<std::string,  uint32_t> m_screenIndexMap;
    std::vector<VertexScreenInfo> m_infos;
    //screen 不超过 4000 / 5 = 800个
    TraditionPipelineState* m_pso;
    static constexpr VertexLayout vertexLayout {
        VertexElement{VtxType::VertexCoord2f},
        VertexElement{VtxType::CommandOffset},
        VertexElement{VtxType::CommandSize},
        VertexElement{VtxType::BatchID,"screenId"},
    };
    static constexpr PixelLayout pixelLayout {
        vertexLayout,
        VertexElement{VtxType::Position4f},
    };
    static constexpr const char* VertexCommandBufferNameInShder = "VertexCommandBuffer";
    static constexpr const char* ScreenInfoNameInShder = "ScreenInfo";
    static constexpr ResourceLayout resourceLayout {
        TextureElement{ScreenInfoNameInShder},
        TextureElement{VertexCommandBufferNameInShder},
    };
    VertexShader* m_vs;
    PixelShader* m_ps;
    std::string m_uploadPassName;
    uint8_t m_targetIndex;
    DynamicMesh<uint32_t>* m_mesh;
    std::vector<float> m_commandBuffer;
    Window* m_wnd;
public:
    VertexContext(Context* ctx,Window* wnd) : m_ctx(ctx) ,m_wnd(wnd)
    {
        m_targetIndex = 0;
        m_passResource = m_ctx->createPassResource();
        m_passResource->bind(wnd);
    }

    void updataCommandInfo();
    void submitPath();
    void addRectangle(uint32_t screenId, uint32_t commandOffset, uint32_t commandSize, const Vec4& bounds);
    void create();

    void addScreen(const std::string& name);
    VertexScreen& setScreen(const std::string& name);
    void mvpUniformLayout(ConstLayout layout);;

    void attachPass(std::string passName,std::string uploadPassName = "")
    {
        m_passName = passName;
        if (uploadPassName.empty())
        {
            uploadPassName = passName + "_VertexUpload";
        }
        m_uploadPassName = uploadPassName;

        Pass* uploadPass = new TraditionRenderPass(m_ctx);
        m_ctx->addPass(uploadPassName, uploadPass);
        uploadPass->release();

        m_ctx->bindOutputImage(uploadPassName, VertexCommandBufferNameInShder, 0);
        m_ctx->bindOutputImage(uploadPassName, ScreenInfoNameInShder, 1);

        m_ctx->bindTextureImage(passName, VertexCommandBufferNameInShder,4096,1,Samples::sample_1);
        m_ctx->bindTextureImage(passName, ScreenInfoNameInShder,4000,1,Samples::sample_1);

        create();
    }
    void generateVertexShader();
    void generatePixelShader();

    void addPath(std::string screenName,VertexPath* path)
    {
        VertexScreen& screen = setScreen(screenName);
        screen.pushPath(path);
    }
    void clearPath(std::string screenName)
    {
        VertexScreen& screen = setScreen(screenName);
        screen.clearPaths();
    }
    void addConstBuffer(RHI::ConstBuffer* constBuffer)
    {
        m_passResource->addConstBuffer(constBuffer);
    }
    void updataScreenInfo()
    {
        auto screenInfoImage = m_ctx->getResourceImage(ScreenInfoNameInShder);

        uint32_t totalElements = m_infos.size() * 5;

        std::vector<float> screenData;
        screenData.reserve(totalElements * 4);

        for (const auto& info : m_infos) {
            screenData.push_back(info.origin.x);
            screenData.push_back(info.origin.y);
            screenData.push_back(info.origin.z);
            screenData.push_back(info.origin.w);

            screenData.push_back(info.XDir.x);
            screenData.push_back(info.XDir.y);
            screenData.push_back(info.XDir.z);
            screenData.push_back(info.XDir.w);

            screenData.push_back(info.YDir.x);
            screenData.push_back(info.YDir.y);
            screenData.push_back(info.YDir.z);
            screenData.push_back(info.YDir.w);

            screenData.push_back(info.originVertexCoord.x);
            screenData.push_back(info.originVertexCoord.y);
            screenData.push_back(info.originVertexCoord.z);
            screenData.push_back(info.originVertexCoord.w);

            screenData.push_back(info.VertexCoordSize.x);
            screenData.push_back(info.VertexCoordSize.y);
            screenData.push_back(info.VertexCoordSize.z);
            screenData.push_back(info.VertexCoordSize.w);
        }

        if (screenInfoImage->width() * screenInfoImage->height() >= totalElements) {
            auto res = screenInfoImage->updateToCurrent(screenData.data(), screenData.size() * sizeof(float));
            auto job = new VertexSubmitJob([res]()
            {
                res->waitFor();
            });
            m_ctx->submit(job,m_uploadPassName);
            job->release();
        } else {
            ferr << "目前仅支持 4000 / 5 = 800 个screen" << std::endl;
        }
    }

    void submit()
    {
        updataScreenInfo();
        submitPath();
        updataCommandInfo();
        auto job = new TraditionRenderJob();
        job->addMesh(m_mesh)
            .setPassResource(m_passResource)
            .setPipelineState(m_pso);

        m_ctx->submit(job, m_passName);
        job->release();
    }
};
/*
 *
 *attachPass
 *  |create
 *    |generateVertexShader
 *    |generatePixelShader
 *setScreen
 *addPath
 *submit
 *  |updataScreenInfo
 *  |submitPath
 *     |copyToCommandQueue
 *     |addRectangle
 *  |updataCommandInfo
 *
 *
 */
void VertexContext::updataCommandInfo()
{
    auto commandBufferImage = m_ctx->getResourceImage(VertexCommandBufferNameInShder);

    if (commandBufferImage->width() * commandBufferImage->height() >= m_commandBuffer.size()) {

    } else {
        uint32_t requiredHeight = (m_commandBuffer.size() + 4095) / 4096;
        commandBufferImage->resize(4096, requiredHeight);
    }
    auto res = commandBufferImage->updateToCurrent(m_commandBuffer.data(), m_commandBuffer.size() * sizeof(float));
    auto job = new VertexSubmitJob([res]()
    {
        res->waitFor();
    });
    m_ctx->submit(job,m_uploadPassName);
    job->release();
}
void VertexContext::submitPath()
{
    m_commandBuffer.clear();

    if (m_screens.empty()) {
        return;
    }

    m_mesh->clear();

    for (size_t screenIndex = 0; screenIndex < m_screens.size(); screenIndex++) {
        const auto& screen = m_screens[screenIndex];
        const auto& paths = screen.getPaths();

        for (const auto& path : paths) {
            const auto& commands = path->getCommandQueue();
            if (commands.empty()) {
                continue;
            }

            uint32_t commandOffset = m_commandBuffer.size();
            uint32_t commandSize = commands.size();

            m_commandBuffer.insert(m_commandBuffer.end(), commands.begin(), commands.end());

            Vec4 bounds = path->getBoundingRect();

            addRectangle(screenIndex, commandOffset, commandSize, bounds);
        }
    }

    m_mesh->update();
}
void VertexContext::addRectangle(uint32_t screenId, uint32_t commandOffset, uint32_t commandSize, const Vec4& bounds)
{
    float left = bounds.x;
    float top = bounds.y;
    float right = bounds.x + bounds.z;
    float bottom = bounds.y + bounds.w;

    uint32_t baseIndex = m_mesh->getVertexCount();

    m_mesh->addVertex(Vec2(left, top), commandOffset, commandSize, screenId);

    m_mesh->addVertex(Vec2(right, top), commandOffset, commandSize, screenId);

    m_mesh->addVertex(Vec2(left, bottom), commandOffset, commandSize, screenId);

    m_mesh->addVertex(Vec2(right, bottom), commandOffset, commandSize, screenId);

    m_mesh->addIndex(baseIndex);
    m_mesh->addIndex(baseIndex + 1);
    m_mesh->addIndex(baseIndex + 2);

    m_mesh->addIndex(baseIndex + 1);
    m_mesh->addIndex(baseIndex + 3);
    m_mesh->addIndex(baseIndex + 2);
}
void VertexContext::create()
{
    m_pso = new TraditionPipelineState();
    m_vs = m_ctx->createVertexShader();
    m_vs->pixelLayout(pixelLayout);
    m_vs->addLayout(0, vertexLayout);
    m_vs->addUniform(m_projectionMatrix.first);
    m_vs->addUniform(m_viewMatrix.first);
    m_vs->resourceLayout(resourceLayout);

    m_ps = m_ctx->createPixelShader();
    m_ps->pixelLayout(pixelLayout);
    m_ps->resourceLayout(resourceLayout);

    generateVertexShader();
    generatePixelShader();

    m_vs->create();
    m_ps->create();

    m_pso->vertexLayout = vertexLayout;
    m_pso->pixelLayout = pixelLayout;
    m_pso->resourceLayout = resourceLayout;
    m_pso->vertexShader = m_vs;
    m_pso->pixelShader = m_ps;

    m_passResource->create();

    m_mesh = new DynamicMesh<uint32_t>(m_ctx,vertexLayout);
    m_mesh->reserveIndices(200);
    m_mesh->reserveVertices(200);
    m_mesh->create();
}


void VertexContext::addScreen(const std::string& name)
{
    size_t index = m_infos.size();
    m_screenIndexMap[name] = index;
    m_infos.emplace_back();
    m_screens.emplace_back(m_infos, index);
}
VertexScreen& VertexContext::setScreen(const std::string& name)
{
    auto it = m_screenIndexMap.find(name);
    if (it == m_screenIndexMap.end()) {
        addScreen(name);
        return m_screens.back();
    }
    return m_screens[it->second];
}
void VertexContext::mvpUniformLayout(ConstLayout layout)
{
    auto viewMatrix = layout.getElement(ConstType::ViewMatrix);
    if (viewMatrix)
    {
        m_viewMatrix = { layout, viewMatrix };
    }
    auto projectionMatrix = layout.getElement(ConstType::ProjectionMatrix);
    if (projectionMatrix)
    {
        m_projectionMatrix = { layout, projectionMatrix };
    }
    //todo:支持 vp合一矩阵 支持mvp合一 矩阵
}

void VertexContext::generateVertexShader()
{
    std::string code;
    code += "\n#define vertexOuputTarget " + std::string(ScreenInfoNameInShder);
    code += R"(




ShaderOut main(ShaderIn vsIn) {
    ShaderOut vsOut;

    uint screenId = uint(vsIn.screenId);

    uint baseIndex = screenId * 5;

    float4 origin = ScreenInfo.Load(int3(baseIndex, 0, 0));
    float4 XDir = ScreenInfo.Load(int3(baseIndex + 1, 0, 0));
    float4 YDir = ScreenInfo.Load(int3(baseIndex + 2, 0, 0));
    float4 originVertexCoord = ScreenInfo.Load(int3(baseIndex + 3, 0, 0));
    float4 VertexCoordSize = ScreenInfo.Load(int3(baseIndex + 4, 0, 0));

    float2 vertexCoord = vsIn.vertexcoord;

    float3 worldPos = origin.xyz +
                     XDir.xyz * (originVertexCoord.x + vertexCoord.x * VertexCoordSize.x) +
                     YDir.xyz * (originVertexCoord.y + vertexCoord.y * VertexCoordSize.y);

    float4 position = float4(worldPos, 1.0);
)";

    if (m_viewMatrix.second && m_projectionMatrix.second) {
        std::string viewMatrixName = m_viewMatrix.second.getName();
        std::string projMatrixName = m_projectionMatrix.second.getName();

        code += "    float4 viewPos = mul(" + viewMatrixName + ", position);\n";
        code += "    vsOut.pos = mul(" + projMatrixName + ", viewPos);\n";
    } else if (m_viewMatrix.second) {
        std::string viewMatrixName = m_viewMatrix.second.getName();

        code += "    vsOut.pos = mul(" + viewMatrixName + ", position);\n";
    } else if (m_projectionMatrix.second) {
        std::string projMatrixName = m_projectionMatrix.second.getName();

        code += "    vsOut.pos = mul(" + projMatrixName + ", position);\n";
    } else {
        code += "    vsOut.pos = position;\n";
    }

    code += R"(
    vsOut.cmdoffset = vsIn.cmdoffset;
    vsOut.cmdSize = vsIn.cmdSize;
    vsOut.screenId = vsIn.screenId;
    vsOut.vertexcoord = vsIn.vertexcoord;

    return vsOut;
}
)";

    m_vs->code(code);
}

void VertexContext::generatePixelShader()
{
    std::string code;
    code += "\n#define VertexCommandBuffer " + std::string(VertexCommandBufferNameInShder);
    code += "\n#define vertexOuputTarget target" + std::to_string(m_targetIndex);
    code += R"(


const float VertexCommand_End = -1.0;
const float VertexCommand_MoveTo = 1.0;
const float VertexCommand_LineTo = 2.0;
const float VertexCommand_BezierCurveTo = 3.0;
const float VertexCommand_SetColor = 4.0;
const float VertexCommand_ArcTo = 6.0;
const float VertexCommand_SetTransform = 5.0;
const float VertexCommand_BeginPath = 7.0;
const float VertexCommand_EndPath = 8.0;

int GetCurveRootFlags(float v0, float v1, float v2) {
    int shift = ((v0 > 0.0) ? 2 : 0) + ((v1 > 0.0) ? 4 : 0) + ((v2 > 0.0) ? 8 : 0);
    return (0x2E74 >> shift) & 0x03;
}

float2 QuadCurveSolveXAxis(float2 v0, float2 v1, float2 v2) {
    float2 a = v0 - 2.0 * v1 + v2;
    float2 b = v0 - v1;
    float c = v0.y;
    float ra = 1.0 / a.y;
    float rb = 0.5 / b.y;
    float delta = sqrt(max(b.y * b.y - a.y * c, 0.0));
    float2 t = float2((b.y - delta) * ra, (b.y + delta) * ra);
    if (abs(a.y) < 0.0001220703125) t = float2(c * rb, c * rb);
    return (a.x * t - b.x * 2.0) * t + v0.x;
}

float2 QuadCurveSolveYAxis(float2 v0, float2 v1, float2 v2) {
    float2 a = v0 - 2.0 * v1 + v2;
    float2 b = v0 - v1;
    float c = v0.x;
    float ra = 1.0 / a.x;
    float rb = 0.5 / b.x;
    float delta = sqrt(max(b.x * b.x - a.x * c, 0.0));
    float2 t = float2((b.x - delta) * ra, (b.x + delta) * ra);
    if (abs(a.x) < 0.0001220703125) t = float2(c * rb, c * rb);
    return (a.y * t - b.y * 2.0) * t + v0.y;
}

float CurveTestXAxis(float2 v0, float2 v1, float2 v2, float2 pixelsPerUnit) {
    if (max(max(v0.x, v1.x), v2.x) * pixelsPerUnit.x < -0.5) return 0.0;
    int flags = GetCurveRootFlags(v0.y, v1.y, v2.y);
    if (flags == 0) return 0.0;
    float2 x1x2 = QuadCurveSolveXAxis(v0, v1, v2) * pixelsPerUnit.x;
    float ret = 0.0;
    if ((flags & 1) != 0) {
        ret += clamp(x1x2.x + 0.5, 0.0, 1.0);
    }
    if ((flags & 2) != 0) {
        ret -= clamp(x1x2.y + 0.5, 0.0, 1.0);
    }
    return ret;
}

float CurveTestYAxis(float2 v0, float2 v1, float2 v2, float2 pixelsPerUnit) {
    if (max(max(v0.y, v1.y), v2.y) * pixelsPerUnit.y < -0.5) return 0.0;
    int flags = GetCurveRootFlags(v0.x, v1.x, v2.x);
    if (flags == 0) return 0.0;
    float2 y1y2 = QuadCurveSolveYAxis(v0, v1, v2) * pixelsPerUnit.y;
    float ret = 0.0;
    if ((flags & 0x01) != 0) {
        ret -= clamp(y1y2.x + 0.5, 0.0, 1.0);
    }
    if ((flags & 0x02) != 0) {
        ret += clamp(y1y2.y + 0.5, 0.0, 1.0);
    }
    return ret;
}

float2 CurveTest(float2 v0, float2 v1, float2 v2, float2 pixelsPerUnit) {
    return float2(
        CurveTestXAxis(v0, v1, v2, pixelsPerUnit),
        CurveTestYAxis(v0, v1, v2, pixelsPerUnit)
    );
}

float2 LineTest(float2 v0, float2 v1, float2 pixelsPerUnit) {
    float2 result = float2(0.0, 0.0);

    if (max(v0.x, v1.x) * pixelsPerUnit.x >= -0.5) {
        int signX = (v0.y > 0.0 ? 1 : 0) - (v1.y > 0.0 ? 1 : 0);
        if (signX != 0) {
            float xt = (v1.y * v0.x - v0.y * v1.x) / (v1.y - v0.y);
            result.x = float(signX) * clamp(xt * pixelsPerUnit.x + 0.5, 0.0, 1.0);
        }
    }

    if (max(v0.y, v1.y) * pixelsPerUnit.y >= -0.5) {
        int signY = (v1.x > 0.0 ? 1 : 0) - (v0.x > 0.0 ? 1 : 0);
        if (signY != 0) {
            float yt = (v1.x * v0.y - v0.x * v1.y) / (v1.x - v0.x);
            result.y = float(signY) * clamp(yt * pixelsPerUnit.y + 0.5, 0.0, 1.0);
        }
    }

    return result;
}

float2 ArcTest(float2 pos, float2 bp, float2 ep, float2 c, float b, float e, float r, float q, float2 pixelsPerUnit) {
    float2 ret = float2(0.0, 0.0);
    float minX = min(bp.x, ep.x);
    float minY = min(bp.y, ep.y);
    float maxX = max(bp.x, ep.x);
    float maxY = max(bp.y, ep.y);
    bool isInx = (pos.x >= minX && pos.x <= maxX);
    bool isIny = (pos.y >= minY && pos.y <= maxY);
    bool isInCircle = length(pos - c) <= r;
    float bLessE = b < e ? 1.0 : -1.0;

    if (q == 1.0 || q == 4.0) {
        ret.x = float(isIny && (isInCircle || pos.x < minX));
        ret.x *= -bLessE;
    } else {
        ret.x = float(isIny && pos.x < maxX && !isInCircle);
        ret.x *= bLessE;
    }

    if (q == 1.0 || q == 2.0) {
        ret.y = float(isInx && (isInCircle || pos.y < minY));
        ret.y *= -bLessE;
    } else {
        ret.y = float(isInx && pos.y < maxY && !isInCircle);
        ret.y *= bLessE;
    }

    return ret;
}

float NoZeroSign(float x) {
    return x >= 0.0 ? 1.0 : -1.0;
}

float2 applyTransform(float2 needTransformPoint, float3x3 transform) {
    float3 transformedPoint = mul(float3(needTransformPoint, 1.0), transform);
    return transformedPoint.xy / transformedPoint.z;
}

float fetchCommand(int commandIndex) {
    int x = commandIndex % 4096;
    int y = commandIndex / 4096;
    return VertexCommandBuffer.Load(int3(x, y, 0)).r;
}

ShaderOut main(ShaderIn ps_input) {
    float transformSign = 1.0;
    float3x3 transform = float3x3(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);
    ShaderOut ps_output;
    ps_output.vertexOuputTarget = float4(1.0, 1.0, 1.0, 1.0);

    float2 unitsPerPixel = fwidth(ps_input.vertexcoord);
    float2 pixelsPerUnit = 1.0 / unitsPerPixel;

    float2 crossings = float2(0.0, 0.0);

    int i = int(ps_input.cmdoffset);
    float2 lastPos;
    float4 fillColor = float4(1.0, 0.647, 0.0, 1.0);
    float4 currentColor = float4(1.0, 0.647, 0.0, 1.0);
    bool isPathStarted = false;
    float2 pathCrossings = float2(0.0, 0.0);

    float pathOperation = 0.0; // 0: 默认 1:并 2:交 3:补
    int edge = i + int(ps_input.cmdSize);
    while(i < edge) {
        float command = fetchCommand(i);
        i++;

        if (command == VertexCommand_End) break;
        if (command == VertexCommand_BeginPath) {
            isPathStarted = true;
            pathCrossings = float2(0.0, 0.0);
            pathOperation = fetchCommand(i);
            i += 1;
        }
        else if (command == VertexCommand_EndPath) {
            isPathStarted = false;
            if (length(pathCrossings) > 0.0) {
                fillColor = currentColor;
            }
            if (pathOperation == 0.0) {
                crossings += pathCrossings;
            } else if (pathOperation == 1.0) {
                crossings = float2(
                    NoZeroSign(crossings.x) * NoZeroSign(pathCrossings.x) * max(abs(crossings.x), abs(pathCrossings.x)),
                    NoZeroSign(crossings.y) * NoZeroSign(pathCrossings.y) * max(abs(crossings.y), abs(pathCrossings.y))
                );
            } else if (pathOperation == 2.0) {
                crossings = float2(
                    sign(crossings.x) * sign(pathCrossings.x) * min(abs(crossings.x), abs(pathCrossings.x)),
                    sign(crossings.y) * sign(pathCrossings.y) * min(abs(crossings.y), abs(pathCrossings.y))
                );
            } else if (pathOperation == 3.0) {
                crossings += pathCrossings;
                crossings = float2(
                    NoZeroSign(crossings.x) * (1.0 - abs(crossings.x)),
                    NoZeroSign(crossings.y) * (1.0 - abs(crossings.y))
                );
            }
            pathCrossings = float2(0, 0);
            pathOperation = 0.0;
        }
        if (command == VertexCommand_MoveTo) {
            lastPos.x = fetchCommand(i);
            lastPos.y = fetchCommand(i+1);
            lastPos = applyTransform(lastPos, transform);
            i += 2;
        }
        else if (command == VertexCommand_LineTo) {
            float2 to;
            to.x = fetchCommand(i);
            to.y = fetchCommand(i+1);
            to = applyTransform(to, transform);
            i += 2;

            float2 v0 = lastPos - ps_input.vertexcoord;
            float2 v1 = to - ps_input.vertexcoord;
            pathCrossings += transformSign * LineTest(v0, v1, pixelsPerUnit);
            lastPos = to;
        }
        else if (command == VertexCommand_SetColor) {
            currentColor.r = fetchCommand(i);
            currentColor.g = fetchCommand(i+1);
            currentColor.b = fetchCommand(i+2);
            currentColor.a = fetchCommand(i+3);
            i += 4;
        }
        else if (command == VertexCommand_ArcTo) {
            float2 bp = lastPos;
            float2 c;
            c.x = fetchCommand(i);
            c.y = fetchCommand(i+1);
            c = applyTransform(c, transform);
            float r = length(c - bp);
            float b = fetchCommand(i+2);
            float e = fetchCommand(i+3);
            float q = fetchCommand(i+4);
            float2 ep = c + float2(r * cos(e), r * sin(e));
            pathCrossings += transformSign * ArcTest(ps_input.vertexcoord, bp, ep, c, b, e, r, q, pixelsPerUnit);
            lastPos = ep;
            i += 5;
        }
        else if (command == VertexCommand_BezierCurveTo) {
            float2 control = float2(fetchCommand(i),
                                fetchCommand(i+1));
            float2 end = float2(fetchCommand(i+2),
                            fetchCommand(i+3));
            control = applyTransform(control, transform);
            end = applyTransform(end, transform);
            i += 4;

            float2 v0 = lastPos - ps_input.vertexcoord;
            float2 v1 = control - ps_input.vertexcoord;
            float2 v2 = end - ps_input.vertexcoord;
            pathCrossings += transformSign * CurveTest(v0, v1, v2, pixelsPerUnit);
            lastPos = end;
        }
        else if (command == VertexCommand_SetTransform) {
            transform = float3x3(
                fetchCommand(i), fetchCommand(i+1), fetchCommand(i+2),
                fetchCommand(i+3), fetchCommand(i+4), fetchCommand(i+5),
                fetchCommand(i+6), fetchCommand(i+7), fetchCommand(i+8)
            );
            i += 16;

            float det = determinant(transform);
			transformSign = NoZeroSign(det);
        }
    }

    float weightX = 1.0 - abs(crossings.x * 2.0 - 1.0);
    float weightY = 1.0 - abs(crossings.y * 2.0 - 1.0);
    float coverage = max(abs(crossings.x * weightX + crossings.y * weightY) / max(weightX + weightY, 0.0001220703125), min(abs(crossings.x), abs(crossings.y)));

    ps_output.vertexOuputTarget = fillColor;
    ps_output.vertexOuputTarget.a *= coverage;
    return ps_output;
}
)";
    m_ps->code(code);
}
/*
 *todo:矢量渲染
 *1.提供屏幕设置,屏幕是空间 四边形
 *  需要描述一个空间四边形的常量缓冲 趣
 *  需要描述符空间四边形的四个顶点的vertexCoord
 *  需要vertexCoord
 *2.传进去的顶点只有vertexCoord,传出的位置通过空间平行四边形计算出来
 *3.传进去的顶点需要指定命令所在缓存区offset
 *3.提供drawPath(Transfomr),提供drawPath(位置)，把位置弄进矩阵，
 *  使用setTransform传递位置，大小
 *4.addScreen("screenName")
 *  draw("screenName",Path,Transform)
 *  draw("screenName",Path,float x,float y)
 *5.setScreen().set().set()
 *6.需要 更新screen 的UniformBuffer
 *7.空间四边形 表示，原点+ halfX+halfY
 *8.使用纹理来进行屏幕数据的传递
 */
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
    AutoReviewport autoReviewport;
    float rotationAngleY;
    StaticMesh<uint32_t>* teapotMesh;
    GLFW_VK_ImGuiContext* imguiCtx;
    TraditionPipelineState* pso;
    DynamicMesh<uint32_t>* dynamicMesh;
    float animationTime;
    VertexContext* vertexCtx;
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

        vertexCtx->addConstBuffer(vertexConstBuffer);
        vertexCtx->attachPass("vertexPass");
        vertexCtx->setScreen("screen")
            .setOrigin(Vec3(-1,-1,0))
            .setX(Vec3(1,0,0))
            .setY(Vec3(0,1,0))
            .setVertexCoordSize(Vec2(800,600))
            .setOriginVertexCoord(Vec2(0,0));

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

        animationTime += deltaTime;

        updateDynamicMesh(animationTime);

        Mat4 ratation;
        ratation.rotateX(rotationAngle);
        ratation.rotateY(rotationAngleY);
        buffer->setValue("modelMatrix", ratation);
        constBuffer->updataData();
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
        path->lineTo(Vec2(100, 200));

        path->endPath();

        path->beginPath();
        path->setColor(Vec4(1.0f, 0.3f, 0.3f, 0.7f));
        path->circle(Vec2(400, 150), 80);
        path->endPath();

        path->beginPath();
        path->setColor(Vec4(0.3f, 0.8f, 0.4f, 0.9f));
        path->moveTo(Vec2(500, 100));
        path->curveTo(Vec2(600, 50), Vec2(700, 200));
        path->lineTo(Vec2(500, 200));
        path->closePath();
        path->endPath();
        vertexCtx->clearPath("screen");
        vertexCtx->addPath("screen", path);

        path->release();

        vertexCtx->submit();

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