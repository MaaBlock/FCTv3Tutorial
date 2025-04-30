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
struct TraditionPipelineState
{
    TraditionPipelineState()
    {
        vertexShader = nullptr;
        pixelShader = nullptr;
        blendState = nullptr;
        rasterizationState = nullptr;
        depthStencilState = nullptr;
    }
    PixelLayout pixelLayout;
    VertexLayout vertexLayout;
    ResourceLayout resourceLayout;
    VertexShader* vertexShader;
    PixelShader* pixelShader;
    BlendState* blendState;
    RasterizationState* rasterizationState;
    DepthStencilState* depthStencilState;
};
enum class JobType
{
    TraditionRender,
    Submit,
};
struct Job : RefCount
{
    virtual JobType getType() const = 0;
    virtual void submit(RHI::CommandBuffer* cmdBuf) = 0;
};


struct TraditionRenderJob : Job
{
    JobType getType() const override
    {
        return JobType::TraditionRender;
    }
    PassResource* resource;
    TraditionPipelineState* state;
    std::vector<Mesh<uint16_t>*> meshes16;
    std::vector<Mesh<uint32_t>*> meshes32;
    bool needsUpdate;
    bool isCreated;
    TraditionRenderJob()
      : resource(nullptr)
      , state(nullptr)
      , needsUpdate(false)
      , isCreated(false)
    {
    }
    TraditionRenderJob& addMesh(Mesh<uint16_t>* mesh)
    {
        meshes16.push_back(mesh);
        needsUpdate = true;
        return *this;
    }
    TraditionRenderJob& addMesh(Mesh<uint32_t>* mesh)
    {
        meshes32.push_back(mesh);
        needsUpdate = true;
        return *this;
    }
    TraditionRenderJob& setPassResource(PassResource* resource)
    {
        this->resource = resource;
        return *this;
    }
    TraditionRenderJob& setPipelineState(TraditionPipelineState* state)
    {
        this->state = state;
        return *this;
    }
    virtual void submit(RHI::CommandBuffer* cmdBuf)
    {
        for (auto& mesh : meshes16)
        {
            mesh->bind(cmdBuf);
            mesh->draw(cmdBuf);
        }
        for (auto& mesh : meshes32)
        {
            mesh->bind(cmdBuf);
            mesh->draw(cmdBuf);
        }
    }
};

struct SubmitJob : Job
{
    JobType getType() const override
    {
        return JobType::Submit;
    }
    virtual void submit(RHI::CommandBuffer* cmdBuf)
    {

    }
};
using ImguiTask = std::function<void()>;
struct ImguiJob : public SubmitJob
{
    std::queue<ImguiTask> m_tasks;
    void addUI(ImguiTask ui)
    {
        m_tasks.push(ui);
    }
    void submit(RHI::CommandBuffer* cmdBuf) override
    {
        while (!m_tasks.empty()) {
            ImguiTask task = m_tasks.front();
            m_tasks.pop();

            if (task) {
                task();
            }
        }
    }
};
/*
 *1.在指定的CmdBuf运行
 */
class Pass
{
protected:
    std::map<uint32_t,Image*> m_targets;
    Context* m_ctx;
    Image* m_depthStencil;
    PassClearValue m_clearValue;
    RHI::Pass* m_pass;
public:
    void setRhiPass(RHI::Pass* pass)
    {
        m_pass = pass;
    }
    virtual ~Pass() = default;

    PassClearValue getClearValue() const
    {
        return m_clearValue;
    }
    Pass& enableClear(ClearTypes type,Vec4 color,float depth = 1.0f, uint8_t stencil = 0)
    {
        m_clearValue.types = type;
        m_clearValue.color = color;
        m_clearValue.depth = depth;
        m_clearValue.stencil = stencil;
        return *this;
    }
    virtual void submit(RHI::CommandBuffer* cmdBuf) = 0;
    virtual void swapJobQueue() = 0;
    void setTarget(uint32_t index, Image* target)
    {
        m_targets[index] = target;
    }
    void setDepthStencil(Image* depthStencil)
    {
        m_depthStencil = depthStencil;
    }
    void setTarget(uint32_t index,Window* wnd)
    {
        setTarget(index,wnd->getCurrentTarget()->getImage());
        setDepthStencil(wnd->getCurrentTarget()->depthStencilBuffer());
    }
    virtual void submit(Job* job) = 0;
};

