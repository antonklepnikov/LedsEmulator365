#define BOOST_LOG_DYN_LINK


#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

namespace logging = boost::log;
namespace src = boost::log::sources;
namespace expr = boost::log::expressions;

static const char SERVER_LOG_FILE_NAME[] = "le365_tcp.log";

bool logging_function(const char *text)
{
    src::logger lg;

    logging::record rec = lg.open_record();
    if (rec)
    {
        logging::record_ostream strm(rec);
        strm << text;
        strm.flush();
        lg.push_record(boost::move(rec));
        return true;
    } 
    else { return false; }
}


int main(int, char*[])
{
	logging::formatter formatter = expr::stream
            << expr::format_date_time< boost::posix_time::ptime >("TimeStamp", "%d-%m-%Y / %H:%M:%S:%f") << " -> "
            << expr::message;

    logging::add_file_log(SERVER_LOG_FILE_NAME)->set_formatter(formatter);
    logging::add_console_log(std::clog)->set_formatter(formatter);
    logging::add_common_attributes();

    logging_function("Hi, world!");

    return 0;
}