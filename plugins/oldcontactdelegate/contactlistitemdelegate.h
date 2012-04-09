/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Alexey Prokhin <alexey.prokhin@yandex.ru>
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

#ifndef CONTACTLISTITEMDELEGATE_H_
#define CONTACTLISTITEMDELEGATE_H_

#include <QItemDelegate>
#include <QAbstractItemDelegate>
#include <QPainter>
#include <QTreeView>
#include <QDomElement>
#include <QDomNode>
#include <QDomDocument>
#include <QTreeView>
#include <QFont>
#include <QVector>
#include <QHash>
#include <QVariant>
#include <qutim/debug.h>
#include <qutim/status.h>
#include <qutim/plugin.h>
#include <qutim/settingslayer.h>

using namespace qutim_sdk_0_3;

enum ShowFlag
{
	ShowStatusText = 0x1,
	ShowExtendedInfoIcons = 0x2,
	ShowAvatars = 0x4
};
Q_DECLARE_FLAGS(ShowFlags, ShowFlag)

enum ContactItemRole
{
	BuddyRole = Qt::UserRole,
	StatusRole,
	ContactsCountRole,
	OnlineContactsCountRole,
	AvatarRole,
	ItemTypeRole,
	AccountRole,
	Color
};
Q_DECLARE_FLAGS(ContactItemRoles,ContactItemRole)
enum ContactItemType
{
	InvalidType = 0,
	TagType = 100,
	ContactType = 101,
	AccountType = 102,
	SeparatorType = 103
};

enum OscarStatusEnum
{
	// Status FLAGS (used to determine status of other users)
	OscarOnline         = 0x0000,
	OscarAway           = 0x0001,
	OscarDND            = 0x0002,
	OscarNA             = 0x0004,
	OscarOccupied       = 0x0010,
	OscarInvisible      = 0x0100,
	// Qutim Status Flags
	OscarOffline        = 0x8000
};

enum QipStatusEnum
{
	// QIP Extended Status flags
	OscarFFC            = 0x0020,
	OscarEvil           = 0x3000,
	OscarDepress        = 0x4000,
	OscarHome           = 0x5000,
	OscarWork           = 0x6000,
	OscarLunch          = 0x2001
};

enum SelectionTypes
{
	Normal = 3000000,
	Hover  = 4000000,
	Down   = 5000000
};

class OldContactDelegatePlugin : public qutim_sdk_0_3::Plugin
{
	Q_OBJECT
	Q_CLASSINFO("DebugName", "OldContactDelegate")
public:
	OldContactDelegatePlugin();
	void init();
	bool load();
	bool unload();
};

class ContactListItemDelegate : public QAbstractItemDelegate
{
	Q_OBJECT
	Q_CLASSINFO("Service","ContactDelegate")
	Q_CLASSINFO("Uses", "IconLoader")
	Q_CLASSINFO("SettingsDescription", "qutIM 0.2 style")
public:
	explicit ContactListItemDelegate(QObject *parent = 0);
	virtual ~ContactListItemDelegate();
	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index ) const;
	QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	void setEditorData(QWidget *editor, const QModelIndex &index) const;
	void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
	void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	bool setThemeStyle(const QString &name = QString());
	QString getThemePath(const QString &themeName);
public slots:
	bool helpEvent(QHelpEvent *event, QAbstractItemView *view,
				   const QStyleOptionViewItem &option,
				   const QModelIndex &index);
	QString styleSheet() const { return m_css; }
	void reloadSettings();
signals:
	void styleSheetChanged(const QString &css);
private:
	QPixmap getAlphaMask(QPainter *painter, QRect rect, int type) const;
	QMap<QString,QVariant> appendStyleFile(QString path);
	QMap<QString,QVariant> parse(const QDomNode & root_element);
	QSize size(const QStyleOptionViewItem &option, const QModelIndex & index, const QVariant &item) const;
	void draw(QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect, const QVariant &item) const;
	QSize uniPaint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index, const bool &paint=false) const;
	QVariant getValue(const QString &name, const QVariant &def) const;
	QColor getColor(QVariant var) const;
	QFont getFont(QVariant var) const;
	void drawGradient(QPainter *painter, const QColor & up, const QColor & down, const QRect & rect/*, int type*/) const;
	void drawRect(QPainter *painter, const QColor & color/*, const QRect & rect, int type*/) const;
	void drawRect(QPainter *painter, QRect rect) const;
	static const QWidget *getWidget(const QStyleOptionViewItem &option);
	static QStyle *getStyle(const QStyleOptionViewItem &option);
	static int statusToID(const Status &status);
	QAbstractItemView *getContactListView();
	inline void setFlag(ShowFlags flag, bool on = true);
private:
	struct StyleVar
	{
		StyleVar() :
			m_fontBold(false),
			m_fontItalic(false),
			m_fontOverline(false),
			m_fontUnderline(false),
			m_borderWidth(0)
		{}
		QFont m_font;
		int m_backgroundType;
		QVector<double> m_stops;
		QVector<QColor> m_colors;
		QColor m_text_color;
		bool m_fontBold;
		bool m_fontItalic;
		bool m_fontOverline;
		bool m_fontUnderline;
		QColor m_background;
		int m_borderWidth;
		QColor m_borderColor;
		QColor m_statusColor;
		QFont m_statusFont;
	};

	struct QutimStyleData
	{
		QHash<int, StyleVar> styles;
		QFont status_font;
		QHash<int, QColor> status_color;
	};

	struct AdiumColors
	{
		QColor fontColor;
		QColor labelColor;
	};

	struct AdiumStyleData
	{
		bool isGroupGradient;
		bool isContactFontValid;
		QFont contactFont;
		QColor backgroundColor;
		AdiumColors onlineStyle;
		AdiumColors offlineStyle;
		AdiumColors awayStyle;
		bool isGrouptFontValid;
		QFont groupFont;
		QColor groupBackground;
		QColor groupGradient;
		AdiumColors groupStyle;
	};

	enum StyleType
	{
		AdiumStyle,
		QutimStyle,
		LightStyle
	};

	union
	{
		AdiumStyleData *a;
		QutimStyleData *q;
	};
	StyleType m_styleType;
private:
	int m_margin;
	QMap<QString,QVariant> m_styleHash;
private:
//	QScopedPointer<qutim_sdk_0_3::SettingsItem> m_settings;
	ShowFlags m_showFlags;
	QHash<QString, bool> m_extInfo;
	int m_statusIconSize;
	int m_extIconSize;
	int m_avatarSize;
	int m_tagIconSize;
	int m_accountIconSize;
	QString m_css;
};

#endif /*CONTACTLISTITEMDELEGATE_H_*/

