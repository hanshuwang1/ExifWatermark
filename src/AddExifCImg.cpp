#include <string>
#include <vector>
#include "CImg.h"
#include <iostream>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

using namespace cimg_library;

class FreeTypeRenderer {
private:
    FT_Library  library = nullptr;
    FT_Face     face = nullptr;

public:
    FreeTypeRenderer(const std::string& fontPath, int fontSize = 32) {
        if (FT_Init_FreeType(&library)) {
            throw std::runtime_error("FT_Init_FreeType failed");
        }
        if (FT_New_Face(library, fontPath.c_str(), 0, &face)) {
            throw std::runtime_error("FT_New_Face failed: " + fontPath);
        }
        FT_Set_Pixel_Sizes(face, 0, fontSize);   // fontsize in pixels
    }

    ~FreeTypeRenderer() {
        if (face) FT_Done_Face(face);
        if (library) FT_Done_FreeType(library);
    }

    /**
      * @brief CImg draw one line text support CN
      * @param img 
      * @param text
      * @param x 
      * @param y 
      * @param color RGB
      * @retval None
      */
    void drawText(CImg<unsigned char>& img, const std::wstring& text, 
                  int x, int y, unsigned char color[3] = nullptr) 
    {
        if (!color) {
            static unsigned char white[3] = {255, 255, 255};
            color = white;
        }

        int pen_x = x;
        int pen_y = y;

        for (wchar_t wc : text) {
            if (FT_Load_Char(face, wc, FT_LOAD_RENDER)) {
                continue;
            }

            FT_GlyphSlot slot = face->glyph;
            FT_Bitmap& bitmap = slot->bitmap;

            // draw bitmap to CImg
            for (unsigned int row = 0; row < bitmap.rows; ++row) {
                for (unsigned int col = 0; col < bitmap.width; ++col) {
                    int px = pen_x + slot->bitmap_left + col;
                    int py = pen_y - slot->bitmap_top + row;

                    if (px < 0 || py < 0 || px >= img.width() || py >= img.height())
                        continue;

                    unsigned char alpha = bitmap.buffer[row * bitmap.pitch + col];
                    if (alpha == 0) continue;

                    // alpha blending
                    for (int c = 0; c < 3; ++c) {
                        img(px, py, c) = (img(px, py, c) * (255 - alpha) + color[c] * alpha) / 255;
                    }
                }
            }

            // move pen position
            pen_x += slot->advance.x >> 6;
        }
    }
};

void addExifInfoToBottom_CImg(
    const std::string& inputPath,
    const std::string& outputPath,
    const std::string& dateTime,
    const std::string& cameraLens,   
    const std::string& exposure,      
    const std::string& location)      
{
    // 1. load image
    CImg<unsigned char> img(inputPath.c_str());

    int w = img.width();
    int h = img.height();

    // 2. calculate bottom bar height (enough for 2 lines of text + padding)
    const int lineHeight = 32;
    const int padding = 20;
    const int barHeight = 2 * lineHeight + 2 * padding + 20;

    // 3. create new image with extra space at the bottom
    CImg<unsigned char> result(w, h + barHeight, 1, 3);   // RGB
    result.fill(255);                                       // white background
    result.draw_image(0, 0, img);                           // copy original image
    unsigned char black[3] = {0, 0, 0};
    unsigned char white[3] = {255, 255, 255};
    unsigned char gray[3]  = {220, 220, 220};
    result.draw_rectangle(0, h, w - 1, h + barHeight - 1, white);
    result.draw_line(0, h + 8, w - 1, h + 8, gray);

    // 5. draw text (using FreeType for better font support)
    int y = h + padding + 10;

    try {
        // font style
        FreeTypeRenderer ft("./file/SourceHanSansCN-Bold.otf", 36);

        unsigned char white[3] = {255, 255, 255};
        unsigned char gray[3]  = {220, 220, 220};
        unsigned char black[3] = {0, 0, 0};

        int y = h + 40;
        int x = 40;

        // std::string -> std::wstring（UTF-8）
        auto toWstring = [](const std::string& str) -> std::wstring {
            std::wstring wstr;
            int len = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
            if (len > 0) {
                wstr.resize(len);
                MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], len);
            }
            return wstr;
        };
        ft.drawText(result, toWstring(dateTime),     x, y + 40, black);
        ft.drawText(result, toWstring(cameraLens),   x, y, black);
        ft.drawText(result, toWstring(exposure),     w - 800, y, black);
        ft.drawText(result, toWstring(location),     w - 800, y + 40, black);


        // FreeTypeRenderer ft("./file/msyhl.ttc", 36);
        // ft.drawText(result, toWstring(dateTime),     x, y + 40, black);

    } catch (const std::exception& e) {
        std::cerr << "FreeType 错误: " << e.what() << std::endl;
    }

    // 6. save result
    result.save(outputPath.c_str());

    std::cout << "CImg Added Exif info to: " << outputPath << std::endl;
}
