#include "quetzalfiledialog.h"
#include <QEvent>
#include <qutim/debug.h>

using namespace qutim_sdk_0_3;

QuetzalFileDialog::QuetzalFileDialog(const char *title, const QString &dirname,
									 GCallback ok_cb, GCallback cancel_cb,
									 void *user_data, QFileDialog *parent)
			  : QObject(parent)
{
	parent->setWindowTitle(title);
	parent->setDirectory(dirname);
	m_ok_cb = (PurpleRequestFileCb) ok_cb;
	m_cancel_cb = (PurpleRequestFileCb) cancel_cb;
	m_user_data = user_data;
	m_dialog = parent;
	connect(parent, SIGNAL(accepted()), this, SLOT(onAccept()));
	connect(parent, SIGNAL(rejected()), this, SLOT(onReject()));
}

void QuetzalFileDialog::onAccept()
{
	if (m_ok_cb)
		m_ok_cb(m_user_data, m_dialog->selectedFiles().value(0).toUtf8().constData());
	m_ok_cb = NULL;
	purple_request_close(m_dialog->fileMode() == QFileDialog::Directory
						 ? PURPLE_REQUEST_FOLDER : PURPLE_REQUEST_FILE, m_dialog);
}

void QuetzalFileDialog::onReject()
{
	if (m_cancel_cb)
		m_cancel_cb(m_user_data, m_dialog->selectedFiles().value(0).toUtf8().constData());
	m_cancel_cb = NULL;
	purple_request_close(m_dialog->fileMode() == QFileDialog::Directory
						 ? PURPLE_REQUEST_FOLDER : PURPLE_REQUEST_FILE, m_dialog);
}
