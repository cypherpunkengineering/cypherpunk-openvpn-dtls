#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/stat.h>

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <linux/if_tun.h>

#include <boost/asio.hpp>

#include <iostream>

#include <cypher/exception>

using namespace boost::asio;

class Tunnelnterface
{
public:
	Tunnelnterface(io_service&, const std::string);
	Tunnelnterface(io_service&);
	~Tunnelnterface();

	void setAddress(ip::address_v4 addr);
private:
	struct ifreq ifr;
	int fd;
	int socket;
};

Tunnelnterface::Tunnelnterface(io_service& io, const std::string name)
{
	int ret;

	this->fd = ::open("/dev/net/tun", O_RDWR);
	if (this->fd < 0)
	{
		THROW(cypher::pexception("blu"));
	}

	this->socket = ::socket(AF_INET, SOCK_DGRAM, 0);
	if (this->socket < 0)
	{
		THROW(cypher::pexception("blu"));
	}

	memset(&this->ifr, 0, sizeof(this->ifr));

	/* Flags: IFF_TUN	- TUN device (no Ethernet headers)
	 *		  IFF_TAP	- TAP device
	 *		  IFF_NO_PI - Do not provide packet information
	 */
	this->ifr.ifr_flags = IFF_TUN;

	if (name.length() != 0)
	{
		if (name.length() >= IFNAMSIZ)
		{
			THROW(cypher::pexception("length"));
		}

		strncpy(this->ifr.ifr_name, name.c_str(), IFNAMSIZ);
	}

	ret = ioctl(this->fd, TUNSETIFF, (void *)&this->ifr);
	if (ret < 0)
	{
		THROW(cypher::pexception("ioctl"));
	}
}

Tunnelnterface::Tunnelnterface(io_service& io) :
	Tunnelnterface(io, "")
{
}

Tunnelnterface::~Tunnelnterface()
{
}

void
Tunnelnterface::setAddress(ip::address_v4 addr)
{
	sockaddr_in *sin;
	int ret;

	/* set the IP of this end point of tunnel */
	sin = reinterpret_cast<sockaddr_in *>(&this->ifr.ifr_addr);
	sin->sin_addr.s_addr = ntohl(addr.to_ulong());
	sin->sin_family = AF_INET;

	ret = ioctl(this->fd, SIOCSIFADDR, &ifr);
	if (ret < 0)
	{
		THROW(cypher::pexception("ioctl"));
	}
}

int main()
{
#if 0
	int fd;

	fd = tun_alloc("tunif");

	io_service io;
	local::datagram_protocol::socket tunif(io);

	tunif.assign(local::datagram_protocol(), fd);

	io.run();
#endif

	cypher::exception::init();

	io_service io;
	Tunnelnterface tunif(io);

	tunif.setAddress(ip::address_v4(0x10101010));

	sleep(10);

	return 0;
}
