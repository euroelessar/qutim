/*
  LogWidgetHome LogsCity

    Copyright (c) 2009 by Rustam Chakin <qutim.develop@gmail.com>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

#ifndef LOGSCITY_H
#define LOGSCITY_H

#include <QObject>
#include <QWebPage>
#include <QTextDocument>
#include <QHash>
#include <QWebFrame>
#include "tempglobalinstance.h"
#include "confcontactlist.h"
#include "chatwindowstyleoutput.h"

/*
char nickname_colors[] = {
"aqua", "aquamarine", "blue", "blueviolet", "brown","burlywood", "cadetblue", "chartreuse",
"chocolate", "coral", "cornflowerblue", "crimson", "cyan", "darkblue", "darkcyan", "darkgoldenrod",
"darkgreen", "darkgrey", "darkkhaki", "darkmagenta", "darkolivegreen", "darkorange", "darkorchid", "darkred",
"darksalmon", "darkseagreen", "darkslateblue", "darkslategrey", "darkturquoise", "darkviolet", "deeppink",
"deepskyblue", "dimgrey", "dodgerblue", "firebrick", "forestgreen", "fuchsia", "gold", "goldenrod", "green",
"greenyellow", "grey", "hotpink", "indianred", "indigo", "lawngreen", "lightblue", "lightcoral", "lightgreen",
"lightgrey", "lightpink", "lightsalmon", "lightseagreen", "lightskyblue", "lightslategrey", "lightsteelblue", "lime",
"limegreen", "magenta", "maroon", "mediumaquamarine", "mediumblue", "mediumorchid", "mediumpurple", "mediumseagreen",
"mediumslateblue", "mediumspringgreen", "mediumturquoise", "mediumvioletred", "midnightblue", "navy", "olive",
"olivedrab", "orange", "orangered", "orchid", "palegreen", "paleturquoise", "palevioletred", "peru", "pink", "plum",
"powderblue", "purple", "red", "rosybrown", "royalblue", "saddlebrown", "salmon", "sandybrown", "seagreen", "sienna",
"silver", "skyblue", "slateblue", "slategrey", "springgreen", "steelblue", "tan", "teal", "thistle", "tomato",
"turquoise", "violet", "yellowgreen"
};
*/



using namespace qutim_sdk_0_2;

struct TextEditState {
    QString m_text;
    quint64 m_cursor_position;
};

class LogWidgetHome;

class LogJSHelper : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(LogJSHelper)
public:
	LogJSHelper(LogWidgetHome *home);
public slots:
	void debugLog( const QVariant &text );
	bool zoomImage( const QVariant &text );
	void helperCleared();
	void appendNick( const QVariant &nick );
	void contextMenu( const QVariant &nick );
private:
	LogWidgetHome *m_home;
};

class LogWidgetHome {
public:

    LogWidgetHome(bool am_i_webkit,bool am_i_conference, const QString &webkit_style, const QString &webkit_variant);
    ~LogWidgetHome();
    bool m_i_am_webkit_unit;
    bool m_i_am_conference;
    QTextDocument *m_text_document;
    QWebPage *m_web_page;
	LogJSHelper *m_web_helper;
    void addMessage(const QString &message,
				     const QDateTime &date, bool history, bool in, const QString &from = "", bool win_active = true);
    ConfContactList *m_contact_list;
    TextEditState m_edit_state;

    QString m_onwer_nickname;
    QString m_contact_nickname;
    QString m_owner_id;
    QString m_contact_id;
    QVector<quint64> m_message_positions;
    bool m_remove_after;
    quint32 m_remove_count;
    bool m_show_names;
    int m_timestamp;

    QString getTimeStamp(const QDateTime &message_date);
    quint64 getSendIconPos();
    quint64 m_last_message_icon_position;
    QHash<quint64,quint64> m_message_position_offset;
    void messageDelievered(int);
    void addConferenceItem(const QString &name);
    QHash<QString, QString> m_color_names;
    void renameConferenceItem(const QString &old_name, const QString &name);
    void removeConferenceItem(const QString &name);
    bool m_colorize_nicknames;
    void clearMyLog();
    void addServiceMessage(const QString &message);
    qint64 m_horizontal_separator_position;
    bool m_separator_added;
    bool m_light_kill;
    QString m_webkit_style_path;
    QString m_webkit_variant;
    ChatWindowStyleOutput *m_style_output;
	void loadWebkitStyle( const QString &head );
    QString m_own_avatar;
    QString m_contact_avatar;
    QString m_protocol_name;
    QString m_now_html;
    QString m_previous_sender;
    bool m_history_loaded;
	TreeModelItem m_item;
};

class LogsCity
{

public:
    LogsCity();
    ~LogsCity();
	void release();
    void createHomeForMe(const TreeModelItem &item, bool webkit_mode, QString contact_nick = "", QString owner_nick = "",
			 const QString &m_own_avatar = "", const QString &m_contact_avatar = "");
    QTextDocument *giveMeMyHomeDocument(const QString & identification);
    QTextDocument *giveMeMyHomeDocument(const TreeModelItem &item);
    QWebPage *giveMeMyHomeWebPage(const QString & identification);
    QWebPage *giveMeMyHomeWebPage(const TreeModelItem &item);
    ConfContactList *getConferenceCL(const TreeModelItem &item);
    void setConferenceListView(const TreeModelItem &item, QListView *list_view);
    static LogsCity &instance()
    {
	static LogsCity lc;
	return lc;
    }

    QHash<QString,LogWidgetHome*> m_city_map;
	void moveMyHome( const QString &old_id, const QString &new_id );
    bool doIHaveHome(const TreeModelItem &item);
    bool doIHaveHome(const QString  &identification);
    void destroyMyHome(const TreeModelItem &item, bool light_bomb = false);
    void addConferenceItem(const TreeModelItem &item, const QString &name);
    void setConferenceItemStatus(const TreeModelItem &item, const QIcon &icon, const QString &status, int mass);
    void renameConferenceItem(const TreeModelItem &item, const QString &new_name);
    void removeConferenceItem(const TreeModelItem &item);
    void setConferenceItemIcon(const TreeModelItem &item, const QIcon &icon, int position);
    void setConferenceItemRole(const TreeModelItem &item, const QIcon &icon, const QString &role, int mass);
    bool addMessage(const TreeModelItem &item, const QString &message,
				     const QDateTime &date, bool history, bool in,bool win_active = true);
    void changeId(const TreeModelItem &item, const QString &new_id);
    void setEditState(const TreeModelItem &item, const QString &text, quint64 position);
    TextEditState getEditState(const TreeModelItem &item);
    void loadSettings(const QString &profile_name);
    bool m_remove_after;
    quint32 m_remove_count;
    bool m_show_names;
    int m_timestamp;
    void messageDelievered(const TreeModelItem &item, int message_position);
    void changeOwnNickNameInConference(const TreeModelItem &item, const QString &new_nickname);
    bool m_colorize_nicknames;
    void clearMyHomeLog(const TreeModelItem &item);
    void addServiceMessage(const TreeModelItem &item, const QString &message);
    void notifyAboutFocusingConference(const TreeModelItem &item);
    void loadGuiSettings();
	void appendHtmlToHead( const QString &html, const QList<quint8> &types );
    QString m_profile_name;
    QString m_webkit_style_path;
    QString m_webkit_variant;
    QString findUrls(const QString &message,bool webkit);
    void addImage(const TreeModelItem &item, const QByteArray &image_raw,bool in) ;
    QString m_tmp_image_path;
	QMap<quint8, QString> m_html_for_head;

};

#endif // LOGSCITY_H
