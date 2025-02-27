#pragma once
#include "../MutilThreadBase/RefCount.h" 
#include "./VertexFactory.h"
#include "./VertexShader.h"
#include "./PixelShader.h"
#include "./VertexBuffer.h"
#include "./Material.h"
#include "./DrawCall.h"
#include "./ConstBuffer.h"
#include "./Texture.h"
#include "./TextureArray.h"
namespace FCT {
	class VertexBuffer;
	class InputLayout;
	class DrawCall;
	class Context : public RefCount {
	public:
		virtual ~Context() {};
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
	private:

	};
}
