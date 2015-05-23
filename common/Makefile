include ../Makefile.common
OUT_LIB_DIR		=	../lib

CFLAGS +=	-I$(OPENNI_INCLUDE_DIR) -I$(EIGEN_INC) -I$(PCL_INC) $(FIXME_INC)
LDFLAGS	+= $(PCL_LD_FLAGS)	$(OPENCV_LD_FLAGS)

all:	$(LIB_NAME)

$(LIB_NAME):	$(OBJ)
	$(CC) -shared -Wl,-soname,$(LIB_NAME) $^ -o $(OUT_LIB_DIR)/$@	$(LDFLAGS)
#	ldconfig -v -n $(OUT_LIB_DIR)
	ln -s --force $(OUT_LIB_DIR)/$(LIB_NAME) $(OUT_LIB_DIR)/lib$(NAME).so

$(OBJ_DIR)/%.o	: %.cpp
	$(CC)	$(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJ_DIR)/*.o	 
	rm -f $(OUT_LIB_DIR)/lib$(NAME).so
	rm -f $(OUT_LIB_DIR)/$(LIB_NAME)
	rm -f $(SRC_DIR)/*.*~
	rm -f $(COMMON_INC)/*.*~
	rm -f $(COMMON_IMPL)/*.*~
	rm -f $(COMMON_OPENCL_IMPL)/*.*~
	rm -f $(COMMON_INC_TYPE)/*.*~
