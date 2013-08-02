///****************************************************************************
//**
//** qutIM - instant messenger
//**
//** Copyright © 2012 Ruslan Nigmatullin <euroelessar@yandex.ru>
//**
//*****************************************************************************
//**
//** $QUTIM_BEGIN_LICENSE$
//** This program is free software: you can redistribute it and/or modify
//** it under the terms of the GNU General Public License as published by
//** the Free Software Foundation, either version 3 of the License, or
//** (at your option) any later version.
//**
//** This program is distributed in the hope that it will be useful,
//** but WITHOUT ANY WARRANTY; without even the implied warranty of
//** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//** See the GNU General Public License for more details.
//**
//** You should have received a copy of the GNU General Public License
//** along with this program.  If not, see http://www.gnu.org/licenses/.
//** $QUTIM_END_LICENSE$
//**
//****************************************************************************/

////#include "jpgpsupport.h"
////#include "jpgpkeydialog.h"
//#include "../jprotocol.h"
//#include "roster/jcontact.h"
//#include "roster/jcontactresource.h"
//#include "roster/jroster.h"
//#include "muc/jmucmanager.h"
//#include <jreen/client.h>
//#include <jreen/pgpencrypted.h>
//#include <jreen/pgpsigned.h>
//#include <qutim/chatunit.h>
//#include <qutim/icon.h>
//#include <qutim/passworddialog.h>
//#include <qutim/notification.h>
//#include <QCoreApplication>
//#include <QtDebug>
////#include <QtConcurrentRun>

//Q_DECLARE_METATYPE(Jreen::Presence)

//template <>
//void *qMetaTypeConstructHelper(const Jreen::Presence *t)
//{
//    if (!t)
//		return new Jreen::Presence(Jreen::Presence::Unavailable, Jreen::JID());
//    return new Jreen::Presence(*t);
//}

//namespace Jabber
//{

//using namespace Jreen;

//AssignPGPKeyActionGenerator::AssignPGPKeyActionGenerator(QObject *obj, const char *slot)
//    : ActionGenerator(QIcon(), LocalizedString(), obj, slot)
//{
//}

//void AssignPGPKeyActionGenerator::showImpl(QAction *action, QObject *obj)
//{
//	JContact *contact = qobject_cast<JContact*>(obj);
//	Q_ASSERT(contact);
//	if (contact->pgpKeyId().isEmpty())
//		action->setText(JPGPSupport::tr("Assign PGP key"));
//	else
//		action->setText(JPGPSupport::tr("Remove PGP key"));
//}

//ToggleEncryptionActionGenerator::ToggleEncryptionActionGenerator(QObject *obj, const char *slot)
//    : ActionGenerator(QIcon(), LocalizedString(), obj, slot)
//{
//	setType(ActionTypeChatButton | ActionTypeContactList);
//}

//void ToggleEncryptionActionGenerator::createImpl(QAction *action, QObject *obj) const
//{
//	Q_UNUSED(obj);
//	action->setCheckable(true);
//}

//void ToggleEncryptionActionGenerator::showImpl(QAction *action, QObject *obj)
//{
//	JContact *contact = qobject_cast<JContact*>(obj);
//	Q_ASSERT(contact);
//	if (contact->pgpKeyId().isEmpty() || !JPGPSupport::instance()->isChannelEncryptable(contact))
//		action->setEnabled(false);
//	else
//		action->setEnabled(true);
//	action->setChecked(contact->isEncrypted());
//	if (contact->isEncrypted()) {
//		action->setText(JPGPSupport::tr("Disable PGP encryption"));
//		action->setIcon(Icon(QLatin1String("document-encrypt")));
//	} else {
//		action->setText(JPGPSupport::tr("Enable PGP encryption"));
//		action->setIcon(Icon(QLatin1String("document-decrypt")));
//	}
//}

//JPGPDecryptReply::JPGPDecryptReply(QObject *parent) : QObject(parent)
//{
//}

