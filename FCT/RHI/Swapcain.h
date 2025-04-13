//
// Created by Administrator on 2025/3/26.
//
#include "Fence.h"
#include "../MutilThreadBase/RefCount.h"
#include "../Context/Format.h"
#include "../Context/ImageRenderTarget.h"
#include "../RHI/Semaphore.h"
#ifndef FCT_SWAPCAIN_H
#define FCT_SWAPCAIN_H
namespace FCT
{
    namespace RHI
    {
        class Swapchain : public RefCount
        {
        public:
            Swapchain()
            {
                m_nativeHandle = nullptr;
                m_width = 0;
                m_height = 0;
            }
            virtual ~Swapchain() = default;
            void nativeHandle(void* handle)
            {
                m_nativeHandle = handle;
            }
            void size(int width,int height)
            {
                m_width = width;
                m_height = height;
            }
            virtual void create() = 0;
            virtual void present() = 0;
            virtual Format getFormat() const = 0;
            virtual Samples getSamples() const = 0;
            virtual void acquireFirstImage() = 0;
            virtual ImageRenderTarget* getCurrentTarget() = 0;
            virtual RHI::Semaphore* getImageAvailableSemaphore() = 0;
            virtual void addRenderFinshSemaphore(RHI::Semaphore* semaphore) = 0;
            virtual void setPresentFinshSemaphore(RHI::Semaphore* semaphore) = 0;
            virtual void needRecreate() = 0;
            virtual void addRenderFinshFence(RHI::Fence* fence)
            {
                m_renderFinshFences.push_back(fence);
            }
        protected:
            std::vector<RHI::Fence*> m_renderFinshFences;
            void* m_nativeHandle;
            uint32_t m_width;
            uint32_t m_height;
        };
    }
}
#endif //FCT_SWAPCAIN_H
