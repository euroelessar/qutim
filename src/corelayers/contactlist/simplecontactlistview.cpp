#include "simplecontactlistview.h"
#include "simplecontactlistitem.h"
#include "libqutim/messagesession.h"
#include <QtGui/QContextMenuEvent>

namespace Core
{
	namespace SimpleContactList
	{
		static int m_icon_size = 16;

		TreeView::TreeView(QWidget *parent) : QTreeView(parent)
		{
			connect(this, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onClick(QModelIndex)));
			setAnimated(true);
			setAlternatingRowColors(true);
			setRootIsDecorated(false);
			setIndentation(0);
			setEditTriggers(QAbstractItemView::EditKeyPressed);
			setIconSize(QSize(m_icon_size,m_icon_size));
		}

		void TreeView::onClick(const QModelIndex &index)
		{
			ItemType type = getItemType(index);
			if (type == ContactType) {
				ContactItem *item = reinterpret_cast<ContactItem *>(index.internalPointer());
				Contact *contact = item->data->contact;
				if (ChatSession *session = ChatLayer::get(contact, true))
					session->activate();
			}
		}

		void TreeView::contextMenuEvent(QContextMenuEvent *event)
		{
			QModelIndex index = indexAt(event->pos());
			ItemType type = getItemType(index);
			if (type == ContactType) {
				ContactItem *item = reinterpret_cast<ContactItem *>(index.internalPointer());
				Contact *contact = item->data->contact;
				qDebug("%s", qPrintable(contact->id()));
				contact->menu(true)->popup(event->globalPos());
			}
		}
	}
}
