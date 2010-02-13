#include "quetzalgui.h"
#include <QLibrary>
#include <QFileInfo>

namespace Quetzal
{
	FileDialog::FileDialog(const KUrl &startDir, const QString &title,
						   GCallback ok_cb, GCallback cancel_cb, void *user_data)
			 : KFileDialog(startDir, QString(), 0)
	{
		setWindowTitle(title);
		m_ok_cb = (PurpleRequestFileCb)ok_cb;
		m_cancel_cb = (PurpleRequestFileCb)cancel_cb;
		m_user_data = user_data;
		setAttribute(Qt::WA_QuitOnClose, false);
	}

	void FileDialog::slotOk()
	{
		if (m_ok_cb)
			m_ok_cb(m_user_data, selectedFile().toUtf8().constData());
		m_ok_cb = NULL;
		close();
	}

	void FileDialog::slotCancel()
	{
		if (m_cancel_cb)
			m_cancel_cb(m_user_data, selectedFile().toUtf8().constData());
		m_cancel_cb = NULL;
		close();
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
		_purple_request_get_ui_ops purple_request_get_ui_ops;
		purple_request_get_ui_ops = (_purple_request_get_ui_ops)lib.resolve("purple_request_get_ui_ops");

		PurpleRequestUiOps *ui_ops = purple_request_get_ui_ops();
		ui_ops->request_file = kde_request_file;
		ui_ops->request_folder = kde_request_folder;
	}
}
