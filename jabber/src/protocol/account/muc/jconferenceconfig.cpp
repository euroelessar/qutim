#include "jconferenceconfig.h"
#include "ui_jconferenceconfig.h"
#include "../dataform/jdataform.h"
#include <gloox/mucroom.h>
#include <QStringBuilder>

namespace Jabber
{
	struct JConferenceConfigPrivate
	{
		Ui::RoomConfig *ui;
		JDataForm *form;
		gloox::MUCRoom *room;
	};

	JConferenceConfig::JConferenceConfig(MUCRoom *room, QWidget *parent) : QWidget(parent), p(new JConferenceConfigPrivate)
	{
		p->room = room;
		p->room->registerMUCRoomConfigHandler(this);
		p->room->requestRoomConfig();
		p->ui = new Ui::RoomConfig();
		p->ui->setupUi(this);
		p->ui->okButton->setIcon(Icon(""));
		p->ui->applyButton->setIcon(Icon(""));
		p->ui->cancelButton->setIcon(Icon(""));
		setWindowTitle(tr("Room configuration: %1")
				.arg(QString::fromStdString(room->name()) % QLatin1Char('@') % QString::fromStdString(room->service())));
	}

	JConferenceConfig::~JConferenceConfig()
	{
	}

	void JConferenceConfig::closeEvent(QCloseEvent *)
	{
		p->room->removeMUCRoomHandler();
		emit destroyDialog();
	}

	void JConferenceConfig::handleMUCConfigForm(MUCRoom *room, const DataForm &form)
	{
		Q_ASSERT(p->room == room);
		p->form = new JDataForm(new DataForm(form), this);
		QGridLayout *layout = new QGridLayout(this);
		p->ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
		p->ui->scrollArea->setWidgetResizable(true);
		p->ui->scrollAreaWidgetContents->setLayout(layout);
		QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
		p->form->setSizePolicy(sizePolicy);
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
