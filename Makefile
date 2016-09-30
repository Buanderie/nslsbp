
OBJDIR := obj
OBJS   := $(addprefix $(OBJDIR)/,xlauncher.o)

INCLUDES  = -I./libraries -I./module_beacon/cc1101_handler/include
LIBRARIES = -L./libraries

CFLAGS  = -Wall -DXLAUNCHER_DEBUG $(INCLUDES)
LDFLAGS = -lcc_beacon_iface_wrapper $(LIBRARIES)


all: xlauncher

$(OBJDIR)/%.o : %.c
	@echo -n -e '---------: COMPILING $< -> $@ : '
	@gcc -c $< -o $@ $(CFLAGS) && echo 'done.'

xlauncher: $(OBJS) | $(OBJDIR)
	@echo -n -e '---------: LINKING $< -> $@ : '
	@gcc $(OBJS) -o $@ $(LDFLAGS) && echo 'done.'

$(OBJS): | $(OBJDIR)

$(OBJDIR):
	@mkdir -p $(OBJDIR)


clean:
	@echo -n '---------: REMOVING binaries... ' && rm xlauncher -f && echo 'done.'
	@echo -n '---------: REMOVING objects... ' && rm $(OBJDIR) -r -f && echo 'done.'

install:
	@echo -n '---------: INSTALLING binaries: ' && ln -s xlauncher /usr/bin/xlauncher && echo 'done.'