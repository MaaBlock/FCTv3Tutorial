//
// Created by Administrator on 2025/5/18.
//

#include "CEGUITextureTarget.h"

namespace FCT {
    CEGUITextureTarget::CEGUITextureTarget(CEGUIRenderer& owner, bool addStencilBuffer) :
        CEGUIRenderTarget(owner),
        TextureTarget(addStencilBuffer)
    {

    }
} // FCT