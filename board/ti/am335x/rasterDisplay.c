/*
 * rasterDisplay.c
 *
 * Sample application for raster
 *
*/

/* Copyright (C) 2010 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 */

#include "soc_AM335x.h"
#include "interrupt.h"
#include "evmAM335x.h"
#include "raster.h"
#include "image.h"
#include "ecap.h"
#include "hsi2c.h"

#include <common.h>
#include <asm/cache.h>
#include <asm/omap_common.h>
#include <asm/io.h>
#include <asm/arch/cpu.h>
#include <asm/arch/ddr_defs.h>
#include <asm/arch/hardware.h>
#include <asm/arch/mmc_host_def.h>
#include <asm/arch/sys_proto.h>
#include <asm/arch/mem.h>
#include <asm/arch/nand.h>
#include <asm/arch/clock.h>
#include <linux/mtd/nand.h>
#include <nand.h>
#include <net.h>
#include <miiphy.h>
#include <netdev.h>
#include <spi_flash.h>
#include "common_def.h"
#include "pmic.h"
#include "tps65217.h"
#include <i2c.h>
#include <serial.h>

/******************************************************************************
**                      INTERNAL FUNCTION PROTOTYPES
*******************************************************************************/
static void LCDIsr(void);
static void SetUpLCD(void);
static void LCDAINTCConfigure(void);
/******************************************************************************
**              FUNCTION DEFINITIONS
******************************************************************************/
	
int fn_lcd_init(void)
{
    //IntMasterIRQEnable();

    //IntAINTCInit();

    //LCDAINTCConfigure();

    //LCDBackLightEnable();

    //UPDNPinControl();

#if (LSD_DEBUG > 0)
	printf("lierda debug:enter File=board/ti/am335x/rasterDisplay.c,Function=fn_lcd_init line1\n");	
#endif
	
    SetUpLCD();
  
    /* Configuring the base ceiling */
    RasterDMAFBConfig(SOC_LCDC_0_REGS, 
                      (unsigned int)image1,
                      (unsigned int)image1 + sizeof(image1) - 2,
                      0);

    RasterDMAFBConfig(SOC_LCDC_0_REGS, 
                      (unsigned int)image1,
                      (unsigned int)image1 + sizeof(image1) - 2,
                      1);

    /* Enable End of frame0/frame1 interrupt */
    RasterIntEnable(SOC_LCDC_0_REGS, RASTER_END_OF_FRAME0_INT |
                                     RASTER_END_OF_FRAME1_INT);

    /* Enable raster */
    RasterEnable(SOC_LCDC_0_REGS);

#if (LSD_DEBUG > 0)
	printf("lierda debug:enter File=board/ti/am335x/rasterDisplay.c,Function=fn_lcd_init line2\n");	
#endif

	
    while(1)
    {
    	LCDIsr();
    }
}

/*
** Configures raster to display image 
*/
static void SetUpLCD(void)
{
    /* Enable clock for LCD Module */ 
    LCDModuleClkConfig();

    LCDPinMuxSetup();

    /* 
    **Clock for DMA,LIDD and for Core(which encompasses
    ** Raster Active Matrix and Passive Matrix logic) 
    ** enabled.
    */
    RasterClocksEnable(SOC_LCDC_0_REGS);

    /* Disable raster */
    RasterDisable(SOC_LCDC_0_REGS);
    
    /* Configure the pclk */
    //RasterClkConfig(SOC_LCDC_0_REGS, 23040000, 192000000);
    RasterClkConfig(SOC_LCDC_0_REGS, 1024*600*60, 192000000);

    /* Configuring DMA of LCD controller */ 
    RasterDMAConfig(SOC_LCDC_0_REGS, RASTER_DOUBLE_FRAME_BUFFER,
                    RASTER_BURST_SIZE_16, RASTER_FIFO_THRESHOLD_8,
                    RASTER_BIG_ENDIAN_DISABLE);

    /* Configuring modes(ex:tft or stn,color or monochrome etc) for raster controller */
    RasterModeConfig(SOC_LCDC_0_REGS, RASTER_DISPLAY_MODE_TFT_UNPACKED,
                     RASTER_PALETTE_DATA, RASTER_COLOR, RASTER_RIGHT_ALIGNED);


     /* Configuring the polarity of timing parameters of raster controller */
    RasterTiming2Configure(SOC_LCDC_0_REGS, RASTER_FRAME_CLOCK_LOW |
                                            RASTER_LINE_CLOCK_LOW  |
                                            RASTER_PIXEL_CLOCK_HIGH|
                                            RASTER_SYNC_EDGE_RISING|
                                            RASTER_SYNC_CTRL_ACTIVE|
                                            RASTER_AC_BIAS_HIGH     , 0, 255);

    /* Configuring horizontal timing parameter */
    //RasterHparamConfig(SOC_LCDC_0_REGS, 800, 48, 40, 40);
	RasterHparamConfig(SOC_LCDC_0_REGS, 1024, 48, 40, 40);

    /* Configuring vertical timing parameters */
    //RasterVparamConfig(SOC_LCDC_0_REGS, 480, 3, 13, 29);
	RasterVparamConfig(SOC_LCDC_0_REGS, 600, 3, 13, 29);


    RasterFIFODMADelayConfig(SOC_LCDC_0_REGS, 128);

}


/*
** configures arm interrupt controller to generate raster interrupt 
*/
static void LCDAINTCConfigure(void)
{
    /* Register the ISR in the Interrupt Vector Table.*/
    IntRegister(SYS_INT_LCDCINT, LCDIsr);

    IntPrioritySet(SYS_INT_LCDCINT, 0, AINTC_HOSTINT_ROUTE_IRQ );

    /* Enable the System Interrupts for AINTC.*/
    IntSystemEnable(SYS_INT_LCDCINT);
}

/*
** For each end of frame interrupt base and ceiling is reconfigured 
*/
static void LCDIsr(void)
{
    unsigned int  status;
    static unsigned long cnt1=0;
    static unsigned long cnt2=0;

    status = RasterIntStatus(SOC_LCDC_0_REGS,RASTER_END_OF_FRAME0_INT_STAT |
                                             RASTER_END_OF_FRAME1_INT_STAT );

    status = RasterClearGetIntStatus(SOC_LCDC_0_REGS, status);   

    if (status & RASTER_END_OF_FRAME0_INT_STAT)
    {
	#if (LSD_DEBUG > 0)
	cnt1++;
	if(cnt1 >= 10)
	{
		printf("lierda debug: RASTER_END_OF_FRAME0_INT_STAT\n");
		cnt1 = 0;
	}
	#endif        
	RasterDMAFBConfig(SOC_LCDC_0_REGS, 
                          (unsigned int)image1,
                          (unsigned int)image1 + sizeof(image1) - 2,
                          0);
    }

    if(status & RASTER_END_OF_FRAME1_INT_STAT)
    {
	#if (LSD_DEBUG > 0)
	cnt2++;
	if(cnt2 >= 10)
	{
		printf("lierda debug: RASTER_END_OF_FRAME1_INT_STAT\n");
		cnt2 = 0;
	}
	#endif           
	RasterDMAFBConfig(SOC_LCDC_0_REGS, 
                          (unsigned int)image1,
                          (unsigned int)image1 + sizeof(image1) - 2,
                          1);
    }

    RasterEndOfInterrupt(SOC_LCDC_0_REGS, 0);
}

/***************************** End Of File ************************************/
