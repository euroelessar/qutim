#ifndef SIMPLESTATUSDIALOG_H
#define SIMPLESTATUSDIALOG_H

#include <QDialog>
#include "simplecontactlist_global.h"

namespace Ui {
class SimpleStatusDialog;
}

namespace Core
{
namespace SimpleContactList
{

class SIMPLECONTACTLIST_EXPORT SimpleStatusDialog : public QDialog
{
	Q_OBJECT

public:
	explicit SimpleStatusDialog(const QString &status, QWidget *parent = 0);
	~SimpleStatusDialog();
	QString statusText() const;
protected:
	void changeEvent(QEvent *e);

private:
	Ui::SimpleStatusDialog *ui;
};

}
}
#endif // SIMPLESTATUSDIALOG_H