//class JPGPSupportPrivate
//{
//public:
//	bool isAvailable;
//	QCA::KeyStoreManager keyStoreManager;
//	QCA::EventHandler *eventHandler;
//	QList<QCA::KeyStore*> keyStores;
//	QMap<JAccount*, QCA::PGPKey> accountKeys;
//	AssignPGPKeyActionGenerator *assignPGPKeyAction;
//	ToggleEncryptionActionGenerator *toggleEncryptionAction;
//	QHash<QString, QCA::SecureArray> passwords;
//	PasswordDialog *dialog;
//	QList<QPair<int, QCA::Event> > queue;
//};

//static void keyStoreManagerStart()
//{
//	qDebug() << Q_FUNC_INFO << "begin";
//	QCA::KeyStoreManager::start();
//	qDebug() << Q_FUNC_INFO << "end";
//}

//JPGPSupport::JPGPSupport() : d_ptr(new JPGPSupportPrivate)
//{
//	Q_D(JPGPSupport);
//	d->dialog = 0;
//	qRegisterMetaType<Jreen::Presence>();
//	d->assignPGPKeyAction = new AssignPGPKeyActionGenerator(this, SLOT(onAssignKeyToggled(QObject*)));
//	MenuController::addAction<JContact>(d->assignPGPKeyAction);
//	d->toggleEncryptionAction = new ToggleEncryptionActionGenerator(this, SLOT(onEncryptToggled(QObject*)));
//	MenuController::addAction<JContact>(d->toggleEncryptionAction);
//	QCA::init();
//	QCA::setAppName(QCoreApplication::applicationName());
//	QCA::setProperty("pgp-always-trust", true);
//	d->isAvailable = QCA::isSupported("openpgp");
	
//	connect(&d->keyStoreManager, SIGNAL(busyFinished()),
//	        SLOT(onKeyStoreManagerLoaded()));
//	connect(&d->keyStoreManager, SIGNAL(keyStoreAvailable(QString)),
//	        SLOT(onKeyStoreAvailable(QString)));
//	QtConcurrent::run(keyStoreManagerStart);
//	d->eventHandler = new QCA::EventHandler(this);
//	connect(d->eventHandler, SIGNAL(eventReady(int,QCA::Event)), SLOT(onEvent(int,QCA::Event)));
//    d->eventHandler->start();
//}

//JPGPSupport::~JPGPSupport()
//{
//}

//JPGPSupport *JPGPSupport::instance()
//{
//	static JPGPSupport self;
//	return &self;
//}

//bool JPGPSupport::isAvailable()
//{
//	return d_func()->isAvailable;
//}

//bool JPGPSupport::canAddKey() const
//{
//	Q_D(const JPGPSupport);
//	foreach (QCA::KeyStore *store, d->keyStores) {
//		// qca-gnupg lies that it's read only
//		if (store->id() == QLatin1String("qca-gnupg")
//		        || (store->type() == QCA::KeyStore::PGPKeyring && !store->isReadOnly())) {
//			return true;
//		}
//	}
//	return false;
//}

//QString JPGPSupport::addKey(const QCA::PGPKey &key)
//{
//	Q_D(JPGPSupport);
//	foreach (QCA::KeyStore *store, d->keyStores) {
//		// qca-gnupg lies that it's read only
//		if (store->id() == QLatin1String("qca-gnupg")
//		        || (store->type() == QCA::KeyStore::PGPKeyring && !store->isReadOnly())) {
//			return store->writeEntry(key);
//		}
//	}
//	return QString();
//}

//class VerifyReply : public QCA::SecureMessage
//{
//public:
//	VerifyReply(QCA::SecureMessageSystem *system);

//	QPointer<JContactResource> resource;
//};

//VerifyReply::VerifyReply(QCA::SecureMessageSystem *system) : QCA::SecureMessage(system)
//{
//	connect(this, SIGNAL(destroyed()), system, SLOT(deleteLater()));
//}

//void JPGPSupport::verifyPGPSigning(JContactResource *resource)
//{
//	Jreen::PGPSigned::Ptr signature = resource->presence().payload<Jreen::PGPSigned>();
//	if (!signature || !d_func()->isAvailable) {
//		resource->setPGPVerifyStatus(QCA::SecureMessageSignature::NoKey);
//		return;
//	}

