.PHONY: clean httpserver httpclient

all : httpserver httpclient

httpserver :
	$(MAKE) -f Makefile.server

httpclient :
	$(MAKE) -f Makefile.client

clean :
	$(MAKE) -f Makefile.server clean
	$(MAKE) -f Makefile.client clean