#define BOOST_LOG_DYN_LINK


#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared_object.hpp>
#include <boost/log/attributes/attribute.hpp>
#include <boost/log/attributes/clock.hpp>
#include <boost/log/attributes/counter.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/core/null_deleter.hpp>
#include <fstream>
#include <iostream>


namespace logging = boost::log;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace expr = boost::log::expressions;
namespace attrs = boost::log::attributes;

/*
//[ example_tutorial_file_manual
void init()
{
    // Construct the sink
    typedef sinks::synchronous_sink<sinks::text_ostream_backend> text_sink;
    boost::shared_ptr<text_sink> sink = boost::make_shared<text_sink>();
    // Add a stream to write log to file
    sink->locked_backend()->add_stream(
    	boost::make_shared<std::ofstream>("le365.log"));
    // Add a stream to write log to console
    boost::shared_ptr<std::ostream> stream(&std::clog, boost::null_deleter());
	sink->locked_backend()->add_stream(stream);
    // Register the sink in the logging core
    logging::core::get()->add_sink(sink);
}
//]
*/

int main(int, char*[])
{
	
    
    //init();
    
    logging::add_file_log("sample.log");
    logging::add_common_attributes();

    src::logger lg;
    
    BOOST_LOG(lg) << "Hello world!";

    return 0;
}