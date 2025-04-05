//
// Created by Administrator on 2025/4/5.
//

#include "../FCTAPI.h"
namespace FCT
{
    RenderGraph::RenderGraph(Context* ctx){
        m_ctx = ctx;
        m_inited = false;
    }

    void RenderGraph::frameInFlight(int frames)
    {
        if (m_inited)
        {
            ferr << "create后设置frameInFlight无效" << std::endl;
            return;
        }
        m_maxFrameInFlight = frames;
    }

    void RenderGraph::addWindows(Window* wnd)
    {
        m_windows.push_back(wnd);
    }

    void RenderGraph::create()
    {
        /*
        m_inited = true;

       for (auto wnd : m_windows)
        {
           m_frameIndex[wnd] = 0;
           for (int i = 0;i < m_maxFrameInFlight;i++)
           {
               m_imageAvailableSemaphores[wnd].push_back(m_ctx->createSemaphore());
           }
           wnd->bind(m_ctx);
           wnd->setPresentFinshSemaphore(m_imageAvailableSemaphores[m_frameIndex[wnd]]);
           wnd->initRender();
        }*/
    }
}
