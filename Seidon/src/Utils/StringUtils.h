#pragma once
#include <string>

namespace Seidon
{
	std::string ChangeSuffix(const std::string& source, const std::string& newSuffix);
	std::string RemoveLeadingSpaces(const std::string& source);
	std::string RemoveEndingSpaces(const std::string& source);
	std::string RemoveLeadingAndEndingSpaces(const std::string& source);
}