#ifndef LOGGER_AK_H
#define LOGGER_AK_H


#define BOOST_LOG_DYN_LINK


////////////////////////////////////////////////////////////////////////////////

/***
     Logger for the LE365 TCP-server
                                     ***/


////////////////////////////////////////////////////////////////////////////////

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
namespace bptm = boost::posix_time;


class SrvLogger {
private:
    src::logger lg;
public:
    SrvLogger(const char *file_name);
    bool WriteLog(const char *msg);
};

////////////////////////////////////////////////////////////////////////////////

#endif
