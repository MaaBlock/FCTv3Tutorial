#pragma once
#include "../ThirdParty.h"
#include "../ToolDefine.h"
#include "../Bases.h"
#include "../Memory/ObjectPool.h"
#include "../MutilThreadBase/Computation.h"
#include "./DataTypes.h"
#include "../RHI/VertexShader.h"
#include "../RHI/PixelShader.h"
#include "../RHI/Swapcain.h"
#include "./VertexBuffer.h"
#include "./Material.h"
#include "./DrawCall.h"
#include "./ConstBuffer.h"
#include "./Texture.h"
#include "./TextureArray.h"
#include "./Format.h"
#include "./Image.h"
#include "./Pass.h"
#include "../RHI/CommandPool.h"
#include "../RHI/Pass.h"
#include "../RHI/Fence.h"
#include "../RHI/Semaphore.h"
#include "../RHI/PassGroup.h"
#include "../RHI/Sampler.h"
#include "../RHI/RasterizationPipeline.h"
#include "../RHI/InputLayout.h"
#include "../RHI/DescriptorPool.h"
#include "./ShaderCompiler.h"
#include "./ShaderGenerator.h"
#include "Mesh.h"
#include "PassResource.h"
#include "../RHI/DepthStencilView.h"
#include "../ModelLoader/ModelLoader.h"
#include "./Mesh.h"
#include "MutilBufferImage.h"
#include "./RenderGraph.h"
#include "FencePool.h"
#include "SemaphorePool.h"

namespace FCT
{
	namespace RHI
	{
		class TextureView;
		class ConstBuffer;
		class IndexBuffer;
	}

	class VertexBuffer;
	//class InputLayout;
	class VertexShader;
	class PixelShader;
	class DrawCall;
	class Window;
	class SemaphorePool;
	class FencePool;
	using SumitTicker = std::function<void()>;
	using TickerToken = uint32_t;
	struct FrameResource
	{
		/*
		 *提供alloc 函数，来在初始化阶段指定需要 几个，如renderFinishedSemaphores
		 *提供获取指定序号的renderFinishedSemaphores和cmdB uf
		 */
		RHI::Semaphore* imageAvailableSemaphore;
		//ObjectPool<RHI::Semaphore> renderFinishedSemaphoresPool;
		SemaphorePool* renderFinishedSemaphoresPool;
		FencePool* presentCompleteFencePool;

		std::vector<RHI::Semaphore*> renderFinishedSemaphores;
		std::vector<RHI::Fence*> presentCompleteFences;
		RHI::CommandPool* cmdPool;
		std::vector<RHI::CommandBuffer*> cmdBufs;
		Context* ctx;
		void init(Context* ctx);
		void allocCommandBuffers(uint32_t additionalCount);

