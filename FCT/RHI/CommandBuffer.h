//
// Created by Administrator on 2025/3/31.
//
#include "../MutilThreadBase/RefCount.h"
#ifndef COMMANDBUFFER_H
#define COMMANDBUFFER_H
namespace FCT
{
    namespace RHI
    {
        enum class CommandBufferLevel
        {
            Primary,
            Secondary
        };
        class CommandBuffer : public RefCount
        {
        public:
            CommandBuffer()
            {
                level(CommandBufferLevel::Primary);
            }
            void level(CommandBufferLevel level)
            {
                m_level = level;
            }
            virtual void create() = 0;
        protected:
            CommandBufferLevel m_level;
        };
    }
}
#endif //COMMANDBUFFER_H
