//
// Created by Administrator on 2025/5/10.
//

#ifndef FCT_AUTOVIEWPORT_H
#define FCT_AUTOVIEWPORT_H
#include "../Context/Job.h"
#include "../Context/Context.h"
namespace FCT
{

    class AutoViewport
    {
    public:
        AutoViewport(Vec2 windowSize, Vec2 viewportSize)
        {
            m_needReviewport = false;
            resize(windowSize.x,windowSize.y);
            m_dstViewportWidth = viewportSize.x;
            m_dstViewPortHeight = viewportSize.y;
            m_viewportOffsetX = 0.0f;
            m_viewportOffsetY = 0.0f;
        }
        void ctx(FCT::Context* ctx)
        {
            this->m_ctx = ctx;
        }
        void resize(int width, int height)
        {
            m_windowWidth = static_cast<float>(width);
            m_windowHeight = static_cast<float>(height);
            m_needReviewport = true;
        }
        void computeViewport()
        {
            float targetAspect = m_dstViewportWidth / m_dstViewPortHeight;
            float windowAspect = m_windowWidth / m_windowHeight;

            if (windowAspect > targetAspect) {
                m_viewportHeight = m_windowHeight;
                m_viewportWidth = m_windowHeight * targetAspect;
                m_viewportOffsetX = (m_windowWidth - m_viewportWidth) / 2.0f;
                m_viewportOffsetY = 0.0f;
            } else {
                m_viewportWidth = m_windowWidth;
                m_viewportHeight = m_windowWidth / targetAspect;
                m_viewportOffsetX = 0.0f;
                m_viewportOffsetY = (m_windowHeight - m_viewportHeight) / 2.0f;
            }
        }
        void submit()
        {
            if (m_needReviewport) {
                computeViewport();
                m_needReviewport = false;
            }
            FCT::ViewportJob* job = new FCT::ViewportJob(
                    FCT::Vec2(m_viewportOffsetX, m_viewportOffsetY), FCT::Vec2(m_viewportOffsetX + m_viewportWidth, m_viewportOffsetY + m_viewportHeight),
            FCT::Vec2(m_viewportOffsetX, m_viewportOffsetY), FCT::Vec2(m_viewportOffsetX + m_viewportWidth, m_viewportOffsetY + m_viewportHeight)
            );
            for (const auto& name : m_passes)
            {
                m_ctx->submit(job, name);
            }
            job->release();
        }
        void addPass(const std::string& name)
        {
            m_passes.push_back(name);
        }
        bool wndPosToViewportPos(Vec2 wndPos,Vec2& viewportPos) const
        {
            Vec2 srcViewportPos(0.0f, 0.0f);
            srcViewportPos.x = (wndPos.x - m_viewportOffsetX) / m_viewportWidth;
            srcViewportPos.y = (wndPos.y - m_viewportOffsetY) / m_viewportHeight;
            viewportPos = srcViewportPos * Vec2(m_dstViewportWidth, m_dstViewPortHeight);
            return srcViewportPos.x >= 0.0f && srcViewportPos.x <= 1.0f && srcViewportPos.y >= 0.0f && srcViewportPos.y <= 1.0f;
        }
    private:
        bool m_needReviewport;
        float m_dstViewportWidth, m_dstViewPortHeight;
        float m_windowWidth, m_windowHeight;
        float m_viewportWidth, m_viewportHeight;
        float m_viewportOffsetX, m_viewportOffsetY;
        FCT::Context* m_ctx;
        std::vector<std::string> m_passes;
    };
}
#endif //AUTOREVIEWPORT_H
