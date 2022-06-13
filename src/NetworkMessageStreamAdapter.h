/**
* NetworkMessageStreamAdapter.h
*
* Copyright:
*   2020, 2021   Matthias Gierlings (matthias.gierlings@rub.de)
*
* This software is released under the Simplified BSD License
* (see license.txt)
**/

#ifndef NETWORK_MESSAGE_STREAM_ADAPTER_H
#define NETWORK_MESSAGE_STREAM_ADAPTER_H

#include <memory>
#include "EventListener.h"

class RawStreamWriter;

namespace Network { class IOEventProvider; }
namespace Network { namespace Events {
    class DataReceived;
    class DataSent;
}}

/**
 * Redirects data invoming over a network connection to an output
 * stream
 **/
class NetworkMessageStreamAdapter:
    public EventListener<Network::Events::DataReceived>,
    public EventListener<Network::Events::DataSent>,
    public std::enable_shared_from_this<NetworkMessageStreamAdapter>
{
    public:
        /**
         * The IOMode dertermines if incoming, outgoing or both
         * types of messages should be redirected.
         **/
        enum Mode
        {
            IN, /* Mode::IN redirects incoming messages only. */
            OUT, /* Mode::OUT redirects outgoing messages only. */
            IN_OUT /* Mode::IN_OUT redirects both incoming and outgoing messages. */
        };


        NetworkMessageStreamAdapter(const NetworkMessageStreamAdapter&) = delete;
        NetworkMessageStreamAdapter& operator=(const NetworkMessageStreamAdapter&) = delete;
        virtual ~NetworkMessageStreamAdapter() {}

        /**
         * Enables reception of messages for a network connection.
         * @param connection A network connection to attach the
         **/
        void attach(std::shared_ptr<Network::IOEventProvider> connection);
        /**
         * Disables reception of messges for a network connection.
         * @param connection A network connection to attach the
         **/
        void detach(std::shared_ptr<Network::IOEventProvider> connection);

        /**
         * Writes network messages to an output strem.
         * @param ostream A writable stream.
         * @param mode An IO mode the adapter should operate in. It
         * Dertermines if incoming, outgoing or both types of
         * messages should be redirected.
         **/
        static std::shared_ptr<NetworkMessageStreamAdapter> create(
            std::shared_ptr<RawStreamWriter> stream_writer,
            NetworkMessageStreamAdapter::Mode mode)
        {
            return std::shared_ptr<NetworkMessageStreamAdapter>(
                new NetworkMessageStreamAdapter(stream_writer, mode));
        }

    protected:
        virtual void handle_event(Network::Events::DataReceived e) override;
        virtual void handle_event(Network::Events::DataSent e) override;

    private:
        NetworkMessageStreamAdapter(
            std::shared_ptr<RawStreamWriter> stream_writer,
            NetworkMessageStreamAdapter::Mode mode)
            : m_stream_writer(stream_writer), m_mode(mode) {}

        std::shared_ptr<RawStreamWriter> m_stream_writer;
        const Mode m_mode;
};


#endif //NETWORK_MESSAGE_STREAM_WRITER_H

