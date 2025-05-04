//
// Created by Administrator on 2025/5/4.
//
#include "./Font.h"
#include "./Freetype_FontCommon.h"
#ifndef FREETYPE_FONT_H
#define FREETYPE_FONT_H
namespace FCT
{
    class Freetype_Font : public Font
    {
    private:
        Freetype_FontCommon* m_common;
		FT_Face m_face;
    public:
        Freetype_Font(Freetype_FontCommon* common);
        FT_UInt toGlyptIndex(char32_t ch)
        {
            FT_UInt glyph_index = FT_Get_Char_Index(m_face, ch);
            return  glyph_index;
        }
        bool create(const char *fontPath);
        VertexPath* translateGlyph(FT_UInt glyph_index);
        void outlineToCommands(FT_Outline* outline, VertexPath* ret);
    };

}
#endif //FREETYPE_FONT_H
