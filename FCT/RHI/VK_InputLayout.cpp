//
// Created by Administrator on 2025/3/23.
//

#include "../FCTAPI.h"
namespace FCT
{
    namespace RHI
    {/*
        void VK_InputLayout::create()
        {

            m_attributeDecriptions.clear();
            m_bindingDescriptions.clear();

            // 为每个绑定点创建一个绑定描述
            for (const auto& [binding, layout] : m_vertexLayouts) {
                vk::VertexInputBindingDescription bindingDescription;
                bindingDescription.binding = binding;
                bindingDescription.stride = static_cast<uint32_t>(layout.getStride());
                bindingDescription.inputRate = vk::VertexInputRate::eVertex; // 默认使用顶点输入速率

                m_bindingDescriptions.push_back(bindingDescription);

                // 为当前绑定点的每个元素创建属性描述
                for (size_t i = 0; i < layout.getElementCount(); i++) {
                    const auto& element = layout.getElement(i);

                    vk::VertexInputAttributeDescription attributeDescription;
                    attributeDescription.binding = binding; // 使用当前绑定点
                    attributeDescription.location = static_cast<uint32_t>(m_nextLocation++); // 使用全局递增的location
                    attributeDescription.format = ToVkFormat(element.getFormat());
                    attributeDescription.offset = static_cast<uint32_t>(layout.getElementOffset(i));

                    m_attributeDecriptions.push_back(attributeDescription);
                }
            }

            // 更新顶点输入状态信息
            m_vertexInputInfo = vk::PipelineVertexInputStateCreateInfo(
                vk::PipelineVertexInputStateCreateFlags(),
                static_cast<uint32_t>(m_bindingDescriptions.size()),
                m_bindingDescriptions.data(),
                static_cast<uint32_t>(m_attributeDecriptions.size()),
                m_attributeDecriptions.data()
            );
        }
        */
    }
}
