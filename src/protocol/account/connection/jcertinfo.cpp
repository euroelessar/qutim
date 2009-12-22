#include "jcertinfo.h"

namespace Jabber
{
	struct JCertInfoPrivate
	{
		Ui::JCertInfo ui;
	};

	JCertInfo::JCertInfo(const CertInfo &info, QWidget *parent) : p(new JCertInfoPrivate), QDialog(parent)
	{
		p->ui.setupUi(this);

		QDateTime from, to;
		from.setTime_t(info.date_from);
		to.setTime_t(info.date_to);

		p->ui.cipherLabel->setText(QString::fromStdString(info.cipher));
		p->ui.compressionLabel->setText(QString::fromStdString(info.compression));
		p->ui.serverLabel->setText(QString::fromStdString(info.server));
		p->ui.fromLabel->setText(from.toString("hh:mm:ss dd/MM/yyyy"));
		p->ui.toLabel->setText(to.toString("hh:mm:ss dd/MM/yyyy"));
	}

	JCertInfo::~JCertInfo()
	{
	}

	bool JCertInfo::exec(bool &result)
	{
		result = QDialog::exec();
		return p->ui.rememberBox->isChecked();
	}
}
