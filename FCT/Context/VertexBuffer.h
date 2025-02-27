#pragma once
#include "./Context.h"
#include "./VertexFactory.h"

namespace FCT {
    class Context;
    class VertexBuffer : public IPipelineResource {
    public:
        PipelineResourceType getType() const override;
        virtual ~VertexBuffer() = default;

        virtual void bind() = 0;
        virtual void unbind() = 0;
        virtual bool create(Context* context) = 0;
        virtual void updata() = 0;

        virtual VertexArray* getVertexArray() const = 0;
        virtual size_t getVertexCount() const = 0;
        virtual const VertexFactory* getFactory() const = 0;

    };

} // namespace FCT
