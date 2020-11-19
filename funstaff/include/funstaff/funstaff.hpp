#pragma once

#include <QSystemTrayIcon>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QDialog>
#include <QAbstractTableModel>
#include <QCloseEvent>
#include <QPainter>
#include <QPointer>

#include "ui_funstaff.h"

class funstaff : public QMainWindow
{
	Q_OBJECT

public:
	funstaff(QWidget* parent = 0);
	~funstaff() = default;

	void hide_tray_icon();

	void closeEvent(QCloseEvent * event);

protected:
	virtual bool nativeEvent(const QByteArray& eventType, void* message, long* result) override;

private:
	Ui::mainClass m_ui;
	QSystemTrayIcon m_systray{ this };
};