/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/
#include "quetzalgui.h"
#include <QLibrary>
#include <QFileInfo>

namespace Quetzal
{
static _purple_request_get_ui_ops purple_request_get_ui_ops = NULL;
static _purple_request_close purple_request_close = NULL;

static void kde_request_close(FileDialog *dialog)
{
	PurpleRequestType type;
	if (dialog->mode() & KFile::File )
		type = PURPLE_REQUEST_FILE;
	else
		type = PURPLE_REQUEST_FOLDER;
	purple_request_close(type, dialog);
}

FileDialog::FileDialog(const KUrl &startDir, const QString &title,
					   GCallback ok_cb, GCallback cancel_cb, void *user_data)
	: KFileDialog(startDir, QString(), 0)
{
	setWindowTitle(title);
	m_ok_cb = (PurpleRequestFileCb)ok_cb;
	m_cancel_cb = (PurpleRequestFileCb)cancel_cb;
	m_user_data = user_data;
	connect(this, SIGNAL(closeClicked()), this, SLOT(slotCancel()));
	setAttribute(Qt::WA_DeleteOnClose, true);
	setAttribute(Qt::WA_QuitOnClose, false);
}

void FileDialog::slotOk()
{
	if (m_ok_cb)
		m_ok_cb(m_user_data, selectedFile().toUtf8().constData());
	m_ok_cb = NULL;
	kde_request_close(this);
}

void FileDialog::slotCancel()
{
	if (m_cancel_cb)
		m_cancel_cb(m_user_data, selectedFile().toUtf8().constData());
	m_cancel_cb = NULL;
	kde_request_close(this);
}

void FileDialog::closeEvent(QCloseEvent *e)
{
	slotCancel();
	KFileDialog::closeEvent(e);
}

static void *kde_request_file(const char *title, const char *filename,
							  gboolean savedialog, GCallback ok_cb,
							  GCallback cancel_cb, PurpleAccount *account,
							  const char *who, PurpleConversation *conv,
							  void *user_data)
{
	Q_UNUSED(account);
	Q_UNUSED(who);
	Q_UNUSED(conv);
	QFileInfo info = QString(filename);
	KFileDialog *dialog = new FileDialog(info.absolutePath(), title, ok_cb, cancel_cb, user_data);
	dialog->setOperationMode(savedialog ? KFileDialog::Saving : KFileDialog::Opening);
	dialog->setMode(KFile::File);
	dialog->show();
	return dialog;
}

static void *kde_request_folder(const char *title, const char *dirname,
								GCallback ok_cb, GCallback cancel_cb,
								PurpleAccount *account, const char *who,
								PurpleConversation *conv, void *user_data)
{
	Q_UNUSED(account);
	Q_UNUSED(who);
	Q_UNUSED(conv);
	FileDialog *dialog = new FileDialog(KUrl(dirname), title, ok_cb, cancel_cb, user_data);
	dialog->setMode(KFile::Directory | KFile::ExistingOnly);
	dialog->show();
	return dialog;
}

void initGui()
{
	QLibrary lib("purple");
	if (!lib.load()) {
#if defined(Q_OS_UNIX) && !defined(Q_OS_SYMBIAN)
		// So try to guess purple path
		QStringList mayBePaths(QLatin1String("/usr/lib"));
#ifdef PURPLE_LIBDIR
		mayBePaths << QString::fromLocal8Bit(PURPLE_LIBDIR);
#endif
#if QT_POINTER_SIZE == 8
		mayBePaths << QLatin1String("/usr/lib64");
#elif QT_POINTER_SIZE == 4
		mayBePaths << QLatin1String("/usr/lib32");
#endif
#ifdef Q_WS_MAEMO_5
		mayBePaths << QLatin1String("/opt/maemo/usr/lib");
#endif
		mayBePaths.removeDuplicates();
		QStringList filter(QLatin1String("libpurple.so*"));
		bool ok = false;
		foreach (const QString &path, mayBePaths) {
			QDir dir(path);
			foreach (const QString &name, dir.entryList(filter, QDir::Files)) {
				lib.setFileName(dir.filePath(name));
				if ((ok |= lib.load()))
					break;
			}
			if (ok)
				break;
		}
		if (!ok)
#endif
			return;
	}
	purple_request_get_ui_ops = (_purple_request_get_ui_ops)lib.resolve("purple_request_get_ui_ops");
	purple_request_close = (_purple_request_close)lib.resolve("purple_request_close");

	PurpleRequestUiOps *ui_ops = purple_request_get_ui_ops();
	ui_ops->request_file = kde_request_file;
	ui_ops->request_folder = kde_request_folder;
}
}

