#include "jconferenceconfig.h"
#include "ui_jconferenceconfig.h"
#include "../dataform/jdataform.h"
#include <QStringBuilder>

namespace Jabber
{
using namespace jreen;
struct JConferenceConfigPrivate
{
//	Ui::RoomConfig *ui;
	JDataForm *form;
	MUCRoom *room;
};

JConferenceConfig::JConferenceConfig(MUCRoom *room, QWidget *parent) : QWidget(parent), p(new JConferenceConfigPrivate)
{
	p->room = room;
	connect(room, SIGNAL(destroyed()), this, SLOT(deleteLater()));
	connect(room, SIGNAL(configurationReceived(jreen::DataForm::Ptr)),
			this, SLOT(onConfigurationReceived(jreen::DataForm::Ptr)));
	p->room->requestRoomConfig();
//	p->ui = new Ui::RoomConfig();
//	p->ui->setupUi(this);
//	p->ui->okButton->setIcon(Icon(""));
//	p->ui->applyButton->setIcon(Icon(""));
//	p->ui->cancelButton->setIcon(Icon(""));
	setWindowTitle(tr("Room configuration: %1").arg(room->id()));
	setAttribute(Qt::WA_QuitOnClose, false);
	setAttribute(Qt::WA_DeleteOnClose, true);
}

JConferenceConfig::~JConferenceConfig()
{
}

void JConferenceConfig::closeEvent(QCloseEvent *)
{
	emit destroyDialog();
}

void JConferenceConfig::onConfigurationReceived(const jreen::DataForm::Ptr &form)
{
	p->form = new JDataForm(form, AbstractDataForm::Ok | AbstractDataForm::Apply | AbstractDataForm::Cancel, this);
	QGridLayout *layout = new QGridLayout(this);
	layout->setMargin(0);
//	connect(p->form->widget(), SIGNAL(clicked(int)))
//	p->ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
//	p->ui->scrollArea->setWidgetResizable(true);
//	p->ui->scrollAreaWidgetContents->setLayout(layout);
//	QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
//	p->form->setSizePolicy(sizePolicy);
	layout->addWidget(p->form);
}

void JConferenceConfig::on_applyButton_clicked()
{
	p->room->setRoomConfig(p->form->getDataForm());
}

void JConferenceConfig::on_okButton_clicked()
{
	on_applyButton_clicked();
	close();
}
}
