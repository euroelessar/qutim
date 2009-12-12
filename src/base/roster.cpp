#include <QVariant>

#include "mrimdebug.h"

#include "roster.h"

struct RosterPrivate
{
    QList<quint32> handledTypes;
    QMap<quint32,QString> groups;
    MrimAccount* account;
};

Roster::Roster(MrimAccount* acc) : p(new RosterPrivate)
{    
    p->account = acc;
}

Roster::~Roster()
{
}

QList<quint32> Roster::handledTypes()
{
    if (p->handledTypes.isEmpty())
    {
        p->handledTypes << MRIM_CS_CONTACT_LIST2
                        << MRIM_CS_USER_STATUS; //TODO: add more types
    }
    return p->handledTypes;
}
    
bool Roster::handlePacket(MrimPacket& packet)
{
    bool handled = true;
    
    switch (packet.msgType())
    {
    case MRIM_CS_CONTACT_LIST2:
        parseList(packet);
        break;
    case MRIM_CS_USER_STATUS:
        //TODO add status change handling
        break;
    default:
        handled = false;
        break;
    }
    return handled;
}

bool Roster::parseList(MrimPacket& packet)
{
    quint32 opResult;
    packet.readTo(opResult);
    
    if (opResult == GET_CONTACTS_OK)
    {
        quint32 grCount = 0;    
        packet.readTo(grCount);
        
        QString grMask, contMask;
        packet.readTo(&grMask);
        packet.readTo(&contMask);
        
        if (parseGroups(packet,grCount,grMask))
        {
            parseContacts(packet,contMask);                    
        }
    }
    //TODO: handle errors
}

bool Roster::parseGroups(MrimPacket& packet, quint32 count, const QString& mask)
{    
    QParseMultiMap parsedGroup; 
    
    for (quint32 i=0; i < count; i++)
    {
        parsedGroup = parseByMask(packet,mask);
        MDEBUG(VeryVerbose,"Group id:" << i);
        MDEBUG(VeryVerbose,"Group flags:" << parsedGroup.getUint(0));
        p->groups[i] = parsedGroup.getString(0,true);
        MDEBUG(VeryVerbose,"Group name:" << p->groups[i]);
    }
    return true;
}

bool Roster::parseContacts(MrimPacket& packet, const QString& mask)
{
 /*   QRosterParseMultiMap parsedContact; 
    
    quint32 count = 0;
    quint32 idCounter = 20;
    quint32 contFlags = 0;
    quint32 grId = 0;
    quint32 serverFlags = 0;
    quint32 currentStatus = 0;                
    quint32 comSupport;
    QString email; //ANSI
    QString contName; //UNICODE
    QString contPhone; //ANSI
    QString specStatusUri; //ANSI
    QString statusTitle; //UNICODE
    QString statusDesc; //UNICODE
    QString userAgent; //ANSI

    while(!packet.atEnd())
    {
        parsedContact = parseByMask(packet,mask);
        contFlags =         
                
        for (qint32 j=0; j < mask.length(); j++)
        {	
            switch (mask.at(j).toAscii())
            {
            case 'u':
                {
                    quint32 val = packet.;
                    switch (j)
                    {
                    case 0:
                        {
                            contFlags = val;
                            qDebug()<<"UL for contact #"<<count+1<<" read. contFlags="<<QString::number(contFlags,16);
                        }
                        break;
                    case 1:
                        {
                            grId = val;
                            qDebug()<<"UL for contact #"<<count+1<<" read. grId="<<QString::number(grId,16);
                        }
                        break;
                    case 4:
                        {
                            serverFlags = val;
                            qDebug()<<"UL for contact #"<<count+1<<" read. serverFlags="<<QString::number(serverFlags,16);
                        }
                        break;
                    case 5:
                        {
                            currentStatus = val;
                            qDebug()<<"UL for contact #"<<count+1<<" read. currentStatus="<<currentStatus;						
                        }
                        break;
                    case 10:
                        {
                            comSupport = val;
                            qDebug()<<"UL for contact #"<<count+1<<" read. comSupport="<<comSupport;
                        }
                        break;
                    }
                }
                break;
            case 's':
                {
                    switch (j)
                    {
                    case 2:
                        {
                        email = ByteUtils::ReadToString(*m_clBuffer);;
                        qDebug()<<"String for contact #"<<count+1<<" read. email="<<email;
                        }
                    break;
                case 3:
                        {
                        contName = ByteUtils::ReadToString(*m_clBuffer,true);
                        qDebug()<<"String for contact #"<<count+1<<" read. contName="<<contName;
                        }
                    break;
                case 6:
                        {
                        contPhone = ByteUtils::ReadToString(*m_clBuffer);
                        qDebug()<<"String for contact #"<<count+1<<" read. contPhone="<<contPhone;
                    }
                    break;
                case 7:
                    {
                        specStatusUri = ByteUtils::ReadToString(*m_clBuffer);
                        qDebug()<<"String for contact #"<<count+1<<" read. spec. status URI="<<specStatusUri;
                    }
                    break;
                case 8:
                    {
                        statusTitle = ByteUtils::ReadToString(*m_clBuffer,true);
                        qDebug()<<"String for contact #"<<count+1<<" read. statusTitle="<<statusTitle;
                    }
                    break;
                case 9:
                    {
                        statusDesc = ByteUtils::ReadToString(*m_clBuffer,true);
                        qDebug()<<"String for contact #"<<count+1<<" read. statusDesc="<<statusDesc;
                    }
                    break;
                case 11:
                    {
                        userAgent = ByteUtils::ReadToString(*m_clBuffer);
                        qDebug()<<"String for contact #"<<count+1<<" read. userAgent="<<userAgent;
                    }
                    break;
                default:
                    ByteUtils::ReadToString(*m_clBuffer); //just skeep bytes, temp solution TODO
                    break;
                    }
                }
                break;				
            default:
                break;
            }
        }
            bool isAuthedMe = !(serverFlags & CONTACT_INTFLAG_NOT_AUTHORIZED);

            if (!(contFlags & CONTACT_FLAG_REMOVED) && grId < 20)
            {
                UserAgent *contAgent = UserAgent::Parse(userAgent);
                Status contFullStatus(currentStatus,statusTitle,statusDesc,specStatusUri);

                MRIMContact* contact = new MRIMContact(m_account, contFlags,contName,email,idCounter,grId,contFullStatus,serverFlags,contPhone,*contAgent,comSupport,true,isAuthedMe);
                delete contAgent;

                AddItem(contact);

                if (contact->Email() == "phone" && !m_isPhoneGroupAdded)
                {
                    MRIMGroup* phoneGroup = new MRIMGroup(m_account, 0,QString::number(contact->GroupId()),tr("Phone contacts"));
                    AddItem(phoneGroup);
                    m_isPhoneGroupAdded = true;
                }
            }
            count++;
            idCounter++;
    }
    #ifdef DEBUG_LEVEL_DEV
    qDebug()<<"Total contacts read:"<<count;
    #endif
    return count;*/
}

QParseMultiMap Roster::parseByMask(MrimPacket& packet, const QString& mask)
{
    QParseMultiMap map;
    
    quint32 num = 0;
    LPString str;

    foreach (QChar ch, mask)
    {	
        if (ch == 'u')
        {
            packet.readTo(num);
            map.insert(ch,num);
        }
        else if (ch == 's')
        {
            packet.readTo(str);
            QVariant val(qMetaTypeId<LPString>());  
            val.setValue(str);
            map.insert(ch,val);
        }
    }
    return map;
}
