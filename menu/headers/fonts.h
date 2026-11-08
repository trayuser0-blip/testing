#pragma once
#include "includes.h"

class c_font
{
public:
    void update();

    ImFont* get(std::vector<unsigned char> font_data, float size, const ImWchar* glyph_ranges = nullptr);

private:
    struct font_data
    {
        std::vector<unsigned char> data;
        float size;
        ImFont* font;
        const ImWchar* glyph_ranges;

        bool operator==(const font_data& other) const
        {
            return data == other.data && size == other.size;
        }
    };

    void add(std::vector<unsigned char> font_data, float size, const ImWchar* glyph_ranges = nullptr);

    std::vector<font_data> data;
};

inline std::unique_ptr<c_font> font = std::make_unique<c_font>();