#pragma once
#include "../ThirdParty.h"
#include "../ToolDefine.h"
#include "../MutilThreadBase/RefCount.h"
#include "../MutilThreadBase/Computation.h"
#include "./VertexFactory.h"
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
#include "../RHI/RasterizationPipeline.h"
#include "../RHI/InputLayout.h"
namespace FCT
{
	class VertexBuffer;
	//class InputLayout;
	class DrawCall;
	class Window;
	using SumitTicker = std::function<void()>;
	using TickerToken = uint32_t;
	class Context : public RefCount {
	public:
		Context();
		virtual ~Context();
		virtual void clear(float r, float g, float b) = 0;
		virtual void viewport(int x, int y, int width, int height) = 0;
		virtual VertexBuffer* createVertexBuffer(VertexArray* array) = 0;
		virtual RHI::VertexShader* createVertexShader(VertexFactory* factory) = 0;
		virtual RHI::VertexShader* createVertexShader() = 0;
		virtual RHI::PixelShader* createPixelShader(const ShaderOutput& output) = 0;
		virtual RHI::PixelShader* createPixelShader() = 0;
		//virtual Material* createMaterial(VertexShader* vertexShader, PixelShader* pixelShader) = 0;
		virtual RHI::InputLayout* createInputLayout(VertexFactory* factory) = 0;
		virtual DrawCall* createDrawCall(PrimitiveType primitiveType, uint32_t startVertex, uint32_t vertexCount) = 0;
		virtual ConstBuffer* createConstBuffer() = 0;
		virtual Texture* createTexture() = 0;
		virtual TextureArray* createTextureArray() = 0;
		virtual Image* createImage() = 0;
		virtual RHI::Swapchain* createSwapchain() = 0;
		virtual RHI::PassGroup* createPassGroup() = 0;
		virtual RHI::Pass* createPass() = 0;
		virtual RHI::RasterizationPipeline* createTraditionPipeline() = 0;
		virtual RHI::Fence* createFence() = 0;
		virtual RHI::Semaphore* createSemaphore() = 0;
		//virtual void create(IRenderTarget* renderTarget) = 0;
		virtual void create() = 0;
		virtual RHI::CommandPool* createCommandPool() = 0;
		virtual void compilePasses() = 0;
		virtual void submitPasses() = 0;
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
	protected:
		SumitTicker m_ticker;
		size_t m_maxFrameInFlights;
		std::vector<Window*> m_bindWindows;
		bool m_nextFrame;
		bool m_currentFlush;
		void nextFrame();
		void currentFlush();
		std::unordered_map<std::string,Computation<Pass>*> m_passQueue0;
		std::unordered_map<std::string,Computation<Pass>*> m_passQueue1;
		std::unordered_map<std::string,Computation<Pass>*>* m_submitQueue;
		std::unordered_map<std::string,Computation<Pass>*>* m_pushQueue;
		std::thread m_submitThread;
		bool m_ctxRunning;
	};
}
#include "../UI/Window.h"
namespace FCT {
	inline void Context::swapBuffers()
	{
		for (auto wnd : m_bindWindows)
		{
			wnd->swapBuffers();
		}
	}

}