//	VerifyReply *reply = new VerifyReply(new QCA::OpenPGP());
//	reply->resource = resource;
//    connect(reply, SIGNAL(finished()), SLOT(onVerifyFinished()));
//    reply->startVerify(addHeader(signature->signature(), Signature).toUtf8());
//    reply->update(resource->presence().status().toUtf8());
//    reply->end();
//}

//void JPGPSupport::onVerifyFinished()
//{
//	Q_ASSERT(qobject_cast<QCA::SecureMessage*>(sender()));
//	VerifyReply *reply = static_cast<VerifyReply*>(sender());
//	reply->deleteLater();
//	if(reply->resource && reply->success()) {
//		JContactResource *resource = reply->resource.data();
//		JContact *contact = qobject_cast<JContact*>(resource->upperUnit());
//		QCA::SecureMessageSignature signature = reply->signer();
//		resource->setPGPKey(signature.key().pgpPublicKey());
//		resource->setPGPVerifyStatus(signature.identityResult());
//		if (contact && resource->pgpKey().keyId() != contact->pgpKeyId())
//			contact->setPGPKeyId(QString());
//		if (contact)
//			updateEncryptionAction(contact);
//	}
//}

//void JPGPSupport::addAccount(JAccount *account)
//{
//	connect(account, SIGNAL(pgpKeyIdChanged(QString)),
//	        SLOT(onPGPKeyIdChanged(QString)));
//}

//void JPGPSupport::removeAccount(JAccount *account)
//{
//	Q_D(JPGPSupport);
//	d->accountKeys.remove(account);
//}

//QString JPGPSupport::stripHeader(const QString &message)
//{
//	QStringList list = message.split(QLatin1Char('\n'));
//	if (list.size() < 4)
//		return QString();
//	if (!list.at(0).startsWith(QLatin1String("-")))
//		return QString();
//	QString result;
//	bool foundEmpty = false;
//	for (int i = 1; i < list.size(); ++i) {
//		if (list.at(i).isEmpty()) {
//			foundEmpty = true;
//			continue;
//		}
//		if (!foundEmpty)
//			continue;
//		if (list.at(i).startsWith(QLatin1Char('-')))
//			break;
//		result += list.at(i);
//		result += QLatin1Char('\n');
//	}
//	return result;
//}

//QString JPGPSupport::addHeader(const QString &message, JPGPSupport::Type type)
//{
//	QString result;
//	if (type == Signature)
//		result += QLatin1String("-----BEGIN PGP SIGNATURE-----\n");
//	else
//		result += QLatin1String("-----BEGIN PGP MESSAGE-----\n");
//	result += QLatin1String("Version: PGP\n");
//	result += QLatin1String("\n");
//	result += message;
//	result += QLatin1String("\n");
//	if (type == Signature)
//		result += QLatin1String("-----END PGP SIGNATURE-----\n");
//	else
//		result += QLatin1String("-----END PGP MESSAGE-----\n");
//	return result;
//}

//class SignReply : public QCA::SecureMessage
//{
//public:
//	SignReply(QCA::SecureMessageSystem *system);
	
//	JAccount *account;
//	Status status;
//	Presence::Type type;
//	QString text;
//	int priority;
//};

//SignReply::SignReply(QCA::SecureMessageSystem *system) : QCA::SecureMessage(system)
//{
//	connect(this, SIGNAL(destroyed()), system, SLOT(deleteLater()));
//}


//void JPGPSupport::send(JAccount *account, const Status &status, int priority)
//{
//	Q_D(JPGPSupport);
//	Presence::Type type = JStatus::statusToPresence(status);
//	const QString text = status.text();
//	Jreen::Presence &presence = account->client()->presence();
//	presence.removePayload<PGPSigned>();
//	QCA::PGPKey pgpKey = d->accountKeys.value(account);
//	if (pgpKey.isNull() || !d->isAvailable) {
//		Client *client = account->client();
//		client->setPresence(type, text, priority);
//		const Presence presence = client->presence();
//		// We need this for peps
//		Presence copy(presence.subtype(), client->jid().bareJID(), presence.status(), presence.priority());
//		client->send(copy);
//		account->conferenceManager()->setPresenceToRooms(presence);
//		account->setAccountStatus(status);
//	} else {
//		QCA::SecureMessageKey secureKey;
//	    secureKey.setPGPSecretKey(pgpKey);
//	    QByteArray plain = text.toUtf8();

