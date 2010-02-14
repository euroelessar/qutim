#ifndef QUETZALINPUTDIALOG_H
#define QUETZALINPUTDIALOG_H

#include "quetzalrequestdialog.h"

class QuetzalInputDialog : public QuetzalRequestDialog
{
	Q_OBJECT
public:
	explicit QuetzalInputDialog(const char *title, const char *primary,
								const char *secondary, const char *default_value,
								gboolean multiline, gboolean masked, gchar *hint,
								const char *ok_text, GCallback ok_cb,
								const char *cancel_text, GCallback cancel_cb,
								void *user_data, QWidget *parent = 0);
protected:
	virtual void closeRequest();
protected slots:
	void onOkClicked();
	void onCancelClicked();
private:
	PurpleRequestInputCb m_ok_cb;
	PurpleRequestInputCb m_cancel_cb;
	QWidget *m_input_widget;
	QByteArray m_property;
};

#endif // QUETZALINPUTDIALOG_H
