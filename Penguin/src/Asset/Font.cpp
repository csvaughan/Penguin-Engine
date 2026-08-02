#include "Asset/Font.h"
#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>
#include <SDL3/SDL.h>

namespace pgn
{
    Font::Font(SDL_Renderer* renderer, const std::string& path, float ptsize)
    {
        if (generateAtlas(renderer, path, ptsize)) 
        {
            // Atlas successfully generated
        }
    }

    Font::~Font()
    {
        if (m_atlasTexture) 
        {
            SDL_DestroyTexture(m_atlasTexture);
        }
    }

    const Glyph& Font::getGlyph(char c) const
    {
        auto it = m_glyphs.find(c);
        return (it != m_glyphs.end()) ? it->second : m_glyphs.at('?');
    }

    bool Font::generateAtlas(SDL_Renderer* renderer, const std::string& path, float ptsize)
    {
        // 1. Read binary font data into memory buffer
        std::ifstream file(path, std::ios::binary | std::ios::ate);
        if (!file.is_open()) return false;

        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);

        std::vector<unsigned char> fontBuffer(size);
        if (!file.read(reinterpret_cast<char*>(fontBuffer.data()), size)) return false;

        // 2. Initialize stb_truetype and extract basic metrics
        stbtt_fontinfo info;
        if (!stbtt_InitFont(&info, fontBuffer.data(), 0)) return false;

        float scale = stbtt_ScaleForPixelHeight(&info, ptsize);
        int ascent, descent, lineGap;
        stbtt_GetFontVMetrics(&info, &ascent, &descent, &lineGap);

        // Round metrics to ensure clean integer boundaries on screen
        m_ascender = std::round(ascent * scale);
        m_lineHeight = std::round((ascent - descent + lineGap) * scale);

        // 3. Set up the Atlas pixel packer
        int atlasSize = 512;
        m_atlasSize = { (float)atlasSize, (float)atlasSize };
        std::vector<unsigned char> alphaPixels(atlasSize * atlasSize, 0);

        stbtt_pack_context pc;
        // 1 pixel of padding between characters to completely prevent bleeding
        if (!stbtt_PackBegin(&pc, alphaPixels.data(), atlasSize, atlasSize, 0, 1, nullptr)) return false;

        // Optional: Set to 2, 2 or higher for subpixel anti-aliasing oversampling
        stbtt_PackSetOversampling(&pc, 1, 1); 

        // We will pack standard printable ASCII characters (32 [space] to 126 ['~'])
        const int firstChar = 32;
        const int numChars = 95;
        std::vector<stbtt_packedchar> packedChars(numChars);

        stbtt_pack_range range{};
        range.font_size = ptsize;
        range.first_unicode_codepoint_in_range = firstChar;
        range.num_chars = numChars;
        range.chardata_for_range = packedChars.data();

        if (!stbtt_PackFontRanges(&pc, fontBuffer.data(), 0, &range, 1))
        {
            stbtt_PackEnd(&pc);
            return false;
        }
        stbtt_PackEnd(&pc);

        // 4. Convert 8-bit Alpha format to 32-bit RGBA for standard GPU blitting
        std::vector<unsigned char> rgbaPixels(atlasSize * atlasSize * 4, 255);
        for (int i = 0; i < atlasSize * atlasSize; ++i)
        {
            // RGB remain pure white, Alpha maps directly from the font rasterizer
            rgbaPixels[i * 4 + 3] = alphaPixels[i]; 
        }

        // 5. Transfer to an SDL Texture
        SDL_Surface* atlasSurf = SDL_CreateSurface(atlasSize, atlasSize, SDL_PIXELFORMAT_RGBA32);
        if (!atlasSurf) return false;

        std::memcpy(atlasSurf->pixels, rgbaPixels.data(), rgbaPixels.size());
        m_atlasTexture = SDL_CreateTextureFromSurface(renderer, atlasSurf);
        SDL_DestroySurface(atlasSurf);

        // 6. Map stb_truetype values directly to your engine metrics
        for (int i = 0; i < numChars; ++i)
        {
            char c = (char)(firstChar + i);
            const auto& pc_char = packedChars[i];

            Glyph& g = m_glyphs[c];
            float w = (float)(pc_char.x1 - pc_char.x0);
            float h = (float)(pc_char.y1 - pc_char.y0);

            g.uvRect = { (float)pc_char.x0, (float)pc_char.y0, w, h };
            g.size = { w, h };
            g.advance = pc_char.xadvance;
            g.bearing.x = pc_char.xoff;
            g.bearing.y = -pc_char.yoff; 
        }

        // Fallback for missing characters
        if (m_glyphs.find('?') == m_glyphs.end()) 
        {
            m_glyphs['?'] = m_glyphs[' '];
        }

        return true;
    }
} // namespace pgn