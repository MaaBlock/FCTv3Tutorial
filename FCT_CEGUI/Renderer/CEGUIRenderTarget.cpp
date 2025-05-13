//
// Created by Administrator on 2025/5/12.
//

#include "CEGUIRenderTarget.h"

namespace FCT {
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
    }

    void CEGUIRenderTarget::activate()
    {
        RenderTarget::activate();
    }

    void CEGUIRenderTarget::deactivate()
    {
        RenderTarget::deactivate();
    }

    void CEGUIRenderTarget::updateMatrix() const
    {
    }
} // FCT