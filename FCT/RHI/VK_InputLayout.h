//
// Created by Administrator on 2025/3/23.
//
#include "../ThirdParty.h"
#include "./InputLayout.h"
#ifndef VK_INPUTLAYOUT_H
#define VK_INPUTLAYOUT_H

namespace FCT
{
    namespace RHI
    {
        class VK_Context;
        class VK_InputLayout : public InputLayout {
        public:
            VK_InputLayout(VK_Context* context);
            ~VK_InputLayout() override;
            void create() override;
        private:
            VK_Context* m_ctx;
            //std::map<uint32_t, vk::VertexInputRate> m_inputRates;
            /*std::vector<vk::VertexInputBindingDescription> m_bindingDescriptions;
            std::vector<vk::VertexInputAttributeDescription> m_attributeDecriptions;
            vk::PipelineVertexInputStateCreateInfo m_vertexInputInfo;
            uint32_t m_nextLocation;*/
        };

    }
}


#endif //VK_INPUTLAYOUT_H
