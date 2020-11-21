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
	box.setWindowTitle(QStringLiteral("�d(�RO�Q)���~"));
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

	// �����������.
	this->setWindowTitle(QCoreApplication::applicationName());

	// �������������.
	m_systray.setToolTip(QStringLiteral("Funstaff"));
	m_systray.setIcon(QIcon(":/funstaff/funstaff.ico"));

	QMenu* menu = new QMenu();
	QAction* reopen = new QAction(QStringLiteral(" �� ʾ (&o)"), this);
	reopen->setIcon(QIcon(":/funstaff/funstaff.ico"));
	QAction* quit = new QAction(QStringLiteral(" �� �� (&q)"), this);
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
			QMessageBox::information(this, QStringLiteral("(* �����)"), flag ? QStringLiteral("�Ҿܾ�!             ") : QStringLiteral("�ҾͲ�!             "), QMessageBox::Ok);
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
			QMessageBox::information(this, QStringLiteral("�d(�R���Q*)o"), QStringLiteral("������, ��ͬ����"), QMessageBox::Ok);
			m_shouldClose = true;
			this->close();
		});

	std::vector<QRect> rects = { QRect(530, 410, 61, 31), QRect(40, 180, 61, 31), QRect(630, 240, 61, 31), QRect(410, 300, 61, 31), QRect(220, 80, 61, 31), QRect(470, 60, 61, 31) };
	for (std::size_t i = 0; i < rects.size(); i++) {
		auto button = new DisappearButton(i, m_ui.centralWidget);
		button->setVisible(i == 0);
		button->setGeometry(rects[i]);
		button->setText(QStringLiteral("��ͬ��"));
		button->setStyleSheet(R"(
			color: black;
			border-width: 2px;
			border-style: solid;
			border-radius: 3px;
		)");
		installFont(button, 16, QStringLiteral("���ķ���"));
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
					QMessageBox::information(this, QStringLiteral("�r(�s���t)�q"), QStringLiteral("��׷�˱�׷��, �Ͻ���ͬ���!"), QMessageBox::Ok);
				}
			});
	}

	installFont(m_ui.label, 27, QStringLiteral("��Բ"));
	installFont(m_ui.label_18, 31, QStringLiteral("����"));
	installFont(m_ui.label_2, 25, QStringLiteral("��������"));
	installFont(m_ui.label_3, 25, QStringLiteral("����"));
	installFont(m_ui.label_4, 23, QStringLiteral("��������"));
	installFont(m_ui.label_5, 24, QStringLiteral("�����п�"));
	installFont(m_ui.label_6, 18, QStringLiteral("��������"));
	installFont(m_ui.label_7, 26, QStringLiteral("����"));
	installFont(m_ui.pushButtonConfirm, 33, QStringLiteral("���Ĳ���"));

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
			makeQMessageBox(this, QStringLiteral("��ȷ��Ҫ�˳���?"), QStringLiteral("û��"), QStringLiteral("��������")) &&
			makeQMessageBox(this, QStringLiteral("�����ȷ��Ҫ�˳���?"), QStringLiteral("�ǵ�"), QStringLiteral("���ٿ���һ��")) &&
			makeQMessageBox(this, QStringLiteral("�����ô��������? ������Ĳ���ʹ��?"), QStringLiteral("����"), QStringLiteral("�ðɺð�, ���������")) &&
			makeQMessageBox(this, QStringLiteral("�������һ��ͬ���, ��Ȼ�һᱻ��Ŷ"), QStringLiteral("�㱻�������ʲô��ϵ?"), QStringLiteral("���ٿ��ǿ���")) &&
			makeQMessageBox(this, QStringLiteral("������, ͬ���"), QStringLiteral("�Ҿܾ�"), QStringLiteral("�ð�, ��������")) &&
			makeQMessageBox(this, QStringLiteral("���������! o(�R�ڨQ)o"), QStringLiteral("�ҾͲ�"), QStringLiteral("��������"));

		if (result)
		{
			QMessageBox::information(this, QStringLiteral("o(�i�n�i)o"), QStringLiteral("�ð�, ��Ӯ��"));
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
						QMessageBox::information(this, QStringLiteral("��(�b���b*)��"), QStringLiteral("����! û�뵽��, �һ��ڣ�"));
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