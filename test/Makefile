TARGET = httpserver

INCLUDES = -I. \
	   -I../server

SOURCES = httpserver.cpp

OBJECTS := $(notdir $(SOURCES:.cpp=.o))

SHARED_OBJECTS := lynxhttp
SHARED_OBJECTS_HOME = ..
SHARED_OBJECTS_DIR = $(SHARED_OBJECTS_HOME)/Bin

all : $(TARGET)

$(TARGET) : $(SHARED_OBJECTS) $(OBJECTS)
	@echo Linking $@
	@g++ $(OBJECTS) -L$(SHARED_OBJECTS_DIR) -Wl,-rpath=$(SHARED_OBJECTS_DIR)  $(addprefix -l, $(SHARED_OBJECTS)) -o $(TARGET)

%.o : %.cpp
	@echo Compiling $<
	@g++ $(INCLUDES) -o $@ -c $<

lynxhttp :
	$(MAKE) --directory $(SHARED_OBJECTS_HOME)


clean:
	@rm -f *.o
	@rm -f $(TARGET)
	$(MAKE) clean --directory $(SHARED_OBJECTS_HOME)
