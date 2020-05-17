#include "Font.hpp"

Fonts g_fonts;

Fonts& fonts() { return g_fonts; }

TTF_Font* Fonts::open(u32 languageCode, int sizeIndex) {
	if (m_fonts[languageCode][sizeIndex]) {
		return m_fonts[languageCode][sizeIndex];
	}
	auto fi = m_fontMap[languageCode];

	m_fonts[languageCode][sizeIndex] = TTF_OpenFontRW(SDL_RWFromMem((void*)m_fontData[fi].address, m_fontData[fi].size), 1, size(sizeIndex));
	return m_fonts[languageCode][sizeIndex];
}

Glyph::Glyph() {
    //STUB
}

Glyph::~Glyph() {
    //STUB
}

int Glyph::width() const {
    return m_width;
}

int Glyph::height() const {
    return m_height;
}

bool Glyph::isValid() const {
    return m_texture != NULL;
}

bool Glyph::close() {
    if (m_texture) {
        SDL_DestroyTexture(m_texture);
        m_texture = NULL;
    }

    return true;
}

void Glyph::load(Context& settings, u32 ordinal) {
    if (m_texture || !settings.font) return;

    std::vector<u8> s;
    u8* p = (u8*)&ordinal;

    if (ordinal <= 0xFF)
    {
        s.push_back(p[0]);
    }
    else if (ordinal <= 0xFFFF)
    {
        s.push_back(p[1]);
        s.push_back(p[0]);
    }
    else if (ordinal <= 0xFFFFFF)
    {
        s.push_back(p[2]);
        s.push_back(p[1]);
        s.push_back(p[0]);
    }
    else
    {
        s.push_back(p[3]);
        s.push_back(p[2]);
        s.push_back(p[1]);
        s.push_back(p[0]);
    }
    s.push_back(0);

    std::string ss(s.begin(), s.end());
    auto m_surface = TTF_RenderUTF8_Blended(settings.font, ss.c_str(), {(settings.color >> 24)&0xFF, (settings.color >> 16)&0xFF, (settings.color >> 8)&0xFF, settings.color&0xFF});
    if (m_surface)
    {
        SDL_SetSurfaceAlphaMod(m_surface, settings.color&0xFF);
        m_texture = SDL_CreateTextureFromSurface(settings.renderer, m_surface);

        if (!m_texture) close();
        m_width = m_surface->w;
        m_height = m_surface->h;
        SDL_FreeSurface(m_surface);
    }
}

void Glyph::draw(Cache* cache, Context& settings, const u32 ordinal, u8 fontIndex) {
    if (!isValid()) {
        settings.font = cache->font(ordinal, fontIndex);
        load(settings, ordinal);

        if (!isValid()) return;
    }

    if (settings.r.w && settings.x > settings.r.w) return;

    SDL_Rect destRect = { settings.r.x + settings.x, settings.r.y + settings.y, width() , height() };

    SDL_RenderCopy(settings.renderer, m_texture, NULL, &destRect);
    settings.x += width();
}


Cache::Cache() : m_font(NULL) {
    
}

Cache::Cache(TTF_Font* font) : m_font(font) {
    
}

Cache::~Cache() {
    
}

u32 Cache::utf8char(char*& s, const char* max) {
    u32 r = 0;
    u8* rp = (u8*)&r;
    const u8* sp = (u8*)s;

    u8 c = *((u8*)s);
    if ((c & 0x80) == 0) {
        rp[0] = sp[0];
        s++;
    }
    else if ((*((u16*)s) & 0xC0E0) == 0x80C0) {
        rp[0] = sp[1];
        rp[1] = sp[0];
        s += 2;
    }
    else if ((*((u32*)s) & 0xC0C0F0) == 0x8080E0) {
        rp[0] = sp[2];
        rp[1] = sp[1];
        rp[2] = sp[0];
        s += 3;
    }
    else {
        s++;
    }
    return r;
}

void Cache::draw(SDL_Renderer* renderer, int x, int y, const std::string& s, u8 fontIndex) {
    static Context settings;
    settings.renderer = renderer;
    settings.x = 0;
    settings.y = 0;
    settings.r = SDL_Rect{ x, y,0,0 };
    settings.color = m_color;

    char* ptr = (char*)s.c_str();
    char* end = ptr + s.length();

    while (ptr < end) {
        u32 ordinal = utf8char(ptr, end);
        Glyph& g = m_glyphs[ordinal];

        g.draw(this, settings, ordinal, fontIndex);
    }
}

