//
// Created by Administrator on 2025/5/12.
//

#include "CEGUIRenderTarget.h"

namespace FCT {
    CEGUIRenderTarget::CEGUIRenderTarget(CEGUIRenderer& renderer)
        : m_renderer(renderer)
    {

    }

    CEGUIRenderTarget::~CEGUIRenderTarget()
    {

    }

    void CEGUIRenderTarget::draw(const CEGUI::GeometryBuffer& buffer)
    {
    }

    void CEGUIRenderTarget::draw(const CEGUI::RenderQueue& queue)
    {
    }

    void CEGUIRenderTarget::setArea(const CEGUI::Rectf& area)
    {
        RenderTarget::setArea(area);
    }

    const CEGUI::Rectf& CEGUIRenderTarget::getArea() const
    {
        return RenderTarget::getArea();
    }

    void CEGUIRenderTarget::activate()
    {
        if (!d_matrixValid)
            updateMatrix();

        /*
         *todo:
         *  设置viewport
         *  防止跟之前的viewport搞混搞混
         *  还要 适配autoViewport
         */

        m_renderer.setViewProjectionMatrix(RenderTarget::d_matrix);
        RenderTarget::activate();
    }

    void CEGUIRenderTarget::updateMatrix() const
    {
        RenderTarget::updateMatrix( RenderTarget::createViewProjMatrixForVulkan() );
    }

    CEGUI::Renderer& CEGUIRenderTarget::getOwner()
    {
        return m_renderer;
    }
} // FCT