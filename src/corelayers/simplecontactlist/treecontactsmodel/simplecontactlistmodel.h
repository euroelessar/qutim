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
struct ModelPrivate;
class Model;

struct ChangeEvent;

class Model : public AbstractContactModel
{
	Q_OBJECT
public:
	Model(QObject *parent = 0);
	virtual ~Model();
	virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	virtual QModelIndex parent(const QModelIndex &child) const;
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
	virtual bool hasChildren(const QModelIndex &parent = QModelIndex()) const;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
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
	bool event(QEvent *ev);
public slots:
	void addContact(qutim_sdk_0_3::Contact *contact);
	void removeContact(qutim_sdk_0_3::Contact *contact);
	void filterList(const QStringList &tags);
	void filterList(const QString &filter);
protected slots:
	void init();
	void contactDeleted(QObject *obj);
	void contactStatusChanged(qutim_sdk_0_3::Status status);
	void contactNameChanged(const QString &name);
	void contactTagsChanged(const QStringList &tags);
	void hideShowOffline();
	void onContactInListChanged(bool isInList);
	void onSessionCreated(qutim_sdk_0_3::ChatSession *session);
	void onUnreadChanged(const qutim_sdk_0_3::MessageList &messages);
	void onAccountCreated(qutim_sdk_0_3::Account *);
protected:
	void timerEvent(QTimerEvent *ev);
private:
	void filterAllList();
	bool isVisible(ContactItem *item);
	bool hideContact(ContactItem *item, bool hide, bool replacing = true);
	void removeFromContactList(Contact *contact, bool deleted);
	void hideTag(TagItem *item);
	void showTag(TagItem *item);
	void updateContact(ContactItem *item, bool placeChanged);
	void initialize();
	void saveConfig();
	TagItem *ensureTag(const QString &name);
	//			QModelIndex createIndex(
	QScopedPointer<ModelPrivate> p;
};
}
}

#endif // SIMPLECONTACTLISTMODEL_H
