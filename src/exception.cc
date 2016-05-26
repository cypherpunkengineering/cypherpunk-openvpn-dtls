#include <iostream>
#include <mutex>

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

// TODO al - generate backtrace & all that good stuff
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
	}
	catch (cypher::pexception& e)
	{
		std::cout << "terminate called after throwing an instance of 'cypher::pexception'" << std::endl;

		std::cout << boost::diagnostic_information(e, false) << std::endl;
	}
	catch (boost::exception& e)
	{
		std::cout << "terminate called after throwing an instance of 'boost::exception'" << std::endl;

		std::cout << boost::diagnostic_information(e, false) << std::endl;
	}
	catch (...)
	{
		std::cout << "terminate called after throwing an instance of an unknown exception" << std::endl;
		std::abort();
	}

	std::abort();
}

// XXX al - dummy global object used for program-wide initialization.
std::once_flag init_flag;
class init
{
public:
	init()
	{
		//std::call_once(init_flag, [](){ std::set_terminate(terminate); });
	}
private:
};

volatile init __init { };

} // local anonymous namespace

namespace cypher
{

#undef runtime_error
runtime_error::runtime_error(std::stringstream& ss) :
	std::runtime_error(ss.str())
{
}

pexception::pexception(std::string name)
{
	*this << boost::errinfo_api_function(name.c_str());
	*this << boost::errinfo_errno(errno);
}

} // cypher::
