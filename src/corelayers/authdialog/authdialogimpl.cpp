#include "authdialogimpl.h"
#include <QDialog>
#include <QPointer>
#include "authdialogimpl_p.h"
#include "modulemanagerimpl.h"
#include <libqutim/debug.h>

namespace Core {
	
	static Core::CoreModuleHelper<AuthDialogImpl> auth_dialog_static(
		QT_TRANSLATE_NOOP("Plugin", "Authorization dialog"),
		QT_TRANSLATE_NOOP("Plugin", "Simple authorization dialog")
	);	
	
	void AuthDialogImpl::setContact(qutim_sdk_0_3::Contact* contact, const QString& text)
	{
		debug() << "Auth request from" << contact->title();
		p->show(contact,text);
	}

	AuthDialogImpl::~AuthDialogImpl()
	{
		debug() << "Dialog deleted";
	}
	
	AuthDialogImpl::AuthDialogImpl() :
	p(new AuthDialogPrivate)
	{
		connect(p.data(),SIGNAL(accepted()),SIGNAL(accepted()));
		connect(p.data(),SIGNAL(rejected()),SIGNAL(rejected()));		
		connect(p.data(),SIGNAL(finished(int)),SLOT(deleteLater()));
		connect(p.data(),SIGNAL(destroyed(QObject*)),SLOT(deleteLater()));
	}


}