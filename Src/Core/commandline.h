#pragma once

#include <stdint.h>

namespace Core
{
	///
	class CommandLine
	{
	public:
		CommandLine(int _argc, const char* const* _argv);

	private:
		int32_t m_argc;
		char const* const* m_argv;
	};

}

