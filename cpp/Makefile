# RingMark-Decoder algo app
# (c) Shahar Gino, May-2020, sgino209@gmail.com

AR		:=	ar
ifeq ($(shell uname -s), Darwin)
CXX		:=	clang++
CXXFLAGS	:=	-std=c++11 -stdlib=libc++ -fpic -O3 -Wall -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS -DNO_HISTOGRAM -Wno-implicit-function-declaration \
                        -Wno-reorder -Wno-overloaded-virtual -Wno-null-conversion -Wno-delete-non-virtual-dtor -Wno-comment -Wno-format -Wno-char-subscripts -Wno-unused-function
else
CXX		:=	g++
CXXFLAGS	:=	-std=c++11 -stdlib=libc++ -fpic -O3 -Wall -DLUA_ENABLED -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS -DNO_HISTOGRAM -Wno-implicit-function-declaration \
                        -Wno-reorder -Wno-overloaded-virtual -Wno-null-conversion -Wno-delete-non-virtual-dtor -Wno-comment -Wno-format -Wno-char-subscripts -Wno-unused-function
endif

LDFLAGS		:= 	-L/usr/local/lib -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_imgcodecs -lopencv_ml -lopencv_video -lcurses 

INC		:= 	-I/usr/local/lib \
    			-I/usr/local/include \
    			-I/usr/local/include/opencv4 \
			-I~/opencv/** \
			-I./

MODULES		:=	mainscan \
			auxiliary

LIB_NAME	:=	ringmark_decoder.a

EXEC_NAME	:=	ringmark_decoder

BUILD		:=	build

EXECUTABLE	:=	$(BUILD)/$(EXEC_NAME)

LIB		:=	$(BUILD)/$(LIB_NAME)

OBJ		:=	$(patsubst %,$(BUILD)/%.o,$(MODULES))


$(EXECUTABLE) : $(LIB)
	$(CXX) $(LIB) $(LDFLAGS) -o $@

$(LIB) : $(BUILD) $(OBJ)
	$(AR) rcu $(LIB) $(OBJ)

$(BUILD)/%.o : %.cpp
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

$(BUILD):
	mkdir -p $(BUILD)

.PHONY: clean
clean:
	rm -f $(LIB)
	rm -f $(OBJ)

.PHONY: clobber
clobber:
	rm -rf $(BUILD)