//		SignReply *reply = new SignReply(new QCA::OpenPGP());
//		reply->account = account;
//		reply->status = status;
//		reply->type = type;
//		reply->text = text;
//		reply->priority = priority;
//		connect(reply, SIGNAL(finished()), SLOT(onSignFinished()));
//	    reply->setFormat(QCA::SecureMessage::Ascii);
//	    reply->setSigner(secureKey);
//	    reply->startSign(QCA::SecureMessage::Detached);
//	    reply->update(plain);
//	    reply->end();
//	}
//}

//void JPGPSupport::onSignFinished()
//{
//	Q_ASSERT(qobject_cast<QCA::SecureMessage*>(sender()));
//	SignReply *reply = static_cast<SignReply*>(sender());
//	reply->deleteLater();
//	if (reply->success()) {
//		QString signature = stripHeader(reply->signature());
//		Jreen::Client *client = reply->account->client();
//		client->presence().addPayload(new Jreen::PGPSigned(signature));
//		client->setPresence(reply->type, reply->text, reply->priority);
		
//		const Presence presence = client->presence();
//		// We need this for peps
//		Presence copy(presence.subtype(), client->jid().bareJID(), presence.status(), presence.priority());
//		client->send(copy);
//		reply->account->conferenceManager()->setPresenceToRooms(presence);
//		reply->account->setAccountStatus(reply->status);
//	} else {
//		if (reply->errorCode() == QCA::SecureMessage::ErrorPassphrase) {
//            QCA::KeyStoreEntry keyEntry = findEntry(reply->account->pgpKeyId(), SecretKey);
//            if (!keyEntry.isNull())
//				d_func()->passwords.remove(keyEntry.id());
//        }
//		reply->account->client()->disconnectFromServer();
//		NotificationRequest request(Notification::System);
//		request.setObject(reply->account);
//		request.setText(tr("There was an error trying to sign your status.\nReason: %1")
//		                .arg(errorText(reply->errorCode())));
//		request.send();

//		qDebug() << Q_FUNC_INFO << reply->errorCode();
//	}
//}

//class EncryptReply : public QCA::SecureMessage
//{
//public:
//	EncryptReply(const Jreen::Message &m, QCA::SecureMessageSystem *system);
	
//	MessageSession *session;
//	ChatUnit *unit;
//	Jreen::Message message;
//};

//EncryptReply::EncryptReply(const Jreen::Message &m, QCA::SecureMessageSystem *system)
//    : QCA::SecureMessage(system), message(m)
//{
//	connect(this, SIGNAL(destroyed()), system, SLOT(deleteLater()));
//}

//bool JPGPSupport::send(MessageSession *session, ChatUnit *unit, Jreen::Message message)
//{
//	if (!d_func()->isAvailable)
//		return false;
//	if (!isChannelEncryptable(unit))
//		return false;
//	if (JContactResource *resource = qobject_cast<JContactResource*>(unit))
//		unit = resource->upperUnit();
//	JContact *contact = qobject_cast<JContact*>(unit);
//	if (!contact->isEncrypted())
//		return false;
//	Q_ASSERT(contact);
//	QCA::KeyStoreEntry keyEntry = findEntry(contact->pgpKeyId(), PublicKey);
//	if (keyEntry.isNull())
//		return false;

//	QCA::SecureMessageKey key;
//	key.setPGPPublicKey(keyEntry.pgpPublicKey());
	
//	EncryptReply *reply = new EncryptReply(message, new QCA::OpenPGP());
//	reply->session = session;
//	reply->unit = unit;
//	connect(reply, SIGNAL(finished()), SLOT(onEncryptFinished()));
//	reply->setFormat(QCA::SecureMessage::Ascii);
//	reply->setRecipient(key);
//	reply->startEncrypt();
//	reply->update(message.body().toUtf8());
//	reply->end();
//	return true;
//}

