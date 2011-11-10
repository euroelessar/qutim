/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Gabriel Schulhof <nix@go-nix.ca>
** Copyright (C) 2011 Evgeniy Degtyarev <degtep@gmail.com>
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

#ifndef MAEMODOCKLETCONNECTOR_H
#define MAEMODOCKLETCONNECTOR_H

#include "maemo5docklet.h"
#include <dbus/dbus.h>
#include <dbus/dbus-glib-lowlevel.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

#define QUTIM_SYSTRAY_DBUS_SERVICE_NAME "im.qutim.qutim-maemo-docklet"
#define QUTIM_SYSTRAY_DBUS_SERVICE_PATH "/"
#define QUTIM_SYSTRAY_DBUS_SERVICE_METHOD_CONNECT "connect"
#define QUTIM_SYSTRAY_DBUS_SERVICE_METHOD_DISCONNECT "disconnect"
#define QUTIM_SYSTRAY_DBUS_SERVICE_METHOD_CLICKED "clicked"
#define QUTIM_SYSTRAY_DBUS_SERVICE_METHOD_SET_TOOLTIP "set_tooltip"
#define QUTIM_SYSTRAY_DBUS_SERVICE_METHOD_UPDATE_ICON "update_icon"
#define QUTIM_SYSTRAY_DBUS_SERVICE_METHOD_MUTED_STATUS_CHANGED "muted_status_changed"


static gchar *last_sent_buffer = NULL;
static gsize last_sent_buffer_size = 0;
static gboolean last_sent_blink = FALSE;
static int my_handle = 0;

static DBusConnection *dbus_connection(void);

static void
update_last_sent_buffer(gchar *buffer, gsize buffer_size, gboolean blink)
{
	g_free(last_sent_buffer);
	last_sent_buffer = buffer;
	last_sent_buffer_size = buffer_size;
	last_sent_blink = blink;
}

static void
qutim_status_menu_set_tooltip(gchar *str)
{
	DBusConnection *connection = dbus_connection();
	DBusMessage *message = dbus_message_new_method_call(
		QUTIM_SYSTRAY_DBUS_SERVICE_NAME,
		QUTIM_SYSTRAY_DBUS_SERVICE_PATH,
		NULL,
		QUTIM_SYSTRAY_DBUS_SERVICE_METHOD_SET_TOOLTIP);
	DBusMessage *reply;

	if (message) {
		dbus_message_append_args(message, DBUS_TYPE_STRING, &str, DBUS_TYPE_INVALID);
		reply = dbus_connection_send_with_reply_and_block(connection, message, -1, NULL);
		dbus_message_unref(message);
	}

	if (reply)
		dbus_message_unref(reply);
}

static gboolean
need_to_send(gchar *buffer, gsize buffer_size, gboolean blink)
{
	if (blink != last_sent_blink)
		return TRUE;

	if (last_sent_buffer == NULL)
		return TRUE;

	if (last_sent_buffer_size != buffer_size)
		return TRUE;

	return (gboolean)memcmp(buffer, last_sent_buffer, MIN(buffer_size, last_sent_buffer_size));
}



static void
qutim_status_menu_plugin_update_icon(GdkPixbuf *pb, gboolean blink)
{
	gchar *buffer = NULL;
	gsize buffer_size = 0;

	if (gdk_pixbuf_save_to_buffer(pb, &buffer, &buffer_size, "png", NULL, NULL)) {
		if (need_to_send(buffer, buffer_size, blink)) {
			DBusConnection *conn = dbus_connection();
			DBusMessage *message = dbus_message_new_method_call(
				QUTIM_SYSTRAY_DBUS_SERVICE_NAME,
				QUTIM_SYSTRAY_DBUS_SERVICE_PATH,
				NULL,
				QUTIM_SYSTRAY_DBUS_SERVICE_METHOD_UPDATE_ICON);
			DBusMessage *reply;

			dbus_message_append_args(message, DBUS_TYPE_ARRAY, DBUS_TYPE_BYTE, &buffer, buffer_size, DBUS_TYPE_BOOLEAN, &blink, DBUS_TYPE_INVALID);
			reply = dbus_connection_send_with_reply_and_block(conn, message, -1, NULL);
			dbus_message_unref(message);
			if (reply)
				dbus_message_unref(reply);

			update_last_sent_buffer(buffer, buffer_size, blink);
		}
		else {
			g_free(buffer);
			buffer = NULL;
			buffer_size = 0;
		}
	}
}

static void
qutim_status_menu_plugin_update_icon(char * path,bool blink)
{
    GdkPixbuf *pb = gdk_pixbuf_new_from_file(path,NULL);
    qutim_status_menu_plugin_update_icon(pb, blink);
}

static void
qutim_status_menu_plugin_disconnect()
{
	DBusConnection *connection = dbus_connection();
	DBusMessage *message = dbus_message_new_method_call(
		QUTIM_SYSTRAY_DBUS_SERVICE_NAME,
		QUTIM_SYSTRAY_DBUS_SERVICE_PATH,
		NULL,
		QUTIM_SYSTRAY_DBUS_SERVICE_METHOD_DISCONNECT);
	DBusMessage *reply;

	if (message) {
		reply = dbus_connection_send_with_reply_and_block(connection, message, -1, NULL);
		dbus_message_unref(message);
	}
	if (reply)
		dbus_message_unref(reply);
}

