//
// Created by Administrator on 2025/5/12.
//
#ifndef FCT_CEGUIRENDERTARGET_H
#define FCT_CEGUIRENDERTARGET_H
#include "../ThirdParty.h"
namespace FCT {
    class CEGUIRenderTarget : virtual public CEGUI::RenderTarget {
    protected:

    public:
        virtual ~CEGUIRenderTarget();
        void draw(const CEGUI::GeometryBuffer& buffer);
        void draw(const CEGUI::RenderQueue& queue);
        void setArea(const CEGUI::Rectf& area) override;
        const CEGUI::Rectf& getArea() const;
        void activate();
        void deactivate();
        void updateMatrix() const override;
    };
} // FCT

#endif //CEGUIRENDERTARGET_H
