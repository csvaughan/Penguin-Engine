#include "Assets/Font.h"
#include <SDL3_ttf/SDL_ttf.h>
#define STB_RECT_PACK_IMPLEMENTATION
#include <stb_rect_pack.h>

namespace pgn
{

    Font::Font(SDL_Renderer *renderer, const std::string &path, float ptsize)
    {
        m_font = TTF_OpenFont(path.c_str(), ptsize);
        if (!m_font) return;

        TTF_SetFontHinting(m_font, TTF_HINTING_LIGHT);
        generateAtlas(renderer);
    }

    Font::~Font()
    {
        if (m_atlasTexture) SDL_DestroyTexture(m_atlasTexture);
        if (m_font) TTF_CloseFont(m_font);
    }

    float Font::getLineHeight() const { return (float)TTF_GetFontHeight(m_font); }
    float Font::getAscender() const   { return (float)TTF_GetFontAscent(m_font); }

    const Glyph &Font::getGlyph(char c) const
    {
        auto it = m_glyphs.find(c);
        return (it != m_glyphs.end()) ? it->second : m_glyphs.at('?');
    }

    bool Font::generateAtlas(SDL_Renderer *renderer)
    {
        const char* charset = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
        size_t numGlyphs = strlen(charset);

        std::vector<SDL_Surface*> surfaces(numGlyphs);
        std::vector<stbrp_rect> rects(numGlyphs);
        SDL_Color white = { 255, 255, 255, 255 };

        for (size_t i = 0; i < numGlyphs; i++) 
        {
            surfaces[i] = TTF_RenderGlyph_Blended(m_font, charset[i], white);
            if (surfaces[i]) 
            {
                rects[i].id = (int)i; // +1 padding prevents texture bleeding (neighboring pixels showing up)
                rects[i].w = (stbrp_coord)(surfaces[i]->w + 1);
                rects[i].h = (stbrp_coord)(surfaces[i]->h + 1);
            }
        }

        int atlasSize = 512;
        m_atlasSize = {atlasSize, atlasSize};
        stbrp_context context;
        std::vector<stbrp_node> nodes(atlasSize);
        stbrp_init_target(&context, atlasSize, atlasSize, nodes.data(), (int)nodes.size());
        stbrp_pack_rects(&context, rects.data(), (int)numGlyphs);

        SDL_Surface* atlasSurf = SDL_CreateSurface(atlasSize, atlasSize, SDL_PIXELFORMAT_RGBA32);
        SDL_FillSurfaceRect(atlasSurf, nullptr, 0x00000000);

        for (size_t i = 0; i < numGlyphs; i++) 
        {
            if (rects[i].was_packed && surfaces[i]) 
            {
                SDL_Rect dst = { rects[i].x, rects[i].y, surfaces[i]->w, surfaces[i]->h };
                SDL_BlitSurface(surfaces[i], nullptr, atlasSurf, &dst);

                int adv, minx, maxx, miny, maxy;
                TTF_GetGlyphMetrics(m_font, charset[i], &minx, &maxx, &miny, &maxy, &adv);

                Glyph& g = m_glyphs[charset[i]];
                g.uvRect = { (float)rects[i].x, (float)rects[i].y, (float)surfaces[i]->w, (float)surfaces[i]->h };
                g.size = { (float)surfaces[i]->w, (float)surfaces[i]->h };
                g.advance = (float)adv;
                g.bearing = { (float)minx, g.size.y + (float)miny };
            }
            if (surfaces[i]) SDL_DestroySurface(surfaces[i]);
        }

        m_atlasTexture = SDL_CreateTextureFromSurface(renderer, atlasSurf);
        SDL_DestroySurface(atlasSurf);
        return true;
    }  
} // namespace pgn