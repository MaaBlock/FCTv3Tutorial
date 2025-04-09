//
// Created by Administrator on 2025/4/9.
//
#include "../FCTAPI.h"
namespace FCT
{
    void PixelShader::generateDefaultCode()
    {
        m_userSource = generatDefaultUserPixelMain(m_pixelLayout);
    }

    void PixelShader::preprocess()
    {
        m_source = generatPixelShader(m_pixelLayout, m_userSource);
    }
}



