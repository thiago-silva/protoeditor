perl_dbgp   = $(top_srcdir)/data/libs/perl_dbgp.tar.gz
python_dbgp = $(top_srcdir)/data/libs/python_dbgp.tar.gz

EXTRA_DIST = perl_dbgp.tar.gz python_dbgp.tar.gz

install:
	mkdir -p $(perllibdir)
	tar xfz $(perl_dbgp) -C $(perllibdir)
	mkdir -p $(pythonlibdir)
	tar xfz $(python_dbgp) -C $(pythonlibdir)

uninstall:
	rm -rf $(perllibdir)
	rm -rf $(pythonlibdir)