#include <iostream>

#include <cypher/packet>
#include <cypher/udp_session>

#include <cypher/exception>

UDPSession::UDPSession(io_service& io, ip::address_v4 host, short unsigned port) :
	remote(host, port),
	socket(io)
{
	socket.open(ip::udp::v4());

	if (this->remote.address().is_unspecified())
	{
		socket.set_option(ip::udp::socket::reuse_address(true));
		socket.bind(this->remote);
	}

	readbuf.reserve(4096);
}

UDPSession::~UDPSession()
{
}

void
UDPSession::prime()
{
	this->asyncReceiveFrom();
}

void
UDPSession::write(std::shared_ptr<Packet>& packet)
{
	if (this->remote.address().is_unspecified())
	{
		return;
	}

	this->socket.async_send_to(buffer(packet->data(), packet->size()),
		this->remote,
		std::bind(&UDPSession::asyncSendToHandler,
			this,
			packet,
			std::placeholders::_1,
			std::placeholders::_2));
}

void
UDPSession::asyncReceiveFrom()
{
	auto packet = std::make_shared<Packet>();

	packet->put(packet->capacity());
	packet->reserve(128);

	this->socket.async_receive_from(buffer(packet->data(), packet->capacity()),
		this->remote,
		std::bind(&UDPSession::asyncReceiveFromHandler,
			this,
			packet,
			std::placeholders::_1,
			std::placeholders::_2));
}

void
UDPSession::asyncSendToHandler(std::shared_ptr<Packet> packet, const boost::system::error_code& ec, size_t bytes_transferred)
{
}

void
UDPSession::asyncReceiveFromHandler(std::shared_ptr<Packet> packet, const boost::system::error_code& ec, size_t bytes_transferred)
{
	packet->trim(bytes_transferred);

	// XXX to our processing chain

	this->asyncReceiveFrom();
}
