//
// Created by Administrator on 2025/5/18.
//

#ifndef CEGUITEXTURETARGET_H
#define CEGUITEXTURETARGET_H
#include "../ThirdParty.h"
#include "./CEGUIRenderTarget.h"
#include "./CEGUITexture.h"
namespace FCT {

class CEGUITextureTarget : public CEGUIRenderTarget, public CEGUI::TextureTarget{
protected:
    CEGUITexture* m_texture;

public:
    CEGUITextureTarget(CEGUIRenderer& owner, bool addStencilBuffer);
    bool isImageryCache() const override;
    ::CEGUI::Renderer& getOwner() override;
    void updateMatrix() const override;
    void clear() override;
    ::CEGUI::Texture& getTexture() const override;
    void declareRenderSize(const ::CEGUI::Sizef& sz) override;
};
} // FCT

#endif //CEGUITEXTURETARGET_H
