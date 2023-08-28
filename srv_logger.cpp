////////////////////////////////////////////////////////////////////////////////

/***
     IMPLEMENTATION:
     Logger for the LE365 TCP-server
                                     ***/


////////////////////////////////////////////////////////////////////////////////

#include "srv_logger.h"


////////////////////////////////////////////////////////////////////////////////

SrvLogger::SrvLogger(const char *file_name) : lg()
{
    logging::formatter formatter = expr::stream
        << expr::format_date_time<bptm::ptime>("TimeStamp", 
                                               "%d-%m-%Y / %H:%M:%S:%f") 
        << " -> " << expr::message;
    logging::add_file_log(file_name)->set_formatter(formatter);
    logging::add_console_log(std::clog)->set_formatter(formatter);
    logging::add_common_attributes();
}

bool SrvLogger::WriteLog(const char *msg)
{
    logging::record rec = lg.open_record();
    if(rec) {
        logging::record_ostream strm(rec);
        strm << msg;
        strm.flush();
        lg.push_record(boost::move(rec));
        return true;
    }  else { return false; }
}