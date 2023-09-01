////////////////////////////////////////////////////////////////////////////////

/***
    IMPLEMENTATION:
    Function process_exception() for handling various exceptions 
    using a generalized function 
                                ***/


////////////////////////////////////////////////////////////////////////////////                                

#include "process_exception.h"


////////////////////////////////////////////////////////////////////////////////

template <typename T>
void print_code_exception(const T& e)
{
    using namespace std;
    auto c = e.code();
    cerr << "- category:\t" << c.category().name() << endl;
    cerr << "- value:\t" << c.value() << endl;
    cerr << "- msg:\t\t" << c.message()  << endl;
    cerr << "- def category:\t" 
         << c.default_error_condition().category().name() << endl;
    cerr << "- def value:\t" << c.default_error_condition().value() << endl;
    cerr << "- def msg:\t" << c.default_error_condition().message() << endl;
}


int process_exception(std::exception_ptr ep)
{
    using namespace std;
    // Re-generating the exception to handle it here:
    try {
        if(ep != nullptr) {
            std::rethrow_exception(ep);
        }
    }
    catch(const TcpServerFault& e) {
        cerr << "TCP-SERVER FAULT, " << e.what()  << endl;
    }
    catch(const ios_base::failure& e) {
        cerr << "I/O EXCEPTION, " << e.what()  << endl;
        print_code_exception(e);
    }
    catch(const system_error& e) {
        cerr << "SYSTEM EXCEPTION,  " << e.what()  << endl;
        print_code_exception(e);
    }
    catch(const future_error& e) {
        cerr << "FUTURE EXCEPTION, " << e.what()  << endl;
        print_code_exception(e);
    }
    catch(const bad_alloc& e) {
        cerr << "BAD ALLOC EXCEPTION, " << e.what()  << endl;
    }
    catch(const exception& e) {
        cerr << "EXCEPTION, " << e.what()  << endl;
    }
    catch(...) {
        cerr << "EXCEPTION (unknown)" << endl;
    }
    return 1;
}


////////////////////////////////////////////////////////////////////////////////
