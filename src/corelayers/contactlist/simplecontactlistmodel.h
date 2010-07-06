#ifndef SIMPLECONTACTLISTMODEL_H
#define SIMPLECONTACTLISTMODEL_H

#include <QAbstractItemModel>
#include "simplecontactlistitem.h"

namespace Core
{
	namespace SimpleContactList
	{
		struct ModelPrivate;
		class Model;
		
		struct ChangeEvent;
		
		class AddRemoveContactActionGenerator : public ActionGenerator
		{
		public:
			AddRemoveContactActionGenerator(Model *model);
		protected:
			virtual void showImpl(QAction* action, QObject* obj);
		};

		class Model : public QAbstractItemModel
		{
			Q_OBJECT
		public:
			Model(QObject *parent);
			virtual ~Model();
			virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
			virtual QModelIndex parent(const QModelIndex &child) const;
			virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
			virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
			virtual bool hasChildren(const QModelIndex &parent = QModelIndex()) const;
			virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
			virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
			void addContact(Contact *contact);
			void removeContact(Contact *contact);
			bool containsContact(Contact *contact) const;
			bool setData(const QModelIndex &index, const QVariant &value, int role);
			Qt::ItemFlags flags(const QModelIndex &index) const;
			Qt::DropActions supportedDropActions() const;
			QStringList mimeTypes() const;
			QMimeData *mimeData(const QModelIndexList &indexes) const;
			bool dropMimeData(const QMimeData *data, Qt::DropAction action,
							  int row, int column, const QModelIndex &parent);
			bool showOffline() const;
			QStringList tags() const;
			QStringList selectedTags() const;

			void processEvent(ChangeEvent *ev);
			bool eventFilter(QObject *obj, QEvent *ev);
		public slots:
			void onFilterList(const QStringList &tags);
		protected slots:
			void contactDeleted(QObject *obj);
			void contactStatusChanged(qutim_sdk_0_3::Status status);
			void contactNameChanged(const QString &name);
			void contactTagsChanged(const QStringList &tags);
			void onHideShowOffline();
			void onFilterList(const QString &filter);
			void onContactRenameAction(QObject*);
			void onContactRenameResult(const QString &name);
			void onContactAddRemoveAction(QObject*);
			void onCollapsed(const QModelIndex &index);
			void onTagsEditAction(QObject*);
			void onExpanded(const QModelIndex &index);
			void onContactInListChanged(bool isInList);
		protected:
			void timerEvent(QTimerEvent *ev);
		private:
			void filterAllList();
			bool isVisible(ContactItem *item);
			void hideContact(int index, const QModelIndex &tagIndex, bool hide);
			void removeFromContactList(Contact *contact, bool deleted);
			void recheckTag(TagItem *item, int index = -1);
			TagItem *ensureTag(const QString &name);
//			QModelIndex createIndex(
			QScopedPointer<ModelPrivate> p;
		};
	}
}

#endif // SIMPLECONTACTLISTMODEL_H
