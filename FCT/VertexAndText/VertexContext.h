#include "./VertexPath.h"
namespace FCT
{
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
        //PassResource* m_passResource;
        std::vector<PassResource*> m_passResources;
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
            for (int i = 0;i < m_ctx->maxFrameInFlight();i++)
            {
                m_passResources.push_back(m_ctx->createPassResource());
                m_passResources[i]->bind(wnd);
            }
            /*
            m_passResource = m_ctx->createPassResource();
            m_passResource->bind(wnd);
            */
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
            for (auto& resource : m_passResources)
            {
                resource->addConstBuffer(constBuffer);
            }
            //m_passResource->addConstBuffer(constBuffer);
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
                    ScopeTimer waitForUploadToGpuTimer("waitForUploadToGpu");
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
                .setPassResource(m_passResources[m_ctx->currentFrameIndex()])
                //.setPassResource(m_passResource)
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
}