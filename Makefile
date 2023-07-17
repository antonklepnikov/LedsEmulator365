CXXFLAGS = -ggdb -pedantic-errors -Wall -Weffc++ -Wextra -Wsign-conversion -Werror -std=c++20 -I ./h


fastled_port.o: fastled_port.cpp ./h/fastled_port.h
	$(CXX) $(CXXFLAGS) -c $< -o $@


led_core.o: led_core.cpp ./h/led_core.h ./h/common.h ./h/fastled_port.h ./h/oofl.h ./h/timer.h
	$(CXX) $(CXXFLAGS) -c $< -o $@


led_gui.o: led_gui.cpp ./h/led_gui.h ./h/common.h ./h/led_core.h ./h/oofl.h ./h/selector.h
	$(CXX) $(CXXFLAGS) -c $< -o $@


selector.o: selector.cpp ./h/selector.h ./h/common.h ./h/fastled_port.h ./h/led_core.h ./h/oofl.h ./h/fastled_port.h
	$(CXX) $(CXXFLAGS) -c $< -o $@


process_exception.o: process_exception.cpp ./h/process_exception.h
	$(CXX) $(CXXFLAGS) -c $< -o $@


build: main.cpp fastled_port.o led_core.o led_gui.o process_exception.o selector.o ./h/led_gui.h ./h/led_core.h ./h/process_exception.h
	$(CXX) $(CXXFLAGS) main.cpp fastled_port.o led_core.o led_gui.o process_exception.o selector.o -o le365 -lfltk -lX11


clean:
	rm -rf *.o
