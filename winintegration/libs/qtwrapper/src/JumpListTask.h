/*#include <QObject>
#include <QPixmap>

class QIcon;

class JumpListTask : public QObject
{
	Q_OBJECT

	QPixmap m_icon;
	QString m_title;
	QString m_description;
	QString m_appPath;

public:
	JumpListTask(QObject *parent = 0);
	JumpListTask &operator=(JumpListTask &);

	QString title();
	QString tooltip();
	QString appPath();
	QPixmap icon();
	void setTitle  (const QString &);
	void setTooltip(const QString &);
	void setAppPath(const QString &);
	void setIcon   (const QPixmap &);
	void setIcon   (const QIcon &);

};
*/