//void JPGPSupport::onEncryptFinished()
//{
//	Q_ASSERT(qobject_cast<QCA::SecureMessage*>(sender()));
//	EncryptReply *reply = static_cast<EncryptReply*>(sender());
//	reply->deleteLater();
//	if(reply->success()) {
//		QString text = stripHeader(QString::fromLatin1(reply->read()));
//		reply->message.setBody(tr("[ERROR: This message is encrypted, and you are unable to decrypt it.]"));
//		reply->message.addPayload(new PGPEncrypted(text));
//		reply->session->sendMessage(reply->message);
//	} else {
//		qWarning() << "Something went wrong with encryption" << reply->errorCode();
//	}
//}

//class DecryptReply : public QCA::SecureMessage
//{
//public:
//	DecryptReply(const Jreen::Message &m, QCA::SecureMessageSystem *system);
	
//	JPGPDecryptReply *reply;
//	QPointer<ChatUnit> unit;
//	QPointer<ChatUnit> unitForSession;
//	Jreen::Message message;
//};

//DecryptReply::DecryptReply(const Jreen::Message &m, QCA::SecureMessageSystem *system)
//    : QCA::SecureMessage(system), message(m)
//{
//	connect(this, SIGNAL(destroyed()), system, SLOT(deleteLater()));
//}

//JPGPDecryptReply *JPGPSupport::decrypt(ChatUnit *unit, ChatUnit *unitForSession, const Jreen::Message &message)
//{
//	PGPEncrypted::Ptr encrypted = message.payload<Jreen::PGPEncrypted>();
//	if (!encrypted)
//		return 0;
	
//	DecryptReply *reply = new DecryptReply(message, new QCA::OpenPGP());
//	reply->unit = unit;
//	reply->unitForSession = unitForSession;
//	reply->message = message;
//	reply->reply = new JPGPDecryptReply(reply);
//	connect(reply, SIGNAL(finished()), SLOT(onDecryptFinished()));
//	reply->setFormat(QCA::SecureMessage::Ascii);
//	reply->startDecrypt();
//    reply->update(addHeader(encrypted->encryptedText(), Message).toUtf8());
//	reply->end();
//	return reply->reply;
//}

//void JPGPSupport::onDecryptFinished()
//{
//	Q_ASSERT(qobject_cast<QCA::SecureMessage*>(sender()));
//	DecryptReply *reply = static_cast<DecryptReply*>(sender());
//	reply->deleteLater();
//	if (reply->success())
//		reply->message.setBody(QString::fromUtf8(reply->read()));
//	emit reply->reply->finished(reply->unit.data(), reply->unitForSession.data(), reply->message);
//}

//bool JPGPSupport::isChannelEncryptable(ChatUnit *unit)
//{
//	if (JContactResource *resource = qobject_cast<JContactResource*>(unit)) {
//		return resource->pgpVerifyStatus() == QCA::SecureMessageSignature::Valid;
//	} else if (JContact *contact = qobject_cast<JContact*>(unit)) {
//		foreach (JContactResource *resource, contact->resources()) {
//			if (resource->pgpVerifyStatus() == QCA::SecureMessageSignature::Valid)
//				return true;
//		}
//	}
//	return false;
//}

//QList<QCA::KeyStoreEntry> JPGPSupport::pgpKeys(JPGPSupport::KeyType type) const
//{
//	Q_D(const JPGPSupport);
//	QList<QCA::KeyStoreEntry> keys;
//	foreach (QCA::KeyStore *keyStore, d->keyStores) {
//		foreach (const QCA::KeyStoreEntry &entry, keyStore->entryList()) {
//			if ((type == PublicKey && entry.type() == QCA::KeyStoreEntry::TypePGPPublicKey)
//			        || entry.type() == QCA::KeyStoreEntry::TypePGPSecretKey) {
//				keys << entry;
//			}
//		}
//	}
//	return keys;
//}

