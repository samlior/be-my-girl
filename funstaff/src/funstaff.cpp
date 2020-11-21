#include <QTimer>
#include <QMenu>
#include <QMessageBox>
#include <QCheckBox>
#include <QSettings>
#include <QMovie>
#include <QDebug>

#ifdef _WIN32
#include <windows.h>
#endif

#include "funstaff/funstaff.hpp"

DisappearButton::DisappearButton(std::size_t id, QWidget* parent /* = 0 */)
	: m_id(id)
	, QPushButton(parent)
{
}

void DisappearButton::enterEvent(QEvent* event)
{
	emit mouse_enter(m_id);
}

funstaff::funstaff(QWidget* parent /* = 0 */)
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
			this->showNormal();
			this->activateWindow();
			static bool flag = true;
			QMessageBox::information(this, QStringLiteral("(* ￣︿￣)"), flag ? QStringLiteral("我拒绝!             ") : QStringLiteral("我就不!             "), QMessageBox::Ok);
			flag = !flag;
		});

	connect(&m_systray, &QSystemTrayIcon::activated, this, [this](QSystemTrayIcon::ActivationReason ar) mutable
		{
			if (ar == QSystemTrayIcon::DoubleClick)
			{
				this->showNormal();
				this->activateWindow();
			}
		});

	connect(m_ui.pushButtonConfirm, &QPushButton::clicked, this, [this]() mutable
		{
			QMessageBox::information(this, QStringLiteral("ヾ(≧▽≦*)o"), QStringLiteral("嘻嘻嘻, 你同意了"), QMessageBox::Ok);
			m_shouldClose = true;
			this->close();
		});

	std::vector<QRect> rects = { QRect(530, 410, 61, 31), QRect(40, 180, 61, 31), QRect(630, 240, 61, 31), QRect(410, 300, 61, 31), QRect(220, 80, 61, 31), QRect(470, 60, 61, 31) };
	for (std::size_t i = 0; i < rects.size(); i++) {
		auto button = new DisappearButton(i, m_ui.centralWidget);
		button->setVisible(i == 0);
		button->setGeometry(rects[i]);
		button->setText(QStringLiteral("不同意"));
		button->setStyleSheet(R"(
			font: 8pt "华文仿宋";
			color: black;
			border-width: 2px;
			border-style: solid;
			border-radius: 3px;
		)");
		m_buttons.push_back(button);

		connect(button, &DisappearButton::mouse_enter, this, [this, button](std::size_t id) mutable
			{
				id = id == m_buttons.size() - 1 ? 0 : id + 1;
				button->setVisible(false);
				m_buttons[id]->setVisible(true);

				static std::size_t count = 0;
				if (++count == m_buttons.size())
				{
					count = 0;
					QMessageBox::information(this, QStringLiteral("╮(╯▽╰)╭"), QStringLiteral("别追了别追了, 赶紧点同意吧!"), QMessageBox::Ok);
				}
			});
	}

	this->show();
}

void funstaff::closeEvent(QCloseEvent* event)
{
	if (m_shouldClose)
	{
		event->accept();
		emit sig_close();
	}
	else
	{
		this->hide();
		event->ignore();
	}
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