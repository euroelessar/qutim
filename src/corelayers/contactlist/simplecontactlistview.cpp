#include "simplecontactlistview.h"
#include "simplecontactlistitem.h"
#include "simplecontactlistmodel.h"
#include <qutim/messagesession.h>
#include "tagsfilterdialog.h"
#include <QtGui/QContextMenuEvent>
#include <QHeaderView>
#include <qutim/icon.h>
#include <qutim/config.h>
#include <qutim/event.h>
#include <QPainter>
#include <QDebug>
#include <QLabel>
#include <QApplication>

namespace Core
{
	namespace SimpleContactList
	{
		
		TreeView::TreeView(QWidget *parent) : QTreeView(parent)
		{
			connect(this, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onClick(QModelIndex)));
			setAnimated(true);
			setAlternatingRowColors(true);
			setRootIsDecorated(false);
			setIndentation(0);
			setEditTriggers(QAbstractItemView::EditKeyPressed);
			header()->setContextMenuPolicy(Qt::DefaultContextMenu);
			header()->installEventFilter(this);
		}

		void TreeView::onClick(const QModelIndex &index)
		{
			ContactItemType type = getItemType(index);
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
			ContactItemType type = getItemType(index);
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

		void TreeView::startDrag(Qt::DropActions supportedActions)
		{
			QModelIndex index = selectedIndexes().value(0);
			if (!index.isValid())
				return;
			
			QMimeData *data = model()->mimeData(QModelIndexList() << index);
			if (!data)
				return;
			QRect rect;
			QPixmap pixmap;
			QPoint point;
			{
				QAbstractItemDelegate *delegate = itemDelegate(index);
				QStyleOptionViewItemV4 option = viewOptions();
				option.locale = this->locale();
				option.locale.setNumberOptions(QLocale::OmitGroupSeparator);
				option.widget = this;
				option.state |= QStyle::State_Selected;
				option.rect = visualRect(index);
				point = option.rect.topLeft();
				option.rect.moveTo(0, 0);
				option.rect.setSize(delegate->sizeHint(option, index));
				rect = option.rect;
				pixmap = QPixmap(rect.size());
				pixmap.fill(Qt::transparent);
				QPainter painter(&pixmap);			
				delegate->paint(&painter, option, index);
			}
			QDrag *drag = new QDrag(this);
			drag->setPixmap(pixmap);
			drag->setMimeData(data);
			point = QCursor::pos() - viewport()->mapToGlobal(point);
			drag->setHotSpot(point);
//			drag->setHotSpot(QCursor::pos() - rect.topLeft());
			Qt::DropAction setDefaultDropAction = QAbstractItemView::defaultDropAction();
			Qt::DropAction defaultDropAction = Qt::IgnoreAction;
			if (setDefaultDropAction != Qt::IgnoreAction && (supportedActions & setDefaultDropAction))
				defaultDropAction = setDefaultDropAction;
			else if (supportedActions & Qt::CopyAction && dragDropMode() != QAbstractItemView::InternalMove)
				defaultDropAction = Qt::CopyAction;
			if (drag->exec(supportedActions, defaultDropAction) == Qt::IgnoreAction
				&& getItemType(index) == ContactType) {
				ContactItem *item = reinterpret_cast<ContactItem*>(index.internalPointer());
				if (QWidget *widget = QApplication::topLevelAt(QCursor::pos())) {
					if (widget->window() == this->window())
						return;
				}
				Event ev("contact-list-drop",
						 QCursor::pos() - point,
						 qVariantFromValue(item->data->contact));
				ev.send();
			}
//			qDebug() << "DropAction" << drag->exec(supportedActions, defaultDropAction);
//			if (drag->exec(supportedActions, defaultDropAction) == Qt::MoveAction)
//				d->clearOrRemove();
//			{}
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
