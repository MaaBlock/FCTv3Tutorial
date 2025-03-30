//
// Created by Administrator on 2025/3/23.
//

#include "../FCTAPI.h"
namespace FCT
{

    VK_InputLayout::VK_InputLayout(VK_Context* context)
    {
        m_ctx = context;
    }

    VK_InputLayout::~VK_InputLayout()
    {
    }

    void VK_InputLayout::bind()
    {
    }

    void VK_InputLayout::unbind()
    {
    }

    void VK_InputLayout::create()
    {
        m_attributeDescriptions.clear();
        for (const auto& element : m_elements) {
            vk::VertexInputAttributeDescription attributeDescription = {};
            attributeDescription.location = element.location;
            attributeDescription.binding = element.slot;
            attributeDescription.format = ToVkFormat(element.format);
            attributeDescription.offset = element.offset;
            m_attributeDescriptions.push_back(attributeDescription);
        }
    }
}
