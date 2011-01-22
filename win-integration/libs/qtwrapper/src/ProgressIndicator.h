#include "global.h"
#include <QObject>
#include "../../apilayer/src/ApiProgressIndicatorStates.h"

class QWidget;

class WTQTEXPORT ProgressIndicator : public QObject
{
	Q_OBJECT

	QWidget *m_window;

public:
	ProgressIndicator(QWidget *, QObject *parent = 0);
	void changeWindow(QWidget*);
	void setState(ProgressStates);
	void setValue(unsigned, unsigned max = 100);
	static void setState(QWidget *, ProgressStates);
	static void setValue(QWidget *, unsigned, unsigned max = 100);
	void clear();
};
