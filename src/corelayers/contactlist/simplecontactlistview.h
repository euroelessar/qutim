#ifndef SIMPLECONTACTLISTVIEW_H
#define SIMPLECONTACTLISTVIEW_H

#include <QTreeView>

namespace Core
{
namespace SimpleContactList
{

class TreeView : public QTreeView
{
	Q_OBJECT
public:
	TreeView(QWidget *parent = 0);

protected:
	void contextMenuEvent(QContextMenuEvent *);
	virtual bool eventFilter(QObject *, QEvent *);
	void startDrag(Qt::DropActions supportedActions);
protected slots:
	void onClick(const QModelIndex &index);
	void onResetTagsTriggered();
	void onSelectTagsTriggered();
};

}
}

#endif // SIMPLECONTACTLISTVIEW_H
