#pragma once
#include "../MutilThreadBase/RefCount.h"
#include "./IPipelineResource.h"
#include "../Context/VertexFactory.h"
#include "../Context/Format.h"
namespace FCT {
    struct VertexInputElement {
        int slot;
        int location;
        //Todo:对于dx，可以采用 FCT_Location_xx（location数字）
        // 来兼容location
        Format format;
        uint32_t offset;
    };
    class Context;
	class VertexBuffer;
    class InputLayout : public IPipelineResource {
    public:
        virtual ~InputLayout() = default;
        PipelineResourceType getType() const override { return PipelineResourceType::InputLayout; }
        void addElement(const VertexInputElement& element) {
            m_elements.push_back(element);
        }
        void stride(uint32_t stride) {
            m_stride = stride;
        }
        uint32_t stride() const {
            return m_stride;
        }
        virtual void create() = 0;
    protected:
        std::vector<VertexInputElement> m_elements;
        uint32_t m_stride = 0;
    };
} // namespace FCT
