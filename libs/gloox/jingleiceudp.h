/*
  Copyright (c) 2008-2009 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#ifndef JINGLEICEUDP_H__
#define JINGLEICEUDP_H__

#include "jingletransport.h"
#include "gloox.h"
#include "tag.h"

#include <string>

namespace gloox
{

  namespace Jingle
  {

    /**
     * @brief An abstraction of the signaling part of Jingle ICE-UDP Transport Method (XEP-0176).
     *
     * XEP Version: 0.23
     * @author Jakob Schroeter <js@camaya.net>
     * @since 1.0
     */
    class ICEUDP : public Transport
    {
      public:
        /**
         *
         */
        ICEUDP();

        /**
         * Virtual destructor.
         */
        virtual ~ICEUDP() {}

        // reimplemented from Plugin
        virtual const std::string& filterString() const;

        // reimplemented from Plugin
        virtual Tag* tag() const;

        // reimplemented from Plugin
        virtual Plugin* clone() const
        {
          return new ICEUDP( *this );
        }

    };

  }

}

#endif // JINGLEICEUDP_H__
