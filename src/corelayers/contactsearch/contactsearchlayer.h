#ifndef CONTACTSEARCHLAYER_H
#define CONTACTSEARCHLAYER_H

#include <QWidget>
#include <QPointer>
#include <QSharedPointer>
#include "libqutim/debug.h"
#include "libqutim/contactsearch.h"
#include "ui_searchcontactform.h"

namespace Core
{
using namespace qutim_sdk_0_3;

typedef QSharedPointer<ContactSearchFactory> FactoryPtr;
typedef QSharedPointer<ContactSearchRequest> RequestPtr;

class ContactModel : public QAbstractTableModel
{
	Q_OBJECT
public:
	RequestPtr request() { return m_request; }
	void setRequest(const RequestPtr &request);
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
private slots:
	void contactAboutToBeAdded(int row);
	void contactAdded(int row);
private:
	friend class ContactSearchForm;
	RequestPtr m_request;
};

class ContactSearchForm : public QWidget
{
	Q_OBJECT
public:
	ContactSearchForm(QWidget *parent = 0);
private slots:
	void updateRequestBox();
	void startSearch();
	void updateRequest(int index);
	void updateFields();
	void clearFields();
	void cancelSearch();
	void done(bool ok);
	void addContact();
	void updateService();
	void updateServiceBox();
protected:
	bool event(QEvent *e);
	void setState(bool search);
private:
	void addContact(int row);
	Ui::SearchContactForm ui;
	QList<FactoryPtr> m_factories;
	struct RequestBoxItem
	{
		RequestBoxItem() {}
		RequestBoxItem(const FactoryPtr &f, const QString &n) :
			factory(f), name(n)
		{}
		FactoryPtr factory;
		QString name;
	};
	QList<RequestBoxItem> m_requestItems;
	RequestPtr m_currentRequest;
	QPointer<QWidget> m_searchWidget;
	ContactModel m_contactModel;
	bool requestListUpdating;
};

class ContactSearch : public QObject
{
	Q_OBJECT
	Q_CLASSINFO("Service", "ContactSearch")
	Q_CLASSINFO("Uses", "IconLoader")
	Q_CLASSINFO("Uses", "ContactList")
public:
    ContactSearch();
public slots:
	void show(QObject*);
private:
	QPointer<ContactSearchForm> searchContactForm;
};

}

#endif // CONTACTSEARCHLAYER_H
