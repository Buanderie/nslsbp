
OBJDIR := obj
LIBDIR := lib
OBJS   := $(addprefix $(OBJDIR)/,xlauncher.o)

CC1101INCLUDE = -I./module_beacon/cc1101_handler/include
CC1101LIBRARY = -L./module_beacon/cc1101_handler/lib

CFLAGS  = -Wall -DXLAUNCHER_DEBUG $(CC1101INCLUDE)
LDFLAGS = -lcc_beacon_iface_wrapper -L./$(LIBDIR)

BLIBSEXTRA = $(CC1101INCLUDE) -lcc_beacon_iface $(CC1101LIBRARY)
BLIBSCFLAGS = -Wall -g -fpic #-DCC_BEACON_IFACE_WRAPPER_DEBUG


all: beaconlibs xlauncher

$(OBJDIR)/%.o : %.c
	@echo -n -e '---------: COMPILING $< -> $@ : '
	@gcc -c $< -o $@ $(CFLAGS) && echo 'done.'

xlauncher: $(OBJS) | $(OBJDIR)
	@echo -n -e '---------: LINKING $< -> $@ : '
	@gcc $(OBJS) -o $@ $(LDFLAGS) && echo 'done.'

$(OBJS): | $(OBJDIR)

beaconlibs: cc_beacon_iface_wrapper.c
	@mkdir -p $(LIBDIR)
	@echo -n -e '---------: BUILDING $< -> $(LIBDIR)/libcc_beacon_iface_wrapper.so : '
	@gcc $(BLIBSCFLAGS) $< -shared -o $(LIBDIR)/libcc_beacon_iface_wrapper.so $(BLIBSEXTRA) && echo 'done.'

install:
	@echo '---------: INSTALLING libcc_beacon_iface_wrapper.so.'
	@sudo mkdir -p /usr/lib
	@sudo cp $(LIBDIR)/libcc_beacon_iface_wrapper.so /usr/lib -u
	@sudo ldconfig

$(OBJDIR):
	@mkdir -p $(OBJDIR)


clean:
	@echo -n '---------: REMOVING binaries... ' && rm xlauncher -f && echo 'done.'
	@echo -n '---------: REMOVING objects... ' && rm $(OBJDIR) -r -f && echo 'done.'
	@echo -n '---------: REMOVING libaries... ' && rm $(LIBDIR) -r -f && echo 'done.'