//QCA::KeyStoreEntry JPGPSupport::findEntry(const QString &keyId, JPGPSupport::KeyType type) const
//{
//	Q_D(const JPGPSupport);
//	foreach (QCA::KeyStore *keyStore, d->keyStores) {
//		foreach (const QCA::KeyStoreEntry &entry, keyStore->entryList()) {
//			if (type == PublicKey && entry.type() == QCA::KeyStoreEntry::TypePGPPublicKey
//			        && entry.pgpPublicKey().keyId() == keyId) {
//				return entry;
//			} else if (entry.type() == QCA::KeyStoreEntry::TypePGPSecretKey
//			           && entry.pgpSecretKey().keyId() == keyId) {
//				return entry;
//			}
//		}
//	}
//	return QCA::KeyStoreEntry();
//}

//QCA::PGPKey JPGPSupport::findKey(const QString &keyId, JPGPSupport::KeyType type) const
//{
//	QCA::KeyStoreEntry keyEntry = findEntry(keyId, type);
//	if (!keyEntry.isNull() && type == PublicKey)
//		return keyEntry.pgpPublicKey();
//	else if (!keyEntry.isNull())
//		return keyEntry.pgpSecretKey();
//	return QCA::PGPKey();
//}

//void JPGPSupport::onEvent(int id, const QCA::Event &event)
//{
//	if (event.type() == QCA::Event::Password) {
//		Q_D(JPGPSupport);
//        QCA::KeyStoreEntry entry = event.keyStoreEntry();
//		if (!entry.isNull() && d->passwords.contains(entry.id())) {
//			d->eventHandler->submitPassword(id, d->passwords.value(entry.id()));
//		} else if (d->dialog) {
//			d->queue.append(qMakePair(id, event));
//		} else {
//			QString title = tr("%1 : OpenPGP Passphrase");
//			QString description = tr("Your passphrase is needed to use OpenPGP security.");
			
//			QCA::KeyStoreEntry entry = event.keyStoreEntry();
//			QString name;
//			QString entryId;
//		    if(!entry.isNull()) {
//		        name = entry.name();
//		        entryId = entry.id();
//		    } else {
//		        name = event.keyStoreInfo().name();
//		        entryId = QString();
//		    }

//			d->dialog = PasswordDialog::request(title.arg(name), description);
//			d->dialog->setSaveButtonVisible(false);
//			d->dialog->setProperty("eventId", id);
//			d->dialog->setProperty("entryId", entryId);
//			connect(d->dialog, SIGNAL(finished(int)),
//			        SLOT(onPasswordDialogFinished(int)));
//		}
//	}
//}

//void JPGPSupport::onPasswordDialogFinished(int result)
//{
//	Q_D(JPGPSupport);
//	Q_ASSERT(d->dialog);
//	d->dialog->deleteLater();
//	int eventId = d->dialog->property("eventId").toInt();
//	if (result == PasswordDialog::Rejected) {
//		d->eventHandler->reject(eventId);
//	} else {
//		QString entryId = d->dialog->property("entryId").toString();
//		d->eventHandler->submitPassword(eventId, QCA::SecureArray(d->dialog->password().toUtf8()));
//		d->passwords.insert(entryId, QCA::SecureArray(d->dialog->password().toUtf8()));
//	}
//	d->dialog = 0;
//	if (!d->queue.isEmpty()) {
//		QList<QPair<int, QCA::Event> > queue;
//		qSwap(queue, d->queue);
//		for (int i = 0; i < queue.size(); ++i)
//			onEvent(queue[i].first, queue[i].second);
//	}
//}

//void JPGPSupport::onKeyStoreManagerLoaded()
//{
//	foreach (const QString &key, d_func()->keyStoreManager.keyStores())
//		onKeyStoreAvailable(key);
//}

//void JPGPSupport::onKeyStoreAvailable(const QString &id)
//{
//	Q_D(JPGPSupport);
//	QCA::KeyStore *keyStore = new QCA::KeyStore(id, &d->keyStoreManager);
//	connect(keyStore, SIGNAL(updated()), SIGNAL(keysUpdated()));
//	connect(keyStore, SIGNAL(updated()), SLOT(onKeysUpdated()));
//	d->keyStores << keyStore;
//}