		RHI::Fence* allocPresentCompleteFence();
		RHI::Semaphore* allocRenderFinishedSemaphore();
		uint32_t allocBaseCommandBuffers();
		void allocBaseCommandBuffers(uint32_t index);
		void freeCommandBuffers(uint32_t index);
		RHI::Semaphore* getImageAvailableSemaphore();
		RHI::CommandPool* getCmdPool();
		RHI::CommandBuffer* getCmdBuf(uint32_t index);
		//注：不应该频繁的 分配  和 删除  fence/ 信号量/命令队列
	};
	/*Context作用
	 *1.作为Context接口
	 *2.作为RenderGraph系统
	 *3.作为帧 管理器（合并在RenderGraph系统）
	 */
	class Runtime;
	class Context : public RefCount
	{
	public:
		Context(Runtime* runtime);
		virtual ~Context();
		virtual void clear(float r, float g, float b) = 0;
		virtual void viewport(int x, int y, int width, int height) = 0;
		virtual VertexShader* createVertexShader() = 0;
		virtual RHI::VertexShader* newRhiVertexShader() = 0;
		virtual RHI::PixelShader* newRhiPixelShader() = 0;
		virtual PixelShader* createPixelShader() = 0;
		virtual RHI::InputLayout* createInputLayout() = 0;
		virtual DrawCall* createDrawCall(PrimitiveType primitiveType, uint32_t startVertex, uint32_t vertexCount) = 0;
		virtual RHI::ConstBuffer* createConstBuffer() = 0;
		virtual Texture* createTexture() = 0;
		virtual TextureArray* createTextureArray() = 0;
		virtual Image* createImage() = 0;
		virtual MutilBufferImage* createMutilBufferImage() = 0;
		virtual RHI::Image* newRhiImage() = 0;
		virtual RHI::Swapchain* createSwapchain() = 0;
		virtual RHI::PassGroup* createPassGroup() = 0;
		virtual RHI::Pass* createPass() = 0;
		virtual RHI::RasterizationPipeline* createTraditionPipeline() = 0;
		virtual RHI::Fence* createFence() = 0;
		virtual RHI::Semaphore* createSemaphore() = 0;
		virtual void create() = 0;
		virtual RHI::CommandPool* createCommandPool() = 0;
		virtual RHI::VertexBuffer* createVertexBuffer() = 0;
		virtual RHI::IndexBuffer* createIndexBuffer() = 0;
		virtual RHI::DescriptorPool* createDescriptorPool() = 0;
		virtual RHI::DepthStencilView* createDepthStencilView() = 0;
		virtual PassResource* createPassResource() = 0;
		virtual RHI::TextureView* createTextureView() = 0;
		virtual Sampler* createSampler() = 0;
		virtual SemaphorePool* createSemaphorePool() = 0;
		virtual FencePool* createFencePool() = 0;
	protected:
		ModelLoader* m_modelLoader;
	public:
		StaticMesh<uint32_t>* createMesh(const ModelMesh* modelMesh, const VertexLayout& layout)
		{
			if (!modelMesh) {
				return nullptr;
			}

			StaticMesh<uint32_t>* mesh = new StaticMesh<uint32_t>(this, layout);
			VertexBuffer* vertexBuffer = mesh->getVertexBuffer();
			vertexBuffer->resize(modelMesh->vertices.size());
			for (uint32_t i = 0; i < modelMesh->vertices.size(); ++i) {
				const auto& modelVertex = modelMesh->vertices[i];
				Vertex vertex = (*vertexBuffer)[i];

				for (size_t j = 0; j < layout.getElementCount(); ++j) {
					const VertexElement& element = layout.getElement(j);
					setVertexAttributeFromModel(vertex, j, element, modelVertex);
				}
			}

			std::vector<uint32_t> indices = modelMesh->indices;
			mesh->setIndices(indices);

			mesh->create();

			return mesh;
		}
		StaticMesh<uint32_t>* loadMesh(const std::string& filename,const std::string& meshName, const VertexLayout& layout)
		{
			auto md = m_modelLoader->loadModel(filename);
			auto mMesh =  md->findMesh("teapot");
			return createMesh(mMesh,layout);
		}
	protected:
		struct LogicTaskData
		{
			std::function<void()> task;
		};
		boost::lockfree::queue<LogicTaskData*,boost::lockfree::capacity<1024>> m_logicTask;
		void postLogicTask(const std::function<void()>& task)
		{
			LogicTaskData* data = FCT_NEW(LogicTaskData);
			data->task = task;
			m_logicTask.push(data);
		}
	public:
		void flush()
		{
			m_logicTask.consume_all([](LogicTaskData*& data) {
                data->task();
				FCT_DELETE(data);
            });
			advanceLogicFrame();
			FCT_WAIT_FOR(m_currentFlush);
            m_currentGraph->swapJobQueue();
			nextFrame();
		}
		virtual void swapQueue();
		void submitThread()
		{

			while (m_ctxRunning) {
				FCT_WAIT_FOR(m_nextFrame);
				tick();
				currentFlush();
			}
		}
		void swapBuffers();
		void defaultTick()
		{
			auto cmdBuf = getCmdBuf(m_bindWindows[0], 0);
			cmdBuf->reset();
			cmdBuf->begin();
			excutePasses(cmdBuf);
			cmdBuf->end();
			cmdBuf->submit();
			std::this_thread::yield();
			swapBuffers();
		}
		virtual RHI::RenderTargetView* createRenderTargetView() = 0;
		void addBindWindow(Window* wnd)
		{
			m_bindWindows.push_back(wnd);
			m_currentGraph->addWindowResource(wnd);
			initWndFrameResources(wnd);
		}
		void tick()
		{
			m_currentGraph->updateFrameIndices();
			m_currentGraph->checkAndUpdateResourceSizes();
			m_currentGraph->updateResource();
			m_ticker();
		}
		/*
		 * 初始化阶段 可以在flush前任意修改，因为提交线程一直在等待下一帧
		 * 运行阶段 不允许修改 或 拆分flush函数，在wait currentFlush和nextFrame之间修改
		 */
		void submitTicker(SumitTicker ticker)
		{
			m_ticker = ticker;
		}
		void createCompiler();
		ShaderCompiler* getCompiler() { return m_compiler; }
		ShaderGenerator* getGenerator() { return m_generator; }
		//maxFrameInFlight 是用来封装在RenerGraph系统里的（Context扮演RenderGraph的角色）
		void waitCurrentFlush()
		{
			FCT_WAIT_FOR(m_currentFlush);
		}
	public:
		void nextFrame();
		void currentFlush();
	protected:
		SumitTicker m_ticker;
		std::vector<Window*> m_bindWindows;
		bool m_nextFrame;
		bool m_currentFlush;
		std::thread m_submitThread;
		bool m_ctxRunning;
		ShaderCompiler* m_compiler;
		ShaderGenerator* m_generator;
	protected:
		uint32_t m_maxFrameInFlight;
		std::map<Window*, std::vector<FrameResource>> m_frameResources;
		std::map<Window*, RHI::DescriptorPool*> m_descriptorPools;
		size_t m_frameIndex = 0;
		size_t m_logicFrameIndex = 0; //逻辑帧index
		std::thread::id m_submitThreadId;
		//todo:未分离线程时，m_submitThreadId = 逻辑and提交线程 所在id
	public:
		RHI::CommandBuffer* getCmdBuf(Window* wnd,uint32_t index);
		uint32_t allocBaseCommandBuffers(Window* wnd);
		void freeCommandBuffers(Window* wnd, uint32_t index);
		void maxFrameInFlight(uint32_t maxFrameInFlight);
		uint32_t maxFrameInFlight() const { return m_maxFrameInFlight; }
		void initFrameManager();
		uint32_t currentSubmitFrameIndex() const { return m_frameIndex; }
		/*
		 *分离了submit线程
		 *逻辑线程返回逻辑帧index
		 *提交线程返回提交帧index
		 *未分离submit 线程
		 *逻辑部分返回提交帧 index
		 *提交帧部分返返回提交帧 index
		 */
		uint32_t currentFrameIndex()
		{
			if (m_submitThreadId == std::this_thread::get_id())
			{
				return m_frameIndex;
			}
			return m_logicFrameIndex;
		}
		RHI::DescriptorPool* getDescriptorPool(Window* wnd);
		//todo: 考虑可能要变更为 [IRenderTarget*]<->[DescriptorPool*] map
		void advanceLogicFrame()
		{
			m_logicFrameIndex = (m_logicFrameIndex + 1) % m_maxFrameInFlight;
		}
	protected:
		void initWndFrameResources(Window* wnd);
	public:
		Image* loadTexture(const std::string& filename);
	protected:
		ImageLoader* m_imageLoader;

