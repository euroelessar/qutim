#ifndef SIMPLECONTACTLISTVIEW_H
#define SIMPLECONTACTLISTVIEW_H

#include <QTreeView>
#include <QPersistentModelIndex>
#include "simplecontactlist_global.h"

namespace Core
{
namespace SimpleContactList
{

class AbstractContactModel;

class SIMPLECONTACTLIST_EXPORT TreeView : public QTreeView
{
	Q_OBJECT
public:
	TreeView(AbstractContactModel *model, QWidget *parent = 0);
	virtual void dataChanged(const QModelIndex & topLeft, const QModelIndex & bottomRight);
	virtual ~TreeView();
protected:
	void contextMenuEvent(QContextMenuEvent *);
	void startDrag(Qt::DropActions supportedActions);
protected slots:
	void onClick(const QModelIndex &index);
	void initScrolling();
	void onResetTagsTriggered();
	void onSelectTagsTriggered();
	void onCollapsed(const QModelIndex &index);
	void onExpanded(const QModelIndex &index);
	void onTagVisibilityChanged(const QModelIndex &index, const QString &name, bool shown);
private:
	QSet<QString> m_closedTags;
	QHash<quint64, QString> m_visibleTags;
};

}
}

#endif // SIMPLECONTACTLISTVIEW_H
