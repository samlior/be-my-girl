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

static bool makeQMessageBox(QWidget* parent, const QString& strText, const QString& strYes, const QString& strNo) {
	QMessageBox box(parent);
	box.setWindowTitle(QStringLiteral("ヾ(≧O≦)〃嗷~"));
	box.setText(strText);
	box.setIcon(QMessageBox::Icon::Information);
	box.addButton(strYes, QMessageBox::YesRole);
	box.addButton(strNo, QMessageBox::NoRole);
	return box.exec() == 0;
}

template<class T>
static void installFont(T* p, int size, const QString& fontName)
{
	QFont font(fontName);
	font.setPixelSize(size);
	p->setFont(font);
}

funstaff::funstaff(QWidget* parent /* = 0 */)
	: QMainWindow(parent)
{
	m_ui.setupUi(this);

	// 设置软件标题.
	this->setWindowTitle(QCoreApplication::applicationName());

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

	std::vector<QRect> rects = { QRect(530, 410, 61, 31), QRect(40, 180, 61, 31), QRect(630, 40, 61, 31), QRect(410, 370, 61, 31), QRect(220, 80, 61, 31), QRect(470, 60, 61, 31) };
	for (std::size_t i = 0; i < rects.size(); i++) {
		auto button = new DisappearButton(i, m_ui.centralWidget);
		button->setVisible(i == 0);
		button->setGeometry(rects[i]);
		button->setText(QStringLiteral("不同意"));
		button->setStyleSheet(R"(
			color: black;
			border-width: 2px;
			border-style: solid;
			border-radius: 3px;
		)");
		installFont(button, 16, QStringLiteral("华文仿宋"));
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

	installFont(m_ui.label, 27, QStringLiteral("幼圆"));
	installFont(m_ui.label_18, 31, QStringLiteral("黑体"));
	installFont(m_ui.label_2, 25, QStringLiteral("华文琥珀"));
	installFont(m_ui.label_3, 25, QStringLiteral("楷体"));
	installFont(m_ui.label_4, 23, QStringLiteral("华文宋体"));
	installFont(m_ui.label_5, 24, QStringLiteral("华文行楷"));
	installFont(m_ui.label_6, 18, QStringLiteral("方正舒体"));
	installFont(m_ui.label_7, 26, QStringLiteral("等线"));
	installFont(m_ui.pushButtonConfirm, 33, QStringLiteral("华文彩云"));

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
		auto result =
			makeQMessageBox(this, QStringLiteral("你确定要退出吗?"), QStringLiteral("没错"), QStringLiteral("我再想想")) &&
			makeQMessageBox(this, QStringLiteral("你真的确定要退出吗?"), QStringLiteral("是的"), QStringLiteral("我再考虑一下")) &&
			makeQMessageBox(this, QStringLiteral("你就这么抛弃我了? 你的良心不会痛吗?"), QStringLiteral("不会"), QStringLiteral("好吧好吧, 给你点面子")) &&
			makeQMessageBox(this, QStringLiteral("拜托你点一下同意吧, 不然我会被打哦"), QStringLiteral("你被打和我有什么关系?"), QStringLiteral("我再考虑考虑")) &&
			makeQMessageBox(this, QStringLiteral("求你了, 同意吧"), QStringLiteral("我拒绝"), QStringLiteral("好吧, 我再想想")) &&
			makeQMessageBox(this, QStringLiteral("你这个坏人! o(≧口≦)o"), QStringLiteral("我就不"), QStringLiteral("我再想想"));

		if (result)
		{
			QMessageBox::information(this, QStringLiteral("o(╥﹏╥)o"), QStringLiteral("好吧, 你赢了"));
			this->hide();

			QTimer* timer = new QTimer(this);
			timer->setInterval(2000);
			connect(timer, &QTimer::timeout, [this, timer]()
				{
					timer->stop();
					if (!this->isVisible())
					{
						this->showNormal();
						this->activateWindow();
						QMessageBox::information(this, QStringLiteral("φ(゜▽゜*)φ"), QStringLiteral("啊哈! 没想到吧, 我还在！"));
					}
				});
			timer->start();
		}
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