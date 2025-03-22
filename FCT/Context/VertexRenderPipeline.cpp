#include "../headers.h"
#include <iostream>
#define _USE_MATH_DEFINES
#include <cmath>
#ifdef FCT_DEPRECATED
namespace FCT
{
	FCT::VertexRenderScreen::VertexRenderScreen(VertexRenderPipeline *pl) : Object(pl)
	{
		m_pl = pl;
		m_vf = pl->getVertexFactory();
		m_ctx = pl->getContext();
		m_arr = new VertexArray(m_vf, 6);
		size(Vec3(1, 0,0),Vec3(0,1,0));
		viewport(0, 0, 1, 1);
	}

	void FCT::VertexRenderScreen::size(Vec3 halfSizeX, Vec3 halfSizeY)
	{
		Vec3 center(0, 0, 0);
		Vec3 topRight = center + halfSizeX + halfSizeY;
		Vec3 topLeft = center - halfSizeX + halfSizeY;
		Vec3 bottomRight = center + halfSizeX - halfSizeY;
		Vec3 bottomLeft = center - halfSizeX - halfSizeY;

		m_arr->setPosition(0, topLeft);
		m_arr->setPosition(1, topRight);
		m_arr->setPosition(2, bottomLeft);
		m_arr->setPosition(3, bottomRight);
		m_arr->setPosition(4, topRight);
		m_arr->setPosition(5, bottomLeft);
	}

	void VertexRenderScreen::viewport(float x, float y, float w, float h)
	{
		Vec2 bottomLeft(x, y);
		Vec2 bottomRight(x + w, y);
		Vec2 topLeft(x, y + h);
		Vec2 topRight(x + w, y + h);
		m_arr->setAttribute(0, "vectorCoord", topLeft);
		m_arr->setAttribute(1, "vectorCoord", topRight);
		m_arr->setAttribute(2, "vectorCoord", bottomLeft);
		m_arr->setAttribute(3, "vectorCoord", bottomRight);
		m_arr->setAttribute(4, "vectorCoord", topRight);
		m_arr->setAttribute(5, "vectorCoord", bottomLeft);
	}

	void VertexRenderScreen::create()
	{
		m_il = m_ctx->createInputLayout(m_vf);
		m_vb = m_ctx->createVertexBuffer(m_arr);
		m_vb->create(m_ctx);
		m_il->create(m_ctx, m_vb);
		m_dc = m_ctx->createDrawCall(FCT::PrimitiveType::Triangles, 0, 6);
		addResource(m_il);
		addResource(m_vb);
		addResource(m_dc);
	}

