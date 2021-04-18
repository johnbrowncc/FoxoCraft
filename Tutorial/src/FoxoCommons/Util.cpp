#include "Util.h"

#include <fstream>

namespace FoxoCommons
{
	std::optional<std::string> ReadTextFile(std::string_view filename)
	{
		std::FILE* fp = std::fopen(filename.data(), "rb");

		if (fp)
		{
			std::string contents;
			std::fseek(fp, 0, SEEK_END);
			contents.resize(std::ftell(fp));
			std::rewind(fp);
			std::fread(&contents[0], 1, contents.size(), fp);
			std::fclose(fp);
			return contents;
		}

		return {};
	}
}