/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

typedef enum
{
	PURPLE_REQUEST_INPUT = 0,  /**< Text input request.        */
	PURPLE_REQUEST_CHOICE,     /**< Multiple-choice request.   */
	PURPLE_REQUEST_ACTION,     /**< Action request.            */
	PURPLE_REQUEST_FIELDS,     /**< Multiple fields request.   */
	PURPLE_REQUEST_FILE,       /**< File open or save request. */
	PURPLE_REQUEST_FOLDER      /**< Folder selection request.  */

} PurpleRequestType;

class FileDialog : public KFileDialog
{
	Q_OBJECT
public:
	FileDialog(const KUrl &startDir, const QString &title,
			   GCallback ok_cb, GCallback cancel_cb, void *user_data);
protected:
	virtual void slotOk();
	virtual void slotCancel();
	virtual void closeEvent(QCloseEvent *e);
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

	void (*close_request)(PurpleRequestType type, void *ui_handle);

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
typedef void (*_purple_request_close)(PurpleRequestType type, void *uihandle);

extern void initGui();
}

#endif // QUETZALGUI_H

