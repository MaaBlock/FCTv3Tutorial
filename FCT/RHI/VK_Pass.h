//
// Created by Administrator on 2025/3/28.
//

#ifndef VK_PASS_H
#define VK_PASS_H
#include "./Pass.h"
namespace FCT
{
    class VK_Context;
    namespace RHI
    {
        class VK_Pass : public Pass{
        public:
            VK_Pass(VK_Context* ctx);
            void create(PassGroup* group) override;
            vk::SubpassDescription getDescription() { return m_desc; }
        protected:
            VK_Context* m_ctx;
            vk::SubpassDescription m_desc;
            std::vector<vk::AttachmentReference> m_renderTargetRefs;
        };
    }
}


#endif //VK_PASS_H