		//render graph部分
	protected:
		RenderGraph* m_defaultGraph;
		RenderGraph* m_currentGraph;
	public:
		RenderGraph* currentGraph() { return m_currentGraph; }
		Image* getResourceImage(std::string name)
		{
			return m_currentGraph->getResourceImage(name);
		}
		//todo:这一系列转发函数想个办法优化
		void setCurrentGraph(RenderGraph* graph)
		{
			m_currentGraph->release();
			m_currentGraph = graph;
			m_currentGraph->addRef();
		}
		void addPass(const std::string& name, Pass* pass)
		{
			pass->addRef();
			m_currentGraph->addPass(name, pass);
		}

		void submit(Job* job,std::string name)
		{
			m_currentGraph->submit(job, name);
		}
		void compilePasses()
		{
			m_currentGraph->compile();
		}
		void addPassDenpendency(const std::string& from,const std::string& to)
		{
			m_currentGraph->addPassDenpendency(from,to);
		}
		void bindOutputImage(const std::string& name, std::string image,uint8_t slot = 0)
		{
			m_currentGraph->bindOutputImage(name, image, slot);
		}
		void bindTextureImage(std::string name, std::string image,uint32_t width = 0, uint32_t height = 0,Samples samples = Samples::sample_undefined)
		{
			m_currentGraph->bindTextureImage(name, image, width, height, samples);
		}
		void bindDepthStencil(const char* name, std::string image)
		{
			m_currentGraph->bindDepthStencil(name, image);
		}
		void bindOutput(const char* name, Window* wnd, uint8_t slot = 0)
		{
			m_currentGraph->bindOutput(name, wnd, slot);
		}
		void excutePasses(RHI::CommandBuffer* cmdBuf)
		{
			m_currentGraph->execute(cmdBuf);
		}
		void excute()
		{

		}
		Pass* findPass(const std::string& name)
		{
			return m_currentGraph->getPassByName(name);
		}
	};
}
#include "../UI/Window.h"
namespace FCT {
	//这部分代码是放出来看的， 用于，比如可以自己实现flush，然后调用里面的函数
	inline void FrameResource::init(Context* ctx)
	{
		this->ctx = ctx;

		cmdPool = ctx->createCommandPool();
		cmdPool->create();

		renderFinishedSemaphoresPool = ctx->createSemaphorePool();
		renderFinishedSemaphoresPool->setDestroyCallback([this](RHI::Semaphore* semaphore)
		{
			auto ret = std::find(renderFinishedSemaphores.begin(),renderFinishedSemaphores.end(),semaphore);
			if (ret != renderFinishedSemaphores.end())
			{
				*ret = nullptr;
			}
		});
		presentCompleteFencePool = ctx->createFencePool();
		presentCompleteFencePool->setDestroyCallback([this](RHI::Fence* fence)
		{
			auto ret = std::find(presentCompleteFences.begin(),presentCompleteFences.end(),fence);
			if (ret != presentCompleteFences.end())
			{
				*ret = nullptr;
			}
		});
		imageAvailableSemaphore = ctx->createSemaphore();
		imageAvailableSemaphore->create();
	}
	inline void FrameResource::allocCommandBuffers(uint32_t additionalCount)
	{
		uint32_t currentCount = cmdBufs.size();

		cmdBufs.resize(currentCount + additionalCount);

		for (uint32_t i = currentCount; i < cmdBufs.size(); i++) {
			cmdBufs[i] = cmdPool->createCommandBuffer();
			cmdBufs[i]->create();
		}
	}

