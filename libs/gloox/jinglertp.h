/*
  Copyright (c) 2008-2009 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#ifndef JINGLERTP_H__
#define JINGLERTP_H__

#include "jingledescription.h"

#include <string>
#include <list>

namespace gloox
{

  class Tag;

  namespace Jingle
  {

    /**
     * @brief An abstraction of the signaling part of Jingle RTP Sessions (XEP-0167).
     *
     * XEP Version: 0.25
     * @author Jakob Schroeter <js@camaya.net>
     * @since 1.1
     */
    class GLOOX_API RTP : public Description
    {
      public:
        /**
         * @brief An abstraction of the &let;payload-type&gt; element used as child of
         * the &lt;description&gt; element.
         *
         * @author Jakob Schroeter <js@camaya.net>
         * @since 1.0
         */
        class GLOOX_API Payload
        {
          public:
            /**
             * Creates a new Payload object from the given attributes and parameters.
             * @param attribs A map of name/value pairs added as XML attributes to the
             * &lt;payload-type&gt; element. See the respective XEP for valid values.
             * @param parameters A map of name/value pairs added as &lt;parameter&gt; child
             * to the &lt;payload-type&gt; element. See the respective XEP and codec specs
             * for valid values.
             */
            Payload( const StringMap& attribs, const StringMap& parameters )
              : m_attribs( attribs ), m_parameters( parameters )
            {}

            /**
             * Destructor.
             */
            ~Payload() {}

            /**
             * Creates a Tag representation of the object.
             * @return A Tag representation of the object. The caller is responsible for
             * deleting the Tag.
             */
            Tag* tag() const;

          private:
            StringMap m_attribs;
            StringMap m_parameters;

        };

        /**
         * A list of payloads.
         */
        typedef std::list<const RTP::Payload*> PayloadList;

        /**
         * Creates a new wrapper for the Audio via RTP Application Format defined in XEP-0167.
         * @param payload A list of acceptable Payload Types.
         */
        RTP( const PayloadList& payload );

        /**
         * Virtual destructor.
         */
        virtual ~RTP();

        // reimplemented from Description
        virtual const std::string& xmlns() const { return XMLNS_JINGLE_RTP; }

        // reimplemented from Plugin
        virtual const std::string& filterString() const;

        // reimplemented from Plugin
        virtual Tag* tag() const;

        // reimplemented from Plugin
        virtual Plugin* clone() const
        {
          return new RTP( *this );
        }

    };

  }

}

#endif // JINGLERTP_H__
