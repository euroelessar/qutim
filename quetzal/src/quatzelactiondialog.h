#ifndef QUATZELACTIONDIALOG_H
#define QUATZELACTIONDIALOG_H

#include <QDialog>
#include <purple.h>

class QAbstractButton;

namespace Ui {
	class QuetzalActionDialog;
}

typedef QPair<QString, PurpleRequestActionCb> QuetzalRequestAction;
typedef QList<QuetzalRequestAction> QuetzalRequestActionList;

class QuetzalActionDialog : public QDialog {
    Q_OBJECT
public:
	QuetzalActionDialog(const QString &title, const QString &primary,
						const QString &secondary, int default_action,
						const QuetzalRequestActionList &actions, gpointer user_data,
						QWidget *parent = 0);
	~QuetzalActionDialog();

protected slots:
	void on_buttonBox_clicked(QAbstractButton *button);

protected:
    void changeEvent(QEvent *e);

private:
	Ui::QuetzalActionDialog *ui;
	QuetzalRequestActionList m_actions;
	int m_default_action;
	gpointer m_user_data;
};

#endif // QUATZELACTIONDIALOG_H
