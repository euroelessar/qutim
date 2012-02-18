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
//#include "quetzalfiletransfer.h"
//#include "quetzalaccount.h"
//#include "quetzalcontact.h"
//#include <QtPlugin>
//
//QuetzalFileTransfer::QuetzalFileTransfer()
//{
//}
//
//bool QuetzalFileTransfer::check(ChatUnit *unit)
//{
//	return !!qobject_cast<QuetzalContact *>(unit);
//}
//
//FileTransferEngine *QuetzalFileTransfer::create(ChatUnit *unit)
//{
//	if (QuetzalContact *contact = qobject_cast<QuetzalContact *>(unit))
//	{
//		return new QuetzalFileTransferEngine(contact, FileTransferEngine::Send, this);
//	}
//	return 0;
//}
//
//FileTransferEngine::Direction cast(PurpleXferType type)
//{
//	switch (type)
//	{
//	 case PURPLE_XFER_SEND:
//		 return FileTransferEngine::Send;
//	 case PURPLE_XFER_RECEIVE:
//		 return FileTransferEngine::Receive;
//	 default:
//		 return static_cast<FileTransferEngine::Direction>(-1);
//	}
//}
//
//static void quetzal_xfer_new_xfer(PurpleXfer *xfer)
//{
//	QuetzalAccount *acc = reinterpret_cast<QuetzalAccount *>(xfer->account->ui_data);
//	QuetzalContact *contact = qobject_cast<QuetzalContact *>(acc->getUnit(xfer->who, false));
//	if (!contact)
//		return;
////	xfer->
//	new QuetzalFileTransferEngine(xfer);
//}
//
//static void quetzal_xfer_destroy(PurpleXfer *xfer)
//{
//	reinterpret_cast<QuetzalFileTransferEngine*>(xfer->ui_ops)->deleteLater();
//}
//
//static void quetzal_xfer_add_xfer(PurpleXfer *xfer)
//{
//}
//
//static void quetzal_xfer_update_progress(PurpleXfer *xfer, double percent)
//{
//}
//
//static void quetzal_xfer_cancel_local(PurpleXfer *xfer)
//{
//}
//
//static void quetzal_xfer_cancel_remote(PurpleXfer *xfer)
//{
//}
//
//
//PurpleXferUiOps quetzal_xfer_uiops =
//{
//	quetzal_xfer_new_xfer,
//	quetzal_xfer_destroy,
//	quetzal_xfer_add_xfer,
//	quetzal_xfer_update_progress,
//	quetzal_xfer_cancel_local,
//	quetzal_xfer_cancel_remote
//};
//
//Q_EXTERN_C Q_DECL_EXPORT void Q_STANDARD_CALL
//purple_xfer_request(PurpleXfer *xfer)
//{
//	g_return_if_fail(xfer != NULL);
//	g_return_if_fail(xfer->ops.init != NULL);
//
//	purple_xfer_ref(xfer);
//	if (xfer->type == PURPLE_XFER_RECEIVE) {
//
//	} else {
//		// Is it really possible?
//	}
//}

