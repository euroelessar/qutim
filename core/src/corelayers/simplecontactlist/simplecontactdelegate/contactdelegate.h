#ifndef CONTACTDELEGATE_H
#define CONTACTDELEGATE_H

#include <QAbstractItemDelegate>
#include <qutim/libqutim_global.h>
#include <qutim/plugin.h>

namespace Core
{

using namespace qutim_sdk_0_3;

enum ContactItemRole
{
	BuddyRole = Qt::UserRole,
	StatusRole,
	ContactsCountRole,
	OnlineContactsCountRole,
	AvatarRole,
	ItemTypeRole,
	AccountRole,
	Color,
};
Q_DECLARE_FLAGS(ContactItemRoles,ContactItemRole);
enum ContactItemType
{
	InvalidType = 0,
	TagType = 100,
	ContactType = 101,
	AccountType = 102
};

struct ContactDelegatePrivate;

class ContactDelegatePlugin: public Plugin
{
	Q_OBJECT
	Q_CLASSINFO("DebugName", "ContactDelegate")
public:
	ContactDelegatePlugin();
	void init();
	bool load();
	bool unload();
};

class ContactDelegate : public QAbstractItemDelegate
{
	Q_OBJECT
	Q_CLASSINFO("Service","ContactDelegate")
	Q_CLASSINFO("Uses", "IconLoader")
	Q_CLASSINFO("SettingsDescription", "Default style")
public:
	enum ShowFlag
	{
		ShowStatusText = 0x1,
		ShowExtendedInfoIcons = 0x2,
		ShowAvatars = 0x4
	};
	Q_DECLARE_FLAGS(ShowFlags, ShowFlag);
	explicit ContactDelegate(QObject *parent = 0);
	~ContactDelegate();
	void paint(QPainter *painter, const QStyleOptionViewItem &option,
			   const QModelIndex &index) const;
	QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
public slots:
	bool helpEvent(QHelpEvent *event, QAbstractItemView *view,
				   const QStyleOptionViewItem &option,
				   const QModelIndex &index);
	void reloadSettings();
protected:
	void drawDisplay(QPainter *painter, const QStyleOptionViewItem &option,
					 const QRect &rect, const QString &text) const;
	void drawDecoration(QPainter *painter, const QStyleOptionViewItem &option,
						const QRect &rect, const QPixmap &pixmap) const;
	void drawFocus(QPainter *painter, const QStyleOptionViewItem &option,
				   const QRect &rect) const;
	void setExtInfo(const QHash<QString, bool> &info);
	void setFlag(ContactDelegate::ShowFlags flag, bool on);
private:
	QScopedPointer<ContactDelegatePrivate> p;
};

}

Q_DECLARE_OPERATORS_FOR_FLAGS(Core::ContactItemRoles);
Q_DECLARE_OPERATORS_FOR_FLAGS(Core::ContactDelegate::ShowFlags)

#endif // CONTACTDELEGATE_H
