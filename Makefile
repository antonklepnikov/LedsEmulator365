CXXFLAGS = -s -O2 -DNDEBUG -pedantic-errors -Wall -Weffc++ -Wextra -Wsign-conversion -Werror -std=c++20 -I ./h -I /usr/local/include

fastled_port.o: fastled_port.cpp ./h/fastled_port.h
	$(CXX) $(CXXFLAGS) -c $< -o $@


led_core.o: led_core.cpp ./h/led_core.h ./h/common.h ./h/fastled_port.h ./h/oofl.h ./h/timer.h
	$(CXX) $(CXXFLAGS) -c $< -o $@


led_gui.o: led_gui.cpp ./h/led_gui.h ./h/common.h ./h/led_core.h ./h/oofl.h ./h/main_loop.h
	$(CXX) $(CXXFLAGS) -c $< -o $@


srv_logger.o: srv_logger.cpp ./h/srv_logger.h
	$(CXX) $(CXXFLAGS) -c $< -o $@ -lboost_log -lboost_thread


tcp_srv.o: tcp_srv.cpp ./h/tcp_srv.h ./h/common.h ./h/srv_logger.h
	$(CXX) $(CXXFLAGS) -c $< -o $@


main_loop.o: main_loop.cpp ./h/main_loop.h ./h/common.h ./h/fastled_port.h ./h/led_core.h ./h/oofl.h ./h/fastled_port.h ./h/tcp_srv.h
	$(CXX) $(CXXFLAGS) -c $< -o $@


process_exception.o: process_exception.cpp ./h/process_exception.h ./h/tcp_srv.h
	$(CXX) $(CXXFLAGS) -c $< -o $@


build: main.cpp fastled_port.o led_core.o led_gui.o srv_logger.o tcp_srv.o main_loop.o process_exception.o ./h/common.h ./h/led_core.h ./h/tcp_srv.h ./h/led_gui.h ./h/main_loop.h ./h/process_exception.h 
	$(CXX) $(CXXFLAGS) main.cpp fastled_port.o led_core.o led_gui.o srv_logger.o tcp_srv.o main_loop.o process_exception.o -o le365r -lfltk -lX11 -lboost_log -lboost_thread


clean:
	rm -rf *.o
