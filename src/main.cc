#include <iostream>

#include <memory>

#include <cypher/packet>

#include <cypher/processing_chain>

#include <cypher/tunnel_interface>
#include <cypher/udp_session>

#include <cypher/exception>

using namespace boost::asio;

template <typename T>
class Sink : public ProcessingLink
{
public:
	Sink(std::shared_ptr<T>& session) : session(session) { }
	~Sink() {};

	int operator()(std::shared_ptr<Packet>& p) { session->write(p); };
private:
	std::shared_ptr<T> session;
};

int main(int argc, char** argv)
{
	io_service io;
	io_service::work work(io);
#if 0
	std::string host;

	if (argc == 1)
	{
		host = "0.0.0.0";
	}
	else
	{
		host = argv[1];
	}
#endif

	auto udp_server = std::make_shared<UDPSession>(io, ip::address_v4::from_string("0.0.0.0"), 9000);
	auto udp_client = std::make_shared<UDPSession>(io, ip::address_v4::from_string("127.0.0.1"), 9000);

	auto tunif = std::make_shared<Tunnelnterface>(io);
	tunif->setAddress( { ip::address_v4::from_string("10.10.10.1"), ip::address_v4::from_string("10.10.10.2") } );
	tunif->up();
	tunif->prime();

	std::shared_ptr<ProcessingLink> sink = std::make_shared<Sink<UDPSession>>(udp_client);

	ProcessingChain default_chain({ sink });

	tunif->setProcessingChain(default_chain);

	io.run();

	return 0;
}
