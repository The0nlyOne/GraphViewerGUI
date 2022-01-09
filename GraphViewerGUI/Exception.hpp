#pragma once

#include <stdexcept>

#pragma warning(push, 0)
#include <QObject>
#pragma pop()

namespace Model
{

	// not used anymore I believe
	class SameName : public std::logic_error
	{
	public:
		using std::logic_error::logic_error;
	};
}