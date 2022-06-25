#include "Font.h"
#include <msdfgen/core/edge-coloring.h>
#include <thread>

namespace Seidon
{
	Font::Font(UUID id)
    {
        this->id = id;
        fontAtlas = new Texture();
    }

    void Font::Destroy()
    {
        delete fontAtlas;
        glyphs.clear();
        kernings.clear();
    }

    float Font::GetAdvance(const Glyph& g1, const Glyph& g2)
    {
        float kerning = 0;

        auto pair = std::make_pair(g1.character, g2.character);
        if (kernings.count(pair) > 0) kerning = kernings[pair];

        return g1.advance + kerning;
    }

    float Font::GetAdvance(char32_t c1, char32_t c2)
    {
        float kerning = 0;

        auto pair = std::make_pair(c1, c2);
        if (kernings.count(pair) > 0) kerning = kernings[pair];

        return glyphs[c1].advance + kerning;
    }

	void Font::Save(std::ofstream& out)
	{
        out.write((char*)&id, sizeof(UUID));

        size_t size = name.length() + 1;
        out.write((char*)&size, sizeof(size_t));

        out.write(name.c_str(), size * sizeof(char));

        size = glyphs.size();
        out.write((char*)&size, sizeof(size_t));
        for (auto& [character, glyph] : glyphs)
        {
            out.write((char*)&character, sizeof(char32_t));
            out.write((char*)&glyph, sizeof(Glyph));
        }

        size = kernings.size();
        out.write((char*)&size, sizeof(size_t));
        for (auto& [pair, kerning] : kernings)
        {
            out.write((char*)&pair, sizeof(pair));
            out.write((char*)&kerning, sizeof(float));
        }

        fontAtlas->Save(out);
	}

	void Font::Load(std::ifstream& in)
	{
        in.read((char*)&id, sizeof(UUID));

        size_t size = 0;
        in.read((char*)&size, sizeof(size_t));

        char buffer[2048];
        in.read(buffer, size * sizeof(char));
        name = buffer;

        in.read((char*)&size, sizeof(size_t));

        Glyph glyph;
        char32_t character;
        for (int i = 0; i < size; i++)
        {
            in.read((char*)&character, sizeof(char32_t));
            in.read((char*)&glyph, sizeof(Glyph));

            glyphs[character] = glyph;
        }

        in.read((char*)&size, sizeof(size_t));

        std::pair<char32_t, char32_t> pair;
        float kerning;
        for (int i = 0; i < size; i++)
        {
            in.read((char*)&pair, sizeof(pair));
            in.read((char*)&kerning, sizeof(float));

            kernings[pair] = kerning;
        }

        fontAtlas->Load(in);
	}

	bool Font::Import(const std::string& path)
	{
        name = path;

        msdfgen::FreetypeHandle * freetype = msdfgen::initializeFreetype();
        if (!freetype)
        {
            std::cerr << "Error inizializing freetype for font: " << path << std::endl;
            return false;
        }

        msdfgen::FontHandle* font = msdfgen::loadFont(freetype, path.c_str());
        if (!font) 
        {
            std::cerr << "Error importing font: " << path << std::endl;
            return false;
        }

        std::vector<msdf_atlas::GlyphGeometry> glyphContainer;
        msdf_atlas::FontGeometry fontGeometry(&glyphContainer);

        fontGeometry.loadCharset(font, 1.0, msdf_atlas::Charset::ASCII);
                
        const double maxCornerAngle = 3.0;
        for (msdf_atlas::GlyphGeometry& glyph : glyphContainer)
            glyph.edgeColoring(&msdfgen::edgeColoringInkTrap, maxCornerAngle, 0);

        msdf_atlas::TightAtlasPacker packer;
        packer.setDimensionsConstraint(msdf_atlas::TightAtlasPacker::DimensionsConstraint::POWER_OF_TWO_SQUARE);
        packer.setMinimumScale(24.0);
        packer.setPixelRange(2.0);
        packer.setMiterLimit(1.0);
        packer.pack(glyphContainer.data(), glyphContainer.size());

        int width = 0, height = 0;
        packer.getDimensions(width, height);

        msdf_atlas::ImmediateAtlasGenerator<
            float,
            3,
            msdf_atlas::msdfGenerator,
            msdf_atlas::BitmapAtlasStorage<msdf_atlas::byte, 3>
        > generator(width, height);

        msdf_atlas::GeneratorAttributes attributes;
        generator.setAttributes(attributes);
        generator.setThreadCount(std::thread::hardware_concurrency() - 1);
        generator.generate(glyphContainer.data(), glyphContainer.size());

        msdfgen::BitmapConstRef<msdf_atlas::byte, 3> bitmap = generator.atlasStorage();

        fontAtlas->Create(bitmap.width, bitmap.height, (unsigned char*)bitmap.pixels);

        for (msdf_atlas::GlyphGeometry& g : glyphContainer)
        {
            Glyph glyph;
            glyph.character = g.getCodepoint();
            glyph.advance = g.getAdvance();
            
            double uvLeft, uvBottom, uvRight, uvTop;
            g.getQuadAtlasBounds(uvLeft, uvBottom, uvRight, uvTop);

            double boundLeft, boundBottom, boundRight, boundTop;
            g.getQuadPlaneBounds(boundLeft, boundBottom, boundRight, boundTop);

            glyph.uvBounds = { (float)uvLeft, (float)uvRight, (float)uvBottom, (float)uvTop };
            glyph.bounds = { (float)boundLeft, (float)boundRight, (float)boundBottom, (float)boundTop };

            glyphs[glyph.character] = glyph;

            //for (msdf_atlas::GlyphGeometry& g1 : glyphContainer)
                //kernings[std::make_pair(glyph.character, g1.getCodepoint())] = (float)fontGeometry.getKerning().at(std::make_pair(g.getIndex(), g1.getIndex()));
        }

        for (auto& [pair, kerning] : fontGeometry.getKerning())
            kernings[std::make_pair(fontGeometry.getGlyph(pair.first)->getCodepoint(), fontGeometry.getGlyph(pair.second)->getCodepoint())] = kerning;


        msdfgen::FontMetrics m = fontGeometry.getMetrics();

        metrics.ascenderY = m.ascenderY;
        metrics.descenderY = m.descenderY;
        metrics.emSize = m.emSize;
        metrics.lineHeight = m.lineHeight;
        metrics.underlineThickness = m.underlineThickness;
        metrics.underlineY = m.underlineY;

        msdfgen::destroyFont(font);
        msdfgen::deinitializeFreetype(freetype);
        return true;
	}
}