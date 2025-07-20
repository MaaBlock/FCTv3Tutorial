#include "../headers.h"
namespace FCT {
    void CEGUIRenderer::initialiseSamplerStates()
    {
        m_sampler = m_ctx->createSampler();
        m_sampler = m_ctx->createSampler();
        m_sampler->setAddressMode(AddressMode::ClampToEdge, AddressMode::ClampToEdge, AddressMode::ClampToEdge);
        m_sampler->setFilter(FilterMode::Linear, FCT::FilterMode::Linear, FilterMode::Linear);
        m_sampler->setLodRange(0,FLT_MAX,0);
        m_sampler->create();
    }

    void CEGUIRenderer::initialiseBlendStates()
    {
        m_blendStateNormal = m_ctx->createBlendState();
        m_blendStateNormal->blendEnable(true);
        m_blendStateNormal->srcColorBlendFactor(FCT::BlendFactor::SrcAlpha);
        m_blendStateNormal->dstColorBlendFactor(FCT::BlendFactor::OneMinusSrcAlpha);
        m_blendStateNormal->colorBlendOp(FCT::BlendOp::Add);
        m_blendStateNormal->srcAlphaBlendFactor(FCT::BlendFactor::OneMinusDstAlpha);
        m_blendStateNormal->dstAlphaBlendFactor(FCT::BlendFactor::One);
        m_blendStateNormal->alphaBlendOp(FCT::BlendOp::Add);
        m_blendStateNormal->colorWriteMask(0xF); // 相当于D3D11_COLOR_WRITE_ENABLE_ALL
        m_blendStateNormal->create();

        m_blendStatePreMultiplied = m_ctx->createBlendState();
        m_blendStatePreMultiplied->blendEnable(true);
        m_blendStatePreMultiplied->srcColorBlendFactor(FCT::BlendFactor::One);
        m_blendStatePreMultiplied->dstColorBlendFactor(FCT::BlendFactor::OneMinusSrcAlpha);
        m_blendStatePreMultiplied->colorBlendOp(FCT::BlendOp::Add);
        m_blendStatePreMultiplied->srcAlphaBlendFactor(FCT::BlendFactor::One);
        m_blendStatePreMultiplied->dstAlphaBlendFactor(FCT::BlendFactor::OneMinusSrcAlpha);
        m_blendStatePreMultiplied->alphaBlendOp(FCT::BlendOp::Add);
        m_blendStatePreMultiplied->colorWriteMask(0xF); // 相当于D3D11_COLOR_WRITE_ENABLE_ALL
        m_blendStatePreMultiplied->create();
    }

    void CEGUIRenderer::initialiseDepthStencilState()
    {

    }

    CEGUIRenderer& CEGUIRenderer::bootstrapSystem(Context* ctx, const int abi)
    {
        CEGUI::System::performVersionTest(CEGUI_VERSION_ABI, abi, CEGUI_FUNCTION_NAME);
        if (CEGUI::System::getSingletonPtr())
            throw CEGUI::InvalidRequestException(
                "CEGUI::System object is already initialised.");
        CEGUIRenderer& renderer = *(new CEGUIRenderer(ctx));
        CEGUI::System::create(renderer);

        return renderer;
    }

    CEGUIRenderer::CEGUIRenderer(Context* ctx)
        : m_ctx(ctx)
    {
        m_defaultTarget = new CEGUIWindowTarget(*this);
    }

    CEGUI::RenderTarget& CEGUIRenderer::getDefaultRenderTarget()
    {
        return *m_defaultTarget;
    }

    CEGUI::GeometryBuffer& CEGUIRenderer::createGeometryBufferColoured(
        CEGUI::RefCounted<CEGUI::RenderMaterial> renderMaterial)
    {
        CEGUIGeometryBuffer* geom_buffer = new CEGUIGeometryBuffer(*this, renderMaterial);

        geom_buffer->addVertexAttribute(CEGUI::VertexAttributeType::Position0);
        geom_buffer->addVertexAttribute(CEGUI::VertexAttributeType::Colour0);
        geom_buffer->finaliseVertexAttributes();

        addGeometryBuffer(*geom_buffer);
        return *geom_buffer;
    }

    CEGUI::GeometryBuffer& CEGUIRenderer::createGeometryBufferTextured(
        CEGUI::RefCounted<CEGUI::RenderMaterial> renderMaterial)
    {
        CEGUIGeometryBuffer* geom_buffer = new CEGUIGeometryBuffer(*this, renderMaterial);

        geom_buffer->addVertexAttribute(CEGUI::VertexAttributeType::Position0);
        geom_buffer->addVertexAttribute(CEGUI::VertexAttributeType::Colour0);
        geom_buffer->addVertexAttribute(CEGUI::VertexAttributeType::TexCoord0);
        geom_buffer->finaliseVertexAttributes();

        addGeometryBuffer(*geom_buffer);
        return *geom_buffer;
    }

    CEGUI::TextureTarget* CEGUIRenderer::createTextureTarget(bool addStencilBuffer)
    {
        auto ret = new CEGUITextureTarget(*this, addStencilBuffer);
        return ret;
    }
}
