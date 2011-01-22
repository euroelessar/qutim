/*#include "global.h"
#include "JumpListTask.h"
#include <QList>

typedef QList<JumpListTask> JumpListTasks;

class W7QTEXPORT JumpList : public QObject
{
	Q_OBJECT

	JumpListTask m_bufferized;
	static unsigned maxSlotsCount;

public:
	JumpList(QObject *parent);
	void addToList(JumpListTask &);
	void addToList(JumpListTasks &);
	void clearList();
	void beginList();
	void commitList();
	static void setAppId(const QString &);
	static void add(JumpListTask &);
	static void add(JumpListTasks &);
	static void clear();
};
*/
