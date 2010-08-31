#include "simplecontactlistmodel.h"
#include <qutim/notificationslayer.h>
#include <QDebug>
#include <qutim/messagesession.h>
#include <qutim/status.h>
#include <QTreeView>
#include <qutim/icon.h>
#include <qutim/configbase.h>
#include <qutim/debug.h>
#include <qutim/metacontact.h>
#include <qutim/metacontactmanager.h>
#include <QBasicTimer>
#include <QInputDialog>
#include <QMimeData>
#include <QMessageBox>
#include "simpletagseditor/simpletagseditor.h"
#include <qutim/mimeobjectdata.h>
#include <qutim/event.h>
#include <QApplication>

#define QUTIM_MIME_CONTACT_INTERNAL QLatin1String("application/qutim-contact-internal")

namespace Core
{
	namespace SimpleContactList
	{
		struct ChangeEvent
		{
			enum Type { ChangeTags, MergeContacts } type;
			void *child;
			void *parent;
			
		};
		
		struct ModelPrivate
		{
			QTreeView *view;
			QList<TagItem *> tags;
			QHash<QString, TagItem *> tagsHash;
			QMap<Contact *, ContactData::Ptr> contacts;
			QSet<QString> closedTags;
			QList<ChangeEvent*> events;
			QBasicTimer timer;
			QString lastFilter;
			QStringList filteredTags;
			bool showOffline;		
			QBasicTimer unreadTimer;
			QMap<ChatSession*, QSet<Contact*> > unreadBySession;
			QSet<Contact*> unreadContacts;	
			bool showMessageIcon;
			QIcon unreadIcon;
			quint16 realUnitRequestEvent;
		};

		AddRemoveContactActionGenerator::AddRemoveContactActionGenerator(Model *model) :
				ActionGenerator(QIcon(), "", model, SLOT(onContactAddRemoveAction(QObject*)))
		{
			addHandler(ActionVisibilityChangedHandler,model);
		}

		Model::Model(QObject *parent) : QAbstractItemModel(parent), p(new ModelPrivate)
		{
			p->showMessageIcon = false;
			p->realUnitRequestEvent = Event::registerType("real-chatunit-request");
			p->unreadIcon = Icon(QLatin1String("mail-unread-new"));
			p->view = static_cast<QTreeView*>(parent);
			connect(p->view, SIGNAL(collapsed(QModelIndex)), this, SLOT(onCollapsed(QModelIndex)));
			connect(p->view, SIGNAL(expanded(QModelIndex)), this, SLOT(onExpanded(QModelIndex)));
			connect(ChatLayer::instance(), SIGNAL(sessionCreated(qutim_sdk_0_3::ChatSession*)),
					this, SLOT(onSessionCreated(qutim_sdk_0_3::ChatSession*)));
			ConfigGroup group = Config().group("contactList");
			p->showOffline = group.value("showOffline", true);
			p->closedTags = group.value("closedTags", QStringList()).toSet();
			ActionGenerator *gen = new ActionGenerator(Icon("user-properties"),
													   QT_TRANSLATE_NOOP("ContactList", "Rename contact"),
													   this, SLOT(onContactRenameAction(QObject*)));
			MenuController::addAction<Contact>(gen);
			gen = new ActionGenerator(Icon("feed-subscribe"),
									  QT_TRANSLATE_NOOP("ContactList", "Edit tags"),
									  this, SLOT(onTagsEditAction(QObject*)));
			MenuController::addAction<Contact>(gen);
			MenuController::addAction<Contact>(new AddRemoveContactActionGenerator(this));

			p->view->setDragEnabled(true);
			p->view->setAcceptDrops(true);
			p->view->setDropIndicatorShown(true);
		}

