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
		};
	}
}

#endif // SIMPLECONTACTLISTVIEW_H
