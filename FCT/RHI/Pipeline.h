//
// Created by Administrator on 2025/3/24.
//
#include "../MutilThreadBase/RefCount.h"
#ifndef FCT_PIPELINE_H
#define FCT_PIPELINE_H
namespace FCT
{
    namespace RHI
    {
        class Pipeline : public RefCount
        {
        public:
            virtual void create() = 0;
        protected:

        };
    }
}
#endif //FCT_PIPELINE_H
