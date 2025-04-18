//
// Created by Administrator on 25-4-18.
//

#ifndef MENUPAGE_H
#define MENUPAGE_H
#include "common.h"
#include "Page.h"
class MenuPage : public Page {
private:
    static constexpr VertexLayout vertexLayout {
        VertexElement{VtxType::Position2f},
        VertexElement{VtxType::Color4f},
        VertexElement{VtxType::TexCoord2f},
        VertexElement{VtxType::Custom,Format::R32_UINT,"id"}
    };
    static constexpr PixelLayout pixelLayout {
        vertexLayout
    };

    static constexpr ResourceLayout resourceLayout {
        TextureElement{"bkTex"},
        TextureElement{"startBtnTex"},
        TextureElement{"quitBtnTex"},
        SamplerElement{"imgSampler"}
    };

    PixelShader* ps;

    RHI::Pass* pass;
    RHI::PassGroup* passGroup;
    RHI::RasterizationPipeline* pipeline;

    RHI::IndexBuffer* index;
    RHI::VertexBuffer* gpuVertex;
    VertexBuffer* cpuVertex;

    Image* startBtnTex;
    Image* quitBtnTex;
    Image* bkgTex;
    RHI::Sampler* sampler;

    PassResource* passResource;

public:
    MenuPage(Context* ctx,Window* wnd) : Page(ctx, wnd) {
        prepareVertex();
        pass = ctx->createPass();
        pass->bindTarget(0,wnd->getCurrentTarget()->targetImage());
        pass->depthStencil(wnd->getCurrentTarget()->depthStencilBuffer());
        passGroup = ctx->createPassGroup();
        passGroup->addPass(pass);
        passGroup->create();

        ps = ctx->createPixelShader();
        ps->pixelLayout(pixelLayout);
        ps->resourceLayout(resourceLayout);
        ps->code(R"(
ShaderOut main(ShaderIn psIn) {
    ShaderOut psOut;
    float4 texColor;

    uint textureId = psIn.id;
    if (textureId == 0) {
        texColor = bkTex.Sample(imgSampler, psIn.texcoord);
    } else if (textureId == 1) {
        texColor = startBtnTex.Sample(imgSampler, psIn.texcoord);
    } else if (textureId == 2) {
        texColor = quitBtnTex.Sample(imgSampler, psIn.texcoord);
    } else {
        texColor = float4(1.0, 1.0, 1.0, 1.0);
    }
    psOut.target0 = texColor * psIn.color;
    return psOut;
}
)");
        ps->create();

        pipeline = ctx->createTraditionPipeline();
        pipeline->pixelLayout(pixelLayout);
        pipeline->vertexLayout(vertexLayout);
        pipeline->resourceLayout(resourceLayout);
        pipeline->addResources(ps);
        pipeline->bindPass(pass);
        pipeline->create();

        startBtnTex = ctx->loadTexture("./Res/start.png");
        quitBtnTex = ctx->loadTexture("./Res/quit.png");
        bkgTex = ctx->loadTexture("./Res/menu.jpg");

        sampler = ctx->createSampler();
        sampler->setAnisotropic();
        sampler->create();

        passResource = ctx->createPassResource();
        passResource->bind(wnd);
        passResource->pipeline(pipeline);
        passResource->addTexture(bkgTex,resourceLayout.findTexture("bkTex"));
        passResource->addTexture(quitBtnTex,resourceLayout.findTexture("quitBtnTex"));
        passResource->addTexture(startBtnTex,resourceLayout.findTexture("startBtnTex"));
        passResource->addSampler(sampler, resourceLayout.findSampler("imgSampler"));
        passResource->create();
    }
    void renderTick(RHI::CommandBuffer *cmdBuf) override {
        passGroup->beginSubmit(cmdBuf);
        cmdBuf->bindPipieline(pipeline);
        passResource->bind(cmdBuf);
        gpuVertex->bind(cmdBuf);
        index->bind(cmdBuf);
        cmdBuf->drawIndex(0,0,18,1);
        passGroup->endSubmit(cmdBuf);

    }
    PageType logicTick(InputState state, float deltaTime) override {
        if (state.leftButtonDown) {
            float viewportX = state.mouseX;
            float viewportY = state.mouseY;

            if (viewportX >= 280 && viewportX <= 520 &&
                viewportY >= 210 && viewportY <= 270) {
                return PageType::Game;
                }

            if (viewportX >= 280 && viewportX <= 520 &&
                viewportY >= 330 && viewportY <= 390) {
                exit(0);
                }
        }
        return PageType::Last;
    }
    void start() override {

    }
    void stop() {

    }
    //240 60

