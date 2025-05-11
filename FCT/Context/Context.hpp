#ifndef FCT_CONTEXT_IH
#define FCT_CONTEXT_IH
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
#endif