//
// Created by Administrator on 2025/5/4.
//
#include "../headers.h"
namespace FCT
{
    void VertexContext::updataCommandInfo()
    {
        auto commandBufferImage = m_ctx->getResourceImage(VertexCommandBufferNameInShder);

        if (commandBufferImage->width() * commandBufferImage->height() >= m_commandBuffer.size()) {

        } else {
            uint32_t requiredHeight = (m_commandBuffer.size() + 4095) / 4096;
            commandBufferImage->resize(4096, requiredHeight);
        }
        auto res = commandBufferImage->updateToCurrent(m_commandBuffer.data(), m_commandBuffer.size() * sizeof(float));
        auto job = new VertexSubmitJob([res]()
        {
            res->waitFor();
        });
        m_ctx->submit(job,m_uploadPassName);
        job->release();
    }
    void VertexContext::submitPath()
    {
        m_commandBuffer.clear();

        if (m_screens.empty()) {
            return;
        }

        m_mesh->clear();

        for (size_t screenIndex = 0; screenIndex < m_screens.size(); screenIndex++) {
            const auto& screen = m_screens[screenIndex];
            const auto& paths = screen.getPaths();

            for (const auto& path : paths) {
                const auto& commands = path->getCommandQueue();
                if (commands.empty()) {
                    continue;
                }

                uint32_t commandOffset = m_commandBuffer.size();
                uint32_t commandSize = commands.size();

                //m_commandBuffer.push_back(VertexCommand_SetTransform);
                //for (int i = 0; i < 16; i++) {
                //    m_commandBuffer.push_back(path->transform.m[i]);
                //}
                m_commandBuffer.insert(m_commandBuffer.end(), commands.begin(), commands.end());

                Vec4 bounds = path->getBoundingRect();

                addRectangle(screenIndex, commandOffset, commandSize, bounds);
            }
        }

        m_mesh->update();
    }
    void VertexContext::addRectangle(uint32_t screenId, uint32_t commandOffset, uint32_t commandSize, const Vec4& bounds)
    {
        float left = bounds.x;
        float top = bounds.y;
        float right = bounds.x + bounds.z;
        float bottom = bounds.y + bounds.w;

        uint32_t baseIndex = m_mesh->getVertexCount();

        m_mesh->addVertex(Vec2(left, top), commandOffset, commandSize, screenId);

        m_mesh->addVertex(Vec2(right, top), commandOffset, commandSize, screenId);

        m_mesh->addVertex(Vec2(left, bottom), commandOffset, commandSize, screenId);

        m_mesh->addVertex(Vec2(right, bottom), commandOffset, commandSize, screenId);

        m_mesh->addIndex(baseIndex);
        m_mesh->addIndex(baseIndex + 1);
        m_mesh->addIndex(baseIndex + 2);

        m_mesh->addIndex(baseIndex + 1);
        m_mesh->addIndex(baseIndex + 3);
        m_mesh->addIndex(baseIndex + 2);
    }
    void VertexContext::create()
    {
        m_pso = new TraditionPipelineState();
        m_vs = m_ctx->createVertexShader();
        m_vs->pixelLayout(pixelLayout);
        m_vs->addLayout(0, vertexLayout);
        m_vs->addUniform(m_projectionMatrix.first);
        m_vs->addUniform(m_viewMatrix.first);
        m_vs->resourceLayout(resourceLayout);

        m_ps = m_ctx->createPixelShader();
        m_ps->pixelLayout(pixelLayout);
        m_ps->resourceLayout(resourceLayout);

        generateVertexShader();
        generatePixelShader();

        m_vs->create();
        m_ps->create();

        m_pso->vertexLayout = vertexLayout;
        m_pso->pixelLayout = pixelLayout;
        m_pso->resourceLayout = resourceLayout;
        m_pso->vertexShader = m_vs;
        m_pso->pixelShader = m_ps;

        for (auto& resource : m_passResources)
        {
            resource->create();
        }
        // m_passResource->create();

        m_mesh = new DynamicMesh<uint32_t>(m_ctx,vertexLayout);
        m_mesh->reserveIndices(200);
        m_mesh->reserveVertices(200);
        m_mesh->create();
    }


