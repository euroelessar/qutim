#ifndef MOBILECONTACTINFO_H
#define MOBILECONTACTINFO_H

#include "QWidget"
#include <QScrollArea>
#include <libqutim/buddy.h>
#include <libqutim/inforequest.h>
#include <libqutim/actiontoolbar.h>

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
		MobileContactInfoWindow();
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
		QScopedPointer<QWidget> avatarWidget;
		QScopedPointer<QWidget> dataWidget;
		ActionToolBar *toolBar;
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
	private:
		QPointer<MobileContactInfoWindow> info;
	};

}

#endif // MOBILECONTACTINFO_H