void Cache::draw(SDL_Renderer* renderer, const SDL_Rect& rect, const std::string& s, u8 fontIndex) {
    static Context settings;
    settings.renderer = renderer;
    settings.x = 0;
    settings.y = 0;
    settings.r = rect;
    settings.color = m_color;

    char* ptr = (char*)s.c_str();
    char* end = ptr + s.length();

    while (ptr < end) {
        u32 ordinal = utf8char(ptr, end);
        Glyph& g = m_glyphs[ordinal];

        g.draw(this, settings, ordinal, fontIndex);
    }
}

bool isSpace(char c) {
    switch (c) {
        case ' ':
        case '\t':
        case '\r':
        case '\n':
        case '\v':
        case '\f':
            return true;
    }
    return false;
}

void Cache::drawWord(Context& context, char*& ptr, const char* end, u8 fontIndex) {
    while (!isSpace(*ptr) && ptr < end) {
        u32 ordinal = utf8char(ptr, end);
        Glyph& g = m_glyphs[ordinal];

        g.draw(this, context, ordinal, fontIndex);
    }
}

u64 Cache::sizeWord(Context& context, char* ptr, const char* end) {
    u64 width = 0;
    while (!isSpace(*ptr) && ptr < end) {
        u32 ordinal = utf8char(ptr, end);
        Glyph& g = m_glyphs[ordinal];

        width += g.width();
    }
    return width;
}

u64 Cache::drawParagraph(SDL_Renderer* renderer, const SDL_Rect& r, const std::string& s, u8 fontIndex) {
    Context settings;
    settings.renderer = renderer;
    settings.x = 0;
    settings.y = 0;
    settings.r = r;
    settings.color = m_color;

    Glyph& space = m_glyphs[' '];
    u32 spaceWidth = space.width();
    if (!spaceWidth)
        spaceWidth = Fonts::size(settings.sizeIndex) / 2;

    char* ptr = (char*)s.c_str();
    char* end = ptr + s.length();

    while (ptr < end) {
        while (isSpace(*ptr) && ptr < end) {
            switch (*ptr) {
                case ' ':
                    settings.x += spaceWidth;
                    break;
                case '\t':
                    settings.x += spaceWidth * 4;
                    break;
                case '\r':
                    break;
                case '\n':
                    settings.x = 0;
                    settings.y += Fonts::size(settings.sizeIndex) * 3 / 2;
                    break;
                case '\v':
                    break;
                case '\f':
                    break;
            }
            ptr++;
        }

        if (ptr < end) {
            u64 width = sizeWord(settings, ptr, end);

            if (settings.x + width >= r.w) {
                if (settings.x > 0) {
                    settings.x = 0;
                    settings.y += Fonts::size(settings.sizeIndex) * 3 / 2;
                }
            }
            drawWord(settings, ptr, end, fontIndex);
        }
    }
    return settings.y;
}

SDL_Rect Cache::rect(const std::string& s, u8 fontIndex) {
    SDL_Rect r = { 0,0,0,0 };
    TTF_SizeUTF8(font('a', fontIndex), s.c_str(), &r.w, &r.h);
    return r;
}

int Cache::getFontIndex(u32 c, u8 fontIndex) {
    if (fontIndex == FIXED) return FIXED;
    if (c < 0x80) return Standard;
    if (c >= 0xEE8080 && c <= 0xEE8592) return Extended;
    if (c >= 0xE18480 && c <= 0xE187BF) return Korean;
    if (c >= 0xEAB080 && c <= 0xED9EA3) return Korean;
    if (c >= 0xE384B0 && c <= 0xE3868F) return Korean;
    if (c >= 0xEAA5A0 && c <= 0xEAA5BF) return Korean;
    if (c >= 0xED9EB0 && c <= 0xED9FBF) return Korean;
    if (c >= 0xE4B880 && c <= 0xE9BFBF) return Chinese_Simplified;
    if (c >= 0xEE8880 && c <= 0xEE8C80) return SYMBOLS1;
    return Standard;
}

TTF_Font* Cache::font(u32 c, u8 fontIndex) {
    return fonts().open(getFontIndex(c, fontIndex), sizeIndex());
}