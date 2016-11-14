DBMAN  = dbman

INSTALL_DIR  = /usr/lib/nslsbp/
LD_CONF_FILE = nslsbp.conf

DBMAN_SO  := $(addprefix lib,$(addsuffix .so,$(DBMAN)))

CFLAGS  = -Wall -g -fpic
LDFLAGS = -shared

DBMAN_CFLAGS   = -I../module_gps_temp/include
DBMAN_LDFLAGS  = -L/usr/lib/x86_64-linux-gnu -lmysqlclient -lpthread


all: dbman

dbman: dbman.c
	@echo -n -e '---------: BUILDING $< -> $(DBMAN_SO) : '
	@gcc $(CFLAGS) $(DBMAN_CFLAGS) $< $(LDFLAGS) -o $(DBMAN_SO) $(DBMAN_LDFLAGS) && echo 'done.'

install:
	@mkdir -p $(INSTALL_DIR)
	@echo '---------: INSTALLING $(DBMAN_SO)'
	@cp $(DBMAN_SO) $(INSTALL_DIR) -u
	@touch /etc/ld.so.conf.d/$(LD_CONF_FILE)
	@chmod 664 /etc/ld.so.conf.d/$(LD_CONF_FILE)
	@echo "$(INSTALL_DIR)" > /etc/ld.so.conf.d/$(LD_CONF_FILE)
	@ldconfig

$(OBJDIR):
	@mkdir -p $(OBJDIR)


clean:
	@echo -n '---------: REMOVING objects... ' && rm $(OBJDIR) -r -f && echo 'done.'
	@echo -n '---------: REMOVING libraries... ' && \
	rm $(DBMAN_SO) -f && echo 'done.'
	@echo -n '---------: UNINSTALLING libraries (1)... ' && \
	sudo rm $(INSTALL_DIR) -r -f && echo 'done.'
	@echo -n '---------: UNINSTALLING libraries (2)... ' && \
	sudo rm /etc/ld.so.conf.d/$(LD_CONF_FILE) -f && sudo ldconfig && echo 'done.'
