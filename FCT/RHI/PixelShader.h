#pragma once
#include <string>
#include "../MutilThreadBase/RefCount.h"
#include "./IPipelineResource.h"

namespace FCT {
    namespace RHI
    {
        class PixelShader : public IPipelineResource
        {
        public:
            PipelineResourceType getType() const override
            {
                return PipelineResourceType::PixelShader;
            }
            virtual ~PixelShader() {}
            virtual void code(const std::vector<char>& code)
            {
                m_code = code;
            }
            virtual void code(std::vector<char>&& code)
            {
                m_code = std::move(code);
            }
            virtual void create() = 0;
        protected:
            std::vector<char> m_code;
        };
    }
} // namespace FCT