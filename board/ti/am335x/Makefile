#
# Makefile
#
# Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation version 2.
#
# This program is distributed "as is" WITHOUT ANY WARRANTY of any
# kind, whether express or implied; without even the implied warranty
# of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#

include $(TOPDIR)/config.mk

LIB	= $(obj)lib$(BOARD).o

#COBJS	:= evm.o pll.o mux.o lcd.o raster.o rasterDisplay.o logo_linux_clut224_800_480.o
#COBJS	:= evm.o pll.o mux.o lcd.o raster.o rasterDisplay.o logo_linux_clut224_1024_600.o
#COBJS	:= evm.o pll.o mux.o lcd.o raster.o rasterDisplay.o logo_linux_clut224_1024_768.o
#COBJS	:= evm.o pll.o mux.o lcd.o raster.o rasterDisplay.o logo_linux_clut224_1280_800.o
COBJS	:= evm.o pll.o mux.o lcd.o raster.o rasterDisplay.o logo_linux_clut224_800x600.o
	
SRCS	:= $(SOBJS:.o=.S) $(COBJS:.o=.c)
OBJS	:= $(addprefix $(obj),$(COBJS))

$(LIB):	$(OBJS)
	$(call cmd_link_o_target, $(OBJS))

clean:
	rm -f $(OBJS)

distclean:	clean
	rm -f $(LIB) core *.bak $(obj).depend

#########################################################################

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

#########################################################################
