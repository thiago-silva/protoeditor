perl_dbgp  = $(top_srcdir)/data/libs/perl_dbgp.tar.gz

EXTRA_DIST = perl_dbgp.tar.gz

install:
	mkdir -p $(perllibdir)
	tar xvfz $(perl_dbgp) -C $(perllibdir)

uninstall:
	rm -rf $(perllibdir)