class TraditionRenderPass : public ::Pass
{
protected:
     struct TraditionRenderJobQueue
     {
         std::vector<TraditionRenderJob*> m_traditionRenderJobs;
         std::vector<SubmitJob*> m_submitJobs;
         void clear()
         {
             for (auto& job : m_traditionRenderJobs)
             {
                 job->release();
             }
             for (auto& job : m_submitJobs)
             {
                 job->release();
             }
             m_traditionRenderJobs.clear();
             m_submitJobs.clear();
         }
     } m_jobQueues[2];
    TraditionRenderJobQueue* m_submitQueue;
    TraditionRenderJobQueue* m_currentQueue;
    std::map<TraditionPipelineState*,RHI::RasterizationPipeline*> m_pipelineStates;
public:
    TraditionRenderPass(Context* ctx)
    {
        m_submitQueue = &m_jobQueues[0];
        m_currentQueue = &m_jobQueues[1];
        m_ctx = ctx;
    }
    void submitJob(SubmitJob* job)
    {
        job->addRef();
        m_currentQueue->m_submitJobs.push_back(job);
    }
    void submitJob(TraditionRenderJob* job)
    {
        job->addRef();
        m_currentQueue->m_traditionRenderJobs.push_back(job);
    }
    void submit(RHI::CommandBuffer* cmdBuf) override
    {
        m_pass->beginSubmit(cmdBuf);
        for (auto& job : m_submitQueue->m_traditionRenderJobs)
        {
            auto pipeline = getOrCreateTraditionPipeline(job->state);
            cmdBuf->bindPipieline(pipeline);
            job->resource->bind(cmdBuf,pipeline);
            job->submit(cmdBuf);
        }
        for (auto& job : m_submitQueue->m_submitJobs)
        {
            job->submit(cmdBuf);
        }
        m_pass->endSubmit();
    }
    RHI::RasterizationPipeline* getOrCreateTraditionPipeline(TraditionPipelineState* state)
    {
        auto it = m_pipelineStates.find(state);
        if (it!= m_pipelineStates.end())
            return it->second;
        auto pipeline = m_ctx->createTraditionPipeline();
        pipeline->pixelLayout(state->pixelLayout);
        pipeline->vertexLayout(state->vertexLayout);
        pipeline->resourceLayout(state->resourceLayout);
        pipeline->addResources(state->vertexShader);
        pipeline->addResources(state->pixelShader);
        pipeline->addResources(state->blendState);
        pipeline->addResources(state->rasterizationState);
        pipeline->addResources(state->depthStencilState);
        pipeline->bindPass(m_pass);
        pipeline->create();
        m_pipelineStates[state] = pipeline;
        return pipeline;
    }
    void swapJobQueue() override
    {
        std::swap(m_submitQueue, m_currentQueue);
        m_currentQueue->clear();
    }
    void submit(Job* job) override
    {
        switch (job->getType())
        {
            case JobType::TraditionRender:
                submitJob(static_cast<TraditionRenderJob*>(job));
                break;
            case JobType::Submit:
                submitJob(static_cast<SubmitJob*>(job));
                break;
            default:
                break;
        }
    }
};
//只能有一个imgui pass，否则是未定义行为
class ImguiRenderPass : public ::Pass
{
protected:
    Context* m_ctx;
    GLFW_VK_ImGuiContext*  m_imguiCtx;
    std::vector<SubmitJob*> m_jobs[2];
    //理论上应该只需要一个job，会合批为一个Job，不需要 vector，但也许 以后可以有多个job的 需求
    std::vector<SubmitJob*>* m_renderJobs;
    std::vector<SubmitJob*>* m_submitJobs;
public:

    ImguiRenderPass(Context* ctx,GLFW_VK_ImGuiContext* imguiCtx)
    {
        m_ctx = ctx;
        m_imguiCtx = imguiCtx;
        m_renderJobs = &m_jobs[0];
        m_submitJobs = &m_jobs[1];
    }
    void submit(RHI::CommandBuffer* cmdBuf) override
    {
        m_imguiCtx->newFrame();
        m_pass->beginSubmit(cmdBuf);
        for (auto& job : *m_renderJobs)
        {
            job->submit(cmdBuf);
        }
        m_imguiCtx->submitTick(cmdBuf);
        m_pass->endSubmit();
    }
    void submitJob(SubmitJob* job)
    {
        job->addRef();
        m_submitJobs->push_back(job);
    }
    void submit(Job* job) override
    {
        switch (job->getType())
        {
        case JobType::Submit:
            submitJob(static_cast<SubmitJob*>(job));
            break;
        default:
            fout << "imgui pass被提交了不受支持的job" << std::endl;
            break;
        }
    }
    void swapJobQueue() override
    {
        std::swap(m_renderJobs, m_submitJobs);
        m_submitJobs->clear();
    }
};

inline void attachImguiToPass()
{

}

enum class RenderGraphVertexType {
    Pass,
    Resource
};

struct PassData
{

};
struct ResourceData
{

};


struct PassGraphVertex {
    std::string name;
    ::Pass* pass;
    RHI::Pass* rhiPass;
    std::string target[8];
    std::string depthStencil;
    std::vector<std::string> textures;
    PassGraphVertex()
    {
        pass = nullptr;
        rhiPass = nullptr;
    }
};

enum class PassGraphEdgeType {
    Dependency
};

struct PassGraphEdge {
    PassGraphEdgeType type;
};


