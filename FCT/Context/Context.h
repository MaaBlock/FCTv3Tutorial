#pragma once
#include "../ThirdParty.h"
#include "../MutilThreadBase/RefCount.h"
#include "../MutilThreadBase/Computation.h"
#include "./VertexFactory.h"
#include "./VertexShader.h"
#include "./PixelShader.h"
#include "./VertexBuffer.h"
#include "./Material.h"
#include "./DrawCall.h"
#include "./ConstBuffer.h"
#include "./Texture.h"
#include "./TextureArray.h"
#include "./Format.h"
#include "./Image.h"
#include "./Pass.h"
namespace FCT {
	class VertexBuffer;
	class InputLayout;
	class DrawCall;
    class Window;
	class Context : public RefCount {
	public:
        Context();
		virtual ~Context();
		virtual void clear(float r, float g, float b) = 0;
		virtual void viewport(int x, int y, int width, int height) = 0;
		virtual VertexBuffer* createVertexBuffer(VertexArray* array) = 0;
		virtual VertexShader* createVertexShader(VertexFactory* factory) = 0;
		virtual PixelShader* createPixelShader(const ShaderOutput& output) = 0;
		virtual Material* createMaterial(VertexShader* vertexShader, PixelShader* pixelShader) = 0;
		virtual InputLayout* createInputLayout(VertexFactory* factory) = 0;
		virtual DrawCall* createDrawCall(PrimitiveType primitiveType, uint32_t startVertex, uint32_t vertexCount) = 0;
		virtual ConstBuffer* createConstBuffer() = 0;
		virtual Texture* createTexture() = 0;
		virtual TextureArray* createTextureArray() = 0;
        virtual Image* createImage() = 0;
        virtual void create(IRenderTarget* renderTarget) = 0;
        virtual void setFlushWindow(Window* wnd) {
            m_flushWnd = wnd;
        }
        virtual void compilePasses() = 0;
        virtual void submitPasses() = 0;
        virtual void flush();
        virtual void swapQueue();
        virtual void submitThread();
    protected:
        Window* m_flushWnd;
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
