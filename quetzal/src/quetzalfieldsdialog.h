#ifndef QUETZALFIELDSDIALOG_H
#define QUETZALFIELDSDIALOG_H

#include "quetzalrequestdialog.h"

class QuetzalFieldsDialog : public QuetzalRequestDialog
{
	Q_OBJECT
public:
	explicit QuetzalFieldsDialog(const char *title, const char *primary,
								 const char *secondary, PurpleRequestFields *fields,
								 const char *ok_text, GCallback ok_cb,
								 const char *cancel_text, GCallback cancel_cb,
								 void *user_data, QWidget *parent = 0);
	~QuetzalFieldsDialog();
protected:
	virtual void closeRequest();
protected slots:
	void onStringFieldChanged(const QString &text);
	void onIntFieldChanged(int value);
	void onBooleanFieldChanged(bool value);
	void onChoiceFieldChanged(int id);
	void onListFieldChanged();
	void onAccountFieldChanged(int id);
	void onOkClicked();
	void onCancelClicked();
private:
	PurpleRequestFieldsCb m_ok_cb;
	PurpleRequestFieldsCb m_cancel_cb;
	PurpleRequestFields *m_fields;
};

#endif // QUETZALFIELDSDIALOG_H
