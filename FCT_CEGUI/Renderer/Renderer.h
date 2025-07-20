#ifndef FCT_CEGUI_RENDERER_H
#define FCT_CEGUI_RENDERER_H
#include "../ThirdParty.h"
namespace FCT
{
    class CEGUIRenderer : public CEGUI::Renderer
    {
    protected:
        Context* m_ctx;
        CEGUI::RenderTarget* m_defaultTarget;
        TraditionPipelineState m_state[2][2];
        Sampler* m_sampler;
        BlendState* m_blendStateNormal;
        BlendState* m_blendStatePreMultiplied;
        static constexpr ResourceLayout resourceLayout {
            TextureElement{"CEGUITexture"},
            SamplerElement{"CEGUISampler"}
        };
        bool m_scissorEnabled;
    protected://[finsh:]
        void initialiseSamplerStates();
        void initialiseBlendStates();
    protected://[todo:]
        void initialiseDepthStencilState();
        void initialiseShaders();
        void initialisePipelineState();
    public://[finsh:]
        static CEGUIRenderer& bootstrapSystem(Context* ctx,
                                                       const int abi = CEGUI_VERSION_ABI);

        CEGUI::RenderTarget& getDefaultRenderTarget();
    public://[todo:]
        CEGUIRenderer(Context* ctx);
        CEGUI::GeometryBuffer& createGeometryBufferColoured(CEGUI::RefCounted<CEGUI::RenderMaterial> renderMaterial) override;
        CEGUI::GeometryBuffer& createGeometryBufferTextured(CEGUI::RefCounted<CEGUI::RenderMaterial> renderMaterial) override;
        Context* ctx() const { return m_ctx; }
        TraditionPipelineState* getPipelineState(CEGUI::BlendMode blend, CEGUI::DefaultShaderType type)
        {
            uint8_t blendIndex =  blend == CEGUI::BlendMode::Normal ? 1 : 0;
            uint8_t shaderIndex = type == CEGUI::DefaultShaderType::Solid ? 1 : 0;
            return &m_state[blendIndex][shaderIndex];
        }

        CEGUI::TextureTarget* createTextureTarget(bool addStencilBuffer) override;
        void destroyTextureTarget(CEGUI::TextureTarget* target) override;
        void destroyAllTextureTargets() override;
        CEGUI::Texture& createTexture(const CEGUI::String& name) override;
        CEGUI::Texture& createTexture(const CEGUI::String& name, const CEGUI::String& filename,
            const CEGUI::String& resourceGroup) override;
        CEGUI::Texture& createTexture(const CEGUI::String& name, const CEGUI::Sizef& size) override;
        void destroyTexture(CEGUI::Texture& texture) override;
        void destroyTexture(const CEGUI::String& name) override;
        void destroyAllTextures() override;
        CEGUI::Texture& getTexture(const CEGUI::String& name) const override;
        bool isTextureDefined(const CEGUI::String& name) const override;
        void beginRendering() override;
        void endRendering() override;
        void setDisplaySize(const CEGUI::Sizef& size) override;
        const CEGUI::Sizef& getDisplaySize() const override;
        unsigned getMaxTextureSize() const override;
        const CEGUI::String& getIdentifierString() const override;
        CEGUI::RefCounted<CEGUI::RenderMaterial>
        createRenderMaterial(const CEGUI::DefaultShaderType shaderType) const override;
        bool isTexCoordSystemFlipped() const override;
    };



    //4种管线状态
    /*todo:
    *   1. blend2 * Scissor2
    *   2.实现预乘 alpha blend，普通 blend
    *   3.启用 or 禁用 裁剪
    */
}

#endif //RENDERER_H
