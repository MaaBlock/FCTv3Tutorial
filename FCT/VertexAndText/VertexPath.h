//
// Created by Administrator on 2025/5/4.
//

#ifndef VERTEXPATH_H
#define VERTEXPATH_H
namespace FCT {


constexpr float VertexCommand_End = -1.0;
constexpr float VertexCommand_MoveTo = 1.0;
constexpr float VertexCommand_LineTo = 2.0;
constexpr float VertexCommand_BezierCurveTo = 3.0;
constexpr float VertexCommand_SetColor = 4.0;
constexpr float VertexCommand_ArcTo = 6.0;
constexpr float VertexCommand_SetTransform = 5.0;
constexpr float VertexCommand_BeginPath = 7.0;
constexpr float VertexCommand_EndPath = 8.0;
class VertexPath : public RefCount
{
protected:
    std::vector<float> m_commandQueue;
    Vec2 m_currentPos;
    Vec2 m_pathStart;
    Vec2 m_minBounds;
    Vec2 m_maxBounds;
    bool m_hasPoints;

    enum CommandType {
    };

    void updateBounds(const Vec2& point) {
        if (!m_hasPoints) {
            m_minBounds = point;
            m_maxBounds = point;
            m_hasPoints = true;
        } else {
            m_minBounds.x = std::min(m_minBounds.x, point.x);
            m_minBounds.y = std::min(m_minBounds.y, point.y);
            m_maxBounds.x = std::max(m_maxBounds.x, point.x);
            m_maxBounds.y = std::max(m_maxBounds.y, point.y);
        }
    }
    void unwrapperArcTo(Vec2 center, float beginAngle, float endAngle, float q) {
        m_commandQueue.push_back(VertexCommand_ArcTo);
        m_commandQueue.push_back(center.x);
        m_commandQueue.push_back(center.y);
        m_commandQueue.push_back(beginAngle);
        m_commandQueue.push_back(endAngle);
        m_commandQueue.push_back(q);

        float radius = m_currentPos.distance(center);
        updateBounds(Vec2(center.x - 2 * radius, center.y - 2 * radius));
        updateBounds(Vec2(center.x + 2 * radius, center.y + 2 * radius));
    }

public:
    Mat3 transform;
    void end()
    {
        m_commandQueue.push_back(VertexCommand_End);
    }
    VertexPath() : m_hasPoints(false) {
        transform = Mat3();
        clear();
    }

    void clear() {
        m_commandQueue.clear();
        m_hasPoints = false;
        m_minBounds = Vec2(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
        m_maxBounds = Vec2(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest());
    }

    void beginPath(float sign = 1.0f) {
        m_commandQueue.push_back(VertexCommand_BeginPath);
        m_commandQueue.push_back(sign);
    }

    void endPath() {
        m_commandQueue.push_back(VertexCommand_EndPath);
    }

    void moveTo(Vec2 to) {
        m_commandQueue.push_back(VertexCommand_MoveTo);
        m_commandQueue.push_back(to.x);
        m_commandQueue.push_back(to.y);
        m_currentPos = to;
        m_pathStart = to;
        updateBounds(to);
    }

    void lineTo(Vec2 to) {
        m_commandQueue.push_back(VertexCommand_LineTo);
        m_commandQueue.push_back(to.x);
        m_commandQueue.push_back(to.y);
        m_currentPos = to;
        updateBounds(to);
    }

    void fillRect(Vec2 pos, Vec2 size) {
        moveTo(pos);
        lineTo(Vec2(pos.x, pos.y + size.y));
        lineTo(Vec2(pos.x + size.x, pos.y + size.y));
        lineTo(Vec2(pos.x + size.x, pos.y));
        lineTo(pos);
    }
    void arcTo(Vec2 center, float beginAngle, float endAngle)
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
                endAngle += 2 * M_PI;

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

    void curveTo(Vec2 control, Vec2 to) {
        m_commandQueue.push_back(VertexCommand_BezierCurveTo);
        m_commandQueue.push_back(control.x);
        m_commandQueue.push_back(control.y);
        m_commandQueue.push_back(to.x);
        m_commandQueue.push_back(to.y);

        updateBounds(m_currentPos);
        updateBounds(control);
        updateBounds(to);

        m_currentPos = to;
    }
    void curveTo(Vec2 control1, Vec2 control2, Vec2 to) {
        Vec2 start = m_currentPos;

        Vec2 q1 = start + (control1 - start) * 2.0f / 3.0f;
        Vec2 q2 = to + (control2 - to) * 2.0f / 3.0f;
        Vec2 m = (q1 + q2) * 0.5f;

        m_commandQueue.push_back(VertexCommand_BezierCurveTo);
        m_commandQueue.push_back(q1.x);
        m_commandQueue.push_back(q1.y);
        m_commandQueue.push_back(m.x);
        m_commandQueue.push_back(m.y);

        m_commandQueue.push_back(VertexCommand_BezierCurveTo);
        m_commandQueue.push_back(q2.x);
        m_commandQueue.push_back(q2.y);
        m_commandQueue.push_back(to.x);
        m_commandQueue.push_back(to.y);

        updateBounds(start);
        updateBounds(control1);
        updateBounds(control2);
        updateBounds(to);

        m_currentPos = to;
    }

    void circle(Vec2 center, float radius) {
        moveTo(Vec2(center.x + radius, center.y));
        arcTo(center, 0, 2 * M_PI);

        updateBounds(Vec2(center.x - 2 * radius, center.y - 2 * radius));
        updateBounds(Vec2(center.x + 2 * radius, center.y + 2 * radius));
    }

    void setColor(Vec4 color) {
        m_commandQueue.push_back(VertexCommand_SetColor);
        m_commandQueue.push_back(color.x);
        m_commandQueue.push_back(color.y);
        m_commandQueue.push_back(color.z);
        m_commandQueue.push_back(color.w);
    }

    void setTransform(const Mat4 &transform) {
        m_commandQueue.push_back(VertexCommand_SetTransform);
        for (int i = 0; i < 16; i++) {
            m_commandQueue.push_back(transform.m[i]);
        }
    }

    void closePath() {
        lineTo(m_pathStart);
    }

    Vec4 getBoundingRect() const {
        if (!m_hasPoints) {
            return Vec4(0, 0, 0, 0);
        }
        return Vec4(m_minBounds.x, m_minBounds.y,
                   m_maxBounds.x - m_minBounds.x,
                   m_maxBounds.y - m_minBounds.y);
    }

    const std::vector<float>& getCommandQueue() const {
        return m_commandQueue;
    }
};
}


#endif //VERTEXPATH_H
