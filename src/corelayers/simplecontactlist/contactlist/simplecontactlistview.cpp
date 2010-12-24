#include "simplecontactlistview.h"
#include "simplecontactlistitem.h"
#include "abstractcontactmodel.h"
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
	connect(this,
#ifndef Q_WS_MAEMO_5
	SIGNAL(doubleClicked(QModelIndex)),
#else
	SIGNAL(clicked(QModelIndex)),
#endif
	this, SLOT(onClick(QModelIndex)));

	setAnimated(true);
	setAlternatingRowColors(true);
	setRootIsDecorated(false);
	setIndentation(0);
	setEditTriggers(QAbstractItemView::EditKeyPressed);
	setHeaderHidden(true);

#ifndef QUTIM_MOBILE_UI
	setDragEnabled(true);
	setAcceptDrops(true);
	setDropIndicatorShown(true);
#endif
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
	AbstractContactModel *m = qobject_cast<AbstractContactModel*>(model());
	Q_ASSERT(m);
	m->filterList(QStringList());
}

void TreeView::onSelectTagsTriggered()
{
	AbstractContactModel *m = qobject_cast<AbstractContactModel*>(model());
	Q_ASSERT(m);
	QStringList tags = m->tags();
	TagsFilterDialog *dialog = new TagsFilterDialog(tags,this);
	if (!m->selectedTags().isEmpty())
		tags = m->selectedTags();
	dialog->setSelectedTags(tags);
	dialog->show();
	centerizeWidget(dialog);
	if (dialog->exec()) {
		m->filterList(dialog->selectedTags());
	}
	dialog->deleteLater();
}
}
}
