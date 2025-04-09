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
    namespace RHI
    {
        constexpr vk::VertexInputRate ToVkVertexInputRate(InputRate rate)
        {
            switch (rate)
            {
                case InputRate::PerVertex:
                    return vk::VertexInputRate::eVertex;
                case InputRate::PerInstance:
                    return vk::VertexInputRate::eInstance;
                default:
                    return vk::VertexInputRate::eVertex;
            }
        }
        class VK_InputLayout : public InputLayout {
        public:

            VK_InputLayout(VK_Context* context);
            ~VK_InputLayout() override {};
            void create() override;
            vk::PipelineVertexInputStateCreateInfo inputStateInfo() const { return m_vertexInputStateInfo; }
            vk::PipelineVertexInputStateCreateInfo* pInputStateInfo() { return &m_vertexInputStateInfo; }
        private:
            VK_Context* m_ctx;
            std::vector<vk::VertexInputBindingDescription> m_bindingDescriptions;
            std::vector<vk::VertexInputAttributeDescription> m_attributeDescriptions;
            vk::PipelineVertexInputStateCreateInfo m_vertexInputStateInfo;
        };

    }
}


#endif //VK_INPUTLAYOUT_H
