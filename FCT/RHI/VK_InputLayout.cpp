//
// Created by Administrator on 2025/3/23.
//

#include "../FCTAPI.h"
namespace FCT
{
    namespace RHI
    {
        VK_InputLayout::VK_InputLayout(VK_Context* context)
        {
            m_ctx = context;
        }

        void VK_InputLayout::create()
        {
            m_bindingDescriptions.clear();
            m_attributeDescriptions.clear();

            for (const auto& [slot, layout] : m_vertexLayouts) {
                uint32_t stride = static_cast<uint32_t>(layout.getStride());
                vk::VertexInputRate inputRate = ToVkVertexInputRate(m_inputRates[slot]);

                m_bindingDescriptions.push_back(
                    vk::VertexInputBindingDescription(
                        slot,
                        stride,
                        inputRate
                    )
                );

                for (size_t i = 0; i < layout.getElementCount(); i++) {
                    const VertexElement& element = layout.getElement(i);
                    const char* semantic = element.getSemantic();

                    int32_t location = m_shaderBinary.locationBySemantic(semantic);

                    if (location < 0) {
                        continue;
                    }

                    vk::Format format = ToVkFormat(element.getFormat());

                    uint32_t offset = static_cast<uint32_t>(layout.getElementOffset(i));

                    m_attributeDescriptions.push_back(
                        vk::VertexInputAttributeDescription(
                            static_cast<uint32_t>(location),
                            slot,
                            format,
                            offset
                        )
                    );
                }
            }

            m_vertexInputStateInfo = vk::PipelineVertexInputStateCreateInfo(
                vk::PipelineVertexInputStateCreateFlags(),
                static_cast<uint32_t>(m_bindingDescriptions.size()),
                m_bindingDescriptions.data(),
                static_cast<uint32_t>(m_attributeDescriptions.size()),
                m_attributeDescriptions.data()
            );

        }
    }
}
