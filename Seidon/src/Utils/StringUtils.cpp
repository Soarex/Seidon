#include "StringUtils.h"

#include <codecvt>

namespace Seidon
{
	std::string GetNameFromPath(const std::string& source)
	{
		size_t separatorIndex = source.find_last_of('\\');
		return source.substr(separatorIndex + 1, source.size() - separatorIndex);
	}

	std::string ChangeSuffix(const std::string& source, const std::string& newSuffix)
	{
		int i;
		for (i = source.size() - 1; i > 0; i--)
			if (source[i] == '.') break;

		if (i == 0) return source;

		return source.substr(0, i).append(newSuffix);
	}

	std::string RemoveLeadingSpaces(const std::string& source)
	{
		int i;
		for (i = 0; i < source.size(); i++)
			if (source[i] != ' ') break;

		if (i == source.size()) return source;

		return source.substr(i, source.size());
	}

	std::string RemoveEndingSpaces(const std::string& source)
	{
		int i;
		for (i = source.size() - 1; i > 0; i--)
			if (source[i] != ' ') break;

		if (i == 0) return source;

		return source.substr(0, i);
	}

	std::string RemoveLeadingAndEndingSpaces(const std::string& source)
	{
		int begin;
		for (begin = 0; begin < source.size(); begin++)
			if (source[begin] != ' ' && source[begin] != '\t') break;

		if (begin == source.size()) begin = 0;

		int end;
		for (end = source.size() - 1; end > 0; end--)
			if (source[end] != ' ' && source[end] != '\t') break;

		if (end == 0) end = source.size() - 1;

		return source.substr(begin, end - begin + 1);
	}

	std::u32string ConvertToUTF32(const std::string& s)
	{
		std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
		return conv.from_bytes(s);
	}
}