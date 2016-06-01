#include <memory>

#include <cypher/packet>

Packet::Packet() :
	begin(new uint8_t[4096]),
	end(begin + 4096),
	head(begin),
	tail(begin)
{
}

Packet::~Packet()
{
	delete[] begin;
}

uint8_t *
Packet::data()
{
	return this->head;
}

const size_t
Packet::size() const
{
	return this->tail - this->head;
}

const size_t
Packet::capacity() const
{
	return this->end - this->begin;
}

void
Packet::reserve(size_t len)
{
	// assert(this->head == this->begin);
	this->head += len;
}

void
Packet::push(size_t len)
{
	this->head -= len;
}

void
Packet::pull(size_t len)
{
	this->head += len;
}

void
Packet::put(size_t len)
{
	this->tail += len;
}

void
Packet::trim(size_t len)
{
	this->tail = this->head + len;
}

