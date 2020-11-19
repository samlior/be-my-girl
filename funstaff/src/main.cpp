#ifdef _WIN32
#define QT_QPA_PLATFORM_WINDOWS
#endif

#ifdef QT_STATICPLUGIN
#include <QtPlugin>
Q_IMPORT_PLUGIN(QICOPlugin);

#if defined(QT_QPA_PLATFORM_XCB)
Q_IMPORT_PLUGIN(QXcbIntegrationPlugin);
#elif defined(QT_QPA_PLATFORM_WINDOWS)
Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin);
Q_IMPORT_PLUGIN(QWindowsVistaStylePlugin);
#elif defined(QT_QPA_PLATFORM_COCOA)
Q_IMPORT_PLUGIN(QCocoaIntegrationPlugin);
#endif

#endif

#include <QApplication>
#include <QtWidgets>
#include <QCommandLineParser>

#include <thread>


#if defined(_MSC_VER)
#if defined _M_IX86
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

#include <Objbase.h>

#pragma comment(lib, "Version.lib")
#pragma comment(lib, "Ole32.lib")

#endif

#include "funstaff/funstaff.hpp"

QFont* global_default_font = nullptr;

int main(int argc, char *argv[])
{
#if defined(WIN32)
	if (FAILED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED)))
		return -1;
#endif

#if defined(WIN32)
	std::unique_ptr<QFont> default_font_ptr(new QFont("Consolas", 12));
	global_default_font = default_font_ptr.get();
#endif

#if 0
	static const char ENV_VAR_QT_DEVICE_PIXEL_RATIO[] = "QT_DEVICE_PIXEL_RATIO";
	if (!qEnvironmentVariableIsSet(ENV_VAR_QT_DEVICE_PIXEL_RATIO)
		&& !qEnvironmentVariableIsSet("QT_AUTO_SCREEN_SCALE_FACTOR")
		&& !qEnvironmentVariableIsSet("QT_SCALE_FACTOR")
		&& !qEnvironmentVariableIsSet("QT_SCREEN_SCALE_FACTORS")) {
		QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	}
#endif

	QApplication app(argc, argv);

	// QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QCoreApplication::setOrganizationName("zhaozhao");
	QCoreApplication::setApplicationName("Wuhu");
	QCoreApplication::setApplicationVersion("1.0.0");

#ifdef _WIN32
	// 使用互斥锁
	/*
	HANDLE single_app_mutex_lock = NULL;
	single_app_mutex_lock = CreateMutexExW(NULL, L"superminer", CREATE_MUTEX_INITIAL_OWNER, EVENT_ALL_ACCESS);
	while (single_app_mutex_lock == NULL)
	{
		// SEND
		for (auto FirstQtWindow = FindWindowExW(NULL, NULL, L"Qt5QWindowIcon", NULL); FirstQtWindow != NULL; FirstQtWindow = FindWindowExW(NULL, FirstQtWindow, L"Qt5QWindowIcon", NULL))
		{
			wchar_t titlestring[255];
			int s;
			if ((s = GetWindowTextW(FirstQtWindow, titlestring, sizeof(titlestring))) > 0)
			{
				// do regex match
				boost::wsmatch m;
				if (boost::regex_match(std::wstring(titlestring, s), m, boost::wregex(L"Superminer.*")))
				{
					SendMessage(FirstQtWindow, WM_USER + 33, 0, 0);
					ExitProcess(0);
				}
			}
		}
		Sleep(1);
	}
	*/

	// 启动守护进程.
	/*
	{
		STARTUPINFOA si = { 0 };
		PROCESS_INFORMATION pi = { 0 };

		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		ZeroMemory(&pi, sizeof(pi));

		CreateProcessA(NULL, (LPSTR)"C:\\chs\\deamon.exe", NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);

		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
	*/

#endif

	funstaff q;

#ifdef _WIN32
	/*
	QObject::connect(&q, &superminer::miner::about_to_restart,  &q, [single_app_mutex_lock](){
		CloseHandle(single_app_mutex_lock);
	}, Qt::DirectConnection);
	*/
#endif

	auto exitcode = app.exec();

	return exitcode;
}
