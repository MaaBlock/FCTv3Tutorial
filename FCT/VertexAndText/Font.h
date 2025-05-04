//
// Created by Administrator on 2025/5/4.
//

#ifndef FONT_H
#define FONT_H
#include "./VertexPath.h"
class Font {
protected:
    float m_ascender;
    float m_descender;
    float m_height;
    struct GlyphInfo
    {
        float advanceX;
        float advanceY;
        float bitmapWidth;
        float bitmapHeight;
        float bitmapLeft;
        float bitmapTop;
        std::vector<float> outlineCommands;
    };//从FCTv2移植过来的， todo：待删除
public:

};



#endif //FONT_H
