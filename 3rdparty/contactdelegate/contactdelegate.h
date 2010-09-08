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
};

Q_DECLARE_FLAGS(ContactItemRoles,ContactItemRole);

class LIBQUTIM_EXPORT ContactDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit ContactDelegate(QObject *parent = 0);
	void paint(QPainter *painter, const QStyleOptionViewItem &option,
			   const QModelIndex &index) const;
	QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
public Q_SLOTS:
	bool helpEvent(QHelpEvent *event, QAbstractItemView *view,
				   const QStyleOptionViewItem &option,
				   const QModelIndex &index);
};

}

Q_DECLARE_OPERATORS_FOR_FLAGS(qutim_sdk_0_3::ContactItemRoles);

#endif // CONTACTDELEGATE_H
