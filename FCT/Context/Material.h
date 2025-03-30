#pragma once
#include "../RHI/VertexShader.h"
#include "../RHI/PixelShader.h"

namespace FCT {

class Material {
public:
    virtual ~Material() = default;

    virtual bool compile() = 0;
    virtual void bind() = 0;
    virtual void unbind() = 0;

    virtual VertexShader* getVertexShader() const = 0;
    virtual PixelShader* getPixelShader() const = 0;

protected:
    Material(VertexShader* vertexShader, PixelShader* pixelShader)
        : m_vertexShader(vertexShader), m_pixelShader(pixelShader) {}

    VertexShader* m_vertexShader;
    PixelShader* m_pixelShader;
};

} // namespace FCT
