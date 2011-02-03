#ifndef MOBILECONTACTINFO_H
#define MOBILECONTACTINFO_H

#include "QWidget"
#include <QScrollArea>
#include <qutim/buddy.h>
#include <qutim/inforequest.h>
#include <qutim/actionbox.h>
#include <qutim/status.h>

class QLabel;
class QGridLayout;
class QVBoxLayout;

namespace Core
{
using namespace qutim_sdk_0_3;

typedef InfoRequestCheckSupportEvent::SupportType RequestType;

class MobileContactInfoWindow : public QScrollArea
{
	Q_OBJECT
public:
	MobileContactInfoWindow(QWidget *parent = 0);
	void setObject(QObject *object, RequestType type);
private slots:
	void onRequestStateChanged(InfoRequest::State state);
	void onRequestButton();
	void onSaveButton();
private:
	void setRequest(InfoRequest *request);
	DataItem filterItems(const DataItem &item, bool readOnly = false);
	void filterItemsHelper(const DataItem &item, DataItem &result, bool readOnly);
	bool isItemEmpty(const DataItem &item);
private:
	InfoRequest *request;
	QObject *object;
	bool readWrite;
	QVBoxLayout *layout;
	QScrollArea *scrollArea;
	QScopedPointer<AbstractDataForm> avatarWidget;
	QScopedPointer<AbstractDataForm> dataWidget;
	ActionBox *actionBox;
	QAction *saveAction;
};

class MobileContactInfo : public QObject
{
	Q_OBJECT
	Q_CLASSINFO("Service", "ContactInfo")
	Q_CLASSINFO("Uses", "IconLoader")
	Q_CLASSINFO("Uses", "DataFormsBackend")
public:
	MobileContactInfo();
	bool event(QEvent *);
public slots:
	void show(QObject *object);
private slots:
	void onShow(QObject *controller);
	void onAccountStatusChanged(qutim_sdk_0_3::Status);
private:
	QPointer<MobileContactInfoWindow> info;
	ActionGenerator *m_gen;
};

}

#endif // MOBILECONTACTINFO_H
