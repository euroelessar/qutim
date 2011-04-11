#ifndef ACCOUNT_SELECTION_H
#define ACCOUNT_SELECTION_H

#include <QDialog>

namespace Ui {
    class AccSelectionDlg;
}

namespace qutim_sdk_0_3
{
	class Account;
}


class AccSelectionDlg : public QDialog
{
	Q_OBJECT

public:
	explicit AccSelectionDlg(QWidget *parent = 0);
	~AccSelectionDlg();
	void setQuestion(const QString &q);
	void setDescription(const QString &d);
	qutim_sdk_0_3::Account *selectedAcc();
	void setAccsList(QList<qutim_sdk_0_3::Account*> accs);

private:
	Ui::AccSelectionDlg *ui;
	friend class AbstractHandler;
};

#endif // ACCOUNT_SELECTION_H
