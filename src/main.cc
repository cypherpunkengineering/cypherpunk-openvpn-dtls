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
		throw cypher::pexception("fd");
	}

	this->socket = ::socket(AF_INET, SOCK_DGRAM, 0);
	if (this->socket < 0)
	{
		throw cypher::pexception("socket");
	}

	memset(&this->ifr, 0, sizeof(this->ifr));

	/* Flags: IFF_TUN   - TUN device (no Ethernet headers)
	 *	  IFF_TAP   - TAP device
	 *	  IFF_NO_PI - Do not provide packet information
	 */
	this->ifr.ifr_flags = IFF_TAP;

	size_t namelen = name.length();
	if (namelen != 0)
	{
		if (namelen >= IFNAMSIZ)
		{
			throw cypher::runtime_error("Name too long " << namelen << " (max " << IFNAMSIZ << ")");
		}

		strncpy(this->ifr.ifr_name, name.c_str(), IFNAMSIZ);
	}

	ret = ioctl(this->fd, TUNSETIFF, (void *)&this->ifr);
	if (ret < 0)
	{
		throw cypher::pexception("ioctl");
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

	ret = ioctl(this->socket, SIOCSIFADDR, &ifr);
	if (ret < 0)
	{
		throw cypher::pexception("ioctl");
	}
}

class UDPSession
{
public:
	UDPSession(io_service& io, ip::address_v4&& host, short unsigned port):
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

	~UDPSession()
	{
	}

	void bootstrap()
	{
		if (this->remote.address().is_unspecified())
		{
			this->socket.async_receive_from(buffer(this->readbuf.data(), this->readbuf.capacity()),
				this->remote,
				[this](const boost::system::error_code& ec, size_t bytes_transferred)
				{
					std::cout << "read " << bytes_transferred << " from " << this->remote << std::endl;
				});
		}
		else
		{
			this->socket.async_send_to(buffer(this->readbuf.data(), 128),
				this->remote,
				[](const boost::system::error_code& ec, size_t bytes_transferred)
				{
					std::cout << "wrote " << bytes_transferred << std::endl;
				});
		}
	}

private:
	ip::udp::endpoint remote;
	ip::udp::socket socket;

	std::vector<uint8_t> readbuf;
};

int main(int argc, char** argv)
{
	io_service io;
	std::string host;


	std::cout << argc << std::endl;

	if (argc == 1)
	{
		host = "0.0.0.0";
	}
	else
	{
		host = argv[1];
	}

	std::cout << host << std::endl;

	UDPSession session(io, ip::address_v4::from_string(host), 9000);

	session.bootstrap();

	io.run();

/*
	Tunnelnterface tunif(io);

	tunif.setAddress(ip::address_v4::from_string("10.10.10.2"));

	sleep(10);
*/

	return 0;
}
