#include "chatsessionitemdelegate.h"
#include "libqutim/tooltip.h"
#include "libqutim/buddy.h"
#include <QHelpEvent>
#include <QAbstractItemView>

using namespace qutim_sdk_0_3;

namespace Core
{
	namespace AdiumChat
	{
		ChatSessionItemDelegate::ChatSessionItemDelegate(QObject *parent) :
				QStyledItemDelegate(parent)
		{
		}

		bool ChatSessionItemDelegate::helpEvent(QHelpEvent *event, QAbstractItemView *view,
												const QStyleOptionViewItem &option,
												const QModelIndex &index)
		{
			if (event->type() == QEvent::ToolTip) {
				Buddy *buddy = index.data(Qt::UserRole).value<Buddy*>();
				ToolTip::instance()->showText(event->globalPos(), buddy, view);
				return true;
			} else {
				return QAbstractItemDelegate::helpEvent(event, view, option, index);
			}
		}
	}
}
