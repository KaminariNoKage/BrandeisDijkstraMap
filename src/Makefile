CXXFLAGS =	-O2 -g -Wall -fmessage-length=0

OBJS =		BrandeisMap.o

LIBS =

TARGET =	BrandeisMap.exe

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
