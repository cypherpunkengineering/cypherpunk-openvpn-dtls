#include <iostream>

#include <cypher/packet>
#include <cypher/tunnel_interface>

#include <cypher/exception>

Tunnelnterface::Tunnelnterface(io_service& io, const std::string name) :
	sd(io)
{
	int ret;

	this->fd = ::open("/dev/net/tun", O_RDWR);
	if (this->fd < 0)
	{
		throw cypher::pexception("fd");
	}

	this->socket = ::socket(AF_INET, SOCK_DGRAM, 0);
	if (this->socket < 0)
	{
		throw cypher::pexception("socket");
	}

	memset(&this->ifr, 0, sizeof(this->ifr));

	size_t namelen = name.length();
	if (namelen != 0)
	{
		if (namelen >= IFNAMSIZ)
		{
			throw cypher::runtime_error("Name too long " << namelen << " (max " << IFNAMSIZ << ")");
		}

		strncpy(this->ifr.ifr_name, name.c_str(), IFNAMSIZ);
	}

	/* Flags: IFF_TUN   - TUN device (no Ethernet headers)
	 *	  IFF_TAP   - TAP device
	 *	  IFF_NO_PI - Do not provide packet information
	 */
	this->ifr.ifr_flags = IFF_TUN;

	ret = ioctl(this->fd, TUNSETIFF, (void *)&this->ifr);
	if (ret < 0)
	{
		throw cypher::pexception("ioctl");
	}

	this->sd.assign(this->fd);
}

Tunnelnterface::Tunnelnterface(io_service& io) :
	Tunnelnterface(io, "")
{
}

Tunnelnterface::~Tunnelnterface()
{
}

void
Tunnelnterface::setAddress(std::pair<ip::address_v4, ip::address_v4> endpoints)
{
	sockaddr_in *sin;
	int ret;

	/* set the IP of this end point of tunnel */
	sin = reinterpret_cast<sockaddr_in *>(&this->ifr.ifr_addr);
	sin->sin_addr.s_addr = ntohl(endpoints.first.to_ulong());
	sin->sin_family = AF_INET;

	ret = ioctl(this->socket, SIOCSIFADDR, &this->ifr);
	if (ret < 0)
	{
		throw cypher::pexception("ioctl (SIOCSIFADDR)");
	}

	sin->sin_addr.s_addr = ntohl(endpoints.second.to_ulong());

	ret = ioctl(this->socket, SIOCSIFDSTADDR, &this->ifr);
	if (ret < 0)
	{
		throw cypher::pexception("ioctl (SIOCSIFDSTADDR)");
	}

	memset(sin, 0, sizeof *sin);
}

void
Tunnelnterface::setAddress(ip::address_v4 addr, ip::address_v4 mask)
{
	sockaddr_in *sin;
	int ret;

	/* set the IP of this end point of tunnel */
	sin = reinterpret_cast<sockaddr_in *>(&this->ifr.ifr_addr);
	sin->sin_addr.s_addr = ntohl(addr.to_ulong());
	sin->sin_family = AF_INET;

	ret = ioctl(this->socket, SIOCSIFADDR, &this->ifr);
	if (ret < 0)
	{
		throw cypher::pexception("ioctl (SIOCSIFADDR)");
	}

	sin->sin_addr.s_addr = ntohl(mask.to_ulong());
	ret = ioctl(this->socket, SIOCSIFNETMASK, &this->ifr);
	if (ret < 0)
	{
		throw cypher::pexception("ioctl (SIOCSIFNETMASK)");
	}

	memset(sin, 0, sizeof *sin);
}


void
Tunnelnterface::up()
{
	int ret;

	// XXX al: this is gonna get clobbered, we need to keep a state
	// independantly of this->ifr
	this->ifr.ifr_flags = IFF_UP | IFF_RUNNING;

	ret = ioctl(this->socket, SIOCSIFFLAGS, &this->ifr);
	if (ret < 0)
	{
		throw cypher::pexception("ioctl (SIOCSIFFLAGS)");
	}
}

void
Tunnelnterface::prime()
{
	this->asyncReadSome();
}

void
Tunnelnterface::asyncReadSome()
{
	auto packet = std::make_shared<Packet>();

	packet->put(packet->capacity());
	packet->reserve(128);

	this->sd.async_read_some(buffer(packet->data(), packet->size()),
		std::bind(&Tunnelnterface::asyncReadSomeHandler,
			this,
			packet,
			std::placeholders::_1,
			std::placeholders::_2));
}

void
Tunnelnterface::asyncReadSomeHandler(std::shared_ptr<Packet> packet, const boost::system::error_code& ec, size_t bytes_transferred)
{
	packet->trim(bytes_transferred);

	this->chain(packet);

	this->asyncReadSome();
}

void
Tunnelnterface::setProcessingChain(ProcessingChain pc)
{
	this->chain = pc;
}
