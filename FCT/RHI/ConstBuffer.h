#include "../Context/Uniform.h"
#ifndef FCT_RHI_CONSTBUFFER_H
#define FCT_RHI_CONSTBUFFER_H
namespace FCT {
    namespace RHI
    {
        class ConstBuffer
        {
        public:
            void layout(const UniformLayout& layout)
            {
                m_layout = layout;
            }
            void updataData()
            {
                mapData();
            }
            virtual void mapData() = 0;
            virtual void create() = 0;
            void buffer(UniformBuffer* buffer)
            {
                m_buffer = buffer;
            }
            UniformBuffer* getBuffer() const { return m_buffer; }
            UniformLayout layout() const { return m_layout; }
        protected:
            UniformLayout m_layout;
            UniformBuffer* m_buffer;
        };
    }
}
#endif //FCT_RHI_CONSTBUFFER_H
