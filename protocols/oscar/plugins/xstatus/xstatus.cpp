/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Alexey Prokhin <alexey.prokhin@yandex.ru>
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

#include "xstatus.h"
#include "../../src/icqcontact.h"
#include "../../src/icqaccount.h"
#include "../../src/oscarconnection.h"
#include "../../src/sessiondataitem.h"
#include "statusdialog.h"
#include "../../src/icqprotocol.h"
#include <qutim/icon.h>
#include <qutim/extensionicon.h>
#include <qutim/statusactiongenerator.h>
#include <qutim/message.h>
#include <qutim/event.h>
#include "xstatusrequester.h"
#include "xstatussender.h"
#include "xstatussettings.h"
#include <QApplication>
#include <qutim/systemintegration.h>

namespace qutim_sdk_0_3 {

namespace oscar {

QHash<Capability, OscarStatusData> XStatusHandler::qipstatuses;

static XStatusList init_xstatus_list()
{
    XStatusList list;
    list << XStatus("edit-delete-status-xstatus")
         << XStatus(QT_TRANSLATE_NOOP("XStatus", "Angry"), "angry", 23,
                    Capability(0x01, 0xD8, 0xD7, 0xEE, 0xAC, 0x3B, 0x49, 0x2A,
                               0xA5, 0x8D, 0xD3, 0xD8, 0x77, 0xE6, 0x6B, 0x92))
         << XStatus(QT_TRANSLATE_NOOP("XStatus", "Taking a bath"), "grooming-taking_a_bath", 1,
                    Capability(0x5A, 0x58, 0x1E, 0xA1, 0xE5, 0x80, 0x43, 0x0C,
                               0xA0, 0x6F, 0x61, 0x22, 0x98, 0xB7, 0xE4, 0xC7))
         << XStatus(QT_TRANSLATE_NOOP("XStatus", "Tired"), "tired", 2,
                    Capability(0x83, 0xC9, 0xB7, 0x8E, 0x77, 0xE7, 0x43, 0x78,
                               0xB2, 0xC5, 0xFB, 0x6C, 0xFC, 0xC3, 0x5B, 0xEC))
         << XStatus(QT_TRANSLATE_NOOP("XStatus", "Party"), "relaxing-partying", 3,
                    Capability(0xE6, 0x01, 0xE4, 0x1C, 0x33, 0x73, 0x4B, 0xD1,
                               0xBC, 0x06, 0x81, 0x1D, 0x6C, 0x32, 0x3D, 0x81))
         << XStatus(QT_TRANSLATE_NOOP("XStatus", "Drinking beer"), "drinking-having_a_beer", 4,
                    Capability(0x8C, 0x50, 0xDB, 0xAE, 0x81, 0xED, 0x47, 0x86,
                               0xAC, 0xCA, 0x16, 0xCC, 0x32, 0x13, 0xC7, 0xB7))
         << XStatus(QT_TRANSLATE_NOOP("XStatus", "Thinking"), "inactive-thinking", 5,
                    Capability(0x3F, 0xB0, 0xBD, 0x36, 0xAF, 0x3B, 0x4A, 0x60,
                               0x9E, 0xEF, 0xCF, 0x19, 0x0F, 0x6A, 0x5A, 0x7F))
         << XStatus(QT_TRANSLATE_NOOP("XStatus", "Eating"), "eating", 6,
                    Capability(0xF8, 0xE8, 0xD7, 0xB2, 0x82, 0xC4, 0x41, 0x42,
                               0x90, 0xF8, 0x10, 0xC6, 0xCE, 0x0A, 0x89, 0xA6))
         << XStatus(QT_TRANSLATE_NOOP("XStatus", "Watching TV"), "relaxing-watching_tv", 7,
                    Capability(0x80, 0x53, 0x7D, 0xE2, 0xA4, 0x67, 0x4A, 0x76,
                               0xB3, 0x54, 0x6D, 0xFD, 0x07, 0x5F, 0x5E, 0xC6))
         << XStatus(QT_TRANSLATE_NOOP("XStatus", "Meeting"), "working-in_a_meeting", 8,
                    Capability(0xF1, 0x8A, 0xB5, 0x2E, 0xDC, 0x57, 0x49, 0x1D,
                               0x99, 0xDC, 0x64, 0x44, 0x50, 0x24, 0x57, 0xAF))
         << XStatus(QT_TRANSLATE_NOOP("XStatus", "Coffee"), "drinking-having_coffee", 9,
                    Capability(0x1B, 0x78, 0xAE, 0x31, 0xFA, 0x0B, 0x4D, 0x38,
                               0x93, 0xD1, 0x99, 0x7E, 0xEE, 0xAF, 0xB2, 0x18))
         << XStatus(QT_TRANSLATE_NOOP("XStatus", "Listening to music"), "listening_to_music", 10,
                    Capability(0x61, 0xBE, 0xE0, 0xDD, 0x8B, 0xDD, 0x47, 0x5D,
                               0x8D, 0xEE, 0x5F, 0x4B, 0xAA, 0xCF, 0x19, 0xA7))
         << XStatus(QT_TRANSLATE_NOOP("XStatus", "Business"), "traveling-commuting", 11,
                    Capability(0x48, 0x8E, 0x14, 0x89, 0x8A, 0xCA, 0x4A, 0x08,
                               0x82, 0xAA, 0x77, 0xCE, 0x7A, 0x16, 0x52, 0x08))
         << XStatus(QT_TRANSLATE_NOOP("XStatus", "Shooting"), "shooting", 12,
                    Capability(0x10, 0x7A, 0x9A, 0x18, 0x12, 0x32, 0x4D, 0xA4,
                               0xB6, 0xCD, 0x08, 0x79, 0xDB, 0x78, 0x0F, 0x09))
         << XStatus(QT_TRANSLATE_NOOP("XStatus", "Having fun"), "playful", 13,
                    Capability(0x6F, 0x49, 0x30, 0x98, 0x4F, 0x7C, 0x4A, 0xFF,
                               0xA2, 0x76, 0x34, 0xA0, 0x3B, 0xCE, 0xAE, 0xA7))
         << XStatus(QT_TRANSLATE_NOOP("XStatus", "On the phone"), "talking", 14,
                    Capability(0x12, 0x92, 0xE5, 0x50, 0x1B, 0x64, 0x4F, 0x66,
                               0xB2, 0x06, 0xB2, 0x9A, 0xF3, 0x78, 0xE4, 0x8D))
         << XStatus(QT_TRANSLATE_NOOP("XStatus", "Gaming"), "relaxing-gaming", 15,
                    Capability(0xD4, 0xA6, 0x11, 0xD0, 0x8F, 0x01, 0x4E, 0xC0,
                               0x92, 0x23, 0xC5, 0xB6, 0xBE, 0xC6, 0xCC, 0xF0))
         << XStatus(QT_TRANSLATE_NOOP("XStatus", "Studying"), "working-studying", 16,
                    Capability(0x60, 0x9D, 0x52, 0xF8, 0xA2, 0x9A, 0x49, 0xA6,
                               0xB2, 0xA0, 0x25, 0x24, 0xC5, 0xE9, 0xD2, 0x60))
         << XStatus(QT_TRANSLATE_NOOP("XStatus", "Shopping"), "relaxing-shopping", 0,
                    Capability(0x63, 0x62, 0x73, 0x37, 0xA0, 0x3F, 0x49, 0xFF,
                               0x80, 0xE5, 0xF7, 0x09, 0xCD, 0xE0, 0xA4, 0xEE))
         << XStatus(QT_TRANSLATE_NOOP("XStatus", "Feeling sick"), "feeling_sick", 17,
                    Capability(0x1F, 0x7A, 0x40, 0x71, 0xBF, 0x3B, 0x4E, 0x60,
                               0xBC, 0x32, 0x4C, 0x57, 0x87, 0xB0, 0x4C, 0xF1))
         << XStatus(QT_TRANSLATE_NOOP("XStatus", "Sleeping"), "sleepy", 18,
                    Capability(0x78, 0x5E, 0x8C, 0x48, 0x40, 0xD3, 0x4C, 0x65,
                               0x88, 0x6F, 0x04, 0xCF, 0x3F, 0x3F, 0x43, 0xDF))
         << XStatus(QT_TRANSLATE_NOOP("XStatus", "Surfing"), "surfing", 19,
                    Capability(0xA6, 0xED, 0x55, 0x7E, 0x6B, 0xF7, 0x44, 0xD4,
                               0xA5, 0xD4, 0xD2, 0xE7, 0xD9, 0x5C, 0xE8, 0x1F))
         << XStatus(QT_TRANSLATE_NOOP("XStatus", "Browsing"), "browsing", 20,
                    Capability(0x12, 0xD0, 0x7E, 0x3E, 0xF8, 0x85, 0x48, 0x9E,
                               0x8E, 0x97, 0xA7, 0x2A, 0x65, 0x51, 0xE5, 0x8D))
         << XStatus(QT_TRANSLATE_NOOP("XStatus", "Working"), "working", 21,
                    Capability(0xBA, 0x74, 0xDB, 0x3E, 0x9E, 0x24, 0x43, 0x4B,
                               0x87, 0xB6, 0x2F, 0x6B, 0x8D, 0xFE, 0xE5, 0x0F))
         << XStatus(QT_TRANSLATE_NOOP("XStatus", "Typing"), "working-coding", 22,
                    Capability(0x63, 0x4F, 0x6B, 0xD8, 0xAD, 0xD2, 0x4A, 0xA1,
                               0xAA, 0xB9, 0x11, 0x5B, 0xC2, 0x6D, 0x05, 0xA1))
         << XStatus(QT_TRANSLATE_NOOP("XStatus", "Picnic"), "picnic", -1,
                    Capability(0x2C, 0xE0, 0xE4, 0xE5, 0x7C, 0x64, 0x43, 0x70,
                               0x9C, 0x3A, 0x7A, 0x1C, 0xE8, 0x78, 0xA7, 0xDC))
         << XStatus(QT_TRANSLATE_NOOP("XStatus", "Cooking"), "doing_chores-cooking", -1,
                    Capability(0x10, 0x11, 0x17, 0xC9, 0xA3, 0xB0, 0x40, 0xF9,
                               0x81, 0xAC, 0x49, 0xE1, 0x59, 0xFB, 0xD5, 0xD4))
         << XStatus(QT_TRANSLATE_NOOP("XStatus", "Mobile"), "talking-on_the_phone", -1,
                    Capability(0x16, 0x0C, 0x60, 0xBB, 0xDD, 0x44, 0x43, 0xF3,
                               0x91, 0x40, 0x05, 0x0F, 0x00, 0xE6, 0xC0, 0x09))
         << XStatus(QT_TRANSLATE_NOOP("XStatus", "I'm high"), "i_am_high", -1,
                    Capability(0x64, 0x43, 0xC6, 0xAF, 0x22, 0x60, 0x45, 0x17,
                               0xB5, 0x8C, 0xD7, 0xDF, 0x8E, 0x29, 0x03, 0x52))
         << XStatus(QT_TRANSLATE_NOOP("XStatus", "On WC"), "on_wc", -1,
                    Capability(0x16, 0xF5, 0xB7, 0x6F, 0xA9, 0xD2, 0x40, 0x35,
                               0x8C, 0xC5, 0xC0, 0x84, 0x70, 0x3C, 0x98, 0xFA))
         << XStatus(QT_TRANSLATE_NOOP("XStatus", "To be or not to be"), "to_be_or_not_to_be", -1,
                    Capability(0x63, 0x14, 0x36, 0xff, 0x3f, 0x8a, 0x40, 0xd0,
                               0xa5, 0xcb, 0x7b, 0x66, 0xe0, 0x51, 0xb3, 0x64))
         << XStatus(QT_TRANSLATE_NOOP("XStatus", "Watching movie"), "relaxing-watching_a_movie", -1,
                    Capability(0xb7, 0x08, 0x67, 0xf5, 0x38, 0x25, 0x43, 0x27,
                               0xa1, 0xff, 0xcf, 0x4c, 0xc1, 0x93, 0x97, 0x97))
         << XStatus(QT_TRANSLATE_NOOP("XStatus", "Love"), "love", -1,
                    Capability(0xdd, 0xcf, 0x0e, 0xa9, 0x71, 0x95, 0x40, 0x48,
                               0xa9, 0xc6, 0x41, 0x32, 0x06, 0xd6, 0xf2, 0x80))
         << XStatus(QT_TRANSLATE_NOOP("XStatus", "Searching"), "searching", -1,
                    Capability(0xd4, 0xe2, 0xb0, 0xba, 0x33, 0x4e, 0x4f, 0xa5,
                               0x98, 0xd0, 0x11, 0x7d, 0xbf, 0x4d, 0x3c, 0xc8))
         << XStatus(QT_TRANSLATE_NOOP("XStatus", "Writing"), "working-writing", -1,
                    Capability(0x00, 0x72, 0xd9, 0x08, 0x4a, 0xd1, 0x43, 0xdd,
                               0x91, 0x99, 0x6f, 0x02, 0x69, 0x66, 0x02, 0x6f))
         << XStatus(QT_TRANSLATE_NOOP("XStatus", "Sex"), "sex", 33,
                    Capability(0xe6, 0x01, 0xe4, 0x1c, 0x33, 0x73, 0x4b, 0xd1,
                               0xbc, 0x06, 0x81, 0x1d, 0x6c, 0x32, 0x3d, 0x82))
		 << XStatus(QT_TRANSLATE_NOOP("XStatus", "Smoking"), "pipe", 32,
                    Capability(0x3F, 0xB0, 0xBD, 0x36, 0xAF, 0x3B, 0x4A, 0x60,
                               0x9E, 0xEF, 0xCF, 0x19, 0x0F, 0x6A, 0x5A, 0x7E))
		#if 0 // disabled as plugin does not support icq moods.
	  << XStatus(QT_TRANSLATE_NOOP("XStatus", "Cold"), "cold", 60)
	  << XStatus(QT_TRANSLATE_NOOP("XStatus", "Crying"), "depressed", 61)
	  << XStatus(QT_TRANSLATE_NOOP("XStatus", "Fear"), "afraid", 62)
	  << XStatus(QT_TRANSLATE_NOOP("XStatus", "Reading"), "relaxing-reading", 63)
	  << XStatus(QT_TRANSLATE_NOOP("XStatus", "Sport"), "exercising-working_out", 64)
	  << XStatus(QT_TRANSLATE_NOOP("XStatus", "In transport"), "traveling-on_a_bus", 65)
		#endif
            ;
    return list;
}

XStatusList *xstatusList()
{
    static QScopedPointer<XStatusList> list(new XStatusList(init_xstatus_list()));
    return list.data();
}

int xstatusIndexByName(const QString &name)
{
    static QHash<QString, int> hash;
    if (hash.isEmpty()) {
        for (int i = 0; i < xstatusList()->size(); ++i)
            hash.insert(xstatusList()->at(i).name, i);
    }
    return hash.value(name);
}

QipExtendedStatus::QipExtendedStatus(int statusId, quint16 status, const QString &iconName, const LocalizedString &name, quint16 id):
    OscarStatusData(statusId, status == OscarAway ? Status::Away : Status::Online, statusId, iconName, name)
{
    Capability cap(0xb7074378, 0xf50c7777, 0x97775778, (quint32)0x502d << 16 | id);
    caps.insert("qipstatus", cap);
    XStatusHandler::qipstatuses.insert(cap, *this);
}

XStatus::XStatus(const QString &icon_) :
    icon(icon_), mood(0)
{
}

XStatus::XStatus(const LocalizedString &status_, const QString &icon_,
                 qint8 mood_, const Capability &capability_) :
    name(icon_), value(status_), icon(QLatin1String("user-status-") + icon_ + "-icq"),
    mood(mood_), capability(capability_)
{
}

void XStatusPlugin::init()
{
    setInfo(QT_TRANSLATE_NOOP("Plugin", "OscarXStatus"),
            QT_TRANSLATE_NOOP("Plugin", "Additional statuses for oscar protocol"),
            PLUGIN_VERSION(0, 0, 1, 0));
    setCapabilities(Loadable);
    addAuthor(
                QT_TRANSLATE_NOOP("Author", "Alexey Prokhin"),
                QT_TRANSLATE_NOOP("Task", "Author"),
                QLatin1String("alexey.prokhin@yandex.ru"));
    addExtension(
                QT_TRANSLATE_NOOP("Plugin", "ICQ"),
                QT_TRANSLATE_NOOP("Plugin", "Additional statuses for oscar protocol"),
                new SingletonGenerator<XStatusHandler, Tlv2711Plugin, RosterPlugin>,
                ExtensionIcon("im-icq"));
    addExtension<XStatusSettings, SettingsExtension>(
                QT_TRANSLATE_NOOP("Plugin", "XStatus settings"),
                QT_TRANSLATE_NOOP("Plugin", "XStatus settings"),
                ExtensionIcon("im-icq"));
}

bool XStatusPlugin::load()
{
    Protocol *proto = Protocol::all().value("icq");
    if (!proto || proto->metaObject() != &IcqProtocol::staticMetaObject)
        return false;

    return true;
}

bool XStatusPlugin::unload()
{
    return false;
}

XStatusHandler::XStatusHandler()
{
    m_tlvs2711Types << Tlv2711Type(MSG_XSTRAZ_SCRIPT, xtrazNotify);

    m_aboutToBeChanged = Event::registerType("icq-xstatus-about-to-be-changed");
    m_changed = Event::registerType("icq-xstatus-changed");
    m_change = Event::registerType("icq-change-xstatus");

    {
        Capability cap(0xb7074378, 0xf50c7777, 0x97775778, 0x502d0575);
        OscarStatusData status(OscarFFC, Status::FreeChat);
        status.flag = 0;
        status.caps.insert("qipstatus", cap);
        qipstatuses.insert(cap, status);
    }
    QipExtendedStatus(OscarEvil, OscarOnline, "online-angry",
                      QT_TRANSLATE_NOOP("Status", "Angry"), 0x0579);
    QipExtendedStatus(OscarDepress, OscarOnline, "online-depression",
                      QT_TRANSLATE_NOOP("Status", "Depression"), 0x0570);
    QipExtendedStatus(OscarHome, OscarOnline, "online-athome",
                      QT_TRANSLATE_NOOP("Status", "At home"), 0x0576);
    QipExtendedStatus(OscarWork, OscarOnline, "online-atwork",
                      QT_TRANSLATE_NOOP("Status", "At work"), 0x0577);
    QipExtendedStatus(OscarLunch, OscarAway, "away-eating",
                      QT_TRANSLATE_NOOP("Status", "Eating"), 0x0578);
    foreach (const OscarStatusData &data, qipstatuses) {
        OscarStatus::registerStatus(data);
        MenuController::addAction<IcqAccount>(new StatusActionGenerator(OscarStatus(data)));
    }

    m_gen.reset(new ActionGenerator(Icon("user-status-xstatus"),
                                    QT_TRANSLATE_NOOP("Status", "Custom status"),
                                    this, SLOT(onSetCustomStatus(QObject*))));
    m_gen->setPriority(25);
    m_gen->setType(0x0004);
    MenuController::addAction<IcqAccount>(m_gen.data());

    foreach (Account *account, IcqProtocol::instance()->accounts())
        onAccountAdded(account);
    connect(IcqProtocol::instance(), SIGNAL(accountCreated(qutim_sdk_0_3::Account*)),
            SLOT(onAccountAdded(qutim_sdk_0_3::Account*)));
    connect(IcqProtocol::instance(), SIGNAL(settingsUpdated()), SLOT(loadSettings()));

    IcqProtocol::instance()->installEventFilter(this);
    loadSettings();
}

XStatusHandler::~XStatusHandler()
{
    //MenuController::removeAction<IcqAccount>(m_gen.data());
}

void XStatusHandler::processTlvs2711(IcqContact *contact, Capability guid, quint16 type, const DataUnit &data, const Cookie &cookie)
{
    QString message = data.read<QString, quint32>(LittleEndian);
    Q_ASSERT(guid == MSG_XSTRAZ_SCRIPT && type == xtrazNotify);
    Xtraz xtraz(message);
    if (xtraz.type() == Xtraz::Request) {
        XtrazRequest request = xtraz.request();
        if (request.pluginId() != "srvMng" && request.serviceId() != "cAwaySrv" &&
                request.value("senderId") != contact->id())
        {
            debug() << "Skipped xtraz request" << request.serviceId() << "from" << request.value("senderId");
            return;
        }
        XStatusSender::sendXStatus(contact, cookie.id());
    } else if (xtraz.type() == Xtraz::Response) {
        XtrazResponse response = xtraz.response();
        if (response.serviceId() != "cAwaySrv" && response.event() != "OnRemoteNotification" &&
                response.value("uin") != contact->id())
        {
            debug() << "Skipped xtraz response" << response.serviceId() << "from" << response.value("uin");
            return;
        }
        setXstatus(contact, response.value("title"), response.value("desc"));
    }
}

void XStatusHandler::statusChanged(IcqContact *contact, Status &status, const TLVMap &tlvs)
{
    if (status.type() == Status::Offline) {
        status.removeExtendedInfo("xstatus"); // Offline contacts cannot have xstatus
        return;
    }
    SessionDataItemMap statusNoteData(tlvs.value(0x1D));
    qint8 moodIndex = -1;
    if (statusNoteData.contains(0x0e)) {
        QString moodStr = statusNoteData.value(0x0e).read<QString>(Util::asciiCodec());
        if (moodStr.startsWith("icqmood")) {
            bool ok;
            moodIndex = moodStr.mid(7, -1).toInt(&ok);
            if (!ok)
                moodIndex = -1;
        }
    }
    XStatus xstatus = findXStatus(contact, moodIndex);
    if (!xstatus.name.isEmpty()) {
        setXstatus(status, xstatus.value, xstatus.icon);
        if (m_xstatusAutoRequest)
            XStatusRequester::updateXStatus(contact);
    } else {
        status.removeExtendedInfo("xstatus");
    }
    foreach (const Capability &cap, contact->capabilities()) {
        if (qipstatuses.contains(cap)) {
            OscarStatus qipStatus = qipstatuses.value(cap);
            status.setIcon(qipStatus.icon());
            status.setName(qipStatus.name());
        }
    }
}

XStatus XStatusHandler::findXStatus(IcqContact *contact, qint8 mood)
{
    const Capabilities &caps = contact->capabilities();
    foreach(const XStatus &status, *xstatusList())
    {
        if ((!status.capability.isNull() && caps.match(status.capability)) || (mood != -1 && status.mood == mood))
            return status;
    }
    return XStatus();
}

void XStatusHandler::removeXStatuses(Capabilities &caps)
{
    foreach (const XStatus &xstatus, *xstatusList())
        caps.removeAll(xstatus.capability);
}

void XStatusHandler::setXstatus(IcqContact *contact, const QString &title, const QString &desc)
{
    Status status = contact->status();
    QVariant icon = status.extendedInfo("xstatus").value("icon");
    setXstatus(status, title, icon.value<ExtensionIcon>(), desc);
    contact->setStatus(status, false);
}

void XStatusHandler::setXstatus(Status &status, const QString &title, const ExtensionIcon &icon, const QString &desc)
{
    QVariantHash extStatus;
    extStatus.insert("id", "xstatus");
    extStatus.insert("title", unescape(title));
    extStatus.insert("icon", QVariant::fromValue(icon));
    if (!desc.isNull())
        extStatus.insert("description", unescape(desc));
    extStatus.insert("showInTooltip", true);
    status.setExtendedInfo("xstatus", extStatus);
}

void XStatusHandler::setAcountXstatus(IcqAccount *account, QVariantHash extStatus, const XStatus &xstatus, bool saveToConfig)
{
    {
        // Send icq-xstatus-about-to-be-changed event
        Event ev(m_aboutToBeChanged, extStatus);
        qApp->sendEvent(account, &ev);
        extStatus = ev.at<QVariantHash>(0);
    }
    extStatus.insert("id", "xstatus");
    if (!extStatus.contains("icon"))
        extStatus.insert("icon", xstatus.icon.toIcon());
    account->setProperty("xstatus", extStatus);
    account->setCapability(xstatus.capability, "xstatus");
    if (saveToConfig) {
        Config cfg = account->config("xstatus");
        QHashIterator<QString, QVariant> itr(extStatus);
        while (itr.hasNext()) {
            itr.next();
            cfg.setValue(itr.key(), itr.value());
        }
    }
    {
        // Send icq-xstatus-changed event
        Event ev(m_changed, extStatus);
        qApp->sendEvent(account, &ev);
    }
}

void XStatusHandler::setAcountXstatus(IcqAccount *account, QVariantHash extStatus, bool saveToConfig)
{
    int index = xstatusIndexByName(extStatus.value("name").toString());
    XStatus xstatus = xstatusList()->value(index);
    if (index <= 0 || index >= xstatusList()->count()) // unknown x-status
        extStatus.clear();
    setAcountXstatus(account, extStatus, xstatus, saveToConfig);
}

bool XStatusHandler::eventFilter(QObject *obj, QEvent *e)
{
    if (e->type() == ExtendedInfosEvent::eventType() && obj == IcqProtocol::instance()) {
        ExtendedInfosEvent *event = static_cast<ExtendedInfosEvent*>(e);
        QVariantHash extStatus;
        extStatus.insert("id", "xstatus");
        extStatus.insert("name", tr("X-Status"));
        extStatus.insert("settingsDescription", tr("Show contact X-Status icon"));
        event->addInfo("xstatus", extStatus);
    } else if (e->type() == qutim_sdk_0_3::Event::eventType()) {
        qutim_sdk_0_3::Event *customEvent = static_cast<qutim_sdk_0_3::Event*>(e);
        if (customEvent->id == m_change) {
            // Handle icq-change-xstatus event
            IcqAccount *account = qobject_cast<IcqAccount*>(obj);
            if (!account)
                return false;
            setAcountXstatus(account, customEvent->at<QVariantHash>(0));
        }
    }
    return QObject::eventFilter(obj, e);
}

void XStatusHandler::onSetCustomStatus(QObject *object)
{
    Q_ASSERT(qobject_cast<IcqAccount*>(object) != 0);
    IcqAccount *account = reinterpret_cast<IcqAccount*>(object);
    CustomStatusDialog *dialog = new CustomStatusDialog(account);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    connect(dialog, SIGNAL(accepted()), SLOT(onCustomDialogAccepted()));

#ifdef Q_WS_MAEMO_5	
    dialog->setParent(QApplication::activeWindow());
    dialog->setWindowFlags(dialog->windowFlags() | Qt::Window);
    dialog->setAttribute(Qt::WA_Maemo5StackedWindow);
#endif
    SystemIntegration::show(dialog);
}

void XStatusHandler::onCustomDialogAccepted()
{
    // An user updated his/her xstatus.
    Q_ASSERT(qobject_cast<CustomStatusDialog*>(sender()));
    CustomStatusDialog *dialog = reinterpret_cast<CustomStatusDialog*>(sender());
    XStatus xstatus = dialog->status();
    QVariantHash extStatus;
    extStatus.insert("name", xstatus.name);
    extStatus.insert("title", dialog->caption());
    extStatus.insert("description", dialog->message());
    setAcountXstatus(dialog->account(), extStatus, xstatus);
}

void XStatusHandler::onAccountAdded(qutim_sdk_0_3::Account *account)
{
    Q_ASSERT(qobject_cast<IcqAccount*>(account));
    QVariantHash extStatus;
    Config cfg = account->config("xstatus");
    foreach (const QString &key, cfg.childKeys())
        extStatus.insert(key, cfg.value(key));
    setAcountXstatus(static_cast<IcqAccount*>(account), extStatus, false);
    account->installEventFilter(this);
}

void XStatusHandler::loadSettings()
{
    m_xstatusAutoRequest = IcqProtocol::instance()->config("xstatus").value("xstatusAutorequest", true);
}

} } // namespace qutim_sdk_0_3::oscar

QUTIM_EXPORT_PLUGIN(qutim_sdk_0_3::oscar::XStatusPlugin);

