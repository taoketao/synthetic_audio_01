CXX=g++
INCLUDES=

UNAME := $(shell uname)

ifeq ($(UNAME), Linux)
FLAGS=-D__UNIX_JACK__ -c
LIBS=-lasound -lpthread -ljack -lstdc++ -lm
endif
ifeq ($(UNAME), Darwin)
FLAGS=-D__MACOSX_CORE__ -c
LIBS=-framework CoreAudio -framework CoreMIDI -framework CoreFoundation \
	-framework IOKit -framework Carbon -lstdc++ -lm
endif


OBJS=   RtAudio.o Log.o

Log: $(OBJS)
	$(CXX) -o Log $(OBJS) $(LIBS)

Log.o: Log.cpp RtAudio.h
	$(CXX) $(FLAGS) Log.cpp

RtAudio.o: RtAudio.h RtAudio.cpp RtError.h
	$(CXX) $(FLAGS) RtAudio.cpp

clean:
	rm -f *~ *# *.o Log