    //400 240
    //400 360
void prepareVertex() {
        cpuVertex = new VertexBuffer(vertexLayout);
        cpuVertex->emplaceBack(
            Vec3(-1.0f, -1.0f),
            Vec4(1.0f, 1.0f, 1.0f, 1.0f),
            Vec2(0.0f, 0.0f),
            0
        );
        cpuVertex->emplaceBack(
            Vec3(1.0f, -1.0f),
            Vec4(1.0f, 1.0f, 1.0f, 1.0f),
            Vec2(1.0f, 0.0f),
            0
        );
        cpuVertex->emplaceBack(
            Vec3(1.0f, 1.0f),
            Vec4(1.0f, 1.0f, 1.0f, 1.0f),
            Vec2(1.0f, 1.0f),
            0
        );
        cpuVertex->emplaceBack(
            Vec3(-1.0f, 1.0f),
            Vec4(1.0f, 1.0f, 1.0f, 1.0f),
            Vec2(0.0f, 1.0f),
            0
        );
        cpuVertex->emplaceBack(
           Vec3(-0.3f, -0.3f),
           Vec4(1.0f, 1.0f, 1.0f, 1.0f),
           Vec2(0.0f, 0.0f),
           1
       );
        cpuVertex->emplaceBack(
            Vec3(0.3f, -0.3f),
            Vec4(1.0f, 1.0f, 1.0f, 1.0f),
            Vec2(1.0f, 0.0f),
            1
        );
        cpuVertex->emplaceBack(
            Vec3(0.3f, -0.1f),
            Vec4(1.0f, 1.0f, 1.0f, 1.0f),
            Vec2(1.0f, 1.0f),
            1
        );
        cpuVertex->emplaceBack(
            Vec3(-0.3f, -0.1f),
            Vec4(1.0f, 1.0f, 1.0f, 1.0f),
            Vec2(0.0f, 1.0f),
            1
        );


        cpuVertex->emplaceBack(
            Vec3(-0.3f, 0.1f),
            Vec4(1.0f, 1.0f, 1.0f, 1.0f),
            Vec2(0.0f, 0.0f),
            2
        );
        cpuVertex->emplaceBack(
            Vec3(0.3f, 0.1f),
            Vec4(1.0f, 1.0f, 1.0f, 1.0f),
            Vec2(1.0f, 0.0f),
            2
        );
        cpuVertex->emplaceBack(
            Vec3(0.3f, 0.3f),
            Vec4(1.0f, 1.0f, 1.0f, 1.0f),
            Vec2(1.0f, 1.0f),
            2
        );
        cpuVertex->emplaceBack(
            Vec3(-0.3f, 0.3f),
            Vec4(1.0f, 1.0f, 1.0f, 1.0f),
            Vec2(0.0f, 1.0f),
            2
        );


        std::vector<uint16_t> indices = {
            0, 1, 2,
            0, 2, 3,

            4, 5, 6,
            4, 6, 7,

            8, 9, 10,
            8, 10, 11
        };
        index = ctx->createIndexBuffer();
        index->indexBuffer(indices);
        index->create();
        index->updataBuffer();
        gpuVertex = ctx->createVertexBuffer();
        gpuVertex->vertexBuffer(cpuVertex);
        gpuVertex->create();
        gpuVertex->updataBuffer();
    }
};
#endif //MENUPAGE_H
