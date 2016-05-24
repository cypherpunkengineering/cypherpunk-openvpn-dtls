#include <iostream>

#include <boost/exception/diagnostic_information.hpp> 
#include <boost/exception/errinfo_api_function.hpp>
#include <boost/exception/errinfo_at_line.hpp>
#include <boost/exception/errinfo_errno.hpp>
#include <boost/exception/errinfo_file_handle.hpp>
#include <boost/exception/errinfo_file_name.hpp>
#include <boost/exception/errinfo_file_open_mode.hpp>

#include <cypher/exception>

namespace // local anonymous namespace
{

// TODO: al - generate backtrace & all that good stuff
void
terminate()
{
	static bool need_rethrow = true;

	try {
		if (need_rethrow)
		{
			std::cout << "Uncaught exception raised..." << std::endl;
			need_rethrow = false;
			throw;
		}
	} catch (boost::exception& e)
	{
		cypher::pexception* pe = dynamic_cast<cypher::pexception*>(&e);

		std::cout << boost::diagnostic_information(e, false) << std::endl;
	} catch (...)
	{
		std::cout << "got an unknown exception" << std::endl;
		std::abort();
	}

	std::abort();
}

} // local anonymous namespace

namespace cypher
{

pexception::pexception(std::string name)
{
	*this << boost::errinfo_api_function("foo");
	*this << boost::errinfo_errno(errno);
}

void
exception::init()
{
	std::set_terminate(terminate);
}

} // cypher::
