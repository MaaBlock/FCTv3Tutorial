//
// Created by Administrator on 2025/3/23.
//
#include "../ThirdParty.h"
#include "./InputLayout.h"
#ifndef VK_INPUTLAYOUT_H
#define VK_INPUTLAYOUT_H

namespace FCT
{
    class VK_Context;
    class VK_InputLayout : public InputLayout {
    public:
        VK_InputLayout(VK_Context* context);
        ~VK_InputLayout() override;
        void bind() override;
        void unbind() override;
        void create() override;
        std::vector<vk::VertexInputAttributeDescription>& attributeDescriptions() { return m_attributeDescriptions; }
    private:
        VK_Context* m_ctx;
        std::vector<vk::VertexInputAttributeDescription> m_attributeDescriptions;
    };

}


#endif //VK_INPUTLAYOUT_H
