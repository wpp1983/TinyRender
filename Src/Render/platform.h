#pragma once

#include "TinyRender.h"

namespace TinyRender
{
    /// Render frame enum.
	///
	struct RenderFrame
	{
		enum Enum
		{
			NoContext,
			Render,
			Timeout,
			Exiting,

			Count
		};
	};

    RenderFrame::Enum renderFrame(int32_t _msecs = -1);    
}


