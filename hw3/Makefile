CXX = g++
CXXFLAGS = -std=c++11

shell: shell.o command.o builtins.o job.o
	$(CXX) $(CXXFLAGS) $^ -o $@
%.o: %.cpp %.h
	$(CXX) $(CXXFLAGS) -c $< -o $@
clean:
	rm *.o
	rm shell
