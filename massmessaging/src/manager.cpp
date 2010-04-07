#include "manager.h"
#include <QStandardItemModel>
#include <qutim/protocol.h>
#include <qutim/account.h>
#include <qutim/icon.h>
#include <qutim/contact.h>

namespace MassMessaging
{
	class MessagingItem : public QStandardItem
	{
	public:
		MessagingItem(const QString &title) : QStandardItem(title)
		{
			setCheckable(true);
			setEditable(false);			
		};
		virtual void setData(const QVariant& value, int role = Qt::UserRole + 1)
		{
			switch (role) {
				case Qt::CheckStateRole: {
					for (int i=0;i!=rowCount();i++)
						child(i,0)->setData(value,role);
					break;
				}
				default:
					break;
			}
			QStandardItem::setData(value,role);
		};
	};
	
	Manager::Manager(QObject* parent): QObject(parent)
	{
		m_model =  new QStandardItemModel(this);
	}

	QAbstractItemModel* Manager::model() const
	{
		return m_model;
	}
	void Manager::reload()
	{
		m_model->clear();
		m_recievers.clear();
		m_contacts.clear();
		foreach(Protocol *proto, allProtocols()) {
			QStandardItem *proto_item = new MessagingItem(proto->id());
			proto_item->setIcon(Icon("applications-internet"));
			foreach(Account *account, proto->accounts()) {
				QStandardItem *account_item = new MessagingItem (account->id());
				account_item->setToolTip(account->name());
				account_item->setIcon(Icon("applications-internet"));
				foreach (Contact *contact, account->findChildren<Contact *>()) {
					QStandardItem *contact_item = new MessagingItem (contact->title());
					contact_item->setIcon(contact->status().icon());
					contact_item->setData(qVariantFromValue(contact),Qt::UserRole);
					
					account_item->appendRow(contact_item);
					m_contacts.append(contact_item);
				}
			
				proto_item->appendRow(account_item);
			}
			if (proto_item->columnCount())
				m_model->appendRow(proto_item);
			else
				delete proto_item;
		}
	}
	void Manager::start()
	{
		foreach (QStandardItem *item, m_contacts) {
			Qt::CheckState state = static_cast<Qt::CheckState>(item->data(Qt::CheckStateRole).value<int>());
			if (state == Qt::Checked)
				m_recievers.enqueue(item);
		}
	}
	void Manager::stop()
	{

	}
	Manager::~Manager()
	{

	}


}