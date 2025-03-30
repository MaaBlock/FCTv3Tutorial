#pragma once
#include "../RHI/IPipelineResource.h"

namespace FCT {

class ConstBuffer : public IPipelineResource {
public:
    ConstBuffer() = default;
    virtual ~ConstBuffer() = default;
    virtual bool create(size_t size, uint32_t bindIndex) = 0;
    virtual void setData(const void* data, size_t size, size_t offset = 0) = 0;
    PipelineResourceType getType() const override { return PipelineResourceType::ConstBuffer; }
    uint32_t getBindIndex() const { return m_bindIndex; }
    size_t getSize() const { return m_size; }

protected:
    uint32_t m_bindIndex;
    size_t m_size;
};

} // namespace FCT