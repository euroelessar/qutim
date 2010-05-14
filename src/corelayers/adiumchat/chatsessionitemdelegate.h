#ifndef CHATSESSIONITEMDELEGATE_H
#define CHATSESSIONITEMDELEGATE_H

#include <QStyledItemDelegate>

namespace Core
{
	namespace AdiumChat
	{
		class ChatSessionItemDelegate : public QStyledItemDelegate
		{
			Q_OBJECT
		public:
			explicit ChatSessionItemDelegate(QObject *parent = 0);

		public Q_SLOTS:
			bool helpEvent(QHelpEvent *event, QAbstractItemView *view,
						   const QStyleOptionViewItem &option,
						   const QModelIndex &index);
		};
	}
}
#endif // CHATSESSIONITEMDELEGATE_H
