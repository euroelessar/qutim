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

class SoftkeysActionBoxGenerator : public ActionBoxGenerator
{
	Q_OBJECT
public:
	virtual ActionBoxModule* generate() {return new SoftkeysActionBoxModule(); }
};

}
#endif // SOFTKEYSACTIONBOX_H
