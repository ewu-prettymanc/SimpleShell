###########################
#Colton Prettyman         #
#Universal c/c++ Makefile #
#April 2014 		  #
###########################

#####################VARIABLES#####################

EXE=ssh
CFLAGS= 
#-Wall -ansi -pedantic -ggdb 
LFLAGS=

#################### DO NOT CHANGE ################

CXXFLAGS=$(CFLAGS)
CC=gcc


######################LISTS########################

SRCS:=$(wildcard *.c)
OBJS:=$(patsubst %.c,%.o,$(SRCS))
DEPS:=$(patsubst %.c,%.d,$(SRCS))

######################RULES########################

$(EXE): $(DEPS) $(OBJS) 
	$(CC) $(CFLAGS) -o $(EXE) $(OBJS) $(LFLAGS)

# $< is whatever %.cpp was.   $@ is whatever %.d was
# sed stands for stream editor, >> means append
%.d: %.c
	$(CC) $(CFLAGS) -M $< > $@
	$(CC) $(CFLAGS) -M $< | sed s/\\.o/.d/ >> $@

# automatic rule that looks like the following...
# %.o: %.cpp
#	$(CC) $(CFLAGS) -c -o $@ $<



clean:
	rm -rf $(EXE) $(DEPS) $(OBJS) *~ core


-include $(DEPS)
