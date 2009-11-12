/*
  Copyright (c) 2008-2009 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#ifndef JINGLECONTENT_H__
#define JINGLECONTENT_H__


#include "jingleplugin.h"
#include "tag.h"

#include <string>

namespace gloox
{

  namespace Jingle
  {

    class Description;
    class Transport;

    /**
     * @brief An abstraction of a Jingle Content Type.
     *
     * You should not need to use this class directly. See
     * @link gloox::Jingle::Session Jingle::Session @endlink for more info on Jingle.
     *
     * @author Jakob Schroeter <js@camaya.net>
     * @since 1.1
     */
    class GLOOX_API Content : public Plugin
    {
      public:
        /**
         * The original creator of the content type.
         */
        enum Creator
        {
          CInitiator,                /**< The creator is the initiator of the session. */
          CResponder,                /**< The creator is the responder. */
          InvalidCreator             /**< Invalid value. */
        };

        /**
         * The parties in the session that will be generating content.
         */
        enum Senders
        {
          SInitiator,                /**< The initiator sends content. */
          SResponder,                /**< The responder sends content. */
          SBoth,                     /**< Both parties send content( default). */
          InvalidSender             /**< Invalid value. */
        };

        /**
         * Creates a new Content wrapper.
         */
        Content( Description* desc, Transport* trans,
                 const std::string& name, Creator creator = CInitiator,
                 Senders senders = SBoth, const std::string& disposition = EmptyString );

        /**
         *
         */
        Content( const Tag* tag = 0 );

        /**
         * Virtual destructor.
         */
        virtual ~Content();

        // reimplemented from Plugin
        virtual const std::string& filterString() const;

        // reimplemented from Plugin
        virtual Tag* tag() const;

        // reimplemented from Plugin
        virtual Plugin* clone() const;

      private:
        Description* m_description;
        Transport* m_transport;
        Creator m_creator;
        std::string m_disposition;
        std::string m_name;
        Senders m_senders;

    };

  }

}

#endif // JINGLECONTENT_H__
