BEACON = cc_beacon_iface_wrapper
DBMAN  = dbman

INSTALL_DIR  = /usr/lib/bsx/
LD_CONF_FILE = barbie_space_explorer.conf

BEACON_SO := $(addprefix lib,$(addsuffix .so,$(BEACON)))
DBMAN_SO  := $(addprefix lib,$(addsuffix .so,$(DBMAN)))

CFLAGS  = -Wall -g -fpic
LDFLAGS = -shared

BEACON_LDFLAGS = -lcc_beacon_iface -L../module_beacon/cc1101_handler/lib
BEACON_CFLAGS  = -I../module_beacon/cc1101_handler/include #-DCC_BEACON_IFACE_WRAPPER_DEBUG

DBMAN_CFLAGS   = -I/usr/local/mysql/include
DBMAN_LDFLAGS  = -L/usr/local/mysql/lib -lmysqlclient -lpthread



all: beacon dbman

beacon: cc_beacon_iface_wrapper.c
	@echo -n -e '---------: BUILDING $< -> $(BEACON_SO) : '
	@gcc $(CFLAGS) $(BEACON_CFLAGS) $< $(LDFLAGS) -o $(BEACON_SO) $(BEACON_LDFLAGS) && echo 'done.'

dbman: dbman.c
	@echo -n -e '---------: BUILDING $< -> $(DBMAN_SO) : '
	@gcc $(CFLAGS) $(DBMAN_CFLAGS) $< $(LDFLAGS) -o $(DBMAN_SO) $(DBMAN_LDFLAGS) && echo 'done.'

install:
	@mkdir -p $(INSTALL_DIR)
	@echo '---------: INSTALLING $(BEACON_SO)'
	@cp $(BEACON_SO) $(INSTALL_DIR) -u
	@echo '---------: INSTALLING $(DBMAN_SO)'
	@cp $(DBMAN_SO) $(INSTALL_DIR) -u
	@touch /etc/ld.so.conf.d/$(LD_CONF_FILE)
	@chmod 664 /etc/ld.so.conf.d/$(LD_CONF_FILE)
	@echo "$(INSTALL_DIR)" > /etc/ld.so.conf.d/$(LD_CONF_FILE)
	@ldconfig

$(OBJDIR):
	@mkdir -p $(OBJDIR)


clean:
	@echo -n '---------: REMOVING binaries... ' && rm xlauncher -f && echo 'done.'
	@echo -n '---------: REMOVING objects... ' && rm $(OBJDIR) -r -f && echo 'done.'
	@echo -n '---------: REMOVING libraries... ' && \
	rm $(BEACON_SO) $(DBMAN_SO) -f && echo 'done.'
	@echo -n '---------: UNINSTALLING libraries (1)... ' && \
	sudo rm $(INSTALL_DIR) -r -f && echo 'done.'
	@echo -n '---------: UNINSTALLING libraries (2)... ' && \
	sudo rm /etc/ld.so.conf.d/$(LD_CONF_FILE) -f && sudo ldconfig && echo 'done.'
