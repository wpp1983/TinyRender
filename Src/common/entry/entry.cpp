#include <bx/bx.h>
#include <bx/file.h>
#include <bx/sort.h>

#include "entry_p.h"

namespace entry
{

    static bx::FileReaderI* s_fileReader = NULL;
	static bx::FileWriterI* s_fileWriter = NULL;

    extern bx::AllocatorI* getDefaultAllocator();
	bx::AllocatorI* g_allocator = getDefaultAllocator();

	typedef bx::StringT<&g_allocator> String;

	static String s_currentDir;

    class FileReader : public bx::FileReader
	{
		typedef bx::FileReader super;

	public:
		virtual bool open(const bx::FilePath& _filePath, bx::Error* _err) override
		{
			String filePath(s_currentDir);
			filePath.append(_filePath);
			return super::open(filePath.getPtr(), _err);
		}
	};

	class FileWriter : public bx::FileWriter
	{
		typedef bx::FileWriter super;

	public:
		virtual bool open(const bx::FilePath& _filePath, bool _append, bx::Error* _err) override
		{
			String filePath(s_currentDir);
			filePath.append(_filePath);
			return super::open(filePath.getPtr(), _append, _err);
		}
	};

    #if ENTRY_CONFIG_IMPLEMENT_DEFAULT_ALLOCATOR
	bx::AllocatorI* getDefaultAllocator()
	{
BX_PRAGMA_DIAGNOSTIC_PUSH();
BX_PRAGMA_DIAGNOSTIC_IGNORED_MSVC(4459); // warning C4459: declaration of 's_allocator' hides global declaration
BX_PRAGMA_DIAGNOSTIC_IGNORED_CLANG_GCC("-Wshadow");
		static bx::DefaultAllocator s_allocator;
		return &s_allocator;
BX_PRAGMA_DIAGNOSTIC_POP();
	}
#endif // ENTRY_CONFIG_IMPLEMENT_DEFAULT_ALLOCATOR

    void init()
    {
        s_fileReader = new FileReader();
        s_fileWriter = new FileWriter();
    }

    void setCurrentDir(const char* _dir)
    {
        s_currentDir.set(_dir);
    }

	bx::FileReaderI* getFileReader()
	{
		return s_fileReader;
	}

	bx::FileWriterI* getFileWriter()
	{
		return s_fileWriter;
	}

	bx::AllocatorI*  getAllocator()
	{
		if (NULL == g_allocator)
		{
			g_allocator = getDefaultAllocator();
		}

		return g_allocator;
	}


}
