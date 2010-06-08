#ifndef CONTACTINFO_H
#define CONTACTINFO_H

#include "QWidget"
#include "ui_userinformation.h"
#include <libqutim/buddy.h>
#include <libqutim/inforequest.h>

class QLabel;
class QGridLayout;
class QVBoxLayout;

namespace Core
{
using namespace qutim_sdk_0_3;

typedef InfoRequestCheckSupportEvent::SupportType RequestType;

class MainWindow : public QWidget
{
	Q_OBJECT
public:
	MainWindow();
	void setObject(QObject *object, RequestType type);
private slots:
	void onRequestStateChanged(InfoRequest::State state);
	void onRequestButton();
	void onSaveButton();
private:
	void setRequest(InfoRequest *request);
	void addItems(const DataItem &items);
	QWidget *getPage(DataItem item);
	QString summary(const DataItem &item);
private:
	Ui::userInformationClass ui;
	InfoRequest *request;
	QObject *object;
	bool readWrite;
	QScopedPointer<QWidget> avatarWidget;
};

class ContactInfo : public QObject
{
	Q_OBJECT
	Q_CLASSINFO("Service", "ContactInfo")
	Q_CLASSINFO("Uses", "IconLoader")
	Q_CLASSINFO("Uses", "DataFormsBackend")
public:
	ContactInfo();
public slots:
	void show(QObject *object);
private slots:
	void onShow();
private:
	QPointer<MainWindow> info;
};

}

#endif // CONTACTINFO_H
