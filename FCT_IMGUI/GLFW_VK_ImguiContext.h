//
// Created by Administrator on 2025/4/26.
//
#include "../FCT/FCTAPI.h"
#ifndef VK_GLFW_IMGUICONTEXT_H
#define VK_GLFW_IMGUICONTEXT_H
namespace FCT
{
    class GLFW_VK_ImGuiContext
    {
    protected:
        RHI::PassGroup* m_passGroup;
        ImDrawData* drawData[2];
   public:
        GLFW_VK_ImGuiContext(GLFW_Window* wnd,VK_Context* ctx,RHI::PassGroup* passGroup);

        void submitTick(RHI::CommandBuffer* cmdBuffer);
        void logicTick();
    };

    inline GLFW_VK_ImGuiContext::GLFW_VK_ImGuiContext(GLFW_Window* wnd, VK_Context* ctx,RHI::PassGroup* passGroup)
    {
        m_passGroup = passGroup;
        ImGui_ImplGlfw_InitForVulkan(wnd->getWindow(),true);
        ImGui_ImplVulkan_InitInfo init_info{};
        init_info.Instance = ctx->getVkInstance();
        init_info.PhysicalDevice = ctx->getPhysicalDevice();
        init_info.Device = ctx->getDevice();
        init_info.QueueFamily = ctx->getGraphicsQueueFamily();
        init_info.Queue = ctx->getGraphicsQueue();
        init_info.PipelineCache = nullptr;
        init_info.DescriptorPool = static_cast<RHI::VK_DescriptorPool*>(ctx->getDescriptorPool(wnd))->getPool();
        init_info.Allocator = nullptr;
        init_info.MinImageCount = 2;
        init_info.ImageCount = wnd->getSwapchainImageCount();
        init_info.MSAASamples = static_cast<VkSampleCountFlagBits>(ToVkSampleCount(wnd->getSwapchainSampleCount()));
        init_info.CheckVkResultFn = nullptr;
        init_info.RenderPass = static_cast<RHI::VK_PassGroup*>(m_passGroup)->getRenderPass();
        init_info.Subpass = 0;
        ImGui_ImplVulkan_Init(&init_info);
    }


    inline void GLFW_VK_ImGuiContext::submitTick(RHI::CommandBuffer* cmdBuffer)
    {

        ImGui::Render();
        ImDrawData* drawData = ImGui::GetDrawData();

        VkCommandBuffer vkCmdBuffer = static_cast<RHI::VK_CommandBuffer*>(cmdBuffer)->commandBuffer();
        ImGui_ImplVulkan_RenderDrawData(drawData, vkCmdBuffer);
    }

    inline void GLFW_VK_ImGuiContext::logicTick()
    {
    }
}
#endif //VK_GLFW_IMGUICONTEXT_H
