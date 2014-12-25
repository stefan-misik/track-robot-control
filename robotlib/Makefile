#compilers
CC=gcc

#sources list
SRC=main.c robot.c
OUT=robot
PKG=

#flags
CFLAGS=-c -Wall
LDFLAGS=

#debug options
DEBUGFLAG=-ggdb

##############################################################

#CFLAGS +=$(shell pkg-config --cflags $(PKG))
#LDFLAGS +=$(shell pkg-config --libs $(PKG))

ifeq ($(DBG), y)
  CFLAGS +=$(DEBUGFLAG)
  LDFLAGS +=$(DEBUGFLAG)
  DBGINFO=debug
  _OUT=$(addprefix dbg_,$(OUT))
  OBJ=$(addprefix dbg_,$(SRC:.c=.o))
else  
  DBGINFO=release
  _OUT=$(OUT)
  OBJ=$(SRC:.c=.o)
endif

all: onstart $(SRC) $(_OUT)	
	
$(_OUT): $(OBJ)	
	$(CC) $(LDFLAGS) $(OBJ) -o $@

%.o: %.c	
	$(CC) $(CFLAGS) $< -o $@

$(addprefix dbg_,%.o): %.c	
	$(CC) $(CFLAGS) $< -o $@

.PHONY: onstart clean install

clean:
	rm -rf $(OBJ) $(OUT) $(addprefix dbg_, $(OBJ)) $(addprefix dbg_, $(OUT)) 

