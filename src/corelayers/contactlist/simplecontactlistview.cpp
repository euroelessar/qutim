#include "simplecontactlistview.h"
#include "simplecontactlistitem.h"
#include "libqutim/messagesession.h"

namespace Core
{
	namespace SimpleContactList
	{
		TreeView::TreeView(QWidget *parent) : QTreeView(parent)
		{
			connect(this, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onClick(QModelIndex)));
		}

		void TreeView::onClick(const QModelIndex &index)
		{
			ContactItem *item = reinterpret_cast<ContactItem *>(index.internalPointer());
			Contact *contact = item->data->contact;
			if (ChatSession *session = ChatLayer::get(contact, true))
				session->activate();
		}
	}
}
