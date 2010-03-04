#include "simplecontactlistmodel.h"
#include "libqutim/notificationslayer.h"
#include <QDebug>
#include "libqutim/messagesession.h"
#include "libqutim/status.h"
#include <libqutim/debug.h>
#include <libqutim/icon.h>


namespace Core
{
	namespace SimpleContactList
	{
		struct ModelPrivate
		{
			QList<TagItem *> tags;
			QHash<QString, TagItem *> tags_hash;
			QMap<Contact *, ContactData::Ptr> contacts;
		};

		Model::Model(QObject *parent) : QAbstractItemModel(parent), p(new ModelPrivate)
		{
		}

		Model::~Model()
		{
		}

		QModelIndex Model::index(int row, int, const QModelIndex &parent) const
		{
			if(row < 0)
				return QModelIndex();
			switch(getItemType(parent))
			{
			case TagType: {
				TagItem *item = reinterpret_cast<TagItem *>(parent.internalPointer());
				if(item->contacts.size() <= row)
					return QModelIndex();
				return createIndex(row, 0, item->contacts.at(row));
				}
			case ContactType:
				return QModelIndex();
			default:
				if(p->tags.size() <= row)
					return QModelIndex();
				return createIndex(row, 0, p->tags.at(row));
			}
		}

		QModelIndex Model::parent(const QModelIndex &child) const
		{
			switch(getItemType(child))
			{
			case ContactType: {
				ContactItem *item = reinterpret_cast<ContactItem *>(child.internalPointer());
				return createIndex(p->tags.indexOf(item->parent), 0, item->parent);
				}
			case TagType:
			default:
				return QModelIndex();
			}
		}

		int Model::rowCount(const QModelIndex &parent) const
		{
			switch(getItemType(parent))
			{
			case TagType:
				return reinterpret_cast<TagItem *>(parent.internalPointer())->contacts.size();
			case ContactType:
				return 0;
			default:
				return p->tags.size();
			}
		}

		int Model::columnCount(const QModelIndex &) const
		{
			return 1;
		}

		bool Model::hasChildren(const QModelIndex &parent) const
		{
			switch(getItemType(parent))
			{
			case TagType:
				return !reinterpret_cast<TagItem *>(parent.internalPointer())->contacts.isEmpty();
			case ContactType:
				return false;
			default:
				return !p->tags.isEmpty();
			}
		}

		QVariant Model::data(const QModelIndex &index, int role) const
		{
			switch(getItemType(index))
			{
			case ContactType:
				{
					ContactItem *item = reinterpret_cast<ContactItem *>(index.internalPointer());
					switch(role)
					{
					case Qt::DisplayRole: {
						QString name = item->data->contact->name();
						return name.isEmpty() ? item->data->contact->id() : name;
					}
					case Qt::DecorationRole:
						return item->data->contact->status().icon();
					case ItemDataType:
						return ContactType;
					default:
						return QVariant();
					}
				}
			case TagType:
				{
					TagItem *item = reinterpret_cast<TagItem *>(index.internalPointer());
					switch(role)
					{
					case Qt::DisplayRole:
						return item->name;
					case ItemDataType:
						return TagType;
					case Qt::DecorationRole:
						return Icon("feed-subscribe");
					default:
						return QVariant();
					}
				}
			default:
				return QVariant();
			}
		}
		
		bool contactLessThan (ContactItem *a, ContactItem *b) {
			if (a->data->status.type() < b->data->status.type() )
				return true;
			else if (a->data->status.type() > b->data->status.type())
				return false;
			int result = a->data->contact->title().compare(b->data->contact->title());
			if (result < 0)
				return true;
			return false;
		};		

		void Model::addContact(Contact *contact)
		{
			if(p->contacts.contains(contact))
				return;
			connect(contact, SIGNAL(destroyed(QObject*)), this, SLOT(contactDeleted(QObject*)));
            connect(contact, SIGNAL(statusChanged(qutim_sdk_0_3::Status)), SLOT(contactStatusChanged(qutim_sdk_0_3::Status)));
            connect(contact, SIGNAL(nameChanged(QString)), SLOT(contactNameChanged(QString)));
			connect(contact, SIGNAL(tagsChanged(QSet<QString>)), SLOT(contactTagsChanged(QSet<QString>)));
			QSet<QString> tags = contact->tags();
			if(tags.isEmpty())
				tags << QLatin1String("Default");
			ContactData::Ptr item_data(new ContactData);
			item_data->contact = contact;
			item_data->tags = tags;
			item_data->status = contact->status();
			p->contacts.insert(contact, item_data);
			for(QSet<QString>::const_iterator it = tags.constBegin(); it != tags.constEnd(); it++)
			{
				TagItem *tag = ensureTag(*it);
				beginInsertRows(createIndex(p->tags.indexOf(tag), 0, tag), tag->contacts.size(), tag->contacts.size());
				ContactItem *item = new ContactItem(item_data);
				
				QList<ContactItem *> contacts = tag->contacts;
				QList<ContactItem *>::const_iterator it = qLowerBound(contacts.constBegin(),contacts.constEnd(),item, contactLessThan);
				int index = it - contacts.constBegin();
				
				item->parent = tag;
				item_data->items.insert(index,item);
				tag->contacts.insert(index,item);
				endInsertRows();
			}
		}