struct ResourceGraphVertex {

};
enum class ResourceGraphEdgeType {
    Reference,
    ParentChild,//指向child
};
struct ResourceGraphEdge {
    ResourceGraphEdgeType type;
};
/*
 *1.分配CmdBuf
 *2.给Pass分组
 *3.给每个pass创建rhiPass，并给每个组创建passGroup
 */
using PassGraphType = boost::adjacency_list<
    boost::vecS,              // 外部边列表
    boost::vecS,              // 外部顶点列表
    boost::bidirectionalS,    // 双向图
    PassGraphVertex,          // 顶点属性
    PassGraphEdge             // 边属性
>;
using ResourceGraphType = boost::adjacency_list<
    boost::vecS,              // 外部边列表
    boost::vecS,              // 外部顶点列表
    boost::bidirectionalS,    // 双向图
    ResourceGraphVertex,      // 顶点属性
    ResourceGraphEdge         // 边属性
>;


class RenderGraph
{
protected:
    Context* m_ctx;
    struct ImageResourceDesc
    {
        ImageUsageFlags usage;
        uint32_t width;
        uint32_t height;
        Samples samples;
        bool isWnd;
        bool dynamicSize;
        ImageResourceDesc() : width(0), height(0),dynamicSize(true), samples(Samples::sample_undefined), isWnd(false)
        {

        }
        ImageResourceDesc(bool isWnd) : width(0), height(0), samples(Samples::sample_undefined), isWnd(isWnd),dynamicSize(!isWnd)
        {

        }
    };
    uint32_t m_unnameWndBackBufferCount = 0;
    uint32_t m_unnameWndStencilDepthCount = 0;
    struct WindowImageResource
    {
        Window* wnd;
        Image* img;
    };
    struct WindowDepthStencilResource
    {
        Window* wnd;
        Image* img;
    };
    std::unordered_map<std::string, WindowImageResource> m_windowImageResources;
    std::unordered_map<std::string, WindowDepthStencilResource> m_windowDepthStencilResources;
    std::unordered_map<std::string, Image*> m_images;
    std::unordered_map<Window*,std::string> m_windowBackBufferNames;
    std::unordered_map<Window*,std::string> m_windowDepthStencilNames;
    std::unordered_map<std::string,ImageResourceDesc> m_imageResourceDescs;
    std::unordered_map<std::string, PassGraphType::vertex_descriptor> m_passVertex;
    std::unordered_map<std::string, TextureElement> m_textureLayouts;
    std::unordered_map<std::string, ResourceGraphType::vertex_descriptor> m_resourceVertices;
    std::vector<RHI::PassGroup*> m_passGroups;
    std::vector<MutilBufferImage*> m_needUpdataFramesIndexImages;
    struct ImageResourceVertex {
        Image* img;
        uint8_t slot = 0;
    };
    PassGraphType m_passGraph;
    ResourceGraphType m_resourceGraph;
    struct ExecutionOrder {
        std::vector<PassGraphType::vertex_descriptor> sortedPasses;
        std::vector<std::vector<PassGraphType::vertex_descriptor>> passGroups;
    };
    ExecutionOrder m_executionOrder;
public:
    RenderGraph(Context* ctx) : m_ctx(ctx)
    {

    }
    void addPass(const std::string& name, ::Pass* pass)
    {
        PassGraphVertex v;
        v.name = name;
        v.pass = pass;
        PassGraphType::vertex_descriptor vd = boost::add_vertex(v, m_passGraph);
        m_passVertex[name] = vd;
    }
    void addWindowResource(Window* wnd)
    {
        std::string name = "WndBackBuffer" + std::to_string(m_unnameWndBackBufferCount++);
        WindowImageResource res;
        res.wnd = wnd;
        res.img = wnd->getCurrentTarget()->targetImage();
        m_windowImageResources[name] = res;
        Image* img = res.img;
        img->addRef();
        m_images[name] = img;
        m_windowBackBufferNames[wnd] = name;
        if (wnd->getCurrentTarget()->depthStencilBuffer()) {
            std::string name = "WndDepthStencil" + std::to_string(m_unnameWndStencilDepthCount);
            WindowDepthStencilResource depthRes;
            depthRes.wnd = wnd;
            depthRes.img = wnd->getCurrentTarget()->depthStencilBuffer();
            m_windowDepthStencilResources[name] = depthRes;
            m_windowDepthStencilNames[wnd] = name;
            m_images[name] = depthRes.img;
            m_images[name]->addRef();
        }
    }
    void submit(Job* job,std::string name)
    {
        PassGraphType::vertex_descriptor vd = m_passVertex[name];
        ::Pass* pass = m_passGraph[vd].pass;
        pass->submit(job);
    }
    //显式依赖
    void addPassDenpendency(const std::string& from,const std::string& to) {
        PassGraphEdge e;
        e.type = PassGraphEdgeType::Dependency;
        PassGraphType::edge_descriptor ed = boost::add_edge(m_passVertex[from], m_passVertex[to], e, m_passGraph).first;
    }
    void bindOutputImage(const std::string& name, std::string image,uint8_t slot = 0)
    {
        ImageResourceDesc& desc = m_imageResourceDescs[image];
        desc.usage |= ImageUsage::RenderTarget;
        m_passGraph[m_passVertex[name]].target[slot] = image;
    }
    void bindTextureImage(const char* name, std::string image,uint32_t width = 0, uint32_t height = 0,Samples samples = Samples::sample_undefined)
    {
        ImageResourceDesc& desc = m_imageResourceDescs[image];
        desc.width |= width;
        desc.height |= height;
        desc.dynamicSize = false;
        if (samples != Samples::sample_undefined)
            desc.samples = samples;
        desc.usage |= ImageUsage::Texture;
        m_passGraph[m_passVertex[name]].textures.push_back(image);
    }
    void bindDepthStencil(const char* name, std::string image)
    {
        ImageResourceDesc& desc = m_imageResourceDescs[image];
        desc.usage |= ImageUsage::DepthStencil;
        m_passGraph[m_passVertex[name]].depthStencil = image;
    }
    void bindOutput(const char* name, Window* wnd, uint8_t slot = 0)
    {
        if (m_windowBackBufferNames.find(wnd) != m_windowBackBufferNames.end())
        {
            bindOutputImage(name, m_windowBackBufferNames[wnd], slot);
        }
        if (m_windowDepthStencilNames.find(wnd)!= m_windowDepthStencilNames.end())
        {
            bindDepthStencil(name, m_windowDepthStencilNames[wnd]);
        }
    }
    void checkAndUpdateResourceSizes()
    {
        bool needRecompute = false;

        for (const auto& entry : m_windowImageResources) {
            const std::string& imageName = entry.first;
            const WindowImageResource& resource = entry.second;

            uint32_t currentWidth = resource.wnd->getWidth();
            uint32_t currentHeight = resource.wnd->getHeight();

            if (m_imageResourceDescs[imageName].width != currentWidth ||
                m_imageResourceDescs[imageName].height != currentHeight) {
                m_imageResourceDescs[imageName].width = currentWidth;
                m_imageResourceDescs[imageName].height = currentHeight;
                needRecompute = true;
                }
        }

        for (const auto& entry : m_windowDepthStencilResources) {
            const std::string& imageName = entry.first;
            const WindowDepthStencilResource& resource = entry.second;

            uint32_t currentWidth = resource.wnd->getWidth();
            uint32_t currentHeight = resource.wnd->getHeight();

            if (m_imageResourceDescs[imageName].width != currentWidth ||
                m_imageResourceDescs[imageName].height != currentHeight) {
                m_imageResourceDescs[imageName].width = currentWidth;
                m_imageResourceDescs[imageName].height = currentHeight;
                needRecompute = true;
                }
        }

        if (needRecompute) {
            std::unordered_map<std::string, std::pair<uint32_t, uint32_t>> originalSizes;
            for (const auto& entry : m_imageResourceDescs) {
                if (entry.second.dynamicSize) {
                    originalSizes[entry.first] = {entry.second.width, entry.second.height};
                }
            }

            for (auto& entry : m_imageResourceDescs) {
                if (entry.second.dynamicSize && !entry.second.isWnd) {
                    entry.second.width = 0;
                    entry.second.height = 0;
                }
            }

            computeSize();

            for (const auto& entry : m_imageResourceDescs) {
                const std::string& imageName = entry.first;
                const ImageResourceDesc& desc = entry.second;

                if (m_windowImageResources.find(imageName) != m_windowImageResources.end() ||
                    m_windowDepthStencilResources.find(imageName) != m_windowDepthStencilResources.end()) {
                    continue;
                    }

                auto imgIt = m_images.find(imageName);
                if (imgIt != m_images.end()) {
                    Image* img = imgIt->second;
                    auto originalSizeIt = originalSizes.find(imageName);

                    if (desc.dynamicSize && originalSizeIt != originalSizes.end()) {
                        auto originalSize = originalSizeIt->second;
                        if (img->width() != desc.width || img->height() != desc.height) {
                            img->resize(desc.width, desc.height);
                        }
                    }
                }
            }
        }
    }
    //推导出各个image的大小，从固定大小的 /窗口 的
    //对于可能没有孤立的无法推导的 如果 一个单独的 pass，什么都没 连，然后输出了一个image，image还没有指定大小， 给予256*256
    void generateImageGraph()
    {
        for (const auto& imageEntry : m_imageResourceDescs) {
            const std::string& imageName = imageEntry.first;
            ResourceGraphVertex v;
            ResourceGraphType::vertex_descriptor vd = boost::add_vertex(v, m_resourceGraph);
            m_resourceVertices[imageName] = vd;
        }
        for (const auto& passEntry : m_passVertex) {
            const std::string& passName = passEntry.first;
            PassGraphType::vertex_descriptor passVd = passEntry.second;
            PassGraphVertex& passData = m_passGraph[passVd];

            std::vector<std::string> outputResources;

            for (uint8_t slot = 0; slot < 8; ++slot) {
                if (!passData.target[slot].empty()) {
                    outputResources.push_back(passData.target[slot]);
                }
            }

            if (!passData.depthStencil.empty()) {
                outputResources.push_back(passData.depthStencil);
            }

            if (outputResources.empty()) {
                continue;
            }

            std::string rootResource = outputResources[0];
            bool foundFixedSizeResource = false;

            for (const auto& resource : outputResources) {
                const ImageResourceDesc& desc = m_imageResourceDescs[resource];
                if (desc.isWnd || (desc.width > 0 && desc.height > 0)) {
                    rootResource = resource;
                    foundFixedSizeResource = true;
                    break;
                }
            }

            for (const auto& resource : outputResources) {
                if (resource != rootResource) {
                    ResourceGraphEdge e;
                    e.type = ResourceGraphEdgeType::ParentChild;
                    boost::add_edge(
                        m_resourceVertices[rootResource],
                        m_resourceVertices[resource],
                        e,
                        m_resourceGraph
                    );
                }
            }

            for (const auto& texture : passData.textures) {
                ImageResourceDesc& textureDesc = m_imageResourceDescs[texture];

                if (textureDesc.width == 0 || textureDesc.height == 0) {
                    ResourceGraphEdge e;
                    e.type = ResourceGraphEdgeType::ParentChild;
                    boost::add_edge(
                        m_resourceVertices[rootResource],
                        m_resourceVertices[texture],
                        e,
                        m_resourceGraph
                    );
                } else {
                    ResourceGraphEdge e;
                    e.type = ResourceGraphEdgeType::Reference;
                    boost::add_edge(
                        m_resourceVertices[texture],
                        m_resourceVertices[rootResource],
                        e,
                        m_resourceGraph
                    );
                }
            }
        }
    }
    void computeSize()
    {
        constexpr uint32_t DEFAULT_IMAGE_WIDTH = 256;
        constexpr uint32_t DEFAULT_IMAGE_HEIGHT = 256;

        std::set<std::string> processedResources;
        std::queue<std::string> sizeQueue;

        for (const auto& entry : m_windowImageResources) {
            const std::string& imageName = entry.first;
            const WindowImageResource& resource = entry.second;

            m_imageResourceDescs[imageName].width = resource.wnd->getWidth();
            m_imageResourceDescs[imageName].height = resource.wnd->getHeight();

            sizeQueue.push(imageName);
            processedResources.insert(imageName);
        }

        for (const auto& entry : m_windowDepthStencilResources) {
            const std::string& imageName = entry.first;
            const WindowDepthStencilResource& resource = entry.second;


            m_imageResourceDescs[imageName].width = resource.wnd->getWidth();
            m_imageResourceDescs[imageName].height = resource.wnd->getHeight();

            sizeQueue.push(imageName);
            processedResources.insert(imageName);
        }

        for (auto& entry : m_imageResourceDescs) {
            const std::string& imageName = entry.first;
            ImageResourceDesc& desc = entry.second;

            if (!desc.isWnd && desc.width > 0 && desc.height > 0 && processedResources.find(imageName) == processedResources.end()) {
                sizeQueue.push(imageName);
                processedResources.insert(imageName);
            }
        }

        while (!sizeQueue.empty()) {
            std::string currentResource = sizeQueue.front();
            sizeQueue.pop();

            ResourceGraphType::vertex_descriptor currentVertex = m_resourceVertices[currentResource];
            uint32_t currentWidth = m_imageResourceDescs[currentResource].width;
            uint32_t currentHeight = m_imageResourceDescs[currentResource].height;

            ResourceGraphType::out_edge_iterator ei, ei_end;
            std::pair<ResourceGraphType::out_edge_iterator, ResourceGraphType::out_edge_iterator> edgeRange =
            boost::out_edges(currentVertex, m_resourceGraph);
            if (edgeRange.first != edgeRange.second)
            {
                for (boost::tie(ei, ei_end) = boost::out_edges(currentVertex, m_resourceGraph); ei != ei_end; ++ei) {
                    ResourceGraphEdge& edge = m_resourceGraph[*ei];

                    if (edge.type == ResourceGraphEdgeType::ParentChild) {
                        ResourceGraphType::vertex_descriptor childVertex = boost::target(*ei, m_resourceGraph);

                        std::string childResource;
                        for (const auto& entry : m_resourceVertices) {
                            if (entry.second == childVertex) {
                                childResource = entry.first;
                                break;
                            }
                        }

                        if (!childResource.empty() && processedResources.find(childResource) == processedResources.end()) {
                            m_imageResourceDescs[childResource].width = currentWidth;
                            m_imageResourceDescs[childResource].height = currentHeight;

                            processedResources.insert(childResource);
                            sizeQueue.push(childResource);
                        }
                    }
                }
            }
        }

        for (auto& entry : m_imageResourceDescs) {
            const std::string& imageName = entry.first;
            ImageResourceDesc& desc = entry.second;

            if (desc.width == 0 || desc.height == 0) {
                desc.width = DEFAULT_IMAGE_WIDTH;
                desc.height = DEFAULT_IMAGE_HEIGHT;
            }
        }
    }
    void updateFrameIndices()
    {
        uint32_t currentFrameIndex = m_ctx->currentFrameIndex();
        for (auto* img : m_needUpdataFramesIndexImages) {
            img->changeCurrentIndex(currentFrameIndex);
        }
    }
    void saveExecutionOrder(const std::vector<PassGraphType::vertex_descriptor>& sortedPasses,
                       const std::vector<std::vector<PassGraphType::vertex_descriptor>>& passGroups)
    {
        m_executionOrder.sortedPasses = sortedPasses;
        m_executionOrder.passGroups = passGroups;
    }
    //从shaderGenreate获取resoureLayout，如果没有，就创建并添加新的resoureLayout
    void generateImageResource()
    {
        computeSize();
        for (const auto& entry : m_windowImageResources) {
            const std::string& imageName = entry.first;
            m_images[imageName] = entry.second.img;
        }

        for (const auto& entry : m_windowDepthStencilResources) {
            const std::string& imageName = entry.first;
            m_images[imageName] = entry.second.img;
        }

        for (const auto& entry : m_imageResourceDescs)
        {
            const std::string& imageName = entry.first;
            const ImageResourceDesc& desc = entry.second;

            if (m_images.find(imageName) != m_images.end()) {
                continue;
            }

            Format format = Format::R8G8B8A8_UNORM;
            if (desc.usage & ImageUsage::DepthStencil) {
                format = Format::D32_SFLOAT_S8_UINT;
            }

            if ((desc.usage & ImageUsage::RenderTarget) || (desc.usage & ImageUsage::DepthStencil)) {
                MutilBufferImage* img = m_ctx->createMutilBufferImage();
                img->imageCount(m_ctx->maxFrameInFlight());
                img->samples(desc.samples);
                img->format(format);
                img->width(desc.width);
                img->height(desc.height);
                img->as(desc.usage);
                img->create();
                m_images[imageName] = img;
                m_needUpdataFramesIndexImages.push_back(img);
                img->changeCurrentIndex(m_ctx->currentFrameIndex());
            } else {
                Image* img = m_ctx->createImage();
                img->samples(desc.samples);
                img->format(format);
                img->width(desc.width);
                img->height(desc.height);
                img->as(desc.usage);
                img->create();
                m_images[imageName] = img;
            }
            ShaderGenerator* gen = m_ctx->getGenerator();

            if (desc.usage & ImageUsage::Texture)
            {
                m_textureLayouts[imageName] = gen->findTextureElementByName(imageName);
                if (m_textureLayouts[imageName]) {

                } else
                {
                    m_textureLayouts[imageName] = TextureElement(imageName.c_str());
                    //todo: 暂时只支持使用现成的textureLayout，走这个分支是个未定义 行为
                    //todo: 完善layout/着色器生成系统系统，以可以在这里动态添加 layout
                }
            }
        }

        for (auto& passEntry : m_passVertex) {
            PassGraphType::vertex_descriptor passVd = passEntry.second;
            PassGraphVertex& passData = m_passGraph[passVd];

            for (uint8_t slot = 0; slot < 8; ++slot) {
                if (!passData.target[slot].empty()) {
                    const std::string& targetName = passData.target[slot];
                    if (m_images.find(targetName) != m_images.end()) {
                        passData.rhiPass->bindTarget(slot, m_images[targetName]);
                    }
                }
            }

            if (!passData.depthStencil.empty()) {
                const std::string& dsName = passData.depthStencil;
                if (m_images.find(dsName) != m_images.end()) {
                    passData.rhiPass->depthStencil(m_images[dsName]);
                }
            }
        }
    }
        void compile()
    {
        analyzeImplicitDependencies();
        std::vector<PassGraphType::vertex_descriptor> sortedPasses;
        std::unordered_map<PassGraphType::vertex_descriptor, boost::default_color_type> colorMap;

        try {
            boost::topological_sort(m_passGraph, std::back_inserter(sortedPasses),
                                   boost::color_map(boost::make_assoc_property_map(colorMap)));
            std::reverse(sortedPasses.begin(), sortedPasses.end());
        } catch (const boost::not_a_dag& e) {
            ferr << "拓扑排序失败，检查是否存在环" << endl;
            return;
        }

        for (auto& passEntry : m_passVertex) {
            PassGraphType::vertex_descriptor passVd = passEntry.second;
            PassGraphVertex& passData = m_passGraph[passVd];

            if (!passData.rhiPass)
            {
                passData.rhiPass = m_ctx->createPass();
                PassClearValue clearValue = passData.pass->getClearValue();
                passData.rhiPass->enableClear(clearValue);
                passData.pass->setRhiPass(passData.rhiPass);
            }
        }

        auto getRenderTargetSignature = [this](PassGraphType::vertex_descriptor passVd) -> std::string {
            PassGraphVertex& passData = m_passGraph[passVd];
            std::stringstream ss;

            for (uint8_t slot = 0; slot < 8; ++slot) {
                ss << "T" << static_cast<int>(slot) << ":" << passData.target[slot] << ";";
            }

            ss << "DS:" << passData.depthStencil;

            return ss.str();
        };

        std::vector<std::vector<PassGraphType::vertex_descriptor>> passGroups;
        std::unordered_set<PassGraphType::vertex_descriptor> assignedPasses;

        for (auto passVd : sortedPasses) {
            if (assignedPasses.find(passVd) != assignedPasses.end()) {
                continue;
            }

            std::string currentSignature = getRenderTargetSignature(passVd);

            std::vector<PassGraphType::vertex_descriptor> currentGroup;
            currentGroup.push_back(passVd);
            assignedPasses.insert(passVd);

            PassGraphType::vertex_descriptor currentPassVd = passVd;
            bool continueChain = true;

            while (continueChain) {
                continueChain = false;

                std::vector<PassGraphType::vertex_descriptor> candidatePasses;

                PassGraphType::out_edge_iterator ei, ei_end;
                for (boost::tie(ei, ei_end) = boost::out_edges(currentPassVd, m_passGraph); ei != ei_end; ++ei) {
                    PassGraphType::vertex_descriptor targetVd = boost::target(*ei, m_passGraph);

                    if (assignedPasses.find(targetVd) != assignedPasses.end()) {
                        continue;
                    }

                    std::string targetSignature = getRenderTargetSignature(targetVd);
                    if (targetSignature == currentSignature) {
                        bool allDependenciesProcessed = true;
                        PassGraphType::in_edge_iterator in_ei, in_ei_end;
                        for (boost::tie(in_ei, in_ei_end) = boost::in_edges(targetVd, m_passGraph); in_ei != in_ei_end; ++in_ei) {
                            PassGraphType::vertex_descriptor sourceVd = boost::source(*in_ei, m_passGraph);
                            if (assignedPasses.find(sourceVd) == assignedPasses.end()) {
                                allDependenciesProcessed = false;
                                break;
                            }
                        }

                        if (allDependenciesProcessed) {
                            candidatePasses.push_back(targetVd);
                        }
                    }
                }

                if (!candidatePasses.empty()) {
                    for (auto targetVd : candidatePasses) {
                        currentGroup.push_back(targetVd);
                        assignedPasses.insert(targetVd);
                    }

                    currentPassVd = candidatePasses.back();
                    continueChain = true;
                }
            }

            passGroups.push_back(currentGroup);
        }

        saveExecutionOrder(sortedPasses, passGroups);

        m_passGroups.clear();

        for (const auto& group : passGroups) {
            RHI::PassGroup* passGroup = m_ctx->createPassGroup();
            m_passGroups.push_back(passGroup);

            for (auto passVd : group) {
                PassGraphVertex& passData = m_passGraph[passVd];
                if (passData.rhiPass) {
                    passGroup->addPass(passData.rhiPass);
                }
            }
        }

        generateImageGraph();
        generateImageResource();

        for (auto& group : m_passGroups) {
            group->create();
        }
    }
    void analyzeImplicitDependencies()
    {
        std::unordered_map<std::string, std::vector<std::string>> resourceWriters;
        std::unordered_map<std::string, std::vector<std::string>> resourceReaders;

        for (const auto& passEntry : m_passVertex) {
            const std::string& passName = passEntry.first;

            for (const auto& imageEntry : m_imageResourceDescs) {
                const std::string& imageName = imageEntry.first;

                if (isPassWritingToResource(passName, imageName)) {
                    resourceWriters[imageName].push_back(passName);
                }

                if (isPassReadingFromResource(passName, imageName)) {
                    resourceReaders[imageName].push_back(passName);
                }
            }
        }

        for (const auto& resourceEntry : resourceReaders) {
            const std::string& resourceName = resourceEntry.first;
            const std::vector<std::string>& readers = resourceEntry.second;

            if (resourceWriters.find(resourceName) != resourceWriters.end()) {
                const std::vector<std::string>& writers = resourceWriters[resourceName];

                for (const std::string& reader : readers) {
                    for (const std::string& writer : writers) {
                        if (reader != writer) {
                            addPassDenpendency(writer, reader);
                        }
                    }
                }
            }
        }
    }

