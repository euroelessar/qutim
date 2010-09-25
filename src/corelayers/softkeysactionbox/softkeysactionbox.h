#ifndef SOFTKEYSACTIONBOX_H
#define SOFTKEYSACTIONBOX_H

#include <qutim/startupmodule.h>
#include <qutim/actionbox.h>
#include <QMap>

class QToolButton;
class QHBoxLayout;

namespace Core
{
using namespace qutim_sdk_0_3;

class SoftkeysActionBoxModule : public ActionBoxModule
{
    Q_OBJECT
	Q_CLASSINFO("DebugName","SoftkeysActionBox")
public:
	explicit SoftkeysActionBoxModule();
	virtual void addAction(QAction *action);
	virtual void removeAction(QAction *action);
};

}
#endif // SOFTKEYSACTIONBOX_H
