#pragma once
#include "../MutilThreadBase/RefCount.h"
#include "./IPipelineResource.h"
#include "./VertexFactory.h"

namespace FCT {

    class Context;
	class VertexBuffer;
    class InputLayout : public IPipelineResource {
    public:
        virtual ~InputLayout() = default;

        virtual void bind() = 0;
        virtual void unbind() = 0;

        PipelineResourceType getType() const override { return PipelineResourceType::InputLayout; }

        virtual bool create(Context* context, VertexBuffer* buffer = nullptr) = 0;
    protected:
    };

} // namespace FCT