	FCT::VertexRenderPipeline::VertexRenderPipeline(Context *ctx) : Pipeline(ctx, this->createFactory())
	{
		m_vrvs = ctx->createVertexShader(m_defaultFactory);
		if (!m_vrvs->compileFromSource(R"(
VertexOutput main(VertexInput vs_input)
{
	VertexOutput vs_output;
	vs_output.position = vs_input.position;
	vs_output.vectorCoord = vs_input.vectorCoord;
	return vs_output;
})"))
		{
			std::cout << "vs����:" << std::endl;
			std::cout << m_vrvs->getCompileError() << std::endl;
		}
		m_vrps = ctx->createPixelShader(m_vrvs->getOutput());
		if (!m_vrps->compileFromSource(R"(
layout(binding = 4) uniform sampler2D commandQueue;

const float CommandEnd = -1.0;
const float CommandMoveTo = 1.0;
const float CommandLineTo = 2.0;
const float CommandBezierCurveTo = 3.0;
const float CommandSetColor = 4.0;
const float CommandArcTo = 5.0;
const float CommandSetTransform = 6.0;
const float CommandBeginPath = 7.0;
const float CommandEndPath = 8.0;

int GetCurveRootFlags(float v0, float v1, float v2) {
    int shift = ((v0 > 0.0) ? 2 : 0) + ((v1 > 0.0) ? 4 : 0) + ((v2 > 0.0) ? 8 : 0);
    return (0x2E74 >> shift) & 0x03;
}

vec2 QuadCurveSolveXAxis(vec2 v0, vec2 v1, vec2 v2) {
    vec2 a = v0 - 2.0 * v1 + v2;
    vec2 b = v0 - v1;
    float c = v0.y;
    float ra = 1.0 / a.y;
    float rb = 0.5 / b.y;
    float delta = sqrt(max(b.y * b.y - a.y * c, 0.0));
    vec2 t = vec2((b.y - delta) * ra, (b.y + delta) * ra);
    if (abs(a.y) < 0.0001220703125) t = vec2(c * rb, c * rb);
    return (a.x * t - b.x * 2.0) * t + v0.x;
}

vec2 QuadCurveSolveYAxis(vec2 v0, vec2 v1, vec2 v2) {
    vec2 a = v0 - 2.0 * v1 + v2;
    vec2 b = v0 - v1;
    float c = v0.x;
    float ra = 1.0 / a.x;
    float rb = 0.5 / b.x;
    float delta = sqrt(max(b.x * b.x - a.x * c, 0.0));
    vec2 t = vec2((b.x - delta) * ra, (b.x + delta) * ra);
    if (abs(a.x) < 0.0001220703125) t = vec2(c * rb, c * rb);
    return (a.y * t - b.y * 2.0) * t + v0.y;
}

float CurveTestXAxis(vec2 v0, vec2 v1, vec2 v2, vec2 pixelsPerUnit) {
    if (max(max(v0.x, v1.x), v2.x) * pixelsPerUnit.x < -0.5) return 0.0;
    int flags = GetCurveRootFlags(v0.y, v1.y, v2.y);
    if (flags == 0) return 0.0;
    vec2 x1x2 = QuadCurveSolveXAxis(v0, v1, v2) * pixelsPerUnit.x;
    float ret = 0.0;
    if ((flags & 1) != 0) {
        ret += clamp(x1x2.x + 0.5, 0.0, 1.0);
    }
    if ((flags & 2) != 0) {
        ret -= clamp(x1x2.y + 0.5, 0.0, 1.0);
    }
    return ret;
}

float CurveTestYAxis(vec2 v0, vec2 v1, vec2 v2, vec2 pixelsPerUnit) {
    if (max(max(v0.y, v1.y), v2.y) * pixelsPerUnit.y < -0.5) return 0.0;
    int flags = GetCurveRootFlags(v0.x, v1.x, v2.x);
    if (flags == 0) return 0.0;
    vec2 y1y2 = QuadCurveSolveYAxis(v0, v1, v2) * pixelsPerUnit.y;
    float ret = 0.0;
    if ((flags & 0x01) != 0) {
        ret -= clamp(y1y2.x + 0.5, 0.0, 1.0);
    }
    if ((flags & 0x02) != 0) {
        ret += clamp(y1y2.y + 0.5, 0.0, 1.0);
    }
    return ret;
}

vec2 CurveTest(vec2 v0, vec2 v1, vec2 v2, vec2 pixelsPerUnit) {
    return vec2(
        CurveTestXAxis(v0, v1, v2, pixelsPerUnit),
        CurveTestYAxis(v0, v1, v2, pixelsPerUnit)
    );
}

vec2 LineTest(vec2 v0, vec2 v1, vec2 pixelsPerUnit) {
    vec2 result = vec2(0.0);
    
    if (max(v0.x, v1.x) * pixelsPerUnit.x >= -0.5) {
        int signX = (v0.y > 0.0 ? 1 : 0) - (v1.y > 0.0 ? 1 : 0);
        if (signX != 0) {
            float xt = (v1.y * v0.x - v0.y * v1.x) / (v1.y - v0.y);
            result.x = float(signX) * clamp(xt * pixelsPerUnit.x + 0.5, 0.0, 1.0);
        }
    }
    
    if (max(v0.y, v1.y) * pixelsPerUnit.y >= -0.5) {
        int signY = (v1.x > 0.0 ? 1 : 0) - (v0.x > 0.0 ? 1 : 0);
        if (signY != 0) {
            float yt = (v1.x * v0.y - v0.x * v1.y) / (v1.x - v0.x);
            result.y = float(signY) * clamp(yt * pixelsPerUnit.y + 0.5, 0.0, 1.0);
        }
    }
    
    return result;
}
vec2 ArcTest(vec2 pos, vec2 bp,vec2 ep, vec2 c, float b, float e, float r, float q, vec2 pixelsPerUnit) {
    vec2 ret = vec2(0.0, 0.0);
    float minX = min(bp.x, ep.x);
    float minY = min(bp.y, ep.y);
    float maxX = max(bp.x, ep.x);
    float maxY = max(bp.y, ep.y);
    bool isInx = (pos.x >= minX && pos.x <= maxX);
    bool isIny = (pos.y >= minY && pos.y <= maxY);
    bool isInCircle = length(pos - c) <= r;
    float bLessE = b < e ? 1.0 : -1.0;
    
    if (q == 1.0 || q == 4.0) {
        ret.x = float(isIny && (isInCircle || pos.x < minX));
        ret.x *= -bLessE;
    } else {
        ret.x = float(isIny && pos.x < maxX && !isInCircle);
        ret.x *= bLessE;
    }
    
    if (q == 1.0 || q == 2.0) {
        ret.y = float(isInx && (isInCircle || pos.y < minY));
        ret.y *= -bLessE;
    } else {
        ret.y = float(isInx && pos.y < maxY && !isInCircle);
        ret.y *= bLessE;
    }
    
    return ret;
}
mat3 transform = mat3(1.0);
vec2 applyTransform(vec2 point) {
    vec3 transformedPoint = transform * vec3(point, 1.0);
    return transformedPoint.xy / transformedPoint.z;
}
float NoZeroSign(float x) {
    return x >= 0.0 ? 1.0 : -1.0;
}

PixelOutput main(PixelInput ps_input) {
    PixelOutput ps_output;
    ps_output.position = ps_input.position;
    ps_output.vectorCoord = ps_input.vectorCoord;
    ps_output.color = vec4(1.0, 1.0, 1.0, 1.0);
    
    vec2 unitsPerPixel = fwidth(ps_input.vectorCoord);
    vec2 pixelsPerUnit = 1.0 / unitsPerPixel;
    
    vec2 crossings = vec2(0.0);
    
    int i = 0;
    vec2 lastPos;
	vec4 fillColor = vec4(1.0, 0.647, 0.0, 1.0);
	vec4 currentColor = vec4(1.0, 0.647, 0.0, 1.0);
	bool isPathStarted = false;
	vec2 pathCrossings = vec2(0.0);

	float pathOperation = 0.0; // 0: Ĭ�� 1:�� 2:�� 3:��
    while(true) {
        float command = texelFetch(commandQueue, ivec2(i, 0), 0).x;
        i++;

        if (command == CommandEnd) break;
		if (command == CommandBeginPath) {
        	isPathStarted = true;
        	pathCrossings = vec2(0.0);
			pathOperation = texelFetch(commandQueue, ivec2(i, 0), 0).x;
			i += 1;
    	}
    	else if (command == CommandEndPath) {
        	isPathStarted = false;
        	if (length(pathCrossings) > 0.0) {
            	fillColor = currentColor;
        	}
			if (pathOperation == 0.0) {
            	crossings += pathCrossings;
        	} else if (pathOperation == 1.0) { 
				crossings = vec2(
                	NoZeroSign(crossings.x) * NoZeroSign(pathCrossings.x) * max(abs(crossings.x), abs(pathCrossings.x)),
                	NoZeroSign(crossings.y) * NoZeroSign(pathCrossings.y) * max(abs(crossings.y), abs(pathCrossings.y))
            	);
        	} else if (pathOperation == 2.0) { 
				crossings = vec2(
                	sign(crossings.x) * sign(pathCrossings.x) * min(abs(crossings.x), abs(pathCrossings.x)),
                	sign(crossings.y) * sign(pathCrossings.y) * min(abs(crossings.y), abs(pathCrossings.y))
            	);
        	} else if (pathOperation == 3.0) {
			    crossings += pathCrossings;
				crossings = vec2(
					NoZeroSign(crossings.x) * (1.0 - abs(crossings.x)),
					NoZeroSign(crossings.y) * (1.0 - abs(crossings.y))
            	);
			}
			pathCrossings = vec2(0);
			pathOperation = 0.0;
    	}
        if (command == CommandMoveTo) {
            lastPos.x = texelFetch(commandQueue, ivec2(i, 0), 0).x;
            lastPos.y = texelFetch(commandQueue, ivec2(i+1, 0), 0).x;
			lastPos = applyTransform(lastPos);
            i += 2;
        } 
        else if (command == CommandLineTo) {
            vec2 to;
            to.x = texelFetch(commandQueue, ivec2(i, 0), 0).x;
            to.y = texelFetch(commandQueue, ivec2(i+1, 0), 0).x;
			to = applyTransform(to);
            i += 2;

            vec2 v0 = lastPos - ps_input.vectorCoord;
            vec2 v1 = to - ps_input.vectorCoord;
			//if (isPathStarted) {
				pathCrossings += LineTest(v0, v1, pixelsPerUnit);
			//} else {
			//	crossings += LineTest(v0, v1, pixelsPerUnit);
			//}
            lastPos = to;
        } 
        else if (command == CommandSetColor) { 
			currentColor.r = texelFetch(commandQueue, ivec2(i, 0), 0).x;
        	currentColor.g = texelFetch(commandQueue, ivec2(i+1, 0), 0).x;
        	currentColor.b = texelFetch(commandQueue, ivec2(i+2, 0), 0).x;
        	currentColor.a = texelFetch(commandQueue, ivec2(i+3, 0), 0).x;
            i += 4;
        }
		else if (command == CommandArcTo) {
		    vec2 bp = lastPos;
			vec2 c;
			c.x = texelFetch(commandQueue, ivec2(i, 0), 0).x;
			c.y = texelFetch(commandQueue, ivec2(i+1, 0), 0).x;
			c = applyTransform(c);
			float r = length(c - bp);
			float b = texelFetch(commandQueue, ivec2(i+2, 0), 0).x;
			float e = texelFetch(commandQueue, ivec2(i+3, 0), 0).x;
			float q = texelFetch(commandQueue, ivec2(i+4, 0), 0).x;
    		vec2 ep = c + vec2(r * cos(e), r * sin(e));
			//if (isPathStarted) {
				pathCrossings += ArcTest(ps_input.vectorCoord, bp, ep, c, b, e, r, q, pixelsPerUnit);
			//} else {
			//	crossings += ArcTest(ps_input.vectorCoord, bp, ep, c, b, e, r, q, pixelsPerUnit);
			//}
			lastPos = ep;
			i+=5;
		}
		else if (command == CommandBezierCurveTo) {
            vec2 control = vec2(texelFetch(commandQueue, ivec2(i, 0), 0).x,
                                texelFetch(commandQueue, ivec2(i+1, 0), 0).x);
            vec2 end = vec2(texelFetch(commandQueue, ivec2(i+2, 0), 0).x,
                            texelFetch(commandQueue, ivec2(i+3, 0), 0).x);
			control = applyTransform(control);
			end = applyTransform(end);
            i += 4;

            vec2 v0 = lastPos - ps_input.vectorCoord;
            vec2 v1 = control - ps_input.vectorCoord;
            vec2 v2 = end - ps_input.vectorCoord;
			//if (isPathStarted) {
				pathCrossings += CurveTest(v0, v1, v2, pixelsPerUnit);
			//} else {
            //	crossings += CurveTest(v0, v1, v2, pixelsPerUnit);
			//}
            lastPos = end;
        }
		else if (command == CommandSetTransform) {
		    for (int j = 0; j < 3; j++) {
		        for (int k = 0; k < 3; k++) {
		            transform[j][k] = texelFetch(commandQueue, ivec2(i + j * 4 + k, 0), 0).x;
		        }
		    }
		    i += 16;
		}
    }
    /*
    ps_output.color = vec4((crossings + vec2(2,2)) / 4,0,1.0);
	return ps_output;
	*/
    float weightX = 1.0 - abs(crossings.x * 2.0 - 1.0);
    float weightY = 1.0 - abs(crossings.y * 2.0 - 1.0);
    float coverage = max(abs(crossings.x * weightX + crossings.y * weightY) / max(weightX + weightY, 0.0001220703125), min(abs(crossings.x), abs(crossings.y)));
    
    ps_output.color = fillColor;
    ps_output.color.a *= coverage;
	return ps_output;
}
)"))
		{
			std::cout << "ps����:" << std::endl;
			std::cout << m_vrps->getCompileError() << std::endl;
			std::cout << m_vrps->getSource() << std::endl;
		}
		m_vrMaterial = ctx->createMaterial(m_vrvs, m_vrps);
		m_vrMaterial->compile();
		setDefaultMaterial(m_vrMaterial);
	}
	void VertexRenderPipeline::begin(VertexRenderScreen *screen)
	{
		m_screeen = screen;
		Pipeline::begin();
		m_commandQueue.clear();
	}

	void VertexRenderPipeline::setColor(Vec4 color)
	{
		m_commandQueue.push_back(Command_SetColor);
		m_commandQueue.push_back(color.x);
		m_commandQueue.push_back(color.y);
		m_commandQueue.push_back(color.z);
		m_commandQueue.push_back(color.w);
	}

	void VertexRenderPipeline::moveTo(Vec2 to)
	{
		m_commandQueue.push_back(Command_MoveTo);
		m_commandQueue.push_back(to.x);
		m_commandQueue.push_back(to.y);
	}

	void VertexRenderPipeline::lineTo(Vec2 to)
	{
		m_commandQueue.push_back(Command_LineTo);
		m_commandQueue.push_back(to.x);
		m_commandQueue.push_back(to.y);
	}

	void VertexRenderPipeline::fillrect(Vec2 pos, Vec2 size)
	{
		moveTo(pos);
		lineTo(Vec2(pos.x, pos.y + size.y));
		lineTo(Vec2(pos.x + size.x, pos.y + size.y));
		lineTo(Vec2(pos.x + size.x, pos.y));
		lineTo(pos);
	}
	void VertexRenderPipeline::arcTo(Vec2 center, float beginAngle, float endAngle)
	{
		beginAngle = fmod(beginAngle, 2 * M_PI);
		endAngle = fmod(endAngle, 2 * M_PI);
		if (beginAngle < 0)
		{
			beginAngle += 2 * M_PI;
			if (endAngle)
			{
				endAngle += 2 * M_PI;
				endAngle = fmod(endAngle, 2 * M_PI);
			}
			else
			{
				endAngle += 2 * M_PI;
			}
		}
		if (endAngle < 0)
		{
			if (beginAngle)
			{
				beginAngle += 2 * M_PI;
				beginAngle = fmod(beginAngle, 2 * M_PI);
			}
			else
			{
				beginAngle += 2 * M_PI;
			}
			endAngle += 2 * M_PI;
		}

		if (abs(endAngle - beginAngle) < 1e-6 || abs(endAngle - beginAngle) >= 2 * M_PI - 1e-6)
		{
			for (int q = 1; q <= 4; ++q)
			{
				unwrapperArcTo(center, (q - 1) * M_PI_2, q * M_PI_2, q);
			}
			return;
		}

		bool isClockwise = endAngle < beginAngle;

		if (isClockwise)
		{
			if (endAngle > beginAngle)
				endAngle -= 2 * M_PI;

			for (int q = 4; q >= 1; --q)
			{
				float quadrantStart = q * M_PI_2;
				float quadrantEnd = (q - 1) * M_PI_2;

				if (beginAngle > quadrantEnd && endAngle < quadrantStart)
				{
					float arcStart = std::min(beginAngle, quadrantStart);
					float arcEnd = std::max(endAngle, quadrantEnd);
					unwrapperArcTo(center, arcStart, arcEnd, q);
				}

				if (endAngle >= quadrantEnd)
					break;
			}
		}
		else
		{
			if (endAngle < beginAngle)
				endAngle += 2 * M_PI; // Ensure endAngle is greater than beginAngle

			for (int q = 1; q <= 4; ++q)
			{
				float quadrantStart = (q - 1) * M_PI_2;
				float quadrantEnd = q * M_PI_2;

				if (beginAngle < quadrantEnd && endAngle > quadrantStart)
				{
					float arcStart = std::max(beginAngle, quadrantStart);
					float arcEnd = std::min(endAngle, quadrantEnd);
					unwrapperArcTo(center, arcStart, arcEnd, q);
				}

				if (endAngle <= quadrantEnd)
					break;
			}
		}
	}
	void VertexRenderPipeline::curveTo(Vec2 control, Vec2 to)
	{
		m_commandQueue.push_back(Command_BezierCurveTo);
		m_commandQueue.push_back(control.x);
		m_commandQueue.push_back(control.y);
		m_commandQueue.push_back(to.x);
		m_commandQueue.push_back(to.y);
	}
	void VertexRenderPipeline::curveTo(Vec2 control1, Vec2 control2, Vec2 to)
	{
		Vec2 start = Vec2(m_commandQueue[m_commandQueue.size() - 2], m_commandQueue[m_commandQueue.size() - 1]);

		Vec2 q0 = start;
		Vec2 q1 = start + (control1 - start) * 2.0f / 3.0f;
		Vec2 q2 = to + (control2 - to) * 2.0f / 3.0f;
		Vec2 q3 = to;

		Vec2 m = (q1 + q2) * 0.5f;

		m_commandQueue.push_back(Command_BezierCurveTo);
		m_commandQueue.push_back(q1.x);
		m_commandQueue.push_back(q1.y);
		m_commandQueue.push_back(m.x);
		m_commandQueue.push_back(m.y);

		m_commandQueue.push_back(Command_BezierCurveTo);
		m_commandQueue.push_back(q2.x);
		m_commandQueue.push_back(q2.y);
		m_commandQueue.push_back(q3.x);
		m_commandQueue.push_back(q3.y);
	}
	void VertexRenderPipeline::circle(Vec2 center, float radius)
	{
		moveTo(Vec2(center.x + radius, center.y));
		arcTo(center, 0, M_2_PI);
	}
	void VertexRenderPipeline::roundedRect(Vec2 pos, Vec2 size, float width, float r)
	{
		r = std::min(r, std::min(size.x / 2, size.y / 2));
		float halfWidth = width / 2;

		// ��������˳ʱ�룩
		moveTo(Vec2(pos.x + r, pos.y));
		lineTo(Vec2(pos.x + size.x - r, pos.y));
		arcTo(Vec2(pos.x + size.x - r, pos.y + r), -M_PI_2, 0);
		lineTo(Vec2(pos.x + size.x, pos.y + size.y - r));
		arcTo(Vec2(pos.x + size.x - r, pos.y + size.y - r), 0, M_PI_2);
		lineTo(Vec2(pos.x + r, pos.y + size.y));
		arcTo(Vec2(pos.x + r, pos.y + size.y - r), M_PI_2, M_PI);
		lineTo(Vec2(pos.x, pos.y + r));
		arcTo(Vec2(pos.x + r, pos.y + r), M_PI, 3 * M_PI_2);

		// ����������ʱ�룩
		moveTo(Vec2(pos.x + r + width, pos.y + width));
		arcTo(Vec2(pos.x + r + width, pos.y + r + width), 3 * M_PI_2, M_PI);
		lineTo(Vec2(pos.x + width, pos.y + size.y - r - width));
		arcTo(Vec2(pos.x + r + width, pos.y + size.y - r - width), M_PI, M_PI_2);
		lineTo(Vec2(pos.x + size.x - r - width, pos.y + size.y - width));
		arcTo(Vec2(pos.x + size.x - r - width, pos.y + size.y - r - width), M_PI_2, 0);
		lineTo(Vec2(pos.x + size.x - width, pos.y + r + width));
		arcTo(Vec2(pos.x + size.x - r - width, pos.y + r + width), 0, -M_PI_2);
		lineTo(Vec2(pos.x + r + width, pos.y + width));
	}
	void VertexRenderPipeline::setTransform(const Mat4 &transform)
	{
		m_commandQueue.push_back(Command_SetTransform);
		m_commandQueue.push_back(transform.m[0]);
		m_commandQueue.push_back(transform.m[1]);
		m_commandQueue.push_back(transform.m[2]);
		m_commandQueue.push_back(transform.m[3]);
		m_commandQueue.push_back(transform.m[4]);
		m_commandQueue.push_back(transform.m[5]);
		m_commandQueue.push_back(transform.m[6]);
		m_commandQueue.push_back(transform.m[7]);
		m_commandQueue.push_back(transform.m[8]);
		m_commandQueue.push_back(transform.m[9]);
		m_commandQueue.push_back(transform.m[10]);
		m_commandQueue.push_back(transform.m[11]);
		m_commandQueue.push_back(transform.m[12]);
		m_commandQueue.push_back(transform.m[13]);
		m_commandQueue.push_back(transform.m[14]);
		m_commandQueue.push_back(transform.m[15]);
	}
	void VertexRenderPipeline::drawChar(Font *font, char32_t ch, int x, int y)
	{
		auto data = font->getGlyphInfo(ch);
		if (data && !data->outlineCommands.empty())
		{
			m_commandQueue.insert(m_commandQueue.end(), data->outlineCommands.begin(), data->outlineCommands.end());
		}
	}
	void VertexRenderPipeline::rectangle(Vec2 pos, Vec2 size, float width)
	{
		moveTo(Vec2(pos.x - width / 2, pos.y - width / 2));
		lineTo(Vec2(pos.x + size.x + width / 2, pos.y - width / 2));
		lineTo(Vec2(pos.x + size.x + width / 2, pos.y + size.y + width / 2));
		lineTo(Vec2(pos.x - width / 2, pos.y + size.y + width / 2));
		lineTo(Vec2(pos.x - width / 2, pos.y - width / 2));

		moveTo(Vec2(pos.x + width / 2, pos.y + width / 2));
		lineTo(Vec2(pos.x + width / 2, pos.y + size.y - width / 2));
		lineTo(Vec2(pos.x + size.x - width / 2, pos.y + size.y - width / 2));
		lineTo(Vec2(pos.x + size.x - width / 2, pos.y + width / 2));
		lineTo(Vec2(pos.x + width / 2, pos.y + width / 2));
	}

	void FCT::VertexRenderPipeline::end()
	{
		m_commandQueue.push_back(Command_End);
		Texture *texture = m_context->createTexture();
		texture->setSlot(4);
		texture->create(m_commandQueue.size(), 1, FCT::Texture::Format::R32F);
		texture->setData(m_commandQueue.data(), m_commandQueue.size());
		texture->bind();
		m_screeen->draw();
		Pipeline::end();
		m_screeen = nullptr;
		texture->release();
	}

	void VertexRenderPipeline::unwrapperArcTo(Vec2 center, float beginAngle, float endAngle, float q)
	{
		// std::cout << "beginAngle:" << beginAngle << ":" << endAngle << ":" << q << '\n';
		m_commandQueue.push_back(CommandArcTo);

		m_commandQueue.push_back(center.x);
		m_commandQueue.push_back(center.y);

		m_commandQueue.push_back(beginAngle);
		m_commandQueue.push_back(endAngle);

		m_commandQueue.push_back(q);
	}

	FCT::VertexFactory *FCT::VertexRenderPipeline::createFactory()
	{
		m_vrf = new VertexFactory();
		m_vrf->addAttribute(FCT::PipelineAttributeType::Position3f, "position");
		m_vrf->addAttribute(FCT::PipelineAttributeType::Custom, "vectorCoord", DataType::Vec2);
		return m_vrf;
	}
	void VertexRenderPipeline::beginPath(float sign)
	{
		m_commandQueue.push_back(Command_BeginPath);
		m_commandQueue.push_back(sign);
	}

	void VertexRenderPipeline::endPath()
	{
		m_commandQueue.push_back(Command_EndPath);
	}
}
#endif