		Model::~Model()
		{
			ConfigGroup group = Config().group("contactList");
			group.setValue("closedTags", QStringList(p->closedTags.toList()));
			group.sync();
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
					case Qt::EditRole:
					case Qt::DisplayRole: {
							QString name = item->data->contact->name();
							return name.isEmpty() ? item->data->contact->id() : name;
						}
					case Qt::DecorationRole:
						if (p->showMessageIcon && p->unreadContacts.contains(item->data->contact))
							return p->unreadIcon;
						else
							return item->data->contact->status().icon();
					case ItemDataType:
						return ContactType;
					case ItemStatusRole:
						return qVariantFromValue(item->data->contact->status());
					case ItemAvatarRole:
						return item->data->contact->avatar();
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
					case ItemContactsCountRole:
						return item->contacts.count();
					case ItemOnlineContactsCountRole:
						return item->online;
					default:
						return QVariant();
					}
				}
			default:
				return QVariant();
			}
		}
		
		bool contactLessThan (ContactItem *a, ContactItem *b) {
			int result = a->data->status.type() - b->data->status.type();
			if (result != 0)
				return result < 0;
			return a->data->contact->title().compare(b->data->contact->title(), Qt::CaseInsensitive) < 0;
		};		

		void Model::addContact(Contact *contact)
		{
			//TODO implement more powerfull logic
//			if (!contact->isInList())
//				return;

			if(p->contacts.contains(contact))
				return;

			MetaContact *meta = qobject_cast<MetaContact*>(contact);

			if (!meta) {
				meta = static_cast<MetaContact*>(contact->metaContact());
				if (meta && p->contacts.contains(meta))
					return;
				else if (meta)
					contact = meta;
			}
			
			if (meta) {
				meta->installEventFilter(this);
				foreach (ChatUnit *unit, meta->lowerUnits()) {
					Contact *subContact = qobject_cast<Contact*>(unit);
					if (subContact && p->contacts.contains(subContact))
						removeContact(subContact);
				}
			}

			connect(contact, SIGNAL(destroyed(QObject*)),
					SLOT(contactDeleted(QObject*)));
			connect(contact, SIGNAL(statusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)),
					SLOT(contactStatusChanged(qutim_sdk_0_3::Status)));
			connect(contact, SIGNAL(nameChanged(QString,QString)),
					SLOT(contactNameChanged(QString)));
			connect(contact, SIGNAL(tagsChanged(QStringList,QStringList)),
					SLOT(contactTagsChanged(QStringList)));
			connect(contact, SIGNAL(inListChanged(bool)),
					SLOT(onContactInListChanged(bool)));

			QStringList tags = contact->tags();
			if(tags.isEmpty())
				tags << QLatin1String("Default");

			ContactData::Ptr item_data(new ContactData);
			item_data->contact = contact;
			item_data->tags = QSet<QString>::fromList(tags);
			item_data->status = contact->status();
			int counter = item_data->status.type() == Status::Offline ? 0 : 1;
			p->contacts.insert(contact, item_data);
			for(QSet<QString>::const_iterator it = item_data->tags.constBegin(); it != item_data->tags.constEnd(); it++)
			{
				TagItem *tag = ensureTag(*it);
				int tagIndex = p->tags.indexOf(tag);
				QModelIndex tagModelIndex = createIndex(tagIndex, 0, tag);
				ContactItem *item = new ContactItem(item_data);
				
				QList<ContactItem *> contacts = tag->contacts;
				QList<ContactItem *>::const_iterator contacts_it =
						qLowerBound(contacts.constBegin(), contacts.constEnd(), item, contactLessThan);
				int index = contacts_it - contacts.constBegin();

				beginInsertRows(tagModelIndex, index, index);
				item->parent = tag;
				item_data->items.insert(index,item);
				tag->contacts.insert(index,item);
				endInsertRows();
				bool show = isVisible(item);
				// hideContact will decrease it by one
				tag->visible++;
				tag->online += counter;
				if (show)
					recheckTag(tag, tagIndex);
				else
					hideContact(index, tagModelIndex, true);
			}
		}

		bool Model::containsContact(Contact *contact) const
		{
			return p->contacts.contains(contact);
		}

		bool Model::setData(const QModelIndex &index, const QVariant &value, int role)
		{
			if (role == Qt::EditRole && getItemType(index) == ContactType) {
				ContactItem *item = reinterpret_cast<ContactItem *>(index.internalPointer());
				item->data->contact->setName(value.toString());
				return true;
			}
			return false;
		}

		Qt::ItemFlags Model::flags(const QModelIndex &index) const
		{
			Qt::ItemFlags flags = QAbstractItemModel::flags(index);

			ItemType type = getItemType(index);
			if (type == TagType)
				flags |= Qt::ItemIsDropEnabled;
			else if (type == ContactType)
				flags |= Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | Qt::ItemIsEditable;

			return flags;
		}

		Qt::DropActions Model::supportedDropActions() const
		{
			return Qt::CopyAction | Qt::MoveAction;
		}

		QStringList Model::mimeTypes() const
		{
			QStringList types;
			types << QUTIM_MIME_CONTACT_INTERNAL;
			types << MimeObjectData::objectMimeType();
			return types;
		}

		QMimeData *Model::mimeData(const QModelIndexList &indexes) const
		{
			MimeObjectData *mimeData = new MimeObjectData();
			QModelIndex index = indexes.value(0);
			if (getItemType(index) != ContactType)
				return mimeData;

			ContactItem *item = reinterpret_cast<ContactItem*>(index.internalPointer());

			mimeData->setText(item->data->contact->id());

			QByteArray encodedData;
			QDataStream stream(&encodedData, QIODevice::WriteOnly);
			stream << index.row() << index.column() << qptrdiff(index.internalPointer());
			mimeData->setData(QUTIM_MIME_CONTACT_INTERNAL, encodedData);
			mimeData->setObject(item->data->contact);
			
			return mimeData;
		}

		bool Model::dropMimeData(const QMimeData *data, Qt::DropAction action,
								 int row, int column, const QModelIndex &parent)
		{
			if (action == Qt::IgnoreAction)
				return true;

			if (!data->hasFormat(QUTIM_MIME_CONTACT_INTERNAL))
				return false;

			QByteArray encodedData = data->data(QUTIM_MIME_CONTACT_INTERNAL);
			QDataStream stream(&encodedData, QIODevice::ReadOnly);
			qptrdiff internalId;
			stream >> row >> column >> internalId;

			QModelIndex index = createIndex(row, column, reinterpret_cast<void*>(internalId));
			qDebug() << Q_FUNC_INFO;
			if (getItemType(index) != ContactType
				|| (getItemType(parent) != ContactType && getItemType(parent) != TagType)) {
				return false;
			}
			
			ChangeEvent *ev = new ChangeEvent;
			ev->child = index.internalPointer();
			ev->parent = parent.internalPointer();
			qDebug() << ev->type;
			if (getItemType(parent) == TagType)
				ev->type = ChangeEvent::ChangeTags;
			else if(getItemType(parent) == ContactType)
				ev->type = ChangeEvent::MergeContacts;
			p->events << ev;
			p->timer.start(1, this);

			// We should return false
			return false;
		}

		void Model::removeFromContactList(Contact *contact, bool deleted)
		{
			ContactData::Ptr item_data = p->contacts.value(contact);
			if(!item_data)
				return;
			int counter = item_data->status.type() == Status::Offline ? 0 : -1;
			for(int i = 0; i < item_data->items.size(); i++)
			{
				ContactItem *item = item_data->items.at(i);
				item->parent->online += counter;
				int index = item->index();
				beginRemoveRows(createIndex(p->tags.indexOf(item->parent), 0, item->parent), index, index);
				item->parent->contacts.removeAt(index);
				endRemoveRows();

				int tagIndex = p->tags.indexOf(item->parent);

				if (item->parent->contacts.empty()) {
					beginRemoveRows(QModelIndex(),tagIndex,tagIndex);
					p->tagsHash.remove(p->tags.at(tagIndex)->name);
					p->tags.removeAt(tagIndex);
					endRemoveRows();
				} else if (!deleted && isVisible(item)) {
					item->parent->visible--;
					recheckTag(item->parent, tagIndex);
				}
			}
			p->contacts.remove(contact);
		}

		void Model::contactDeleted(QObject *obj)
		{
			Contact *contact = reinterpret_cast<Contact *>(obj);
			removeFromContactList(contact,true);
		}

		void Model::removeContact(Contact *contact)
		{
			Q_ASSERT(contact);
			if (MetaContact *meta = qobject_cast<MetaContact*>(contact)) {
				contact->removeEventFilter(this);
				foreach (ChatUnit *unit, meta->lowerUnits()) {
					Contact *subContact = qobject_cast<Contact*>(unit);
					if (subContact && !p->contacts.contains(subContact))
						addContact(subContact);
				}
			}
			contact->disconnect(this);
			removeFromContactList(contact,false);
		}

		void Model::contactStatusChanged(Status status)
		{
			Contact *contact = qobject_cast<Contact *>(sender());
			ContactData::Ptr item_data = p->contacts.value(contact);
			if(!item_data)
				return;
			bool statusTypeChanged = status.type() != item_data->status.type();
			int counter = 0;
			if (status.type() == Status::Offline)
				counter = statusTypeChanged ? -1 : 0;
			else if (item_data->status == Status::Offline)
				counter = +1;
			item_data->status = status;
			const QList<ContactItem *> &items = item_data->items;
			bool show = isVisible(item_data->items.value(0));
			for(int i = 0; i < items.size(); i++)
			{
				ContactItem *item = items.at(i);
				item->parent->online += counter;

				QList<ContactItem *> &contacts = item->parent->contacts;
				QModelIndex parentIndex = createIndex(p->tags.indexOf(item->parent), 0, item->parent);
				int from = contacts.indexOf(item);
				int to;
				if (statusTypeChanged) {
					QList<ContactItem *>::const_iterator it =
							qLowerBound(contacts.constBegin(), contacts.constEnd(), item, contactLessThan);
					to = it - contacts.constBegin();
				} else {
					to = from;
				}
				
				if (statusTypeChanged)
					hideContact(from, parentIndex, !show);

				if (from == to) {
					if (show) {
						QModelIndex index = createIndex(item->index(), 0, item);
						emit dataChanged(index, index);
					}
				} else {
					if (to == -1 || to >= contacts.count())
						continue;

					if (beginMoveRows(parentIndex, from, from, parentIndex, to)) {
						if (from < to)
							--to;
						contacts.move(from,to);
						//item_data->items.move(from,to); //FIXME
						endMoveRows();
					}
				}
			}
		}

		void Model::contactNameChanged(const QString &name)
		{
			Q_UNUSED(name);
			Contact *contact = qobject_cast<Contact *>(sender());
			ContactData::Ptr item_data = p->contacts.value(contact);
			if(!item_data)
				return;
			const QList<ContactItem *> &items = item_data->items;
			for(int i = 0; i < items.size(); i++)
			{
				ContactItem *item = items.at(i);
				QList<ContactItem *> contacts = item->parent->contacts;
				QList<ContactItem *>::const_iterator it =
						qLowerBound(contacts.constBegin(), contacts.constEnd(), item, contactLessThan);

				int to = it - contacts.constBegin();
				int from = contacts.indexOf(item);

				QModelIndex parentIndex = createIndex(p->tags.indexOf(item->parent), 0, item->parent);

				if (from == to) {
					QModelIndex index = createIndex(item->index(), 0, item);
					emit dataChanged(index, index);
				} else {
					if (to == -1 || to >= contacts.count())
						continue;

					if (beginMoveRows(parentIndex, from, from, parentIndex, to)) {
						if (from < to)
							--to;
						contacts.move(from,to);
						//item_data->items.move(from,to); //FIXME
						endMoveRows();
					}
				}
			}
		}

		void Model::onContactInListChanged(bool isInList)
		{
			//TODO
		}
		
		void Model::onSessionCreated(qutim_sdk_0_3::ChatSession *session)
		{
			connect(session, SIGNAL(unreadChanged(qutim_sdk_0_3::MessageList)),
					this, SLOT(onUnreadChanged(qutim_sdk_0_3::MessageList)));
		}
		
		void Model::onUnreadChanged(const qutim_sdk_0_3::MessageList &messages)
		{
			ChatSession *session = qobject_cast<ChatSession*>(sender());
			QSet<Contact*> contacts;
			QSet<ChatUnit*> chatUnits;
			for (int i = 0; i < messages.size(); i++) {
				ChatUnit *unit = const_cast<ChatUnit*>(messages.at(i).chatUnit());
				if (chatUnits.contains(unit))
					continue;
				chatUnits.insert(unit);
				Event event(p->realUnitRequestEvent);
				QCoreApplication::sendEvent(unit, &event);
				Contact *contact = event.at<Contact*>(0);
				while (unit && !contact) {
					if (!!(contact = qobject_cast<Contact*>(unit)))
						break;
					unit = unit->upperUnit();
				}
				if (Contact *meta = qobject_cast<MetaContact*>(contact->metaContact()))
					contact = meta;
				if (contact)
					contacts.insert(contact);
			}
			if (contacts.isEmpty())
				p->unreadBySession.remove(session);
			else
				p->unreadBySession.insert(session, contacts);
			foreach (const QSet<Contact*> &contactsSet, p->unreadBySession)
				contacts |= contactsSet;
			
			if (!contacts.isEmpty() && !p->unreadTimer.isActive())
				p->unreadTimer.start(500, this);
			else if (contacts.isEmpty())
				p->unreadTimer.stop();
			
			if (!p->showMessageIcon) {
				p->unreadContacts = contacts;
			} else {
				QSet<Contact*> needUpdate = p->unreadContacts;
				needUpdate.subtract(contacts);
				p->unreadContacts = contacts;
				foreach (Contact *contact, needUpdate) {
					ContactData::Ptr item_data = p->contacts.value(contact);
					for (int i = 0; i < item_data->items.size(); i++) {
						ContactItem *item = item_data->items.at(i);
						QModelIndex index = createIndex(item->index(), 0, item);
						emit dataChanged(index, index);
					}
				}
			}
		}

		void Model::contactTagsChanged(const QStringList &tags_helper)
		{
			Contact *contact = qobject_cast<Contact *>(sender());
			ContactData::Ptr item_data = p->contacts.value(contact);
			if(!item_data)
				return;
			bool show = isVisible(item_data->items.value(0));
			QSet<QString> tags;
			tags = QSet<QString>::fromList(tags_helper);
			if(tags.isEmpty())
				tags << QLatin1String("Default");
			QSet<QString> to_add = tags - item_data->tags;
			for (int i = 0, size = item_data->items.size(); i < size; i++) {
				ContactItem *item = item_data->items.at(i);
				if(tags.contains(item->parent->name))
					continue;
				int index = item->index();
				int tagIndex = p->tags.indexOf(item->parent);
				beginRemoveRows(createIndex(tagIndex, 0, item->parent), index, index);
				item->parent->contacts.removeAt(index);
				item_data->items.removeAt(i);
				i--;
				size--;
				endRemoveRows();
				if (show) {
					item->parent->visible--;
					recheckTag(item->parent, tagIndex);
				}
			}
			for (QSet<QString>::const_iterator it = to_add.constBegin(); it != to_add.constEnd(); it++) {
				TagItem *tag = ensureTag(*it);
				tag->visible++;
				ContactItem *item = new ContactItem(item_data);
				int tagIndex = p->tags.indexOf(tag);
				QModelIndex tagModelIndex = createIndex(tagIndex, 0, tag);
				QList<ContactItem *> contacts = tag->contacts;
				QList<ContactItem *>::const_iterator search_it =
						qLowerBound(contacts.constBegin(), contacts.constEnd(), item, contactLessThan);

				int index = search_it - contacts.constBegin();
				beginInsertRows(tagModelIndex, index, index);
				item->parent = tag;
				item_data->items << item;
				tag->contacts << item;
				endInsertRows();
				if (!show)
					hideContact(index, tagModelIndex, true);
			}
			item_data->tags = tags;
		}

		void Model::onHideShowOffline()
		{
			ConfigGroup group = Config().group("contactList");
			bool show = !group.value("showOffline", true);
			group.setValue("showOffline", show);
			group.sync();
			if (p->showOffline == show)
				return;
			p->showOffline = show;
			filterAllList();
		}

		void Model::onFilterList(const QString &filter)
		{
			if (filter == p->lastFilter)
				return;
			p->lastFilter = filter;
			filterAllList();
		}

		void Model::onFilterList(const QStringList &tags)
		{
			if (tags == p->filteredTags)
				return;
			p->filteredTags = tags;
			filterAllList();
		}

		void Model::onContactRenameAction(QObject *controller)
		{
			Contact *contact = qobject_cast<Contact*>(controller);
			if (!contact)
				return;
			QInputDialog *dialog = new QInputDialog(p->view);
			dialog->setAttribute(Qt::WA_QuitOnClose, false);
			dialog->setAttribute(Qt::WA_DeleteOnClose, true);
			dialog->setInputMode(QInputDialog::TextInput);
			dialog->setProperty("contact", qVariantFromValue(contact));
			centerizeWidget(dialog);
			dialog->open(this, SLOT(onContactRenameResult(QString)));
		}

		void Model::onContactAddRemoveAction(QObject *obj)
		{
			Contact *contact = qobject_cast<Contact*>(obj);
			if (!contact)
				return;
			contact->setInList(!contact->isInList());
		}

		QStringList Model::tags() const
		{
			QStringList all_tags;
			foreach (const TagItem *tag,p->tags)
				all_tags.append(tag->name);
			return all_tags;
		}

		QStringList Model::selectedTags() const
		{
			return p->filteredTags;
		}
		
		void Model::processEvent(ChangeEvent *ev)
		{
			ContactItem *item = reinterpret_cast<ContactItem*>(ev->child);
			if (ev->type == ChangeEvent::ChangeTags) {
				TagItem *tag = reinterpret_cast<TagItem*>(ev->parent);
				if (tag->name == item->parent->name)
					return;

				QSet<QString> tags = item->data->tags;
				tags.remove(item->parent->name);
				tags.insert(tag->name);
				item->data->contact->setTags(tags.toList());
				debug() << "Moving contact from" << item->data->tags << "to" << tags;
			} else if (ev->type == ChangeEvent::MergeContacts) { // MetaContacts
				ContactItem *parentItem = reinterpret_cast<ContactItem*>(ev->parent);
				if (item->data->contact == parentItem->data->contact)
					return;
				MetaContact *childMeta = qobject_cast<MetaContact*>(item->data->contact);
				MetaContact *meta = qobject_cast<MetaContact*>(parentItem->data->contact);
				
				QString text;				
				if (!childMeta && !meta) {
					text = tr("Would you like to merge contacts \"%1\" <%2> and \"%3\" <%4>?");
					text = text.arg(item->data->contact->name(),
									item->data->contact->id(),
									parentItem->data->contact->name(),
									parentItem->data->contact->id());
				} else if (childMeta && meta) {
					text = tr("Would you like to merge metacontacts \"%1\" and \"%2\"?");
					text = text.arg(childMeta->title(), meta->title());
				} else {
					text = tr("Would you like to add \"%1\" <%2> to metacontact \"%3\"?");
					Contact *c = (meta ? item : parentItem)->data->contact;
					MetaContact *m = meta ? meta : childMeta;
					text = text.arg(c->name(), c->id(), m->name());
				}
				
				if (QMessageBox::Yes == QMessageBox::question(qobject_cast<QWidget*>(QObject::parent()),
															  tr("Contacts' merging"), text,
															  QMessageBox::Yes | QMessageBox::No)) {
					if (childMeta && !meta) {
						meta = childMeta;
						childMeta = 0;
					} else if (!meta) {
						meta = MetaContactManager::instance()->createContact();
						meta->addContact(parentItem->data->contact);
					} else if (childMeta && meta) {
						foreach (ChatUnit *unit, childMeta->lowerUnits()) {
							Contact *contact = qobject_cast<Contact*>(unit);
							if (contact)
								meta->addContact(contact);
						}
						childMeta->deleteLater();
						return;
					}
					if (!childMeta)
						meta->addContact(item->data->contact);
				}
			}
		}
		
		void Model::timerEvent(QTimerEvent *timerEvent)
		{
			if (timerEvent->timerId() == p->timer.timerId()) {
				for (int i = 0; i < p->events.size(); i++) {
					processEvent(p->events.at(i));
					delete p->events.at(i);
				}
				p->events.clear();
				p->timer.stop();
				return;
			} else if (timerEvent->timerId() == p->unreadTimer.timerId()) {
				foreach (Contact *contact, p->unreadContacts) {
					ContactData::Ptr item_data = p->contacts.value(contact);
					for (int i = 0; i < item_data->items.size(); i++) {
						ContactItem *item = item_data->items.at(i);
						QModelIndex index = createIndex(item->index(), 0, item);
						emit dataChanged(index, index);
					}
				}
				p->showMessageIcon = !p->showMessageIcon;
				return;
			}
			QAbstractItemModel::timerEvent(timerEvent);
		}

		bool Model::eventFilter(QObject *obj, QEvent *ev)
		{
			if (ev->type() == MetaContactChangeEvent::eventType()) {
				MetaContactChangeEvent *metaEvent = static_cast<MetaContactChangeEvent*>(ev);
				if (metaEvent->oldMetaContact() && !metaEvent->newMetaContact())
					addContact(metaEvent->contact());
				else if (!metaEvent->oldMetaContact() && metaEvent->newMetaContact())
					removeContact(metaEvent->contact());
				return false;
			}
			return QAbstractItemModel::eventFilter(obj, ev);
		}

		bool Model::event(QEvent *ev)
		{
			if (ev->type() == ActionVisibilityChangedEvent::eventType()) {
				ActionVisibilityChangedEvent *event = static_cast<ActionVisibilityChangedEvent*>(ev);
				if (event->isVisible()) {
					QAction *action = event->action();
					Contact *contact = qobject_cast<Contact*>(event->controller());
					if (!contact)
						return false;
					if (contact->isInList()) {
						action->setIcon(Icon("list-remove"));
						action->setText(QT_TRANSLATE_NOOP("ContactList", "Remove contact"));
					} else {
						action->setIcon(Icon("list-add"));
						action->setText(QT_TRANSLATE_NOOP("ContactList", "Add to contact list"));
					}
					action->setEnabled(contact->account()->status() != qutim_sdk_0_3::Status::Offline); //experiment
					return true;
				}
			}
			return QObject::event(ev);
		}

		void Model::onTagsEditAction(QObject *controller)
		{
			Contact *contact = qobject_cast<Contact*>(controller);
			if (!contact)
				return;
			SimpleTagsEditor *editor = new SimpleTagsEditor (contact);
			centerizeWidget(editor);


			editor->setTags(tags());
			editor->load();
			editor->show();
		}

		void Model::onContactRenameResult(const QString &name)
		{
			Contact *contact = sender()->property("contact").value<Contact*>();
			if (contact->name() != name)
				contact->setName(name);
		}

		void Model::onCollapsed(const QModelIndex &index)
		{
			if (getItemType(index) == TagType) {
				TagItem *tag = reinterpret_cast<TagItem*>(index.internalPointer());
				p->closedTags.insert(tag->name);
			}
		}

		void Model::onExpanded(const QModelIndex &index)
		{
			if (getItemType(index) == TagType) {
				TagItem *tag = reinterpret_cast<TagItem*>(index.internalPointer());
				p->closedTags.remove(tag->name);
			}
		}

		void Model::filterAllList()
		{
			for (int i = 0; i < p->tags.size(); i++) {
				TagItem *tag = p->tags.at(i);
				QModelIndex index = createIndex(i, 0, tag);
				tag->visible = 0;
				for (int j = 0; j < tag->contacts.size(); j++) {
					bool show = isVisible(tag->contacts.at(j));
					if (show)
						tag->visible++;
					if (p->view->isRowHidden(j, index) == show)
						p->view->setRowHidden(j, index, !show);
				}

				if (!p->filteredTags.isEmpty()) {
					if (!p->filteredTags.contains(tag->name))
						tag->visible = 0;
				}

				index = QModelIndex();
				if (p->view->isRowHidden(i, index) == !!tag->visible)
					p->view->setRowHidden(i, index, !tag->visible);
			}
		}

		bool Model::isVisible(ContactItem *item)
		{
			if (!item) {
				qWarning() << Q_FUNC_INFO << "item is null";
				return true;
			}
			Contact *contact = item->data->contact;
			if (!p->lastFilter.isEmpty()) {
				return contact->id().contains(p->lastFilter,Qt::CaseInsensitive)
						|| contact->name().contains(p->lastFilter,Qt::CaseInsensitive);
			} else {
				return p->showOffline || contact->status().type() != Status::Offline;
			}
		}

		void Model::hideContact(int index, const QModelIndex &tagIndex, bool hide)
		{
			TagItem *item = reinterpret_cast<TagItem*>(tagIndex.internalPointer());
			if (p->view->isRowHidden(index, tagIndex) != hide) {
				item->visible += hide ? -1 : 1;
				p->view->setRowHidden(index, tagIndex, hide);
				recheckTag(item, tagIndex.row());
				emit dataChanged(tagIndex,tagIndex);

			}
		}

		void Model::recheckTag(TagItem *item, int index)
		{
			if (index < 0)
				index = p->tags.indexOf(item);
			bool hidden = !item->visible;
			if (p->view->isRowHidden(index, QModelIndex()) == hidden)
				return;
			p->view->setRowHidden(index, QModelIndex(), hidden);
		}

		TagItem *Model::ensureTag(const QString &name)
		{
			TagItem *tag = 0;
			if(!(tag = p->tagsHash.value(name, 0)))
			{
				int index = p->tags.size();
				beginInsertRows(QModelIndex(), index, index);
				tag = new TagItem;
				tag->name = name;
				p->tagsHash.insert(tag->name, tag);
				p->tags << tag;
				endInsertRows();
				p->view->setRowHidden(index, QModelIndex(), true);
				if (!p->closedTags.contains(name))
					p->view->setExpanded(createIndex(index, 0, tag), true);
			}
			return tag;
		}
		
		QVariant Model::headerData(int section, Qt::Orientation orientation, int role) const
		{
			if (orientation == Qt::Horizontal && role == Qt::DisplayRole && section==0) {
				if (p->filteredTags.isEmpty())
					return tr("All tags");
				else
					return tr("Custom tags");
			}

			return QVariant();
		}
		
		bool Model::showOffline() const
		{
			return p->showOffline;
		}


	}
}
