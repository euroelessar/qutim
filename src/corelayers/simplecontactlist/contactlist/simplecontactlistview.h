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
	virtual void dataChanged(const QModelIndex & topLeft, const QModelIndex & bottomRight);
	virtual ~TreeView();
protected:
	void contextMenuEvent(QContextMenuEvent *);
	void startDrag(Qt::DropActions supportedActions);
protected slots:
	void onClick(const QModelIndex &index);
	void onResetTagsTriggered();
	void onSelectTagsTriggered();
private:
	QStringList m_closedTags;
};

}
}

#endif // SIMPLECONTACTLISTVIEW_H
