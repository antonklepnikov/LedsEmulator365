// g++ $GPPFLAGS_DEBUG sl_test.cpp -o slt -lboost_log -lboost_thread

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
    SrvLogger(const char *file_name) : lg()
    {
        logging::formatter formatter = expr::stream
            << expr::format_date_time<bptm::ptime>(
                                                       "TimeStamp", 
                                                       "%d-%m-%Y / %H:%M:%S:%f") 
            << " -> " << expr::message;
        logging::add_file_log(file_name)->set_formatter(formatter);
        logging::add_console_log(std::clog)->set_formatter(formatter);
        logging::add_common_attributes();
    }
    bool WriteLog(const char *msg)
    {
        logging::record rec = lg.open_record();
        if(rec) {
            logging::record_ostream strm(rec);
            strm << msg;
            strm.flush();
            lg.push_record(boost::move(rec));
            return true;
        } 
        else { return false; }
    }
};















































#endif