//void JPGPSupport::onPGPKeyIdChanged(const QString &id)
//{
//	Q_D(JPGPSupport);
//	JAccount *account = qobject_cast<JAccount*>(sender());
//	Q_ASSERT(account);
//	QCA::PGPKey pgpKey = findKey(id, SecretKey);
//	d->accountKeys.insert(account, pgpKey);
//}

//void JPGPSupport::onAssignKeyToggled(QObject *obj)
//{
//	JContact *contact = qobject_cast<JContact*>(obj);
//	Q_ASSERT(contact);
//	if (contact->pgpKeyId().isEmpty()) {
//		JPGPKeyDialog *dialog = new JPGPKeyDialog(JPGPKeyDialog::PublicKeys, contact->pgpKeyId());
//		connect(dialog, SIGNAL(accepted()), SLOT(onKeyDialogAccepted()));
//		dialog->setProperty("object", qVariantFromValue(obj));
//		dialog->show();
//	} else {
//		contact->setPGPKeyId(QString());
//		updateEncryptionAction(contact);
//	}
//}

//void JPGPSupport::onEncryptToggled(QObject *obj)
//{
//	JContact *contact = qobject_cast<JContact*>(obj);
//	Q_ASSERT(contact);
//	contact->setEncrypted(!contact->isEncrypted());
//	updateEncryptionAction(contact);
//}

//void JPGPSupport::onKeyDialogAccepted()
//{
//	JPGPKeyDialog *dialog = qobject_cast<JPGPKeyDialog*>(sender());
//	Q_ASSERT(dialog);
//	QObject *obj = dialog->property("object").value<QObject*>();
//	Q_ASSERT(obj);
//	JContact *contact = qobject_cast<JContact*>(obj);
//	Q_ASSERT(contact);
//	QCA::KeyStoreEntry keyEntry = dialog->keyStoreEntry();
//	contact->setPGPKeyId(keyEntry.pgpPublicKey().keyId());
//	updateEncryptionAction(contact);
//}

//void JPGPSupport::onKeysUpdated()
//{
//	Q_D(JPGPSupport);
//	foreach (JAccount *account, d->accountKeys.keys()) {
//		foreach (JContactResource *resource, account->roster()->resources()) {
//			if (resource->pgpVerifyStatus() == QCA::SecureMessageSignature::NoKey
//			        && resource->presence().payload<Jreen::PGPSigned>()) {
//				verifyPGPSigning(resource);
//			}
//		}
//	}
//}

//void JPGPSupport::updateEncryptionAction(QObject *obj)
//{
//	Q_D(JPGPSupport);
//	foreach (QAction *action, d->assignPGPKeyAction->actions(obj))
//		d->assignPGPKeyAction->showImpl(action, obj);
//	foreach (QAction *action, d->toggleEncryptionAction->actions(obj))
//		d->toggleEncryptionAction->showImpl(action, obj);
//}

//QString JPGPSupport::errorText(QCA::SecureMessage::Error error)
//{
//	QString text;
//	switch (error) {
//	case QCA::SecureMessage::ErrorPassphrase:
//		text = tr("Invalid passphrase");
//		break;
//	case QCA::SecureMessage::ErrorFormat:
//		text = tr("Invalid input format");
//		break;
//	case QCA::SecureMessage::ErrorSignerExpired:
//		text = tr("Signing key expired");
//		break;
//	case QCA::SecureMessage::ErrorSignerInvalid:
//		text = tr("Invalid key");
//		break;
//	case QCA::SecureMessage::ErrorEncryptExpired:
//		text = tr("Encryption key expired");
//		break;
//	case QCA::SecureMessage::ErrorEncryptUntrusted:
//		text = tr("Encryption key is untrusted");
//		break;
//	case QCA::SecureMessage::ErrorEncryptInvalid:
//		text = tr("Encryption key is invalid");
//		break;
//	case QCA::SecureMessage::ErrorNeedCard:
//		text = tr("PGP card is missing");
//		break;
//	default:
//		text = tr("Unknown error");
//	}
//	return text;
//}

//}
