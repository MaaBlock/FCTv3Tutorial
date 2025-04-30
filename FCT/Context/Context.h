#pragma once
#include "../ThirdParty.h"
#include "../ToolDefine.h"
#include "../MutilThreadBase/RefCount.h"
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
	using SumitTicker = std::function<void()>;
	using TickerToken = uint32_t;
	struct FrameResource
	{
		/*
		 *提供alloc 函数，来在初始化阶段指定需要 几个，如renderFinishedSemaphores
		 *提供获取指定序号的renderFinishedSemaphores和cmdB uf
		 */
		RHI::Semaphore* imageAvailableSemaphore;
		std::vector<RHI::Semaphore*> renderFinishedSemaphores;
		std::vector<RHI::Fence*> presentCompleteFences;
		RHI::CommandPool* cmdPool;
		std::vector<RHI::CommandBuffer*> cmdBufs;
		Context* ctx;
		void init(Context* ctx);
		void allocCommandBuffers(uint32_t additionalCount);
		void allocPresentCompleteFences(uint32_t additionalCount);
		void allocRenderFinshSemaphores(uint32_t additionalCount);
		uint32_t allocBaseCommandBuffers();
		RHI::Fence* getPresentCompleteFence(uint32_t index);
		RHI::Semaphore* getRenderFinishedSemaphore(uint32_t index);
		RHI::Semaphore* getImageAvailableSemaphore();
		RHI::CommandPool* getCmdPool();
		RHI::CommandBuffer* getCmdBuf(uint32_t index);
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
		virtual void compilePasses() = 0;
		virtual void submitPasses() = 0;
		virtual RHI::VertexBuffer* createVertexBuffer() = 0;
		virtual RHI::IndexBuffer* createIndexBuffer() = 0;
		virtual RHI::DescriptorPool* createDescriptorPool() = 0;
		virtual RHI::DepthStencilView* createDepthStencilView() = 0;
		virtual PassResource* createPassResource() = 0;
		virtual RHI::TextureView* createTextureView() = 0;
		virtual Sampler* createSampler() = 0;
	protected:
		ModelLoader* m_modelLoader;
	public:
		Mesh<uint32_t>* createMesh(const ModelMesh* modelMesh, const VertexLayout& layout)
		{
			if (!modelMesh) {
				return nullptr;
			}

			Mesh<uint32_t>* mesh = new Mesh<uint32_t>(this, layout);
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
		Mesh<uint32_t>* loadMesh(const std::string& filename,const std::string& meshName, const VertexLayout& layout)
		{
			auto md = m_modelLoader->loadModel(filename);
			auto mMesh =  md->findMesh("teapot");
			return createMesh(mMesh,layout);
		}
	public:
		void flush()
		{
			FCT_WAIT_FOR(m_currentFlush);
			swapQueue();
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
			compilePasses();
			submitPasses();
			swapBuffers();
		}
		virtual RHI::RenderTargetView* createRenderTargetView() = 0;
		void addBindWindow(Window* wnd)
		{
			m_bindWindows.push_back(wnd);
			initWndFrameResources(wnd);
		}
		void tick()
		{
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
		void setMaxFrameInFlights(size_t max) { m_maxFrameInFlights = max; }
		void waitCurrentFlush()
		{
			FCT_WAIT_FOR(m_currentFlush);
		}
	public:
		void nextFrame();
		void currentFlush();
	protected:
		SumitTicker m_ticker;
		size_t m_maxFrameInFlights;
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
	public:
		RHI::CommandBuffer* getCmdBuf(Window* wnd,uint32_t index);
		uint32_t allocBaseCommandBuffers(Window* wnd);
		void maxFrameInFlight(uint32_t maxFrameInFlight);
		uint32_t maxFrameInFlight() const { return m_maxFrameInFlight; }
		void initFrameManager();
		uint32_t currentFrameIndex() const { return m_frameIndex; }
		RHI::DescriptorPool* getDescriptorPool(Window* wnd);
		//todo: 考虑可能要变更为 [IRenderTarget*]<->[DescriptorPool*] map
	protected:
		void initWndFrameResources(Window* wnd);
	public:
		Image* loadTexture(const std::string& filename);
	protected:
		ImageLoader* m_imageLoader;

		//render graph部分
	protected:

	public:

	};
}
#include "../UI/Window.h"
namespace FCT {
	inline void FrameResource::init(Context* ctx)
	{
		this->ctx = ctx;

		cmdPool = ctx->createCommandPool();
		cmdPool->create();

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
    inline void FrameResource::allocPresentCompleteFences(uint32_t additionalCount)
    {
        uint32_t currentCount = presentCompleteFences.size();

        presentCompleteFences.resize(currentCount + additionalCount);

        for (uint32_t i = currentCount; i < presentCompleteFences.size(); i++) {
            presentCompleteFences[i] = ctx->createFence();
            presentCompleteFences[i]->createSignaled();
        }
    }
    inline void FrameResource::allocRenderFinshSemaphores(uint32_t additionalCount)
    {
        uint32_t currentCount = renderFinishedSemaphores.size();

        renderFinishedSemaphores.resize(currentCount + additionalCount);

        for (uint32_t i = currentCount; i < renderFinishedSemaphores.size(); i++) {
            renderFinishedSemaphores[i] = ctx->createSemaphore();
            renderFinishedSemaphores[i]->create();
        }
    }
    inline uint32_t FrameResource::allocBaseCommandBuffers()
    {
        size_t cmdBufIndex = cmdBufs.size();
        size_t fenceIndex = presentCompleteFences.size();
        size_t semaphoreIndex = renderFinishedSemaphores.size();
        allocCommandBuffers(1);
        allocRenderFinshSemaphores(1);
        allocPresentCompleteFences(1);
        auto cmdBuf = cmdBufs[cmdBufIndex];
        auto fence = presentCompleteFences[fenceIndex];
        auto semaphore = renderFinishedSemaphores[semaphoreIndex];
        cmdBuf->addWaitSemaphore(imageAvailableSemaphore);
        cmdBuf->addSignalSemaphore(semaphore);
        cmdBuf->fence(fence);
        return cmdBufIndex;
    }
    inline RHI::Fence* FrameResource::getPresentCompleteFence(uint32_t index)
    {
        if (index < presentCompleteFences.size()) {
            return presentCompleteFences[index];
        }
        return nullptr;
    }
    inline RHI::Semaphore* FrameResource::getRenderFinishedSemaphore(uint32_t index)
    {
        if (index < renderFinishedSemaphores.size()) {
            return renderFinishedSemaphores[index];
        }
        return nullptr;
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
		//FrameBufferManager Flush
		for (auto &[wnd, frameResources] : m_frameResources)
		{
			wnd->clearRenderFinshSemaphores();
			for (auto semaphore : frameResources[m_frameIndex].renderFinishedSemaphores)
			{
				wnd->addRenderFinshSemaphore(semaphore);
			}
		}
		m_frameIndex = (m_frameIndex + 1) % m_maxFrameInFlight;
		for (auto &[wnd, frameResources] : m_frameResources)
		{
			wnd->clearRenderFinshFences();
			for (auto fence : frameResources[m_frameIndex].presentCompleteFences)
			{
				wnd->addRenderFinshFence(fence);
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
	}

	inline RHI::DescriptorPool* Context::getDescriptorPool(Window* wnd)
	{
		return m_descriptorPools[wnd];
	}
}
