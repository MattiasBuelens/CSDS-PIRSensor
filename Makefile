CONTIKI_PROJECT = pirSensor
CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"
include $(LOOCI)/lnk/lc_contiki/core/Makefile.component

