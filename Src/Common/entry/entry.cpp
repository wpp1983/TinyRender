#include "entry_p.h"


#include "defines.h"

extern "C" int _main_(int _argc, char** _argv);

namespace Entry
{
    static uint32_t s_debug = BGFX_DEBUG_NONE;
	static uint32_t s_reset = BGFX_RESET_NONE;
	static uint32_t s_width = ENTRY_DEFAULT_WIDTH;
	static uint32_t s_height = ENTRY_DEFAULT_HEIGHT;
	static bool s_exit = false;

class FileReader
{
    public:
    FileReader(const char* _path)
    {
    }
};

class FileWriter
{
    public:
    FileWriter(const char* _path)
    {
    }
};

	void setCurrentDir(const char* _dir)
	{
	}

	static AppInterface*    s_currentApp = nullptr;

    static AppInterface* getCurrentApp(AppInterface* _set = nullptr)
	{
		if (nullptr != _set)
		{
			s_currentApp = _set;
		}
		else if (nullptr == s_currentApp)
		{
		}

		return s_currentApp;
	}

    

	struct AppInternal
	{
		AppInterface* m_next;
		const char* m_name;
		const char* m_description;
		const char* m_url;
	};

    AppInterface::AppInterface(const char* _name, const char* _description, const char* _url)
    : m_name(_name), m_description(_description), m_url(_url)
	{
		s_currentApp = this;
	}

    AppInterface::~AppInterface()
	{
        s_currentApp = nullptr;
    }

    const char* AppInterface::getName() const
	{
		return m_name;
	}

    const char* AppInterface::getDescription() const    
	{
		return m_description;
	}

    const char* AppInterface::getUrl() const
	{
		return m_url;
	}

    int runApp(AppInterface* _app, int _argc, const char* const* _argv)
	{
        // setWindowSize(kDefaultWindowHandle, s_width, s_height);

		_app->Initialize(_argc, _argv, s_width, s_height);

		while (!s_exit)
		{
			_app->Update();
		}

		_app->Shutdown();

		return 0;
    }

    int main(int _argc, char** _argv)
	{
        return _main_(_argc, _argv);
    }
    

}