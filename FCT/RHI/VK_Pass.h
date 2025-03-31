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
            std::map<uint32_t,uint32_t>& targetAttachmentIndices() { return m_targetAttachmentIndices; }
        protected:
            VK_Context* m_ctx;
            vk::SubpassDescription m_desc;
            std::vector<vk::AttachmentReference> m_renderTargetRefs;
            std::map<uint32_t,uint32_t> m_targetAttachmentIndices;//key is slot,value is attachment index
        };
    }
}


#endif //VK_PASS_H
