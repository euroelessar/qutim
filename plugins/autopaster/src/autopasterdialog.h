#ifndef AUTOPASTERDIALOG_H
#define AUTOPASTERDIALOG_H

#include <QDialog>
#include "pasterinterface.h"

namespace Ui {
class Handler;
}

class AutoPasterDialog : public QDialog
{
	Q_OBJECT
public:
	enum DialogCode {
		Rejected = QDialog::Rejected,
		Accepted = QDialog::Accepted,
		Failed
	};

	explicit AutoPasterDialog(QNetworkAccessManager *manager,
							  const QString &message,
							  const QList<PasterInterface*> &pasters,
							  int defaultPaster,
							  QWidget *parent = 0);

	QUrl url() const;
	QString errorString() const;

	void accept();

protected slots:
	void onFinished();

signals:

private:
	Ui::Handler *ui;
	QNetworkAccessManager *m_manager;
	QUrl m_url;
	QString m_message;
	QString m_errorString;
};

#endif // AUTOPASTERDIALOG_H
