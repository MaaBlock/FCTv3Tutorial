//
// Created by Administrator on 25-4-18.
//
#ifndef PAGE_H
#define PAGE_H
#include "common.h"

class Page {
protected:
    Context* ctx;
    Window* wnd;
public:
    Page(Context* ctx,Window* wnd) : ctx(ctx),wnd(wnd) {}
    virtual ~Page() {}
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void renderTick(RHI::CommandBuffer* cmdBuf) = 0;
    virtual PageType logicTick(InputState state,float deltaTime) = 0;
};
#endif //PAGE_H
