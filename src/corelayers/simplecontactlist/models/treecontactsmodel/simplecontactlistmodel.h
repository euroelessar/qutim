#ifndef SIMPLECONTACTLISTMODEL_H
#define SIMPLECONTACTLISTMODEL_H

#include <QAbstractItemModel>
#include <simplecontactlistitem.h>
#include <qutim/messagesession.h>
#include <abstractcontactmodel.h>

namespace Core
{
namespace SimpleContactList
{
class TreeModelPrivate;
class TreeModel;

class ChangeEvent;
class ContactItem;

class TagItem : protected ItemHelper
{
public:
	inline TagItem() : ItemHelper(TagType), online(0) {}
	inline TreeModelPrivate *getTagContainer(AbstractContactModel *m);
	inline void setTagContainer(void *) { }
	inline QString getName() { return name; }
	inline QModelIndex parentIndex(void *) { return QModelIndex(); }
	QList<ContactItem *> visible;
	int online;
	QString name;
	QList<ContactItem *> contacts;
};

class ContactData : public QSharedData
{
public:
	typedef QExplicitlySharedDataPointer<ContactData> Ptr;
	inline ContactData() {}
	inline ContactData(const ContactData &other)
		: QSharedData(other), contact(other.contact), tags(other.tags), items(other.items) {}
	inline ~ContactData() {}
	Contact *contact;
	QSet<QString> tags;
	QList<ContactItem *> items;
	Status status;
};

class ContactItem  : public ItemHelper
{
public:
	inline ContactItem() : ItemHelper(ContactType) {}
	inline ContactItem(const ContactData::Ptr &other_data) : ItemHelper(ContactType), data(other_data) {}
	inline ContactItem(const ContactItem &other) : ItemHelper(ContactType), parent(other.parent), data(other.data) {}
	inline int index() { return parent->visible.indexOf(this); }
	inline QModelIndex parentIndex(AbstractContactModel *m);
	inline Contact *getContact() { return data->contact; }
	inline Status getStatus() { return data->status; }
	inline QList<ContactItem*> &siblings(void*) { return parent->visible; }
	inline bool isInSelectedTag(const QSet<QString> &selectedTags) { return selectedTags.contains(parent->name); }
	TagItem *parent;
	ContactData::Ptr data;
};

class TreeModel : public AbstractContactModel
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(TreeModel)
	Q_CLASSINFO("SettingsDescription", "Show tags and contacts")
public:
	TreeModel(QObject *parent = 0);
	virtual ~TreeModel();
	virtual QList<Contact*> contacts() const;
	virtual void setContacts(const QList<qutim_sdk_0_3::Contact*> &contacts);
	virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	virtual QModelIndex parent(const QModelIndex &child) const;
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
	virtual bool hasChildren(const QModelIndex &parent = QModelIndex()) const;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	virtual bool setData(const QModelIndex &index, const QVariant &value, int role);
	bool containsContact(Contact *contact) const;
	QStringList mimeTypes() const;
	QMimeData *mimeData(const QModelIndexList &indexes) const;
	bool dropMimeData(const QMimeData *data, Qt::DropAction action,
					  int row, int column, const QModelIndex &parent);
	QStringList tags() const;
public slots:
	void addContact(qutim_sdk_0_3::Contact *contact);
	void removeContact(qutim_sdk_0_3::Contact *contact);
protected slots:
	void contactDeleted(QObject *obj);
	void contactStatusChanged(const qutim_sdk_0_3::Status &status);
	void contactNameChanged(const QString &name);
	void contactTagsChanged(const QStringList &tags);
	void onContactInListChanged(bool isInList);
	void onAccountCreated(qutim_sdk_0_3::Account *);
	void init();
protected:
	void addAccount(qutim_sdk_0_3::Account *account);
	void filterAllList();
	void updateContactData(Contact *contact);
	void processEvent(ChangeEvent *ev);
	bool eventFilter(QObject *obj, QEvent *ev);
private:
	friend class TagItem;
	friend class ContactItem;
	void removeFromContactList(Contact *contact, bool deleted);
	void saveTagOrder();
};

}
}

#endif // SIMPLECONTACTLISTMODEL_H
