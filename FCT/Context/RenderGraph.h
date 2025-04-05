//
// Created by Administrator on 2025/4/5.
//

#ifndef RENDERGRAPH_H
#define RENDERGRAPH_H


namespace FCT
{
    class RenderGraph {
    public:
        RenderGraph(Context* ctx);
        /*
         *cpu可以领先gpu的最大帧数，
         *注意，需在creeate前设置，create后设置无效
         **/
        void frameInFlight(int frames);
        /*
         *添加窗口
         *注意，需在creeate前设置，create后设置无效
         **/
        void addWindows(Window* wnd);
        void create();
    private:
        std::vector<Window*> m_windows;
        std::map<Window*,size_t> m_frameIndex;
        std::map<Window*,std::vector<RHI::Semaphore*>> m_imageAvailableSemaphores;
        Context* m_ctx;
        int m_maxFrameInFlight;
        bool m_inited;
    };

}



#endif //RENDERGRAPH_H
