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
	void setModel(AbstractContactModel *model);
protected:
	void contextMenuEvent(QContextMenuEvent *);
	void startDrag(Qt::DropActions supportedActions);
protected slots:
	void onClick(const QModelIndex &index);
	void initScrolling();
	void onCollapsed(const QModelIndex &index);
	void onExpanded(const QModelIndex &index);
	void onIndexVisibilityChanged(const QModelIndex &index, const QString &name, bool shown);
private:
	void storeClosedTags();
	QSet<QString> m_closedIndexes;
	QHash<quint64, QString> m_visibleIndexes;
};

}
}

#endif // SIMPLECONTACTLISTVIEW_H
