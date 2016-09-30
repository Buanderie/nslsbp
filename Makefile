
OBJDIR := obj
OBJSXL := $(addprefix $(OBJDIR)/,xlauncher.o)
OBJSGS := $(addprefix $(OBJDIR)/,ground_station.o)

INCLUDES  = -I./libraries -I./module_beacon/cc1101_handler/include
LIBRARIES = -L./libraries -L./module_beacon/cc1101_handler/lib

CFLAGS  = -Wall -DXLAUNCHER_DEBUG -DGROUND_STATION_DEBUG -DFAKE_BEACON_MSG $(INCLUDES)
LDFLAGS = -ldbman -lcc_beacon_iface_wrapper -lcc_beacon_iface $(LIBRARIES)


all: xlauncher ground_station

$(OBJDIR)/%.o : %.c
	@echo -n -e '---------: COMPILING $< -> $@ : '
	@gcc -c $< -o $@ $(CFLAGS) && echo 'done.'

xlauncher: $(OBJSXL) | $(OBJDIR)
	@echo -n -e '---------: LINKING $< -> $@ : '
	@gcc $(OBJSXL) -o $@ $(LDFLAGS) && echo 'done.'

ground_station: $(OBJSGS) | $(OBJDIR)
	@echo -n -e '---------: LINKING $< -> $@ : '
	@gcc $(OBJSGS) -o $@ $(LDFLAGS) && echo 'done.'

$(OBJSGS): | $(OBJDIR)

$(OBJSXL): | $(OBJDIR)

$(OBJDIR):
	@mkdir -p $(OBJDIR)


clean:
	@echo -n '---------: REMOVING binaries... ' && rm xlauncher -f && echo 'done.'
	@echo -n '---------: REMOVING objects... ' && rm $(OBJDIR) -r -f && echo 'done.'

install:
	@echo -n '---------: INSTALLING binaries: ' && ln -s xlauncher /usr/bin/xlauncher && echo 'done.'
