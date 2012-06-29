#ifndef MEEGOINTEGRATION_QUICKCONTACTINFO_H
#define MEEGOINTEGRATION_QUICKCONTACTINFO_H

#include <qutim/buddy.h>
#include <qutim/inforequest.h>
#include <qutim/actionbox.h>
#include <qutim/status.h>

namespace MeegoIntegration {

class QuickContactInfo : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QObject *object READ object WRITE setObject NOTIFY objectChanged)
    Q_PROPERTY(qutim_sdk_0_3::DataItem item READ item NOTIFY itemChanged)
    Q_PROPERTY(bool readOnly READ readOnly NOTIFY readOnlyChanged)
    Q_CLASSINFO("Service", "ContactInfo")
public:
    explicit QuickContactInfo();
    
    QObject *object();
    void setObject(QObject *object);
    qutim_sdk_0_3::DataItem item();
    bool readOnly();
    
public slots:
    void show(QObject *object); 
    void request();
    void save(const qutim_sdk_0_3::DataItem &item);
    
signals:
    void objectChanged(QObject *object);
    void itemChanged(const qutim_sdk_0_3::DataItem &item);
    void readOnlyChanged(bool readOnly);
    void showRequest();
    
private slots:
	void onRequestStateChanged(qutim_sdk_0_3::InfoRequest::State state);
    
private:
	qutim_sdk_0_3::DataItem filterItems(const qutim_sdk_0_3::DataItem &item, bool readOnly = false);
	void filterItemsHelper(const qutim_sdk_0_3::DataItem &item, qutim_sdk_0_3::DataItem &result, bool readOnly);
	bool isItemEmpty(const qutim_sdk_0_3::DataItem &item);
    
private:
    qutim_sdk_0_3::DataItem m_item;
	qutim_sdk_0_3::InfoRequest *m_request;
	QObject *m_object;
	bool m_readWrite;
};

} // namespace MeegoIntegration

#endif // MEEGOINTEGRATION_QUICKCONTACTINFO_H
