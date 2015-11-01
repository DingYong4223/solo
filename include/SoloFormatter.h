#pragma once

#include <sstream>

#define SL_FMT(...) solo::Formatter()(__VA_ARGS__)

namespace solo
{
	class Formatter
	{
	public:
		std::string operator()() const
		{
			return _buf.str();
		}

		template <class TFirst, class... TRest>
		std::string operator()(TFirst first, TRest... rest)
		{
			_buf << first;
			return operator()(rest...);
		}

	private:
		std::ostringstream _buf;
	};
}
