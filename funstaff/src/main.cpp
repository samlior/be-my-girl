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

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	QCoreApplication::setOrganizationName("Zhaozhao");
	QCoreApplication::setApplicationName(QStringLiteral("嘻嘻嘻Xixixi"));
	QCoreApplication::setApplicationVersion("1.0.0");

#ifdef _WIN32
	// 使用互斥锁
	HANDLE single_app_mutex_lock = NULL;
	single_app_mutex_lock = CreateMutexExW(NULL, L"Zhaozhao", CREATE_MUTEX_INITIAL_OWNER, EVENT_ALL_ACCESS);
	while (single_app_mutex_lock == NULL)
	{
		// SEND
		for (auto FirstQtWindow = FindWindowExW(NULL, NULL, L"Qt5QWindowIcon", NULL); FirstQtWindow != NULL; FirstQtWindow = FindWindowExW(NULL, FirstQtWindow, L"Qt5QWindowIcon", NULL))
		{
			wchar_t titlestring[255];
			int s;
			if ((s = GetWindowTextW(FirstQtWindow, titlestring, sizeof(titlestring))) > 0)
			{
				std::wstring wstr{ titlestring, (std::size_t)s };
				if (wstr.find(L"Xixixi") != std::wstring::npos)
				{
					SendMessage(FirstQtWindow, WM_USER + 33, 0, 0);
					ExitProcess(0);
				}
			}
		}
		Sleep(1);
	}
#endif

	funstaff q;

#ifdef _WIN32
	QObject::connect(&q, &funstaff::close,  &q, [single_app_mutex_lock](){
		CloseHandle(single_app_mutex_lock);
	}, Qt::DirectConnection);
#endif

	auto exitcode = app.exec();

	return exitcode;
}
