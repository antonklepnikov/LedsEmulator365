#include "srv_logger.h"

static const char SERVER_LOG_FILE_NAME[] = "sl_test.log";

int main()
{
    SrvLogger sl(SERVER_LOG_FILE_NAME);
    sl.WriteLog("This is SL test recrod.");
    return 0;
}