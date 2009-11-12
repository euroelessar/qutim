/*
  Copyright (c) 2009 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/



#ifndef MUCINVITE_H__
#define MUCINVITE_H__


#include "stanzaextension.h"
#include "jid.h"
#include "gloox.h"

#include <string>

namespace gloox
{

  class Tag;

  /**
   * @brief This is an implementation of XEP-0249 (Direct MUC Invitations) as a StanzaExtension.
   *
   * XEP Version: 0.1
   * @author Jakob Schroeter <js@camaya.net>
   * @since 1.1
   */
  class GLOOX_API MUCInvite : public StanzaExtension
  {

    public:
      /**
       * Constructs a new object from the given Tag.
       * @param tag The Tag to parse.
       */
      MUCInvite( const Tag* tag = 0 );

      /**
       * Constructs a new object with the given namespace and priority.
       * @param room The bare JID of the room to invite to.
       * @param password An optional password for the room.
       */
      MUCInvite( const JID& room, const std::string& password = EmptyString );

      /**
       * Virtual Destructor.
       */
      virtual ~MUCInvite();

      // reimplemented from StanzaExtension
      virtual const std::string& filterString() const;

      // reimplemented from StanzaExtension
      virtual StanzaExtension* newInstance( const Tag* tag ) const
      {
        return new MUCInvite( tag );
      }

      // reimplemented from StanzaExtension
      virtual Tag* tag() const;

      // reimplemented from StanzaExtension
      virtual StanzaExtension* clone() const
      {
        return new MUCInvite( *this );
      }

    private:
      JID m_room;
      std::string m_pwd;

  };

}

#endif // MUCINVITE_H__
