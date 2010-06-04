#include "simplecontactlistview.h"
#include "simplecontactlistitem.h"
#include "simplecontactlistmodel.h"
#include "libqutim/messagesession.h"
#include "tagsfilterdialog.h"
#include <QtGui/QContextMenuEvent>
#include <QHeaderView>
#include <libqutim/icon.h>

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
			header()->setContextMenuPolicy(Qt::DefaultContextMenu);
			header()->installEventFilter(this);
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

		bool TreeView::eventFilter(QObject *obj, QEvent *e)
		{
			if (obj->metaObject() == &QHeaderView::staticMetaObject) {
				if (e->type() == QEvent::ContextMenu) {
					QContextMenuEvent *event = static_cast<QContextMenuEvent*>(e);
					QMenu *menu = new QMenu(this);
					menu->setAttribute(Qt::WA_DeleteOnClose);
					QAction *act = menu->addAction(Icon("feed-subscribe"),tr("Select tags"));
					connect(act,SIGNAL(triggered()),SLOT(onSelectTagsTriggered()));
					act = menu->addAction(tr("Reset"));
					connect(act,SIGNAL(triggered()),SLOT(onResetTagsTriggered()));
					menu->popup(event->globalPos());
					return true;
				}
			}
			return QObject::eventFilter(obj, e);
		}

		void TreeView::onResetTagsTriggered()
		{
			Model *m = static_cast<Model *>(model());
			Q_ASSERT(m);
			m->onFilterList(QStringList());
		}

		void TreeView::onSelectTagsTriggered()
		{
			Model *m = static_cast<Model *>(model());
			Q_ASSERT(m);
			QStringList tags = m->tags();
			TagsFilterDialog *dialog = new TagsFilterDialog(tags,this);
			if (!m->selectedTags().isEmpty())
				tags = m->selectedTags();
			dialog->setSelectedTags(tags);
			dialog->show();
			centerizeWidget(dialog);
			if (dialog->exec()) {
				m->onFilterList(dialog->selectedTags());
			}
			dialog->deleteLater();
		}
	}
}
