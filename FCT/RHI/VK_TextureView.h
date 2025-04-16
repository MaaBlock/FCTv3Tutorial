#include "./TextureView.h"
#ifndef VK_TEXTUREVIEW_H
#define VK_TEXTUREVIEW_H
namespace FCT
{
    namespace RHI
    {
        class VK_TextureView : public TextureView {
        public:
            VK_TextureView(VK_Context* ctx);
            ~VK_TextureView();
            void create() override;
            vk::ImageView view() const { return m_view; }
        protected:
            VK_Context* m_ctx;
            vk::ImageView m_view;
        };
    }
}
#endif //VK_TEXTUREVIEW_H
