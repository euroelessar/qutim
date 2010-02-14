#ifndef QUETZALREQUESTDIALOG_H
#define QUETZALREQUESTDIALOG_H

#include <QDialog>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <purple.h>

class QuetzalRequestDialog : public QDialog
{
	Q_OBJECT
public:
	explicit QuetzalRequestDialog(const char *title, const char *primary,
								  const char *secondary, PurpleRequestType type,
								  void *user_data, QWidget *parent = 0);
protected slots:
	virtual void closeRequest();
protected:
	virtual void closeEvent(QCloseEvent *);
	void *userData() { return m_user_data; }
	QVBoxLayout *boxLayout() { return m_boxLayout; }
	QDialogButtonBox *buttonBox() { return m_buttonBox; }
private:
	QVBoxLayout *m_boxLayout;
	QDialogButtonBox *m_buttonBox;
	PurpleRequestType m_type;
	void *m_user_data;
};

#endif // QUETZALREQUESTDIALOG_H
