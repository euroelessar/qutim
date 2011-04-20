#include <QCoreApplication>
#include <QSize>

//class QPixmap;

class Win7EventFilter : public QObject
{
	Q_OBJECT

	Win7EventFilter();
	static QCoreApplication::EventFilter replacedFilter;

public:
	static Win7EventFilter *instance();
	static bool eventFilter(void *message, long *result);
};
