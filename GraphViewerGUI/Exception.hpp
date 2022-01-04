#pragma once

#include <stdexcept>

namespace Model
{
	class SameName : public std::logic_error
	{
	public:
		using std::logic_error::logic_error;
	};
}