//
// Created by Administrator on 2025/4/26.
//
#include "./FCTAPI.h"
namespace FCT
{
    void InitImgui()
    {
        ImGui::CreateContext();

    }
   double ImguiJob::m_lastFrameTime = 0;
   double ImguiJob::m_targetFrameTime = 0;
   bool ImguiJob::m_enableFrameLimiter = true;
}