		void Model::removeContact(Contact *contact)
		{
			contact->disconnect(this);
			ContactData::Ptr item_data = p->contacts.value(contact);
			if(!item_data)
				return;
			for(int i = 0; i < item_data->items.size(); i++)
			{
				ContactItem *item = item_data->items.at(i);
				int index = item->index();
				beginRemoveRows(createIndex(p->tags.indexOf(item->parent), 0, item->parent), index, index);
				item->parent->contacts.removeAt(index);
				endRemoveRows();
				
				if (item->parent->contacts.empty()) {
					int tag_index = p->tags.indexOf(item->parent);
					beginRemoveRows(QModelIndex(),tag_index,tag_index);
					p->tags_hash.remove(p->tags.at(tag_index)->name);
					p->tags.removeAt(tag_index);
					endRemoveRows();
				}
			}
			p->contacts.remove(contact);
		}

		bool Model::containsContact(Contact *contact) const
		{
			return p->contacts.contains(contact);
		}

		void Model::contactDeleted(QObject *obj)
		{
			if (Contact *contact = qobject_cast<Contact *>(obj))
				removeContact(contact);
		}

		void Model::contactStatusChanged(Status status)
		{
			// TODO: Add sort by status
			Contact *contact = qobject_cast<Contact *>(sender());
			ContactData::Ptr item_data = p->contacts.value(contact);
			if(!item_data)
				return;
			if(status.type() == item_data->status.type())
				return;
			const QList<ContactItem *> &items = item_data->items;
			for(int i = 0; i < items.size(); i++)
			{
				ContactItem *item = items.at(i);
				QList<ContactItem *> contacts = item->parent->contacts;
				QList<ContactItem *>::const_iterator it = qLowerBound(contacts.constBegin(),contacts.constEnd(),item, contactLessThan);
				
				int to = it - contacts.constBegin();
				int from = contacts.indexOf(item);
				if (to == -1 || to >= contacts.count() || from == to)
					continue;
				debug () << from << to;
				
				QModelIndex parent_index = createIndex(p->tags.indexOf(item->parent), 0, item->parent);
				
				beginMoveRows(parent_index,from,from, parent_index, to);
				item->parent->contacts.move(from,to);
				//item_data->items.move(from,to); //FIXME
				endMoveRows();				
				
 				//QModelIndex index = createIndex(item->index(), 0, item);
// 				dataChanged(index, index);
			}
			//if (ChatLayer::get(contact,false))
			//	return; //TODO FIXME
			Notifications::Type notify;
			if(status.type() == Status::Offline)
				notify = Notifications::Offline;
			else if(item_data->status.type() == Status::Offline)
				notify = Notifications::Online;
			else
				notify = Notifications::StatusChange;
			Notifications::sendNotification(notify, contact);
		}

		void Model::contactNameChanged(const QString &name)
		{
			// TODO: Add sort by name
			Q_UNUSED(name);
			Contact *contact = qobject_cast<Contact *>(sender());
			ContactData::Ptr item_data = p->contacts.value(contact);
			if(!item_data)
				return;
			const QList<ContactItem *> &items = item_data->items;
			for(int i = 0; i < items.size(); i++)
			{
				ContactItem *item = items.at(i);
				QModelIndex index = createIndex(item->index(), 0, item);
				dataChanged(index, index);
			}
		}

		void Model::contactTagsChanged(const QSet<QString> &tags_helper)
		{
			Contact *contact = qobject_cast<Contact *>(sender());
			ContactData::Ptr item_data = p->contacts.value(contact);
			if(!item_data)
				return;
			QSet<QString> tags = tags_helper;
			if(tags.isEmpty())
				tags << QLatin1String("Default");
			QSet<QString> to_add = tags - item_data->tags;
			for(int i = 0, size = item_data->items.size(); i < size; i++)
			{
				ContactItem *item = item_data->items.at(i);
				if(tags.contains(item->parent->name))
					continue;
				int index = item->index();
				beginRemoveRows(createIndex(p->tags.indexOf(item->parent), 0, item->parent), index, index);
				item->parent->contacts.removeAt(index);
				item_data->items.removeAt(i);
				i--;
				size--;
				endRemoveRows();
			}
			for(QSet<QString>::const_iterator it = to_add.constBegin(); it != to_add.constEnd(); it++)
			{
				TagItem *tag = ensureTag(*it);
				beginInsertRows(createIndex(p->tags.indexOf(tag), 0, tag), tag->contacts.size(), tag->contacts.size());
				ContactItem *item = new ContactItem(item_data);
				item->parent = tag;
				item_data->items << item;
				tag->contacts << item;
				endInsertRows();
			}
			item_data->tags = tags;
		}

		TagItem *Model::ensureTag(const QString &name)
		{
			TagItem *tag = 0;
			if(!(tag = p->tags_hash.value(name, 0)))
			{
				beginInsertRows(QModelIndex(), p->tags.size(), p->tags.size());
				tag = new TagItem;
				tag->name = name;
				p->tags_hash.insert(tag->name, tag);
				p->tags << tag;
				endInsertRows();
			}
			return tag;
		}
		
		QVariant Model::headerData(int section, Qt::Orientation orientation, int role) const
		{
 			if (orientation == Qt::Horizontal)
 				return tr("Contacts");
			
			return QAbstractItemModel::headerData(section, orientation, role);
		}

	}
}