static void
mute_changed_cb(bool is_muted)
{
	DBusConnection *conn = dbus_connection();
		DBusMessage *message = dbus_message_new_method_call(
		QUTIM_SYSTRAY_DBUS_SERVICE_NAME,
		QUTIM_SYSTRAY_DBUS_SERVICE_PATH,
		NULL,
		QUTIM_SYSTRAY_DBUS_SERVICE_METHOD_MUTED_STATUS_CHANGED);

	if (message) {
		DBusMessage *reply = NULL;
		dbus_message_append_args(message, DBUS_TYPE_BOOLEAN, &is_muted, DBUS_TYPE_INVALID);
		reply = dbus_connection_send_with_reply_and_block(conn, message, -1, NULL);
		dbus_message_unref(message);
		message = NULL;
		if (reply) {
			dbus_message_unref(reply);
			reply = NULL;
		}
	}
}


static gboolean
qutim_status_menu_plugin_connect()
{
	DBusConnection *connection = dbus_connection();
	DBusMessage *message = dbus_message_new_method_call(
		QUTIM_SYSTRAY_DBUS_SERVICE_NAME,
		QUTIM_SYSTRAY_DBUS_SERVICE_PATH,
		NULL,
		QUTIM_SYSTRAY_DBUS_SERVICE_METHOD_CONNECT);
	DBusMessage *reply = NULL;
	gboolean ret = FALSE;
	DBusError error;

	if (message) {
		dbus_error_init(&error);
		reply = dbus_connection_send_with_reply_and_block(connection, message, -1, &error);
		if (!reply)
		dbus_error_free(&error);
		dbus_message_unref(message);
	}

	if (reply) {
		dbus_error_init(&error);
		if (!dbus_message_get_args(reply, &error, DBUS_TYPE_BOOLEAN, &ret, DBUS_TYPE_INVALID)) {
			ret = FALSE;
		}
		dbus_error_free(&error);
		dbus_message_unref(reply);
	}

	return ret;
}

guint embed_idle_id = 0;

static gboolean
embed_idle(gpointer null) {
	embed_idle_id = 0;
	return FALSE;
}

static void
status_menu_lost() {
	qutim_status_menu_plugin_disconnect();

	if (embed_idle_id) {
		g_source_remove(embed_idle_id);
		embed_idle_id = 0;
	}
}

static void
status_menu_found() {
	update_last_sent_buffer(NULL, 0, FALSE);
}

static DBusHandlerResult message_filter(DBusConnection *connection, DBusMessage *message, gpointer null)
{
	DBusHandlerResult ret = DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
	/* clicked(button) */
	if (DBUS_MESSAGE_TYPE_METHOD_CALL == dbus_message_get_type(message) &&
		!g_strcmp0(QUTIM_SYSTRAY_DBUS_SERVICE_PATH, dbus_message_get_path(message)) &&
		!g_strcmp0(QUTIM_SYSTRAY_DBUS_SERVICE_METHOD_CLICKED, dbus_message_get_member(message))) {
		DBusMessage *reply = dbus_message_new_method_return(message);
		int button = 0;

		if (!dbus_message_get_args(message, NULL, DBUS_TYPE_INT32, &button, DBUS_TYPE_INVALID))
			button = 0;
		if (reply) {
			dbus_connection_send(connection, reply, NULL);
			dbus_connection_flush(connection);
			dbus_message_unref(reply);
		}
		Maemo5Docklet::Instance()->ButtonClickedCallback();
	}
	else
	/* NameOwnerChanged(name, old, new) */
	if (DBUS_MESSAGE_TYPE_SIGNAL == dbus_message_get_type(message) &&
		!g_strcmp0("org.freedesktop.DBus", dbus_message_get_sender(message)) &&
		!g_strcmp0("org.freedesktop.DBus", dbus_message_get_interface(message)) &&
		!g_strcmp0("/org/freedesktop/DBus", dbus_message_get_path(message)) &&
		!g_strcmp0("NameOwnerChanged", dbus_message_get_member(message))) {
		const char *name, *old_owner, *new_owner;

		if (dbus_message_get_args(message, NULL,
			DBUS_TYPE_STRING, &name,
			DBUS_TYPE_STRING, &old_owner,
			DBUS_TYPE_STRING, &new_owner,
			DBUS_TYPE_INVALID)) {
			if (!g_strcmp0(name, QUTIM_SYSTRAY_DBUS_SERVICE_NAME) &&
			    !g_strcmp0(new_owner, ""))
				status_menu_lost();
			else
			if (!g_strcmp0(name, QUTIM_SYSTRAY_DBUS_SERVICE_NAME) &&
			    g_strcmp0(new_owner, "") &&
					g_strcmp0(old_owner, new_owner))
				if (qutim_status_menu_plugin_connect())
					status_menu_found();
		}
	}

	return ret;
}

static DBusConnection *
dbus_connection()
{
	static DBusConnection *conn = NULL;

	if (G_UNLIKELY(NULL == conn)) {
		conn = dbus_bus_get(DBUS_BUS_SESSION, NULL);
		dbus_connection_setup_with_g_main(conn, NULL);
		dbus_connection_add_filter(conn, (DBusHandleMessageFunction)message_filter, NULL, NULL);
		dbus_bus_add_match(conn, "type='signal',sender='org.freedesktop.DBus',interface='org.freedesktop.DBus',path='/org/freedesktop/DBus',member='NameOwnerChanged'", NULL);
		dbus_bus_request_name(conn, "im.qutim.maemo-plugin", DBUS_NAME_FLAG_DO_NOT_QUEUE, NULL);
	}

	return conn;
}


#endif // MAEMODOCKLETCONNECTOR_H

