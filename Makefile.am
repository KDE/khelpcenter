SUBDIRS = kwelcome helpcenter kcmhelpcenter konitemhelp kwid

messages:
	for subdir in $(SUBDIRS); do \
	   echo "Making messages in $$subdir"; \
	   (cd $$subdir && $(MAKE) -f Makefile.am messages) || exit 1; \
	done
