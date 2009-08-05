#include "addaccountwizard.h"
#include "iconmanager.h"

AddAccountWizard::AddAccountWizard(QWidget *parent)
    : QWizard(parent)
{
    m_protocol_page = new ProtocolPage;
    m_login_page = new LastLoginPage;
	setPage(Page_Protocol, m_protocol_page);
    setPage(Page_Login, m_login_page);
    setStartId(Page_Protocol);
#ifndef Q_WS_MAC
    setWizardStyle(ModernStyle);
#endif
	setPixmap(QWizard::LogoPixmap, Icon("qutim").pixmap(QSize(64,64)));
    setWindowTitle(tr("Add Account Wizard"));
    connect(this, SIGNAL(currentIdChanged(int)), SLOT(on_currentIdChanged(int)));
	PluginSystem::instance().centerizeWidget(this);
}

AddAccountWizard::~AddAccountWizard()
{
	delete m_protocol_page;
	delete m_login_page;
}

void AddAccountWizard::addProtocolsToWizardList(const PluginInfoList &protocol_list)
{
	foreach(PluginInfo information_about_plugin, protocol_list)
	{
		m_protocol_page->addItemToList(information_about_plugin);
		
	}
}

QString AddAccountWizard::getChosenProtocol() const
{
	return m_protocol_page->getChosenProtocol();
}

void AddAccountWizard::on_currentIdChanged(int id)
{
	QString protocol_name = m_protocol_page->getChosenProtocol();
	if ( !id && !protocol_name.isEmpty() )
	{	
		PluginSystem &ps = PluginSystem::instance();
		ps.removeLoginWidgetByName(protocol_name);
			
	}
	else if ( id == 1 && !protocol_name.isEmpty())
	{
		PluginSystem &ps = PluginSystem::instance();
		m_login_page->setLoginForm(ps.getLoginWidget(protocol_name));
	}
}

QPoint AddAccountWizard::desktopCenter()
{
	QDesktopWidget &desktop = *QApplication::desktop();
	return QPoint(desktop.width() / 2 - size().width() / 2, desktop.height() / 2 - size().height() / 2);
}

ProtocolPage::ProtocolPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Please choose IM protocol"));
    setPixmap(QWizard::WatermarkPixmap, QPixmap(":/pictures/wizard.png"));

    m_top_label = new QLabel(tr("This wizard will help you add your account of "
                             "chosen protocol. You always can add or delete "
                             "accounts from Main settings -> Accounts"), this);
    m_top_label->setWordWrap(true);

    m_protocol_list = new QListWidget(this);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(m_top_label);
    layout->addWidget(m_protocol_list);
    setLayout(layout);
}

void ProtocolPage::addItemToList(const PluginInfo &information_about_plugin)
{
	QListWidgetItem *item = new QListWidgetItem(information_about_plugin.name);
	item->setIcon(Icon(information_about_plugin.name.toLower(), IconInfo::Protocol));

	m_protocol_list->addItem(item);
	
	if ( m_protocol_list->count() )
	{
		m_protocol_list->setCurrentRow(0);
	}
}

int ProtocolPage::nextId() const
{   
	return AddAccountWizard::Page_Login;
}

QString ProtocolPage::getChosenProtocol() const
{
	if( m_protocol_list->count() )
	{
		return m_protocol_list->currentItem()->text();
	} 
	else
	{
		return QString();
	}
}

LastLoginPage::LastLoginPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Please type chosen protocol login data"));
    setSubTitle(tr("Please fill all fields."));
    layout = new QVBoxLayout;
    setLayout(layout);
}

int LastLoginPage::nextId() const
{
	return -1;
}

void LastLoginPage::setLoginForm(QWidget *login_form)
{
	if ( login_form )
    {
		layout->addWidget(login_form);
    }
}
