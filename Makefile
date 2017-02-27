
OBJDIR := obj
OBJSXL := $(addprefix $(OBJDIR)/,xlauncher.o)
#OBJSGS := $(addprefix $(OBJDIR)/,ground_station.o)

INCLUDES  = -I./libraries -I./module_gps_temp/include
LIBRARIES = -L./libraries

CFLAGS  = -Wall -DXLAUNCHER_DEBUG -DGROUND_STATION_DEBUG -DFAKE_BEACON_MSG $(INCLUDES)
LDFLAGS = -ldbman -pthread -lncurses -lm $(LIBRARIES)


all: xlauncher

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
