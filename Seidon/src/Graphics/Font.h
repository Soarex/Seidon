#pragma once
#include "../Core/UUID.h"
#include "BoundingBox.h"
#include "Texture.h"

#include <msdf-atlas-gen/msdf-atlas-gen.h>

#include <iostream>
#include <fstream>

namespace Seidon
{
	struct Glyph
	{
		char32_t character;
		float advance;

		BoundingBox uvBounds;
		BoundingBox bounds;
	};

	struct FontMetrics
	{
		float emSize;
		float ascenderY, descenderY;
		float lineHeight;
		float underlineY, underlineThickness;
	};

	class Font
	{
	public:
		Font(UUID id = UUID());

		void Destroy();

		inline UUID GetId() { return id; }
		inline const std::string& GetName() { return name; }

		inline const Glyph& GetGlyph(char32_t character) { if (glyphs.count(character) == 0) return glyphs['?']; return glyphs[character]; }
		inline const FontMetrics& GetMetrics() { return metrics; }
		float GetAdvance(const Glyph& g1, const Glyph& g2);
		float GetAdvance(char32_t c1, char32_t c2);

		inline Texture* GetAtlas() { return fontAtlas; }

		void Save(std::ofstream& out);
		void Load(std::ifstream& in);

		void Save(const std::string& path);
		void Load(const std::string& path);

		bool Import(const std::string& path);
	private:
		UUID id;
		std::string name;

		Texture* fontAtlas;
		std::map<char32_t, Glyph> glyphs;
		std::map<std::pair<char32_t, char32_t>, float> kernings;

		FontMetrics metrics;

		friend class AssetImporter;
		friend class ResourceManager;
	};
}