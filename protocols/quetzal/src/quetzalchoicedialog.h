#ifndef QUETZALCHOICEDIALOG_H
#define QUETZALCHOICEDIALOG_H

#include "quetzalrequestdialog.h"
#include <QRadioButton>
#include <purple.h>

class QuetzalChoiceDialog : public QuetzalRequestDialog
{
Q_OBJECT
public:
	explicit QuetzalChoiceDialog(const char *title, const char *primary,
								 const char *secondary, int default_value,
								 const char *ok_text, GCallback ok_cb,
								 const char *cancel_text, GCallback cancel_cb,
								 void *user_data, va_list choices,
								 QWidget *parent = 0);
protected:
	virtual void closeRequest();
protected slots:
	void onOkClicked();
	void onCancelClicked();
private:
	void callBack(PurpleRequestChoiceCb cb);
	PurpleRequestChoiceCb m_ok_cb;
	PurpleRequestChoiceCb m_cancel_cb;
	QList<QRadioButton *> m_radios;
};

#endif // QUETZALCHOICEDIALOG_H
