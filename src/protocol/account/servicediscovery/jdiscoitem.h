#ifndef JDISKOITEM_H
#define JDISKOITEM_H

#include <QStringList>

namespace Jabber
{
	class JDiscoItem : public QObject
	{
		Q_OBJECT
		public:
			enum JDiscoAction
			{
				JDiscoExecute,
				JDiscoRegister,
				JDiscoJoin,
				JDiscoSearch,
				JDiscoAdd,
				JDiscoVCard,
				JDiscoProxy
			};
			struct JDiscoIdentity
			{
				QString name;
				QString category;
				QString type;
			};
			JDiscoItem() {m_empty = true;}
			void setName(const QString &name) {m_name = name;}
			void setJID(const QString &jid) {m_jid = jid;}
			void setNode(const QString &node) {m_node = node;}
			void setError(const QString &error) {m_error = error;}
			void setExpand(bool expand) {m_expand = expand;}
			void addIdentity(const JDiscoIdentity &identity) {m_identities << identity; m_empty = false;}
			bool hasIdentity(const QString &category, const QString &type = "") {
				foreach (JDiscoIdentity identity, m_identities)
					if (identity.category == category && (type.isEmpty() || identity.type == type))
						return true;
			}
			void addFeature(const QString &feature) {m_features << feature; m_empty = false;}
			bool hasFeature(const QString &feature) {
				return m_features.contains(feature);
			}
			void addAction(JDiscoAction action) {m_actions << action; m_empty = false;}
			QString name() const {return m_name;}
			QString jid() const {return m_jid;}
			QString node() const {return m_node;}
			QString error() const {return m_error;}
			bool expand() const {return m_expand;}
			QList<JDiscoIdentity> identities() {return m_identities;}
			QList<JDiscoAction> actions() {return m_actions;}
			QStringList features() {return m_features;}
			bool empty() const {return m_empty;}
		private:
			QString m_name, m_jid, m_node, m_error;
			QList<JDiscoIdentity> m_identities;
			QStringList m_features;
			QList<JDiscoAction> m_actions;
			bool m_expand, m_empty;
	};
}

#endif //JDISKOITEM_H
