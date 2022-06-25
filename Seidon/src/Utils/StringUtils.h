#pragma once
#include <string>

namespace Seidon
{
	std::string GetNameFromPath(const std::string& source);
	std::string ChangeSuffix(const std::string& source, const std::string& newSuffix);
	std::string RemoveLeadingSpaces(const std::string& source);
	std::string RemoveEndingSpaces(const std::string& source);
	std::string RemoveLeadingAndEndingSpaces(const std::string& source);
	std::u32string ConvertToUTF32(const std::string& s);
}