#pragma once

#include "dbg.h"
#include <bx/bx.h>
#include <bx/filepath.h>
#include <bx/string.h>

namespace bx { struct FileReaderI; struct FileWriterI; struct AllocatorI; }
namespace entry
{
    void init();

    void setCurrentDir(const char* _dir);

    ///
	bx::FileReaderI* getFileReader();

	///
	bx::FileWriterI* getFileWriter();

    ///
	bx::AllocatorI*  getAllocator();

}
