
OBJDIR := obj
OBJS  := $(addprefix $(OBJDIR)/,xlauncher.o)

CFLAGS  = -Wall -DXLAUNCHER_DEBUG
LDFLAGS =

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
