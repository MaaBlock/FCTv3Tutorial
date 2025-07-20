//
// Created by Administrator on 2025/5/15.
//

#ifndef CEGUIWINDOWTARGET_H
#define CEGUIWINDOWTARGET_H
#include "../ThirdParty.h"
#include "./CEGUIRenderTarget.h"
namespace FCT
{
    class CEGUIWindowTarget : public CEGUIRenderTarget
    {
    public:
        CEGUIWindowTarget(CEGUIRenderer& renderer);
        bool isImageryCache() const override;
    protected:

    };

}
#endif //CEGUIWINDOWTARGET_H
