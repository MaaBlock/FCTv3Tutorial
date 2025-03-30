//
// Created by Administrator on 2025/3/24.
//
#include "../ThirdParty.h"
#include "./IPipelineResource.h"
#ifndef ROOTSIGN_H
#define ROOTSIGN_H
namespace FCT {
    class RootSign : public IPipelineResource  // 继承 SignToolBase 并重命名为 RootSign
    {
    public:
        PipelineResourceType getType() const override
        {
            return PipelineResourceType::RootSign;
        }
        virtual ~RootSign() = default;
        virtual void create() = 0;
    protected:

    };
}
#endif //ROOTSIGN_H
