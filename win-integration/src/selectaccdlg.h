#ifndef SELECTACCDLG_H
#define SELECTACCDLG_H

#include <QDialog>

namespace Ui {
    class SelectAccDlg;
}

class SelectAccDlg : public QDialog
{
	Q_OBJECT

public:
	explicit SelectAccDlg(QString &jid, QStringList &jids, QWidget *parent = 0);
	~SelectAccDlg();
	QString account();

protected:
	void accept();

private:
	Ui::SelectAccDlg *ui;
	QString jid_;
};

#endif // SELECTACCDLG_H
