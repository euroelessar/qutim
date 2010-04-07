#ifndef MANAGER_H
#define MANAGER_H

#include <QObject>
#include <QQueue>
#include "messaging.h"

class QStandardItem;
class QAbstractItemModel;
class QStandardItemModel;
namespace qutim_sdk_0_3 {
	class Contact;
}
namespace MassMessaging
{
	
	class Manager : public QObject
	{
		Q_OBJECT
	public:
		Manager(QObject* parent = 0);
		virtual ~Manager();
		QAbstractItemModel *model() const;
	public slots:
		void reload();
		void start();
		void stop();
	signals:
		void update (const uint &completed, const uint &total, const QString &text);
	private:
		QList<QStandardItem *> m_contacts;
		QQueue<QStandardItem *> m_recievers;
		QStandardItemModel *m_model;
	};
}

#endif // MANAGER_H
