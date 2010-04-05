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
	
	void AuthDialogImpl::setContact(qutim_sdk_0_3::Contact* contact, const QString& text, bool incoming)
	{
		p->show(contact, text, incoming);
	}

	QString AuthDialogImpl::text() const
	{
		return p->text();
	}

	AuthDialogImpl::~AuthDialogImpl()
	{
	}
	
	AuthDialogImpl::AuthDialogImpl() :
	p(new AuthDialogPrivate)
	{
		connect(p.data(),SIGNAL(finished(int)),SLOT(onFinished(int)));
		connect(p.data(),SIGNAL(destroyed(QObject*)),SLOT(deleteLater()));
	}

	void AuthDialogImpl::onFinished(int result)
	{
		if (result == QDialog::Accepted)
			emit accepted();
		else
			emit rejected();
		emit finished(result == QDialog::Accepted);
		deleteLater();
	}

}
