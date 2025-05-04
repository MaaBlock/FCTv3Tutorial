//
// Created by Administrator on 2025/5/4.
//
#include "../headers.h"
namespace FCT
{
	Freetype_Font::Freetype_Font(Freetype_FontCommon* common)
	{
		m_common = common;
	}
	bool Freetype_Font::create(const char *fontPath)
	{
		if (FT_New_Face(m_common->g_library, fontPath, 0, &m_face))
		{
			std::cout << "Could not load font: " << fontPath << std::endl;
			return false;
		}

		FT_Bool hasColor = FT_HAS_COLOR(m_face);
		if (hasColor)
		{
			std::cout << "Font supports color glyphs." << std::endl;
		}
		else
		{
			std::cout << "Font does not support color glyphs." << std::endl;
		}

		if (m_face->num_fixed_sizes > 0)
		{
			std::cout << "Font has " << m_face->num_fixed_sizes << " fixed size(s)." << std::endl;

			FT_Error error = FT_Select_Size(m_face, 0);
			if (error)
			{
				std::cout << "Could not select font size. Error code: " << error << std::endl;
				FT_Done_Face(m_face);
				m_face = nullptr;
				return false;
			}
		}
		else
		{
			FT_Error error = FT_Set_Pixel_Sizes(m_face, 0, 64);
			if (error)
			{
				std::cout << "Could not set font pixel size. Error code: " << error << std::endl;
				FT_Done_Face(m_face);
				m_face = nullptr;
				return false;
			}
		}
		m_ascender = m_face->ascender / 64.0f;
		m_descender = m_face->descender / 64.0f;
		m_height = m_face->height / 64.0f;
		return true;
	}
	VertexPath* Freetype_Font::translateGlyph(FT_UInt glyph_index)
	{
		VertexPath* ret = new VertexPath();
		FT_Int32 load_flags = FT_LOAD_DEFAULT | FT_LOAD_NO_BITMAP;
		if (FT_HAS_COLOR(m_face))
		{
			load_flags |= FT_LOAD_COLOR;
		}

		if (FT_Load_Glyph(m_face, glyph_index, load_flags))
		{
			std::cout << "Failed to load glyph for glyph index: " << glyph_index << std::endl;
			delete ret;
			return nullptr;
		}

		GlyphInfo glyphInfo;
		glyphInfo.advanceX = m_face->glyph->advance.x / 64.0f;
		glyphInfo.advanceY = m_face->glyph->advance.y / 64.0f;
		glyphInfo.bitmapWidth = m_face->glyph->metrics.width / 64.0f;
		glyphInfo.bitmapHeight = m_face->glyph->metrics.height / 64.0f;
		glyphInfo.bitmapLeft = m_face->glyph->metrics.horiBearingX / 64.0f;
		glyphInfo.bitmapTop = m_face->glyph->metrics.horiBearingY / 64.0f;

		outlineToCommands(&m_face->glyph->outline, ret);
		return ret;
	}
	void Freetype_Font::outlineToCommands(FT_Outline *outline, VertexPath* ret)
	{
		FT_LayerIterator layer_iterator;
		FT_UInt layer_index;
		FT_UInt color_index;
		FT_Bool have_layers;

		layer_iterator.p = nullptr;
		have_layers = FT_Get_Color_Glyph_Layer(m_face, m_face->glyph->glyph_index, &layer_index, &color_index, &layer_iterator);
		ret->beginPath(0);
		ret->endPath();
		do
		{
			ret->beginPath(1);
			if (have_layers)
			{
				FT_Palette_Data palette_data;
				FT_Error palette_data_error = FT_Palette_Data_Get(m_face, &palette_data);
				if (palette_data_error == 0)
				{
					FT_Color *palette;
					FT_Error palette_select_error = FT_Palette_Select(m_face, 0, &palette);
					if (palette_select_error == 0 && color_index < palette_data.num_palette_entries)
					{
						FT_Color color = palette[color_index];
						ret->setColor(Vec4(color.red / 255.0f, color.green / 255.0f, color.blue / 255.0f, color.alpha / 255.0f));
					}
				}
				FT_Error error = FT_Load_Glyph(m_face, layer_index, FT_LOAD_DEFAULT);
				if (error)
				{
					std::cout << "Error loading glyph layer: " << error << std::endl;
					continue;
				}
			}

			FT_Vector* points = outline->points;
			unsigned char* tags = outline->tags;
			unsigned short* contours = outline->contours;
			int n_points = outline->n_points;
			int n_contours = outline->n_contours;
			int start = 0;
			for (int i = 0; i < n_contours; i++)
			{
				int end = contours[i];
				Vec2 last;
				Vec2 control;
				bool hasControl = false;

				for (int j = start; j <= end; j++)
				{
					Vec2 point(points[j].x / 64.0f, points[j].y / 64.0f);

					if (j == start)
					{
						ret->moveTo(point);
						last = point;
					}
					else
					{
						if (FT_CURVE_TAG(tags[j]) == FT_CURVE_TAG_ON)
						{
							if (hasControl)
							{
								ret->curveTo(control,point);
								hasControl = false;
							}
							else
							{
								ret->lineTo(point);
							}
							last = point;
						}
						else if (FT_CURVE_TAG(tags[j]) == FT_CURVE_TAG_CONIC)
						{
							if (hasControl)
							{
								Vec2 mid((control.x + point.x) / 2, (control.y + point.y) / 2);

								ret->curveTo(control,mid);
								last = mid;
							}
							control = point;
							hasControl = true;
						}
						else if (FT_CURVE_TAG(tags[j]) == FT_CURVE_TAG_CUBIC)
						{
							// todo:从FCTv2 搬过来的，其实已经可以使用三次曲线了，添加三次曲线
							std::cout << "Warning: Cubic Bezier curves are not supported in this implementation." << std::endl;
						}
					}
				}
				if (hasControl)
				{
					Vec2 start_point(points[start].x / 64.0f, points[start].y / 64.0f);
					ret->curveTo(control,start_point);
				}
				else
				{
					Vec2 start_point(points[start].x / 64.0f, points[start].y / 64.0f);
					ret->lineTo(start_point);
				}

				start = end + 1;
			}
			ret->endPath();

			if (have_layers)
			{
				have_layers = FT_Get_Color_Glyph_Layer(m_face, m_face->glyph->glyph_index, &layer_index, &color_index, &layer_iterator);
			}
		} while (have_layers);
	}
}