#include "jconferenceconfig.h"
#include "ui_jconferenceconfig.h"
#include "../dataform/jdataform.h"
#include "jmucsession.h"
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

JConferenceConfig::JConferenceConfig() : p(new JConferenceConfigPrivate)
{
	(new QGridLayout(this))->setMargin(0);
	p->room = 0;
	p->form = 0;
//	p->ui = new Ui::RoomConfig();
//	p->ui->setupUi(this);
//	p->ui->okButton->setIcon(Icon(""));
//	p->ui->applyButton->setIcon(Icon(""));
//	p->ui->cancelButton->setIcon(Icon(""));
}

JConferenceConfig::~JConferenceConfig()
{
}

void JConferenceConfig::setController(QObject *controller)
{
	JMUCSession *session = qobject_cast<JMUCSession*>(controller);
	if (!session)
		return;
	p->room = session->room();
	connect(p->room, SIGNAL(configurationReceived(jreen::DataForm::Ptr)),
			this, SLOT(onConfigurationReceived(jreen::DataForm::Ptr)));
}

void JConferenceConfig::loadImpl()
{
	p->room->requestRoomConfig();
}

void JConferenceConfig::saveImpl()
{
	p->room->setRoomConfig(p->form->getDataForm());
}

void JConferenceConfig::cancelImpl()
{
}

void JConferenceConfig::onConfigurationReceived(const jreen::DataForm::Ptr &form)
{
	QGridLayout *gridLayout = qobject_cast<QGridLayout*>(layout());
	if (p->form) {
		p->form->deleteLater();
		gridLayout->removeWidget(p->form);
	}
	p->form = new JDataForm(form, AbstractDataForm::NoButton, this);
	connect(p->form->widget(), SIGNAL(changed()), this, SLOT(onDataChanged()));
//	connect(p->form->widget(), SIGNAL(clicked(int)))
//	p->ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
//	p->ui->scrollArea->setWidgetResizable(true);
//	p->ui->scrollAreaWidgetContents->setLayout(layout);
//	QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
//	p->form->setSizePolicy(sizePolicy);
	gridLayout->addWidget(p->form);
}

void JConferenceConfig::onDataChanged()
{
	emit modifiedChanged(true);
}
}
