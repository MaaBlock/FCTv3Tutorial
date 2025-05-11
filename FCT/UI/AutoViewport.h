//
// Created by Administrator on 2025/5/10.
//

#ifndef FCT_AUTOVIEWPORT_H
#define FCT_AUTOVIEWPORT_H
namespace FCT
{
    class Context;
    class Window;
    class AutoViewport
    {
    public:
        AutoViewport();
        AutoViewport(Vec2 windowSize, Vec2 viewportSize);
        void ctx(FCT::Context* ctx);
        void resize(int width, int height);
        void computeViewport();
        void submit();
        void addPass(const std::string& name);
        bool wndPosToViewportPos(Vec2 wndPos,Vec2& viewportPos) const;
        void enableForWndAllPass(Window* wnd);
    private:
        bool m_needReviewport;
        float m_dstViewportWidth, m_dstViewPortHeight;
        float m_windowWidth, m_windowHeight;
        float m_viewportWidth, m_viewportHeight;
        float m_viewportOffsetX, m_viewportOffsetY;
        FCT::Context* m_ctx;
        std::vector<std::string> m_passes;
    };
}
#endif //AUTOREVIEWPORT_H
