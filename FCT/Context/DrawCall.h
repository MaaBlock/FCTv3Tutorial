#pragma once
#include "../RHI/IPipelineResource.h"

namespace FCT {
	enum class PrimitiveType {
		Lines,
		Triangles,
	};

	class DrawCall : public IPipelineResource {
	public:
		DrawCall(PrimitiveType primitiveType,
			uint32_t startVertex, uint32_t vertexCount)
			: m_primitiveType(primitiveType),
			m_startVertex(startVertex), m_vertexCount(vertexCount) {
		}
		PipelineResourceType getType() const override {
			return PipelineResourceType::DrawCall;
		}

		virtual ~DrawCall() = default;
		void setCount(uint32_t count){
			m_vertexCount = count;
		}
		void primitive(PrimitiveType primitive) {
			m_primitiveType = primitive;
		}

	protected:
		PrimitiveType m_primitiveType;
		uint32_t m_startVertex;
		uint32_t m_vertexCount;

	};
}