#pragma once

#include <optional>
#include <string_view>
#include <string>

namespace FoxoCommons
{
	std::optional<std::string> ReadTextFile(std::string_view filename);

	template<typename t_Type>
	t_Type Map(const t_Type& value, const t_Type& min1, const t_Type& max1, const t_Type& min2, const t_Type& max2)
	{
		return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
	}
}