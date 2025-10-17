CXX = g++
CXXFLAGS = -Wall -g
LDFLAGS = -lreadline -lhistory

TARGET = myshell

all: $(TARGET)

$(TARGET): main.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

main.o: main.cpp autocomplete.h changedir.h foreback.h history.h pinfo.h search.h signal_handle.h showlist.h pipeline.h
	$(CXX) $(CXXFLAGS) -c main.cpp

clean:
	rm -f $(TARGET) main.o