#ifndef SIMPLEACTIONBOX_H
#define SIMPLEACTIONBOX_H

#include <qutim/startupmodule.h>
#include <qutim/actionbox.h>
#include <QMap>

class QToolButton;
class QHBoxLayout;

namespace Core
{
using namespace qutim_sdk_0_3;

class SimpleActionBoxModule : public ActionBoxModule
{
    Q_OBJECT
	Q_CLASSINFO("DebugName","SimpleActionBox")
public:
	explicit SimpleActionBoxModule();
	virtual void addAction(QAction *action);
	virtual void removeAction(QAction *action);
protected slots:
	void onButtonDestroyed(QObject *obj);
	void onChanged();
private:
	QHBoxLayout *m_layout;
	QMap<QAction*,QToolButton*> m_buttons;
};

}
#endif // SIMPLEACTIONBOX_H
