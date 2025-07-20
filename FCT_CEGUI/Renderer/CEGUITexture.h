//
// Created by Administrator on 2025/5/12.
//

#ifndef CEGUITEXTURE_H
#define CEGUITEXTURE_H
#include "../ThirdParty.h"
#include "./Renderer.h"
namespace FCT
{
    class CEGUITexture : public CEGUI::Texture{
    protected:

    public:
        const CEGUI::String& getName() const override;
        const CEGUI::Sizef& getSize() const override;
        const CEGUI::Sizef& getOriginalDataSize() const override;
        const glm::vec2& getTexelScaling() const override;
        void loadFromFile(const CEGUI::String& filename, const CEGUI::String& resourceGroup) override;
        void loadFromMemory(const void* buffer, const CEGUI::Sizef& buffer_size, PixelFormat pixel_format) override;
        void blitFromMemory(const void* sourceData, const CEGUI::Rectf& area) override;
        void blitToMemory(void* targetData) override;
        bool isPixelFormatSupported(const PixelFormat fmt) const override;
    };
} // FCT

#endif //CEGUITEXTURE_H
