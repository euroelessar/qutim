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

//#ifndef PGPSUPPORT_H
//#define PGPSUPPORT_H

////#include <QtCrypto>
//#include <jreen/presence.h>
//#include <jreen/messagesession.h>
//#include "jaccount.h"

//namespace Jabber
//{

//class JContact;
//class JContactResource;
//class JPGPSupportPrivate;

//class AssignPGPKeyActionGenerator : public qutim_sdk_0_3::ActionGenerator
//{
//public:
//	AssignPGPKeyActionGenerator(QObject *obj, const char *slot);
//	void showImpl(QAction *action,QObject *obj);
//};

//class ToggleEncryptionActionGenerator : public qutim_sdk_0_3::ActionGenerator
//{
//public:
//	ToggleEncryptionActionGenerator(QObject *obj, const char *slot);
//	void createImpl(QAction *action, QObject *obj) const;
//	void showImpl(QAction *action,QObject *obj);
//};

//class JPGPDecryptReply : public QObject
//{
//	Q_OBJECT
//	friend class JPGPSupport;
//public:
//	JPGPDecryptReply(QObject *parent);
	
//signals:
//	void finished(qutim_sdk_0_3::ChatUnit *unit, qutim_sdk_0_3::ChatUnit *unitForSession, const Jreen::Message &message);
//};

//class JPGPSupport : public QObject
//{
//	Q_OBJECT
//	Q_DECLARE_PRIVATE(JPGPSupport)
//public:
//	enum Type {
//		Signature,
//		Message
//	};
//	enum KeyType {
//		PublicKey,
//		SecretKey
//	};
	
//	static JPGPSupport *instance();

//	bool isAvailable();
//	bool canAddKey() const;
////	QString addKey(const QCA::PGPKey &key);
////	void verifyPGPSigning(JContactResource *resource);
	
//	void addAccount(JAccount *account);
//	void removeAccount(JAccount *account);
//	QString stripHeader(const QString &message);
//	QString addHeader(const QString &message, Type type);
//	void send(JAccount *account, const Status &status, int priority = 0);
//	bool send(Jreen::MessageSession *session, ChatUnit *unit, Jreen::Message message);
//	JPGPDecryptReply *decrypt(ChatUnit *unit, ChatUnit *unitForSession, const Jreen::Message &message);
//	bool isChannelEncryptable(ChatUnit *unit);
////	QList<QCA::KeyStoreEntry> pgpKeys(KeyType type) const;
////	QCA::KeyStoreEntry findEntry(const QString &keyId, KeyType type) const;
////	QCA::PGPKey findKey(const QString &keyId, KeyType type) const;

//signals:
//	void keysUpdated();

//private slots:
////	void onEvent(int id, const QCA::Event &event);
//	void onPasswordDialogFinished(int result);
//	void onKeyStoreManagerLoaded();
//	void onKeyStoreAvailable(const QString &id);
//	void onPGPKeyIdChanged(const QString &id);
//	void onSignFinished();
//	void onEncryptFinished();
//	void onDecryptFinished();
//	void onVerifyFinished();
//	void onAssignKeyToggled(QObject *obj);
//	void onEncryptToggled(QObject *obj);
//	void onKeyDialogAccepted();
//	void onKeysUpdated();

//private:
////	JPGPSupport();
////	~JPGPSupport();
////	void updateEncryptionAction(QObject *obj);
////	QString errorText(QCA::SecureMessage::Error error);

////	QScopedPointer<JPGPSupportPrivate> d_ptr;
//};

//}

//#endif // PGPSUPPORT_H
