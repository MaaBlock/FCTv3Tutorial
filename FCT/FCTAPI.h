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
#include "./Context/VK_Swapchain.h"
//#include "./Context/VK_Image.h"
#endif
//#include "./Runtime/FCT_Runtime.h"
#ifdef FCT_ANDROID
#include "./Runtime/Android_Runtime.h"
#endif
#endif //TUTORIAL_FCTAPI_H
