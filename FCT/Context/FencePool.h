//
// Created by Administrator on 2025/5/10.
//

#ifndef FCT_FENCEPOOL_H
#define FCT_FENCEPOOL_H
namespace FCT
{
    class FencePool
    {
    public:
        virtual ~FencePool() = default;
        virtual RHI::Fence* alloc() = 0;
        virtual void setDestroyCallback(const std::function<void(RHI::Fence*)>& callback) = 0;
    };
}


#endif //FENCEPOOL_H
