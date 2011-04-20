#include "selectaccdlg.h"
#include "ui_selectaccdlg.h"
#include <QSysInfo>

SelectAccDlg::SelectAccDlg(QString &jid, QStringList &jids, QWidget *parent)
	: QDialog(parent),
    ui(new Ui::SelectAccDlg)
{
	ui->setupUi(this);
	jid_ = jid;
	ui->title->setText(ui->title->text().arg(jid));
	ui->accounts->insertItems(0, jids);
	ui->titleWidget->    setStyleSheet("background-image:url(:res/dialogtitle.png);");
	ui->titleWidget->    setBackgroundRole(QPalette::Window);
	ui->buttonboxWidget->setBackgroundRole(QPalette::Window);
	QPalette pl = palette();
	pl.setColor(QPalette::AlternateBase, QColor(Qt::white));
	setBackgroundRole(QPalette::AlternateBase);
	setPalette(pl);
	setAutoFillBackground(true);
}

SelectAccDlg::~SelectAccDlg()
{
	delete ui;
}

void SelectAccDlg::accept()
{
	jid_ = ui->accounts->itemText(ui->accounts->currentIndex());
	setResult(QDialog::Accepted);
}

QString SelectAccDlg::account()
{
	return jid_;
}
