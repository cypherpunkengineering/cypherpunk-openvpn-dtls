#include <cypher/packet>
#include <cypher/processing_chain>

ProcessingChain::ProcessingChain()
{
}

ProcessingChain::ProcessingChain(std::initializer_list<ProcessingLink> l)
{
	this->links.insert(this->links.end(), l.begin(), l.end());
}

ProcessingChain::~ProcessingChain()
{
}

void
ProcessingChain::operator()(std::shared_ptr<Packet> packet)
{
	for (auto& link : this->links)
	{
		link(packet);

		// The packet has been consumed, bail out
		if (packet == nullptr)
		{
			break;
		}
	}
}

