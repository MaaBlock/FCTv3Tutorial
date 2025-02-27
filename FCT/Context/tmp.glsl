#version 320 es

precision highp float;
precision highp int;
layout(binding = 3) uniform sampler2D mainTexture;

struct PixelInput {
    vec3 position;
    vec2 vectorCoord;
};

struct PixelOutput {
    vec3 position;
    vec2 vectorCoord;
    vec4 color;
};

in vec3 vs2fs_position;
in vec2 vs2fs_vectorCoord;

out vec4 FragColor;

PixelOutput fct_user_main(PixelInput);

void main() {
    PixelInput ps_input;
    ps_input.position = vs2fs_position;
    ps_input.vectorCoord = vs2fs_vectorCoord;
    PixelOutput ps_output = fct_user_main(ps_input);
    FragColor = ps_output.color;
}


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
        int signX = (v0.y > 0.0 ? 1.0 : 0.0) - (v1.y > 0.0 ? 1.0 : 0.0);
        if (signX != 0) {
            float xt = (v1.y * v0.x - v0.y * v1.x) / (v1.y - v0.y);
            result.x = float(signX) * clamp(xt * pixelsPerUnit.x + 0.5, 0.0, 1.0);
        }
    }

    if (max(v0.y, v1.y) * pixelsPerUnit.y >= -0.5) {
        int signY = (v1.x > 0.0 ? 1.0 : 0.0) - (v0.x > 0.0 ? 1.0 : 0.0);
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

PixelOutput fct_user_main(PixelInput ps_input) {
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

        float pathOperation = 0.0; // 0: 默认 1:并 2:交 3:补
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
                        if (pathOperation == 0) {
                crossings += pathCrossings;
                } else if (pathOperation == 1) {
                                crossings = vec2(
                        NoZeroSign(crossings.x) * NoZeroSign(pathCrossings.x) * max(abs(crossings.x), abs(pathCrossings.x)),
                        NoZeroSign(crossings.y) * NoZeroSign(pathCrossings.y) * max(abs(crossings.y), abs(pathCrossings.y))
                );
                } else if (pathOperation == 2) {
                                crossings = vec2(
                        sign(crossings.x) * sign(pathCrossings.x) * min(abs(crossings.x), abs(pathCrossings.x)),
                        sign(crossings.y) * sign(pathCrossings.y) * min(abs(crossings.y), abs(pathCrossings.y))
                );
                } else if (pathOperation == 3) {
                            crossings += pathCrossings;
                                crossings = vec2(
                                        NoZeroSign(crossings.x) * (1 - abs(crossings.x)),
                                        NoZeroSign(crossings.y) * (1 - abs(crossings.y))
                );
                        }
                        pathCrossings = vec2(0);
                        pathOperation = 0;
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
                        //      crossings += LineTest(v0, v1, pixelsPerUnit);
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
                        //      crossings += ArcTest(ps_input.vectorCoord, bp, ep, c, b, e, r, q, pixelsPerUnit);
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
            //  crossings += CurveTest(v0, v1, v2, pixelsPerUnit);
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