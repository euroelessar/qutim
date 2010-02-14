#ifndef QUETZALFILEDIALOG_H
#define QUETZALFILEDIALOG_H

#include <QFileDialog>
#include <purple.h>

class QuetzalFileDialog : public QObject
{
	Q_OBJECT
public:
	explicit QuetzalFileDialog(const char *title, const QString &dirname,
							   GCallback ok_cb, GCallback cancel_cb,
							   void *user_data, QFileDialog *parent);
protected slots:
	void onAccept();
	void onReject();
private:
	PurpleRequestFileCb m_ok_cb;
	PurpleRequestFileCb m_cancel_cb;
	void *m_user_data;
	QFileDialog *m_dialog;
};

#endif // QUETZALFILEDIALOG_H
