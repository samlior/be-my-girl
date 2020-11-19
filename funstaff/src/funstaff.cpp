#include <QTimer>
#include <QMenu>
#include <QMessageBox>
#include <QCheckBox>
#include <QSettings>

#include "funstaff/funstaff.hpp"
#include <windows.h>

funstaff::funstaff(QWidget* parent)
	: QMainWindow(parent)
{
	m_ui.setupUi(this);

	// 设置软件标题.
	this->setWindowTitle(QCoreApplication::applicationName() + "-v" + QCoreApplication::applicationVersion());

	// 设置任务栏相关.
	m_systray.setToolTip(QStringLiteral("Funstaff"));
	m_systray.setIcon(QIcon(":/funstaff/funstaff.ico"));

	QMenu* menu = new QMenu();
	QAction* reopen = new QAction(QStringLiteral(" 显 示 (&o)"), this);
	reopen->setIcon(QIcon(":/funstaff/funstaff.ico"));
	QAction* quit = new QAction(QStringLiteral(" 退 出 (&q)"), this);
	quit->setIcon(QIcon(":/funstaff/quit.ico"));
	menu->addAction(reopen);
	menu->addAction(quit);
	m_systray.setContextMenu(menu);
	m_systray.show();

	connect(reopen, &QAction::triggered, [this]() mutable
		{
			this->showNormal();
			this->activateWindow();
		});

	connect(quit, &QAction::triggered, [this]() mutable
		{
			this->close();
		});

	connect(&m_systray, &QSystemTrayIcon::activated, this, [this](QSystemTrayIcon::ActivationReason ar) mutable
		{
			if (ar == QSystemTrayIcon::DoubleClick)
			{
				this->showNormal();
				this->activateWindow();
			}
		});

	this->show();
}

void funstaff::closeEvent(QCloseEvent* event)
{
	this->hide();
}

void funstaff::hide_tray_icon()
{
	m_systray.hide();
}

bool funstaff::nativeEvent(const QByteArray& eventType, void* message, long* result)
{
#ifdef _WIN32	
	auto _msg = reinterpret_cast<MSG*>(message);

	if (_msg->message == (WM_USER + 33))
	{
		QMetaObject::invokeMethod(this, "showNormal", Qt::QueuedConnection);
		QMetaObject::invokeMethod(this, "activateWindow", Qt::QueuedConnection);
		return true;
	}
#endif

	return false;
}