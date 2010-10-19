#include "mpris.h"
#include <QVariantMap>
#include <QFileInfo>
#include <QTime>
#include <QComboBox>
#include <QVBoxLayout>
#include "../../src/nowplaying.h"

namespace qutim_sdk_0_3 {

namespace nowplaying {

	MprisSettings::MprisSettings(Mpris *mpris) :
		m_mpris(mpris)
	{
		QVBoxLayout *layout = new QVBoxLayout(this);
		m_comboBox = new QComboBox(this);
		m_comboBox->setEditable(true);
		layout->addWidget(m_comboBox);
		connect(m_comboBox, SIGNAL(currentIndexChanged(int)), SIGNAL(modified()));
	}

	void MprisSettings::saveSettings()
	{
		QString player = m_comboBox->currentText();
		config("mpris").setValue("player", player);
		player = "org.mpris." + player;
		if (m_mpris->currentPlayer() != player) {
			m_mpris->stopWatching();
			m_mpris->init();
			m_mpris->startWatching();
		}
	}

	void MprisSettings::loadSettings()
	{
		m_comboBox->clear();
		QStringList values;
		values << "amarok" << "vlc" << "songbird" << "audacious" << "qmmp";
		m_comboBox->addItems(values);
		QString current = config("mpris").value("player", QString());
		if (!current.isEmpty()) {
			int index = m_comboBox->findText(current);
			if (index != -1) {
				m_comboBox->setCurrentIndex(index);
			} else {
				m_comboBox->addItem(current);
				m_comboBox->setCurrentIndex(m_comboBox->count()-1);
			}
		} else {
			m_comboBox->setCurrentIndex(0);
		}
	}

	Mpris::Mpris()
	{
		qDBusRegisterMetaType<DBusStatus>();
	}

	void Mpris::init()
	{
		m_currentPlayer = config("mpris").value("player", QString());
		if (m_currentPlayer.isEmpty())
			return;
		m_currentPlayer = "org.mpris." + m_currentPlayer;
		m_dbusInterface.reset(new QDBusInterface(m_currentPlayer, "/Player", "org.freedesktop.MediaPlayer", QDBusConnection::sessionBus(), this));
	}

	QString Mpris::playerName()
	{
		return QString("Mpris");
	}

	TrackInfo Mpris::trackInfo()
	{
		TrackInfo trackInfo;
		if (!m_dbusInterface)
			return trackInfo;
		QDBusReply<QVariantMap> reply = m_dbusInterface->call("GetMetadata");
		trackInfo.album = reply.value().value("album").toString();
		trackInfo.artist = reply.value().value("artist").toString();
		trackInfo.title = reply.value().value("title").toString();
		trackInfo.uri = reply.value().value("location").toString();
		trackInfo.uri.replace("%20", " ");
		trackInfo.file_name = QFileInfo(trackInfo.uri).baseName();
		trackInfo.track_number = reply.value().value("tracknumber").toString();
		trackInfo.time = QTime().addSecs(reply.value().value("time").toLongLong()).toString("H:mm:ss");
		return trackInfo;
	}

	bool Mpris::isPlaying()
	{
		return m_isPlaying;
	}

	void Mpris::startWatching()
	{
		if (!m_dbusInterface)
			return;
		m_isPlaying = getIsPlaying();
		m_dbusInterface->connection().connect(m_currentPlayer,
											"/Player",
											"org.freedesktop.MediaPlayer",
											"TrackChange",
											this,
											SLOT(trackChanged())
											);
		m_dbusInterface->connection().connect(m_currentPlayer,
											"/Player",
											"org.freedesktop.MediaPlayer",
											"StatusChange",
											this,
											SLOT(statusChanged(DBusStatus))
											);

	}

	void Mpris::stopWatching()
	{
		m_isPlaying = false;
		if (!m_dbusInterface)
			return;
		m_dbusInterface->connection().disconnect(m_currentPlayer,
												"/Player",
												"org.freedesktop.MediaPlayer",
												"TrackChange",
												this,
												SLOT(trackChanged())
												);
		m_dbusInterface->connection().disconnect(m_currentPlayer,
											   "/Player",
											   "org.freedesktop.MediaPlayer",
											   "StatusChange",
											   this,
											   SLOT(statusChanged(DBusStatus))
											   );
	}

	PlayerSettings *Mpris::settings()
	{
		return new MprisSettings(this);
	}

	bool Mpris::getIsPlaying()
	{
		if (!m_dbusInterface)
			 return false;
		QDBusReply<DBusStatus> reply = m_dbusInterface->call("GetStatus");
		return reply.isValid() ? reply.value().Play == 0 : false;
	}

	void Mpris::trackChanged()
	{
		TrackInfo info = trackInfo();
		if (!info.uri.isEmpty() && (info.time != "0:00:00" || info.time.isEmpty()))
			emit Player::trackChanged(info);
	}

	void Mpris::statusChanged(const DBusStatus &status)
	{
		bool is_playing = status.Play == 0;
		if (m_isPlaying && !is_playing) {
			m_isPlaying = false;
			emit playingStatusChanged(false);
		} else if (!m_isPlaying && is_playing) {
			m_isPlaying = true;
			emit playingStatusChanged(true);
		}
	}

} }

const QDBusArgument & operator<<(QDBusArgument &arg, const DBusStatus& status)
{
	arg.beginStructure();
	arg << status.Play << status.Random << status.Repeat << status.RepeatPlaylist;
	arg.endStructure();
	return arg;
}

const QDBusArgument & operator>>(const QDBusArgument& arg, DBusStatus& status)
{
	arg.beginStructure();
	arg >> status.Play >> status.Random >> status.Repeat >> status.RepeatPlaylist;
	arg.endStructure();
	return arg;
}
