NAME = lynxtest

INCLUDES = -I. \
	   -I../server \
	   -I../client

SOURCES = utils.cpp \
          test_server.cpp

BIN_DIR = Bin

TARGET = $(BIN_DIR)/$(NAME)

OBJECTS := $(addprefix $(BIN_DIR)/, $(notdir $(SOURCES:.cpp=.o)))

LIBS = -L/usr/local/lib/ -lboost_system -lpthread -lboost_thread -lssl -lcrypto -lboost_unit_test_framework

FLAGS = -g

SHARED_OBJECTS := lynxhttp
SHARED_OBJECTS_HOME = ../server
SHARED_OBJECTS_DIR = $(SHARED_OBJECTS_HOME)/Bin

all : $(TARGET)

$(TARGET) : $(BIN_DIR) $(SHARED_OBJECTS) $(OBJECTS)
	@echo Linking $@
	@g++ $(OBJECTS) -L$(SHARED_OBJECTS_DIR) -Wl,-rpath=$(SHARED_OBJECTS_DIR)  $(addprefix -l, $(SHARED_OBJECTS)) $(LIBS) -o $(TARGET)

$(BIN_DIR) :
	@echo Creating $(BIN_DIR) directory
	@if [ ! -d $(BIN_DIR) ]; then mkdir $(BIN_DIR); fi

$(BIN_DIR)/%.o : %.cpp
	@echo Compiling $<
	@g++ $(INCLUDES) $(FLAGS) -o $@ -c $<

lynxhttp :
	$(MAKE) --directory $(SHARED_OBJECTS_HOME)


clean:
	@rm -rf $(BIN_DIR)
	$(MAKE) clean --directory $(SHARED_OBJECTS_HOME)
