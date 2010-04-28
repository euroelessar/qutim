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

class MainWindow : public QWidget
{
	Q_OBJECT
public:
	MainWindow();
	void setBuddy(Buddy *buddy, InfoRequest *request);
	void setAccount(Account *account, InfoRequest *request);
private slots:
	void onRequestStateChanged(InfoRequest::State state);
	void onRequestButton();
	void onSaveButton();
private:
	void dump(const InfoItem &item, int ident);
	void setRequest(InfoRequest *request);
	void addItems(const InfoItem &items);
	QString summary(const InfoItem &item);
private:
	Ui::userInformationClass ui;
	InfoRequest *request;
	union {
		Buddy *m_buddy;
		Account *m_account;
	};
	bool m_accountInfo;
};

class ContactInfo : public QObject
{
	Q_OBJECT
	Q_CLASSINFO("Service", "ContactInfo")
	Q_CLASSINFO("Uses", "IconLoader")
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
