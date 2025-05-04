#define VertexCommandBuffer
#define vertexOuputTarget
#define vectorcoord vectorcoord

constexpr float VertexCommand_End = -1.0;
constexpr float VertexCommand_MoveTo = 1.0;
constexpr float VertexCommand_LineTo = 2.0;
constexpr float VertexCommand_BezierCurveTo = 3.0;
constexpr float VertexCommand_SetColor = 4.0;
constexpr float VertexCommand_ArcTo = 6.0;
constexpr float VertexCommand_SetTransform = 5.0;
constexpr float VertexCommand_BeginPath = 7.0;
constexpr float VertexCommand_EndPath = 8.0;

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

float3x3 transform = float3x3(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);
float transformSign = 1.0;

float2 applyTransform(float2 point) {
    float3 transformedPoint = mul(float3(point, 1.0), transform);
    return transformedPoint.xy / transformedPoint.z;
}

float fetchCommand(int index) {
    int x = index % 4096;
    int y = index / 4096;
    return VertexCommandBuffer.Load(int3(x, y, 0)).r;
}


ShaderOut main(ShaderIn ps_input) {
    PixelOutput ps_output;
    ps_output.vertexOuputTarget = float4(1.0, 1.0, 1.0, 1.0);

    float2 unitsPerPixel = fwidth(ps_input.vectorcoord);
    float2 pixelsPerUnit = 1.0 / unitsPerPixel;

    float2 crossings = float2(0.0, 0.0);

    int i = int(ps_input.commandOffset);
    float2 lastPos;
    float4 fillColor = float4(1.0, 0.647, 0.0, 1.0);
    float4 currentColor = float4(1.0, 0.647, 0.0, 1.0);
    bool isPathStarted = false;
    float2 pathCrossings = float2(0.0, 0.0);

    float pathOperation = 0.0; // 0: 默认 1:并 2:交 3:补
    int edge = i + int(ps_input.commandSize);
    while(i < edge) {
        float command = fetchCommand(i);
        i++;

        if (command == VertexCommand_End) break;
        if (command == VertexCommand_BeginPath) {
            isPathStarted = true;
            pathCrossings = float2(0.0, 0.0);
            pathOperation = fetchCommand(i);
            i += 1;
        }
        else if (command == VertexCommand_EndPath) {
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
        if (command == VertexCommand_MoveTo) {
            lastPos.x = fetchCommand(i);
            lastPos.y = fetchCommand(i+1);
            lastPos = applyTransform(lastPos);
            i += 2;
        }
        else if (command == VertexCommand_LineTo) {
            float2 to;
            to.x = fetchCommand(i);
            to.y = fetchCommand(i+1);
            to = applyTransform(to);
            i += 2;

            float2 v0 = lastPos - ps_input.vectorcoord;
            float2 v1 = to - ps_input.vectorcoord;
            pathCrossings += transformSign * LineTest(v0, v1, pixelsPerUnit);
            lastPos = to;
        }
        else if (command == VertexCommand_SetColor) {
            currentColor.r = fetchCommand(i);
            currentColor.g = fetchCommand(i+1);
            currentColor.b = fetchCommand(i+2);
            currentColor.a = fetchCommand(i+3);
            i += 4;
        }
        else if (command == VertexCommand_ArcTo) {
            float2 bp = lastPos;
            float2 c;
            c.x = fetchCommand(i);
            c.y = fetchCommand(i+1);
            c = applyTransform(c);
            float r = length(c - bp);
            float b = fetchCommand(i+2);
            float e = fetchCommand(i+3);
            float q = fetchCommand(i+4);
            float2 ep = c + float2(r * cos(e), r * sin(e));
            pathCrossings += transformSign * ArcTest(ps_input.vectorcoord, bp, ep, c, b, e, r, q, pixelsPerUnit);
            lastPos = ep;
            i += 5;
        }
        else if (command == VertexCommand_BezierCurveTo) {
            float2 control = float2(fetchCommand(i),
                                fetchCommand(i+1));
            float2 end = float2(fetchCommand(i+2),
                            fetchCommand(i+3));
            control = applyTransform(control);
            end = applyTransform(end);
            i += 4;

            float2 v0 = lastPos - ps_input.vectorcoord;
            float2 v1 = control - ps_input.vectorcoord;
            float2 v2 = end - ps_input.vectorcoord;
            pathCrossings += transformSign * CurveTest(v0, v1, v2, pixelsPerUnit);
            lastPos = end;
        }
        else if (command == VertexCommand_SetTransform) {
            transform = float3x3(
                fetchCommand(i), fetchCommand(i+1), fetchCommand(i+2),
                fetchCommand(i+3), fetchCommand(i+4), fetchCommand(i+5),
                fetchCommand(i+6), fetchCommand(i+7), fetchCommand(i+8)
            );
            i += 16;

            float det = determinant(transform);
			transformSign = NoZeroSign(det);
        }
    }

    float weightX = 1.0 - abs(crossings.x * 2.0 - 1.0);
    float weightY = 1.0 - abs(crossings.y * 2.0 - 1.0);
    float coverage = max(abs(crossings.x * weightX + crossings.y * weightY) / max(weightX + weightY, 0.0001220703125), min(abs(crossings.x), abs(crossings.y)));

    ps_output.vertexOuputTarget = fillColor;
    ps_output.vertexOuputTarget.a *= coverage;
    return ps_output;
}