    bool isPassWritingToResource(const std::string& passName, const std::string& resourceName)
    {
        auto it = m_passVertex.find(passName);
        if (it == m_passVertex.end()) {
            return false;
        }

        PassGraphType::vertex_descriptor passVertex = it->second;
        PassGraphVertex& passData = m_passGraph[passVertex];

        for (uint8_t slot = 0; slot < 8; ++slot) {
            if (passData.target[slot] == resourceName) {
                return true;
            }
        }

        if (passData.depthStencil == resourceName) {
            return true;
        }

        return false;
    }

    bool isPassReadingFromResource(const std::string& passName, const std::string& resourceName)
    {
        auto it = m_passVertex.find(passName);
        if (it == m_passVertex.end()) {
            return false;
        }

        PassGraphType::vertex_descriptor passVertex = it->second;
        PassGraphVertex& passData = m_passGraph[passVertex];

        for (const auto& texture : passData.textures) {
            if (texture == resourceName) {
                return true;
            }
        }

        return false;
    }
    void execute(RHI::CommandBuffer* cmdBuf)
    {
        for (size_t i = 0; i < m_executionOrder.passGroups.size(); ++i) {
            if (i >= m_passGroups.size()) {
                ferr << "PassGroup 索引超出范围: " << i << endl;
                continue;
            }

            RHI::PassGroup* passGroup = m_passGroups[i];
            const auto& group = m_executionOrder.passGroups[i];

            passGroup->beginSubmit(cmdBuf);

            for (auto passVd : group) {
                PassGraphVertex& passData = m_passGraph[passVd];
                passData.pass->submit(cmdBuf);
            }

            passGroup->endSubmit(cmdBuf);
        }
    }
    void swapJobQueue()
    {
        for (auto& passEntry : m_passVertex)
        {
            PassGraphType::vertex_descriptor passVd = passEntry.second;
            PassGraphVertex& passData = m_passGraph[passVd];
            passData.pass->swapJobQueue();
        }
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
    RenderGraph* renderGraph;
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
        wnd->bind(ctx);
        wnd->enableDepthBuffer(Format::D32_SFLOAT_S8_UINT);
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
        ::Pass* pass = new ::TraditionRenderPass(ctx);
        //::Pass* imguiPass = new ImguiRenderPass(ctx,imguiCtx);
        pass->enableClear(ClearType::color | ClearType::depthStencil,Vec4(1,1,1,1));
        renderGraph = new RenderGraph(ctx);
        renderGraph->addWindowResource(wnd);
        renderGraph->addPass("nomralObject",pass);

        renderGraph->bindOutput("nomralObject",wnd);
        renderGraph->compile();
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
        pass->enableClear(ClearType::color | ClearType::depthStencil,Vec4(1,1,1,1));
        pass->bindTarget(0,wnd->getCurrentTarget()->targetImage());
        pass->depthStencil(wnd->getCurrentTarget()->depthStencilBuffer());
        passGroup = ctx->createPassGroup();
        passGroup->addPass(pass);
        passGroup->create();
        imguiCtx = new GLFW_VK_ImGuiContext((GLFW_Window*)wnd,(VK_Context*)ctx);
        imguiCtx->create(passGroup,pass);
        pipeline = ctx->createTraditionPipeline();
        pipeline->pixelLayout(pixelLayout);
        pipeline->vertexLayout(vertexLayout);
        pipeline->resourceLayout(resourceLayout);
        pipeline->bindPass(pass);
        pipeline->addResources(vs);
        pipeline->addResources(ps);
        pipeline->create();
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
        renderGraph->submit(job,"nomralObject");
        job->release();
        imguiCtx->logicTick();
        //ctx->flush();
        ctx->waitCurrentFlush();
        //同步时间
        ctx->swapQueue();
        renderGraph->swapJobQueue();
        ctx->nextFrame();
    }
    void submitTick()
    {
        renderGraph->updateFrameIndices();
        renderGraph->checkAndUpdateResourceSizes();
        auto cmdBuf = ctx->getCmdBuf(wnd, 0);
        cmdBuf->reset();
        cmdBuf->begin();
        autoReviewport.onRenderTick(cmdBuf);
        renderGraph->execute(cmdBuf);
/*
        cmdBuf->bindPipieline(pipeline);
        passGroup->beginSubmit(cmdBuf);
        pass->beginSubmit(cmdBuf);
        passResource->bind(cmdBuf,pipeline);

        teapotMesh->bind(cmdBuf);
        teapotMesh->draw(cmdBuf, 1);
        cubeMesh->bind(cmdBuf);
        cubeMesh->draw(cmdBuf, 1);

        imguiCtx->newFrame();
        ImGui::Begin("FCT Debug");
        ImGui::Text("Hello, FCT!");
        ImGui::End();
        imguiCtx->submitTick(cmdBuf);
        pass->endSubmit();
        passGroup->endSubmit(cmdBuf);
*/

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