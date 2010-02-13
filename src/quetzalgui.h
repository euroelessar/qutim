#ifndef QUETZALGUI_H
#define QUETZALGUI_H

#include <qglobal.h>
#include <KDE/KFileDialog>
#include <KDE/KDirSelectDialog>

namespace Quetzal
{
	typedef int gboolean;
	typedef void (*GCallback) (void);
	typedef void (*PurpleRequestFileCb)(void *, const char *filename);
	class PurpleAccount;
	class PurpleConversation;

	class FileDialog : public KFileDialog
	{
		Q_OBJECT
	public:
		FileDialog(const KUrl &startDir, const QString &title,
				   GCallback ok_cb, GCallback cancel_cb, void *user_data);
	protected:
		virtual void slotOk();
		virtual void slotCancel();
	private:
		PurpleRequestFileCb m_ok_cb;
		PurpleRequestFileCb m_cancel_cb;
		void *m_user_data;
	};

	typedef struct
	{
		void (*_purple_unused1)(void);
		void (*_purple_unused2)(void);
		void (*_purple_unused3)(void);
		void (*_purple_unused4)(void);

		void *(*request_file)(const char *title, const char *filename,
							  gboolean savedialog, GCallback ok_cb,
							  GCallback cancel_cb, PurpleAccount *account,
							  const char *who, PurpleConversation *conv,
							  void *user_data);

		void (*_purple_unused5)(void);

		void *(*request_folder)(const char *title, const char *dirname,
								GCallback ok_cb, GCallback cancel_cb,
								PurpleAccount *account, const char *who,
								PurpleConversation *conv, void *user_data);

		void (*_purple_reserved1)(void);
		void (*_purple_reserved2)(void);
		void (*_purple_reserved3)(void);
		void (*_purple_reserved4)(void);
	} PurpleRequestUiOps;

	typedef PurpleRequestUiOps *(*_purple_request_get_ui_ops)(void);

	extern void initGui();
}

#endif // QUETZALGUI_H
