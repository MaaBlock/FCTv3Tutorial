//
// Created by Administrator on 2025/2/27.
//

#ifndef TUTORIAL_FCTAPI_H
#define TUTORIAL_FCTAPI_H
#include "ThirdParty.h"
#include "headers.h"
#ifdef FCT_ANDROID
#include "./UI/Android_Window.h"
#include "./UI/Android_UICommon.h"
#endif
#ifdef FCT_USE_VULKAN
#include "./Context/VK_ContextCommon.h"
#include "./Context/VK_Context.h"
#include "./Context/VK_VertexShader.h"
#include "./RHI/VK_Swapchain.h"
#include "./Context/VK_RenderPass.h"
#include "./RHI/VK_VertexShader.h"
#include "./RHI/VK_PixelShader.h"
#include "./RHI/VK_InputLayout.h"
#include "./RHI/VK_ViewportState.h"
#include "./RHI/VK_RasterizationState.h"
#include "./RHI/VK_BlendState.h"
#include "./RHI/VK_RasterizationPipeline.h"
#include "./RHI/VK_TraditionalPipeline.h"
#include "./RHI/VK_PassGroup.h"
#include "./RHI/VK_Image.h"
#include "./RHI/VK_RenderTargetView.h"
#include "./RHI/VK_Pass.h"
#include "./RHI/VK_PixelShader.h"
#include "./RHI/VK_VertexShader.h"
#include "./RHI/VK_TraditionalPipeline.h"
#include "./RHI/VK_CommandPool.h"
#include "./RHI/VK_CommandBuffer.h"
#include "./RHI/VK_Semaphore.h"
#include "./RHI/VK_Fence.h"
#include "./RHI/VK_ShaderBinary.h"
#include "./RHI/VK_VertexBuffer.h"
#include "./RHI/VK_IndexBuffer.h"
//#include "./Context/VK_Image.h"
#endif
//#include "./Runtime/FCT_Runtime.h"
#ifdef FCT_ANDROID
#include "./Runtime/Android_Runtime.h"
#endif
#ifdef FCT_USE_GLFW
#include "./UI/GLFW_Window.h"
#include "./UI/GLFW_UICommon.h"
#endif
#ifdef FCT_USE_SHADERC
#include "./Context/Shaderc_ShaderCompiler.h"
#endif

#endif //TUTORIAL_FCTAPI_H
