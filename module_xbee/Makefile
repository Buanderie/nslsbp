XBEE  = xbee_lib

OBJDIR := obj

CFLAGS  = -Wall
LDFLAGS = 

OBJSGS := $(addprefix $(OBJDIR)/,xbee_lib.o)

XBEE_CFLAGS   = -I../module_gps_temp/include -I../libraries
XBEE_LDFLAGS  = -L../libraries -ldbman

all: xbee_lib

xbee_lib: xbee_lib.c
	@echo -n -e '---------: BUILDING $< -> $(OBJSGS) : '
	@gcc $(CFLAGS) $(XBEE_CFLAGS) $< -o $@ $(LDFLAGS) $(XBEE_LDFLAGS) && echo 'done.'

$(OBJDIR):
	@mkdir -p $(OBJDIR)

clean:
	@echo -n '---------: REMOVING objects... ' && rm $(OBJDIR) -r -f && echo 'done.'
	@echo -n '---------: REMOVING libraries... ' && \
	rm $(OBJSGS) -f && \
	rm $(XBEE) -f && echo 'done.'
