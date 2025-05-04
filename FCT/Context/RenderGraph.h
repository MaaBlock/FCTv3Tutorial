//
// Created by Administrator on 2025/4/5.
//

#ifndef RENDERGRAPH_H
#define RENDERGRAPH_H


namespace FCT
{
    enum class RenderGraphVertexType {
        Pass,
        Resource
    };

    struct PassGraphVertex {
        std::string name;
        Pass* pass;
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


    class RenderGraph : public RefCount
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
        std::vector<Window*> m_bindedWindows;
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
        Image* getResourceImage(std::string name)
        {
            auto it = m_images.find(name);
            if (it!= m_images.end())
            {
                return it->second;
            }
            return nullptr;
        }
        RenderGraph(Context* ctx);
        void addPass(const std::string& name, Pass* pass);
        void addWindowResource(Window* wnd);
        void submit(Job* job,std::string name);
        //显式依赖
        void addPassDenpendency(const std::string& from,const std::string& to);
        void bindOutputImage(const std::string& name, std::string image,uint8_t slot = 0);
        void bindTextureImage(std::string name, std::string image, uint32_t width = 0, uint32_t height = 0, Samples samples =
                                  Samples::sample_undefined);
        void bindDepthStencil(const char* name, std::string image);
        void bindOutput(const char* name, Window* wnd, uint8_t slot = 0);
        void checkAndUpdateResourceSizes();
        //推导出各个image的大小，从固定大小的 /窗口 的
        //对于可能没有孤立的无法推导的 如果 一个单独的 pass，什么都没 连，然后输出了一个image，image还没有指定大小， 给予256*256
        void generateImageGraph();
        void computeSize();
        void updateFrameIndices();
        void saveExecutionOrder(const std::vector<PassGraphType::vertex_descriptor>& sortedPasses,
                                const std::vector<std::vector<PassGraphType::vertex_descriptor>>& passGroups);
        //从shaderGenreate获取resoureLayout，如果没有，就创建并添加新的resoureLayout
        void generateImageResource();
        void compile();
        void analyzeImplicitDependencies();
        bool isPassWritingToResource(const std::string& passName, const std::string& resourceName);
        bool isPassReadingFromResource(const std::string& passName, const std::string& resourceName);
        void execute(RHI::CommandBuffer* cmdBuf);
        Pass* getPassByName(const std::string& name)
        {
            auto it = m_passVertex.find(name);
            if (it!= m_passVertex.end()) {
                return m_passGraph[it->second].pass;
            }
            return nullptr;
        }
        void swapJobQueue();
    };

}
#endif //RENDERGRAPH_H
