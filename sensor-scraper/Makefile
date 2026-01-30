CC      ?= $(CROSS_COMPILE)gcc
CFLAGS  ?= -g -Wall -Werror
LDFLAGS ?= -lpthread -lrt
RM      = rm -f

default: all

all: sensor-scraper

sensor-scraper: sensor-scraper.c
	$(CC) $(CFLAGS) -o sensor-scraper.o $(INCLUDES) $(LDFLAGS) sensor-scraper.c

driver: sensor-scraper.c
	$(CC) $(CFLAGS) -DUSE_AESD_CHAR_DEVICE -o sensor-scraper.o $(INCLUDES) $(LDFLAGS) sensor-scraper.c

clean veryclean:
	$(RM) sensor-scraper.o
	$(RM) $(LOG_FILE)
