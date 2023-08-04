NAME = lynxhttp

INCLUDES = -I. \
           -Iserver

LIBS = -L/usr/local/lib/ -lboost_system

VPATH := server

FLAGS = -fPIC

SOURCES = lynxhttp_server.cpp

BIN_DIR = ./Bin

TARGET = $(BIN_DIR)/lib$(NAME).so

OBJECTS := $(addprefix $(BIN_DIR)/, $(notdir $(SOURCES:.cpp=.o)))

$(TARGET) : $(BIN_DIR) $(OBJECTS)
	@echo Linking $@
	@g++ -shared $(OBJECTS) $(LIBS) -o $@

$(BIN_DIR) :
	@echo Creating $(BIN_DIR) directory
	@if [ ! -d $(BIN_DIR) ]; then mkdir $(BIN_DIR); fi


$(BIN_DIR)/%.o : %.cpp
	@echo Compiling $<
	@g++ $(FLAGS) $(INCLUDES) -o $@ -c $<


clean:
	@rm -rf $(BIN_DIR)
