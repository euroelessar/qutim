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
	virtual void onClicked(int button);
protected:
	virtual void closeRequest();
private:
	PurpleRequestInputCb m_ok_cb;
	PurpleRequestInputCb m_cancel_cb;
};

#endif // QUETZALINPUTDIALOG_H
