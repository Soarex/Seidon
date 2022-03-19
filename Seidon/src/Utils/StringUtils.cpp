#include "StringUtils.h"

namespace Seidon
{
	std::string ChangeSuffix(const std::string& source, const std::string& newSuffix)
	{
		int i;
		for (i = source.size(); i > 0; i--)
			if (source[i] == '.') break;

		if (i == 0) return source;

		return source.substr(0, i).append(newSuffix);
	}
}