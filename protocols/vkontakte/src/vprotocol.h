#ifndef VPROTOCOL_H
#define VPROTOCOL_H
#include <qutim/protocol.h>
#include <qutim/settingslayer.h>

namespace qutim_sdk_0_3
{
class SettingsItem;
}

class VAccount;
typedef QList<VAccount*> VAccountList;
typedef QHash<QString, VAccount*> VAccountHash;

class  VProtocol : public qutim_sdk_0_3::Protocol
{
    Q_OBJECT
    Q_CLASSINFO("Protocol", "vkontakte")
public:
    VProtocol();

	virtual ~VProtocol();
    virtual qutim_sdk_0_3::Account* account(const QString& id) const;
    virtual void loadAccounts();
    virtual QList<qutim_sdk_0_3::Account*> accounts() const;
    virtual QVariant data(DataType type);

	static VProtocol *instance();
	void addAccount(VAccount *account);
protected:
    qutim_sdk_0_3::Account *doCreateAccount(const QString &email, const QVariantMap &parameters);
    virtual void virtual_hook(int id, void *data);
private slots:
    void onWebPageTriggered(QObject*);
    void onAccountDestroyed(QObject*);
private:
    QScopedPointer<qutim_sdk_0_3::SettingsItem> m_mainSettings;
    VAccountHash m_accounts;
};
#endif // VPROTOCOL_H
