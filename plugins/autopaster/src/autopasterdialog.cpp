#include "autopasterdialog.h"
#include <ui_handler.h>

AutoPasterDialog::AutoPasterDialog(QNetworkAccessManager *manager,
								   const QString &message,
								   const QList<PasterInterface *> &pasters,
								   int defaultPaster,
								   QWidget *parent)
	: QDialog(parent), ui(new Ui::Handler), m_manager(manager), m_message(message)
{
	ui->setupUi(this);
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
	setWindowTitle(tr("Automatic paster request"));

	foreach (PasterInterface *paster, pasters)
		ui->locationBox->addItem(paster->name(), qVariantFromValue(paster));

	ui->locationBox->setCurrentIndex(defaultPaster);

	ui->languageBox->addItem("Plain Text", "text");
	ui->languageBox->addItem("C++", "cpp");
	ui->languageBox->addItem("Bash", "bash");
	ui->languageBox->addItem("Perl", "perl");
	ui->languageBox->addItem("PHP", "php");
	ui->languageBox->addItem("C#", "csharp");
	ui->languageBox->addItem("HTML", "html");
	ui->languageBox->addItem("JavaScript", "js");
	ui->languageBox->addItem("Java", "java");
	ui->languageBox->addItem("Makefile", "make");
	ui->languageBox->addItem("XML", "xml");
	ui->languageBox->addItem("CSS", "css");
}

void AutoPasterDialog::accept()
{
	const int locationIndex = ui->locationBox->currentIndex();
	const int languageIndex = ui->languageBox->currentIndex();
	PasterInterface *paster = ui->locationBox->itemData(locationIndex).value<PasterInterface*>();
	const QString syntax = ui->languageBox->itemData(languageIndex).toString();

	QNetworkReply *reply = paster->send(m_manager, m_message, syntax);
	reply->setProperty("__paster", ui->locationBox->itemData(locationIndex));
	connect(reply, SIGNAL(finished()), SLOT(onFinished()));
	setEnabled(false);
}

void AutoPasterDialog::onFinished()
{
	QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
	reply->deleteLater();

	if (reply->error() == QNetworkReply::NoError) {
		PasterInterface *paster = reply->property("__paster").value<PasterInterface*>();

		m_errorString.clear();
		m_url = paster->handle(reply, &m_errorString);

		if (m_errorString.isEmpty())
			done(Accepted);
		else
			done(Failed);
	} else {
		m_errorString = reply->errorString();
		done(Failed);
	}
}

QUrl AutoPasterDialog::url() const
{
	return m_url;
}

QString AutoPasterDialog::errorString() const
{
	return QString();
}
