//
// Created by Administrator on 2025/5/15.
//
#include "../headers.h"

namespace FCT {
    CEGUIWindowTarget::CEGUIWindowTarget(CEGUIRenderer& renderer)
        : CEGUIRenderTarget(renderer)
    {

    }

    bool CEGUIWindowTarget::isImageryCache() const
    {
        return false;
    }
}