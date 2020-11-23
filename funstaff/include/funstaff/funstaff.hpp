#pragma once

#include <QSystemTrayIcon>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QDialog>
#include <QtWidgets/QPushButton>
#include <QAbstractTableModel>
#include <QCloseEvent>
#include <QPainter>
#include <QPointer>

#include "ui_funstaff.h"

class DisappearButton : public QPushButton
{
	Q_OBJECT

public:
	DisappearButton(std::size_t id, QWidget* parent = 0);
	~DisappearButton() = default;

Q_SIGNALS:
	void mouse_enter(std::size_t id);

protected:
	virtual void enterEvent(QEvent* event) override;

private:
	std::size_t m_id;
};

class funstaff : public QMainWindow
{
	Q_OBJECT

public:
	funstaff(QWidget* parent = 0);
	~funstaff() = default;

	void hide_tray_icon();

	void closeEvent(QCloseEvent * event);

Q_SIGNALS:
	void sig_close();

protected:
	virtual bool nativeEvent(const QByteArray& eventType, void* message, long* result) override;

private:
	bool m_shouldClose = false;
	Ui::mainClass m_ui;
	QSystemTrayIcon m_systray{ this };
	std::vector<DisappearButton*> m_buttons;
};