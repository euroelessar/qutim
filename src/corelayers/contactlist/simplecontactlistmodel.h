#ifndef SIMPLECONTACTLISTMODEL_H
#define SIMPLECONTACTLISTMODEL_H

#include <QAbstractItemModel>
#include "simplecontactlistitem.h"

namespace Core
{
	namespace SimpleContactList
	{
		struct ModelPrivate;

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
		protected slots:
			void contactDeleted(QObject *obj);
			void contactStatusChanged(qutim_sdk_0_3::Status status);
			void contactNameChanged(const QString &name);
			void contactTagsChanged(const QSet<QString> &tags);
			void onHideShowOffline();
			void onFilterList(const QString &filter);
			void onContactRenameAction();
			void onContactRenameResult(const QString &name);
			void onCollapsed(const QModelIndex &index);
			void onTagsEditAction();
			void onExpanded(const QModelIndex &index);
		private:
			void filterAllList();
			bool isVisible(ContactItem *item);
			void hideContact(int index, const QModelIndex &tagIndex, bool hide);
			void recheckTag(TagItem *item, int index = -1);
			TagItem *ensureTag(const QString &name);
//			QModelIndex createIndex(
			QScopedPointer<ModelPrivate> p;
		};
	}
}

#endif // SIMPLECONTACTLISTMODEL_H