    void VertexContext::addScreen(const std::string& name)
    {
        size_t index = m_infos.size();
        m_screenIndexMap[name] = index;
        m_infos.emplace_back();
        m_screens.emplace_back(m_infos, index);
    }
    VertexScreen& VertexContext::setScreen(const std::string& name)
    {
        auto it = m_screenIndexMap.find(name);
        if (it == m_screenIndexMap.end()) {
            addScreen(name);
            return m_screens.back();
        }
        return m_screens[it->second];
    }
    void VertexContext::mvpUniformLayout(ConstLayout layout)
    {
        auto viewMatrix = layout.getElement(ConstType::ViewMatrix);
        if (viewMatrix)
        {
            m_viewMatrix = { layout, viewMatrix };
        }
        auto projectionMatrix = layout.getElement(ConstType::ProjectionMatrix);
        if (projectionMatrix)
        {
            m_projectionMatrix = { layout, projectionMatrix };
        }
        //todo:支持 vp合一矩阵 支持mvp合一 矩阵
    }

    /*
    struct VertexScreenInfo {
        Vec4 origin;
        Vec4 XDir;
        Vec4 YDir;
        Vec4 originVertexCoord;
        Vec4 VertexCoordSize;
    };
     */

    void VertexContext::generateVertexShader()
    {
        std::string code;
        code += "\n#define vertexOuputTarget " + std::string(ScreenInfoNameInShder);
        code += R"(



ShaderOut main(ShaderIn vsIn) {
    ShaderOut vsOut;

    uint screenId = uint(vsIn.screenId);

    uint baseIndex = screenId * 5;

    float4 origin = ScreenInfo.Load(int3(baseIndex, 0, 0));
    float4 XDir = ScreenInfo.Load(int3(baseIndex + 1, 0, 0));
    float4 YDir = ScreenInfo.Load(int3(baseIndex + 2, 0, 0));
    float4 originVertexCoord = ScreenInfo.Load(int3(baseIndex + 3, 0, 0));
    float4 VertexCoordSize = ScreenInfo.Load(int3(baseIndex + 4, 0, 0));

    float2 vertexCoord = vsIn.vertexcoord;

    float3 worldPos = origin.xyz +
                     XDir.xyz * (vertexCoord.x - originVertexCoord.x) / VertexCoordSize.x +
                     YDir.xyz * (vertexCoord.y - originVertexCoord.y) / VertexCoordSize.y;

    float4 position = float4(worldPos, 1.0);
)";

        if (m_viewMatrix.second && m_projectionMatrix.second) {
            std::string viewMatrixName = m_viewMatrix.second.getName();
            std::string projMatrixName = m_projectionMatrix.second.getName();

            code += "    float4 viewPos = mul(" + viewMatrixName + ", position);\n";
            code += "    vsOut.pos = mul(" + projMatrixName + ", viewPos);\n";
        } else if (m_viewMatrix.second) {
            std::string viewMatrixName = m_viewMatrix.second.getName();

            code += "    vsOut.pos = mul(" + viewMatrixName + ", position);\n";
        } else if (m_projectionMatrix.second) {
            std::string projMatrixName = m_projectionMatrix.second.getName();

            code += "    vsOut.pos = mul(" + projMatrixName + ", position);\n";
        } else {
            code += "    vsOut.pos = position;\n";
        }

        code += R"(
    vsOut.cmdoffset = vsIn.cmdoffset;
    vsOut.cmdSize = vsIn.cmdSize;
    vsOut.screenId = vsIn.screenId;
    vsOut.vertexcoord = vsIn.vertexcoord;

    return vsOut;
}
)";

        m_vs->code(code);
    }

    void VertexContext::generatePixelShader()
    {
        std::string code;
        code += "\n#define VertexCommandBuffer " + std::string(VertexCommandBufferNameInShder);
        code += "\n#define vertexOuputTarget target" + std::to_string(m_targetIndex);
        code += R"(


const float VertexCommand_End = -1.0;
const float VertexCommand_MoveTo = 1.0;
const float VertexCommand_LineTo = 2.0;
const float VertexCommand_BezierCurveTo = 3.0;
const float VertexCommand_SetColor = 4.0;
const float VertexCommand_ArcTo = 6.0;
const float VertexCommand_SetTransform = 5.0;
const float VertexCommand_BeginPath = 7.0;
const float VertexCommand_EndPath = 8.0;

int GetCurveRootFlags(float v0, float v1, float v2) {
    int shift = ((v0 > 0.0) ? 2 : 0) + ((v1 > 0.0) ? 4 : 0) + ((v2 > 0.0) ? 8 : 0);
    return (0x2E74 >> shift) & 0x03;
}

float2 QuadCurveSolveXAxis(float2 v0, float2 v1, float2 v2) {
    float2 a = v0 - 2.0 * v1 + v2;
    float2 b = v0 - v1;
    float c = v0.y;
    float ra = 1.0 / a.y;
    float rb = 0.5 / b.y;
    float delta = sqrt(max(b.y * b.y - a.y * c, 0.0));
    float2 t = float2((b.y - delta) * ra, (b.y + delta) * ra);
    if (abs(a.y) < 0.0001220703125) t = float2(c * rb, c * rb);
    return (a.x * t - b.x * 2.0) * t + v0.x;
}

float2 QuadCurveSolveYAxis(float2 v0, float2 v1, float2 v2) {
    float2 a = v0 - 2.0 * v1 + v2;
    float2 b = v0 - v1;
    float c = v0.x;
    float ra = 1.0 / a.x;
    float rb = 0.5 / b.x;
    float delta = sqrt(max(b.x * b.x - a.x * c, 0.0));
    float2 t = float2((b.x - delta) * ra, (b.x + delta) * ra);
    if (abs(a.x) < 0.0001220703125) t = float2(c * rb, c * rb);
    return (a.y * t - b.y * 2.0) * t + v0.y;
}

float CurveTestXAxis(float2 v0, float2 v1, float2 v2, float2 pixelsPerUnit) {
    if (max(max(v0.x, v1.x), v2.x) * pixelsPerUnit.x < -0.5) return 0.0;
    int flags = GetCurveRootFlags(v0.y, v1.y, v2.y);
    if (flags == 0) return 0.0;
    float2 x1x2 = QuadCurveSolveXAxis(v0, v1, v2) * pixelsPerUnit.x;
    float ret = 0.0;
    if ((flags & 1) != 0) {
        ret += clamp(x1x2.x + 0.5, 0.0, 1.0);
    }
    if ((flags & 2) != 0) {
        ret -= clamp(x1x2.y + 0.5, 0.0, 1.0);
    }
    return ret;
}

float CurveTestYAxis(float2 v0, float2 v1, float2 v2, float2 pixelsPerUnit) {
    if (max(max(v0.y, v1.y), v2.y) * pixelsPerUnit.y < -0.5) return 0.0;
    int flags = GetCurveRootFlags(v0.x, v1.x, v2.x);
    if (flags == 0) return 0.0;
    float2 y1y2 = QuadCurveSolveYAxis(v0, v1, v2) * pixelsPerUnit.y;
    float ret = 0.0;
    if ((flags & 0x01) != 0) {
        ret -= clamp(y1y2.x + 0.5, 0.0, 1.0);
    }
    if ((flags & 0x02) != 0) {
        ret += clamp(y1y2.y + 0.5, 0.0, 1.0);
    }
    return ret;
}

float2 CurveTest(float2 v0, float2 v1, float2 v2, float2 pixelsPerUnit) {
    return float2(
        CurveTestXAxis(v0, v1, v2, pixelsPerUnit),
        CurveTestYAxis(v0, v1, v2, pixelsPerUnit)
    );
}

float2 LineTest(float2 v0, float2 v1, float2 pixelsPerUnit) {
    float2 result = float2(0.0, 0.0);

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

float2 ArcTest(float2 pos, float2 bp, float2 ep, float2 c, float b, float e, float r, float q, float2 pixelsPerUnit) {
    float2 ret = float2(0.0, 0.0);
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

float NoZeroSign(float x) {
    return x >= 0.0 ? 1.0 : -1.0;
}

float2 applyTransform(float2 needTransformPoint, float3x3 transform) {
    float3 transformedPoint = mul(float3(needTransformPoint, 1.0), transform);
    return transformedPoint.xy / transformedPoint.z;
}

float fetchCommand(int commandIndex) {
    int pixelIndex = commandIndex / 4;
    int channelIndex = commandIndex % 4;

    int x = pixelIndex % 4096;
    int y = pixelIndex / 4096;

    float4 pixel = VertexCommandBuffer.Load(int3(x, y, 0));

    if (channelIndex == 0) return pixel.r;
    else if (channelIndex == 1) return pixel.g;
    else if (channelIndex == 2) return pixel.b;
    else return pixel.a;
}

ShaderOut main(ShaderIn ps_input) {
    float transformSign = 1.0;
    float3x3 transform = float3x3(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);
    ShaderOut ps_output;
    ps_output.vertexOuputTarget = float4(1.0, 1.0, 1.0, 1.0);

    float2 unitsPerPixel = fwidth(ps_input.vertexcoord);
    float2 pixelsPerUnit = 1.0 / unitsPerPixel;

    float2 crossings = float2(0.0, 0.0);

    int i = int(ps_input.cmdoffset);
    float2 lastPos;
    float4 fillColor = float4(1.0, 0.647, 0.0, 1.0);
    float4 currentColor = float4(1.0, 0.647, 0.0, 1.0);
    bool isPathStarted = false;
    float2 pathCrossings = float2(0.0, 0.0);

    float pathOperation = 0.0; // 0: 默认 1:并 2:交 3:补
    int edge = i + int(ps_input.cmdSize);
    while(i < edge) {
        float command = fetchCommand(i);
        i++;
        switch (int(command)) {
         case int(VertexCommand_BeginPath):
{
            isPathStarted = true;
            pathCrossings = float2(0.0, 0.0);
            pathOperation = fetchCommand(i);
            i += 1;
}
            break;
        case int(VertexCommand_EndPath):
{
            isPathStarted = false;
            if (length(pathCrossings) > 0.0) {
                fillColor = currentColor;
            }
            if (pathOperation == 0.0) {
                crossings += pathCrossings;
            } else if (pathOperation == 1.0) {
                crossings = float2(
                    NoZeroSign(crossings.x) * NoZeroSign(pathCrossings.x) * max(abs(crossings.x), abs(pathCrossings.x)),
                    NoZeroSign(crossings.y) * NoZeroSign(pathCrossings.y) * max(abs(crossings.y), abs(pathCrossings.y))
                );
            } else if (pathOperation == 2.0) {
                crossings = float2(
                    sign(crossings.x) * sign(pathCrossings.x) * min(abs(crossings.x), abs(pathCrossings.x)),
                    sign(crossings.y) * sign(pathCrossings.y) * min(abs(crossings.y), abs(pathCrossings.y))
                );
            } else if (pathOperation == 3.0) {
                crossings += pathCrossings;
                crossings = float2(
                    NoZeroSign(crossings.x) * (1.0 - abs(crossings.x)),
                    NoZeroSign(crossings.y) * (1.0 - abs(crossings.y))
                );
            }
            pathCrossings = float2(0, 0);
            pathOperation = 0.0;
}
            break;
        case int(VertexCommand_MoveTo):
{
            lastPos.x = fetchCommand(i);
            lastPos.y = fetchCommand(i+1);
            lastPos = applyTransform(lastPos, transform);
            i += 2;
}
            break;
        case int(VertexCommand_LineTo):
{
            float2 to;
            to.x = fetchCommand(i);
            to.y = fetchCommand(i+1);
            to = applyTransform(to, transform);
            i += 2;

            float2 v0 = lastPos - ps_input.vertexcoord;
            float2 v1 = to - ps_input.vertexcoord;
            pathCrossings += transformSign * LineTest(v0, v1, pixelsPerUnit);
            lastPos = to;
}
            break;
        case int(VertexCommand_SetColor):
{
            currentColor.r = fetchCommand(i);
            currentColor.g = fetchCommand(i+1);
            currentColor.b = fetchCommand(i+2);
            currentColor.a = fetchCommand(i+3);
            i += 4;
}
            break;
        case int(VertexCommand_ArcTo):
{
            float2 bp = lastPos;
            float2 c;
            c.x = fetchCommand(i);
            c.y = fetchCommand(i+1);
            c = applyTransform(c, transform);
            float r = length(c - bp);
            float b = fetchCommand(i+2);
            float e = fetchCommand(i+3);
            float q = fetchCommand(i+4);
            float2 ep = c + float2(r * cos(e), r * sin(e));
            pathCrossings += transformSign * ArcTest(ps_input.vertexcoord, bp, ep, c, b, e, r, q, pixelsPerUnit);
            lastPos = ep;
            i += 5;
}
            break;
        case int(VertexCommand_BezierCurveTo):
{
            float2 control = float2(fetchCommand(i),
                                fetchCommand(i+1));
            float2 end = float2(fetchCommand(i+2),
                            fetchCommand(i+3));
            control = applyTransform(control, transform);
            end = applyTransform(end, transform);
            i += 4;

            float2 v0 = lastPos - ps_input.vertexcoord;
            float2 v1 = control - ps_input.vertexcoord;
            float2 v2 = end - ps_input.vertexcoord;
            pathCrossings += transformSign * CurveTest(v0, v1, v2, pixelsPerUnit);
            lastPos = end;
}
            break;
        case int(VertexCommand_SetTransform):
{
            transform = float3x3(
                fetchCommand(i), fetchCommand(i+1), fetchCommand(i+2),
                fetchCommand(i+3), fetchCommand(i+4), fetchCommand(i+5),
                fetchCommand(i+6), fetchCommand(i+7), fetchCommand(i+8)
            );
            i += 16;

            float det = determinant(transform);
			transformSign = NoZeroSign(det);
}
            break;
        }
        if (command == VertexCommand_End) break;
    }

    float weightX = 1.0 - abs(crossings.x * 2.0 - 1.0);
    float weightY = 1.0 - abs(crossings.y * 2.0 - 1.0);
    float coverage = max(abs(crossings.x * weightX + crossings.y * weightY) / max(weightX + weightY, 0.0001220703125), min(abs(crossings.x), abs(crossings.y)));


    ps_output.vertexOuputTarget = fillColor;
    ps_output.vertexOuputTarget.a *= coverage;

   //ps_output.vertexOuputTarget.r = crossings.x;
   //ps_output.vertexOuputTarget.g = crossings.y;
   //ps_output.vertexOuputTarget.b = 0.0;
   //ps_output.vertexOuputTarget.a = 1.0;
    return ps_output;
}
)";
        m_ps->code(code);
    }
    /*
     *todo:矢量渲染
     *1.提供屏幕设置,屏幕是空间 四边形
     *  需要描述一个空间四边形的常量缓冲 趣
     *  需要描述符空间四边形的四个顶点的vertexCoord
     *  需要vertexCoord
     *2.传进去的顶点只有vertexCoord,传出的位置通过空间平行四边形计算出来
     *3.传进去的顶点需要指定命令所在缓存区offset
     *3.提供drawPath(Transfomr),提供drawPath(位置)，把位置弄进矩阵，
     *  使用setTransform传递位置，大小
     *4.addScreen("screenName")
     *  draw("screenName",Path,Transform)
     *  draw("screenName",Path,float x,float y)
     *5.setScreen().set().set()
     *6.需要 更新screen 的UniformBuffer
     *7.空间四边形 表示，原点+ halfX+halfY
     *8.使用纹理来进行屏幕数据的传递
     */
}