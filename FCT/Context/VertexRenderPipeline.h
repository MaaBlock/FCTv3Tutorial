#pragma once
#include "./Pipeline.h"
namespace FCT {
	class VertexRenderPipeline;
	constexpr float Command_End = -1;
	constexpr float Command_MoveTo = 1;
	constexpr float Command_LineTo = 2;
	constexpr float Command_BezierCurveTo = 3;
	constexpr float Command_SetColor = 4;
	constexpr float CommandArcTo = 5.0;
	constexpr float Command_SetTransform = 6.0;
	constexpr float Command_BeginPath = 7.0;
	constexpr float Command_EndPath = 8.0;
	class VertexRenderScreen : public Object {
	public:
		VertexRenderScreen(VertexRenderPipeline* pl);
		void size(Vec3 halfSizeX, Vec3 halfSizeY);
		void viewport(float x,float y, float w, float h);
		void create();
	private:
		VertexRenderPipeline* m_pl;
		VertexFactory* m_vf;
		Context* m_ctx;
		VertexArray* m_arr;
		InputLayout* m_il;
		VertexBuffer* m_vb;
		DrawCall* m_dc;
	};
	class VertexRenderPipeline : public Pipeline {
	public:
		VertexRenderPipeline(Context* context);
		void begin(VertexRenderScreen* screen);
		void setColor(Vec4 color);
		void moveTo(Vec2 to);
		void lineTo(Vec2 to);
		void fillrect(Vec2 pos, Vec2 size);
		void rectangle(Vec2 pos, Vec2 size, float width);
		void arcTo(Vec2 center, float beginAngle, float endAngle);
		void curveTo(Vec2 control, Vec2 to);
		void curveTo(Vec2 control1, Vec2 control2, Vec2 to);
		void circle(Vec2 center, float radius);
		void roundedRect(Vec2 pos, Vec2 size, float width, float r);
		void setTransform(const Mat4& transform);
		void beginPath(float sign);
		void endPath();
		void drawChar(Font* font, char32_t ch,int x,int y);
		void end();
	private:
		void unwrapperArcTo(Vec2 center, float beginAngle, float endAngle, float q);
		VertexShader* m_vrvs;
		PixelShader* m_vrps;
		VertexFactory* m_vrf;
		Material* m_vrMaterial;
		VertexRenderScreen* m_screeen;
		VertexFactory* createFactory();
		std::vector<float> m_commandQueue;
	};
}