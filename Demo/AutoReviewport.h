//
// Created by Administrator on 2025/5/4.
//

#ifndef AUTOREVIEWPORT_H
#define AUTOREVIEWPORT_H
class AutoReviewport
{
public:
    AutoReviewport()
    {
        needReviewport = false;
        windowWidth = 800.0f;
        windowHeight = 600.0f;
        viewportWidth = 800.0f;
        viewportHeight = 600.0f;
        viewportOffsetX = 0.0f;
        viewportOffsetY = 0.0f;
    }
    void context(FCT::Context* ctx)
    {
        this->ctx = ctx;
    }
    void resize(int width, int height)
    {
        windowWidth = static_cast<float>(width);
        windowHeight = static_cast<float>(height);
        needReviewport = true;
    }
    void computeViewport()
    {
        float targetAspect = 4.0f / 3.0f;
        float windowAspect = windowWidth / windowHeight;

        if (windowAspect > targetAspect) {
            viewportHeight = windowHeight;
            viewportWidth = windowHeight * targetAspect;
            viewportOffsetX = (windowWidth - viewportWidth) / 2.0f;
            viewportOffsetY = 0.0f;
        } else {
            viewportWidth = windowWidth;
            viewportHeight = windowWidth / targetAspect;
            viewportOffsetX = 0.0f;
            viewportOffsetY = (windowHeight - viewportHeight) / 2.0f;
        }
    }
    void submit()
    {
        if (needReviewport) {
            computeViewport();
            needReviewport = false;
        }
        FCT::ViewportJob* job = new FCT::ViewportJob(
                FCT::Vec2(viewportOffsetX, viewportOffsetY), FCT::Vec2(viewportOffsetX + viewportWidth, viewportOffsetY + viewportHeight),
        FCT::Vec2(viewportOffsetX, viewportOffsetY), FCT::Vec2(viewportOffsetX + viewportWidth, viewportOffsetY + viewportHeight)
        );
        for (const auto& name : passes)
        {
            ctx->submit(job, name);
        }
        job->release();
    }
    void addPass(const std::string& name)
    {
        passes.push_back(name);
    }
private:
    bool needReviewport;
    float windowWidth, windowHeight;
    float viewportWidth, viewportHeight;
    float viewportOffsetX, viewportOffsetY;

    FCT::Context* ctx;
    std::vector<std::string> passes;
};

#endif //AUTOREVIEWPORT_H