	inline RHI::Fence* FrameResource::allocPresentCompleteFence()
	{
		auto ret = presentCompleteFencePool->alloc();
		ret->createSignaled();
		presentCompleteFences.push_back(ret);
		return ret;
	}
	inline RHI::Semaphore* FrameResource::allocRenderFinishedSemaphore()
	{
		auto ret = renderFinishedSemaphoresPool->alloc();
        ret->create();
		renderFinishedSemaphores.push_back(ret);
        return ret;
	}
    inline uint32_t FrameResource::allocBaseCommandBuffers()
    {
        size_t cmdBufIndex = cmdBufs.size();
		allocCommandBuffers(1);
        auto cmdBuf = cmdBufs[cmdBufIndex];
        auto fence = allocPresentCompleteFence();
        auto semaphore = allocRenderFinishedSemaphore();
        cmdBuf->addWaitSemaphore(imageAvailableSemaphore);
        cmdBuf->addSignalSemaphore(semaphore);
        cmdBuf->fence(fence);
		fence->release();
		semaphore->release();
        return cmdBufIndex;
    }

	inline void FrameResource::allocBaseCommandBuffers(uint32_t index)
	{
		freeCommandBuffers(index);

	}

	inline void FrameResource::freeCommandBuffers(uint32_t index)
	{
		if (index < cmdBufs.size())
		{
			FCT_SAFE_RELEASE(cmdBufs[index]);
		}
	}
    inline RHI::Semaphore* FrameResource::getImageAvailableSemaphore()
    {
        return imageAvailableSemaphore;
    }
    inline RHI::CommandPool* FrameResource::getCmdPool()
    {
        return cmdPool;
    }
    inline RHI::CommandBuffer* FrameResource::getCmdBuf(uint32_t index)
    {
        if (index < cmdBufs.size()) {
            return cmdBufs[index];
        }
        return nullptr;
    }
	inline void Context::swapBuffers()
	{
		char const* const test = "test";
		//FrameBufferManager Flush
		for (auto &[wnd, frameResources] : m_frameResources)
		{
			wnd->clearRenderFinshSemaphores();
			for (auto semaphore : frameResources[m_frameIndex].renderFinishedSemaphores)
			{
				if (semaphore)
				{
					wnd->addRenderFinshSemaphore(semaphore);
				}
			}
		}
		m_frameIndex = (m_frameIndex + 1) % m_maxFrameInFlight;
		for (auto &[wnd, frameResources] : m_frameResources)
		{
			wnd->clearRenderFinshFences();
			for (auto fence : frameResources[m_frameIndex].presentCompleteFences)
			{
				if (fence)
				{
					wnd->addRenderFinshFence(fence);
				}
			}
			wnd->setPresentFinshSemaphore(frameResources[m_frameIndex].imageAvailableSemaphore);
		}
		//Wnd SwapBuffers
		for (auto wnd : m_bindWindows)
		{
			wnd->swapBuffers();
		}
	}
	inline RHI::CommandBuffer* Context::getCmdBuf(Window* wnd, uint32_t index)
	{
		auto it = m_frameResources.find(wnd);
		if (it!= m_frameResources.end()) {
			return it->second[m_frameIndex].getCmdBuf(index);
		}
		return nullptr;
	}
	inline uint32_t Context::allocBaseCommandBuffers(Window* wnd)
	{
		auto it = m_frameResources.find(wnd);
		if (it != m_frameResources.end()) {
			std::vector<uint32_t> indices;

			auto fenceIndex = m_frameResources[wnd][0].presentCompleteFences.size();

			for (auto& frame : m_frameResources[wnd]) {
				uint32_t index = frame.allocBaseCommandBuffers();
				indices.push_back(index);
			}

			m_frameResources[wnd][m_frameIndex].presentCompleteFences[fenceIndex]->reset();

			if (!indices.empty()) {
				return indices[0];
			}
		}
		return UINT32_MAX;
	}
	inline void Context::freeCommandBuffers(Window* wnd, uint32_t index)
	{
		auto it = m_frameResources.find(wnd);
        if (it!= m_frameResources.end()) {
            m_frameResources[wnd][m_frameIndex].freeCommandBuffers(index);
        }
	}
	inline void Context::maxFrameInFlight(uint32_t maxFrameInFlight)
	{
		m_maxFrameInFlight = maxFrameInFlight;
		initFrameManager();
	}
	inline void Context::initFrameManager()
	{
		m_frameIndex = 0;

		for (auto wnd : m_bindWindows)
		{
			initWndFrameResources(wnd);
		}
	}
	inline void Context::initWndFrameResources(Window* wnd)
	{
		std::vector<FrameResource> frameResources(m_maxFrameInFlight);
		for (auto& resource : frameResources) {
			resource.init(this);
		}
		wnd->setPresentFinshSemaphore(frameResources[m_frameIndex].imageAvailableSemaphore);
		wnd->initRender();
		m_frameResources[wnd] = std::move(frameResources);
		m_descriptorPools[wnd] = createDescriptorPool();
		m_descriptorPools[wnd]->create();
		allocBaseCommandBuffers(wnd);
	}

	inline RHI::DescriptorPool* Context::getDescriptorPool(Window* wnd)
	{
		return m_descriptorPools[wnd];
	}
}
