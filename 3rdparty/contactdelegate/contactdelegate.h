#ifndef CONTACTDELEGATE_H
#define CONTACTDELEGATE_H

#include <QStyledItemDelegate>
#include <libqutim/libqutim_global.h>

namespace qutim_sdk_0_3
{

enum ContactItemRole
{
	BuddyRole = Qt::UserRole,
	StatusRole,
	ContactsCountRole,
	OnlineContactsCountRole,
	AvatarRole,
	ItemTypeRole
};
Q_DECLARE_FLAGS(ContactItemRoles,ContactItemRole);
enum ContactItemType
{
	InvalidType = 0,
	TagType = 100,
	ContactType = 101
};

struct ContactDelegatePrivate;
class LIBQUTIM_EXPORT ContactDelegate : public QAbstractItemDelegate
{
    Q_OBJECT
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
	void setShowFlags(ShowFlags flags);
	void setExtInfo(const QHash<QString, bool> &info);
public Q_SLOTS:
	bool helpEvent(QHelpEvent *event, QAbstractItemView *view,
				   const QStyleOptionViewItem &option,
				   const QModelIndex &index);
protected:
	void drawDisplay(QPainter *painter, const QStyleOptionViewItem &option,
					 const QRect &rect, const QString &text) const;
	void drawDecoration(QPainter *painter, const QStyleOptionViewItem &option,
						const QRect &rect, const QPixmap &pixmap) const;
	void drawFocus(QPainter *painter, const QStyleOptionViewItem &option,
				   const QRect &rect) const;
private:
	QScopedPointer<ContactDelegatePrivate> p;
};

}

Q_DECLARE_OPERATORS_FOR_FLAGS(qutim_sdk_0_3::ContactItemRoles);
Q_DECLARE_OPERATORS_FOR_FLAGS(qutim_sdk_0_3::ContactDelegate::ShowFlags)

#endif // CONTACTDELEGATE_H
