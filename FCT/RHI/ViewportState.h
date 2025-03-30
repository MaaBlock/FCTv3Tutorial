//
// Created by Administrator on 2025/3/23.
//
#include "../MutilThreadBase/RefCount.h"
#include "./IPipelineResource.h"
#include "../Context/DataTypes.h"
#ifndef VIEWPORT_H
#define VIEWPORT_H
namespace FCT
{
    class ViewportState;
    class ViewportStateBehaviour
    {
    public:
        virtual ~ViewportStateBehaviour() = default;
        virtual void viewport(Vec2 pos,Vec2 size) = 0;
        virtual void depthRange(float minDepth, float maxDepth) = 0;
        virtual void scissor(Vec2 pos,Vec2 size) = 0;
    protected:
        ViewportState* m_state;

    };
    class SetParamViewportStateBehaviour : public ViewportStateBehaviour
    {
    public:
        SetParamViewportStateBehaviour(ViewportState* state)
        {
            m_state = state;
        }
        void viewport(Vec2 pos,Vec2 size) override;
        void depthRange(float minDepth, float maxDepth) override;
        void scissor(Vec2 pos, Vec2 size) override;
    private:

    };

    class ViewportState : public IPipelineResource
    {
    protected:
        ViewportStateBehaviour* m_behaviour;
    public:
        friend class SetParamViewportStateBehaviour;
        PipelineResourceType getType() const override
        {
            return PipelineResourceType::ViewportState;
        }
        ViewportState()
        {
            m_behaviour = new SetParamViewportStateBehaviour(this);
            depthRange(0.0f, 1.0f);
        }
        virtual ~ViewportState()
        {
            if (m_behaviour)
            {
                delete m_behaviour;
                m_behaviour = nullptr;
            }
        }
        virtual void create() = 0;
        void viewport(Vec2 pos,Vec2 size)
        {
            m_behaviour->viewport(pos, size);
        }
        void depthRange(float minDepth, float maxDepth)
        {
            m_behaviour->depthRange(minDepth, maxDepth);
        }
        void scissor(Vec2 pos,Vec2 size)
        {
            m_behaviour->scissor(pos, size);
        }
    protected:
        Vec2 m_viewportPosition;
        Vec2 m_viewportSize;
        float m_minDepth;
        float m_maxDepth;
        Vec2 m_scissorPosition;
        Vec2 m_scissorSize;
    };

    inline void SetParamViewportStateBehaviour::viewport(Vec2 pos, Vec2 size)
    {
        m_state->m_viewportPosition = pos;
        m_state->m_viewportSize = size;
    }
    inline void SetParamViewportStateBehaviour::depthRange(float minDepth, float maxDepth)
    {
        m_state->m_minDepth = minDepth;
        m_state->m_maxDepth = maxDepth;
    }
    inline void SetParamViewportStateBehaviour::scissor(Vec2 pos, Vec2 size)
    {
        m_state->m_scissorPosition = pos;
        m_state->m_scissorSize = size;
    }
}
#endif //VIEWPORT_H
