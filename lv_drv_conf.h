/**
 * @file lv_drv_conf.h
 * Configuration file for v8.3.0-dev
 */

/*
 * COPY THIS FILE AS lv_drv_conf.h
 */

/* clang-format off */
#if 1 /*Set it to "1" to enable the content*/

#ifndef LV_DRV_CONF_H
#define LV_DRV_CONF_H

#include "lib/lv_conf.h"
#include "hardware/spi.h"
#include "pico/stdlib.h"

/*********************
 * DELAY INTERFACE
 *********************/
#define LV_DRV_DELAY_INCLUDE  <stdint.h>            /*Dummy include by default*/
#define LV_DRV_DELAY_US(us)  /*delay_us(us)*/       /*Delay the given number of microseconds*/
#define LV_DRV_DELAY_MS(ms)  /*delay_ms(ms)*/       /*Delay the given number of milliseconds*/

/*********************
 * DISPLAY INTERFACE
 *********************/

/*------------
 *  Common
 *------------*/
#define LV_DRV_DISP_INCLUDE         <stdint.h>           /*Dummy include by default*/
#define LV_DRV_DISP_CMD_DATA(val)  /*pin_x_set(val)*/    /*Set the command/data pin to 'val'*/
#define LV_DRV_DISP_RST(val)       /*pin_x_set(val)*/    /*Set the reset pin to 'val'*/

/*---------
 *  SPI
 *---------*/
#define DISP_CS   10
#define DISP_SCK  18
#define DISP_MOSI 19

#define DISP_SPI_PORT spi0

#define LV_DRV_DISP_SPI_CS(val)          gpio_put(DISP_CS, val);/*spi_cs_set(val)*/     /*Set the SPI's Chip select to 'val'*/
#define LV_DRV_DISP_SPI_WR_BYTE(data)    spi_write_blocking(DISP_SPI_PORT, data, 1) /*spi_wr(data)*/        /*Write a byte the SPI bus*/
#define LV_DRV_DISP_SPI_WR_ARRAY(adr, n) spi_write_blocking(DISP_SPI_PORT, adr, n)/*spi_wr_mem(adr, n)*/  /*Write 'n' bytes to SPI bus from 'adr'*/

/*------------------
 *  Parallel port
 *-----------------*/
#define LV_DRV_DISP_PAR_CS(val)          /*par_cs_set(val)*/   /*Set the Parallel port's Chip select to 'val'*/
#define LV_DRV_DISP_PAR_SLOW             /*par_slow()*/        /*Set low speed on the parallel port*/
#define LV_DRV_DISP_PAR_FAST             /*par_fast()*/        /*Set high speed on the parallel port*/
#define LV_DRV_DISP_PAR_WR_WORD(data)    /*par_wr(data)*/      /*Write a word to the parallel port*/
#define LV_DRV_DISP_PAR_WR_ARRAY(adr, n) /*par_wr_mem(adr,n)*/ /*Write 'n' bytes to Parallel ports from 'adr'*/

/***************************
 * INPUT DEVICE INTERFACE
 ***************************/

/*----------
 *  Common
 *----------*/
#define LV_DRV_INDEV_INCLUDE     <stdint.h>             /*Dummy include by default*/
#define LV_DRV_INDEV_RST(val)    /*pin_x_set(val)*/     /*Set the reset pin to 'val'*/
#define LV_DRV_INDEV_IRQ_READ    0 /*pn_x_read()*/      /*Read the IRQ pin*/

/*---------
 *  SPI
 *---------*/
#define LV_DRV_INDEV_SPI_CS(val)            /*spi_cs_set(val)*/     /*Set the SPI's Chip select to 'val'*/
#define LV_DRV_INDEV_SPI_XCHG_BYTE(data)    0 /*spi_xchg(val)*/     /*Write 'val' to SPI and give the read value*/

/*---------
 *  I2C
 *---------*/
#define LV_DRV_INDEV_I2C_START              /*i2c_start()*/       /*Make an I2C start*/
#define LV_DRV_INDEV_I2C_STOP               /*i2c_stop()*/        /*Make an I2C stop*/
#define LV_DRV_INDEV_I2C_RESTART            /*i2c_restart()*/     /*Make an I2C restart*/
#define LV_DRV_INDEV_I2C_WR(data)           /*i2c_wr(data)*/      /*Write a byte to the I1C bus*/
#define LV_DRV_INDEV_I2C_READ(last_read)    0 /*i2c_rd()*/        /*Read a byte from the I2C bud*/


/*********************
 *  DISPLAY DRIVERS
 *********************/

/*-------------------
 *  SDL
 *-------------------*/

/* SDL based drivers for display, mouse, mousewheel and keyboard*/
#ifndef USE_SDL
# define USE_SDL 0
#endif

/* Hardware accelerated SDL driver */
#ifndef USE_SDL_GPU
# define USE_SDL_GPU 0
#endif

#if USE_SDL || USE_SDL_GPU
#  define SDL_HOR_RES     480
#  define SDL_VER_RES     320

/* Scale window by this factor (useful when simulating small screens) */
#  define SDL_ZOOM        1

/* Used to test true double buffering with only address changing.
 * Use 2 draw buffers, bith with SDL_HOR_RES x SDL_VER_RES size*/
#  define SDL_DOUBLE_BUFFERED 0

/*Eclipse: <SDL2/SDL.h>    Visual Studio: <SDL.h>*/
#  define SDL_INCLUDE_PATH    <SDL2/SDL.h>

/*Open two windows to test multi display support*/
#  define SDL_DUAL_DISPLAY            0
#endif

/*-------------------
 *  Monitor of PC
 *-------------------*/

/*DEPRECATED: Use the SDL driver instead. */
#ifndef USE_MONITOR
#  define USE_MONITOR         0
#endif

#if USE_MONITOR
#  define MONITOR_HOR_RES     480
#  define MONITOR_VER_RES     320

/* Scale window by this factor (useful when simulating small screens) */
#  define MONITOR_ZOOM        1

/* Used to test true double buffering with only address changing.
 * Use 2 draw buffers, bith with MONITOR_HOR_RES x MONITOR_VER_RES size*/
#  define MONITOR_DOUBLE_BUFFERED 0

/*Eclipse: <SDL2/SDL.h>    Visual Studio: <SDL.h>*/
#  define MONITOR_SDL_INCLUDE_PATH    <SDL2/SDL.h>

/*Open two windows to test multi display support*/
#  define MONITOR_DUAL            0
#endif

/*-----------------------------------
 *  Native Windows (including mouse)
 *----------------------------------*/
#ifndef USE_WINDOWS
#  define USE_WINDOWS       0
#endif

#if USE_WINDOWS
#  define WINDOW_HOR_RES      480
#  define WINDOW_VER_RES      320
#endif

/*----------------------------
 *  Native Windows (win32drv)
 *---------------------------*/
#ifndef USE_WIN32DRV
#  define USE_WIN32DRV       0
#endif

#if USE_WIN32DRV
/* Scale window by this factor (useful when simulating small screens) */
#  define WIN32DRV_MONITOR_ZOOM        1
#endif

/*----------------------------------------
 *  GTK drivers (monitor, mouse, keyboard
 *---------------------------------------*/
#ifndef USE_GTK
#  define USE_GTK       0
#endif

/*----------------------------------------
 *  Wayland drivers (monitor, mouse, keyboard, touchscreen)
 *---------------------------------------*/
#ifndef USE_WAYLAND
#  define USE_WAYLAND       0
#endif

#if USE_WAYLAND
/* Support for client-side decorations */
#  ifndef LV_WAYLAND_CLIENT_SIDE_DECORATIONS
#    define LV_WAYLAND_CLIENT_SIDE_DECORATIONS 1
#  endif
/* Support for (deprecated) wl-shell protocol */
#  ifndef LV_WAYLAND_WL_SHELL
#    define LV_WAYLAND_WL_SHELL 1
#  endif
/* Support for xdg-shell protocol */
#  ifndef LV_WAYLAND_XDG_SHELL
#    define LV_WAYLAND_XDG_SHELL 0
#  endif
#endif

/*----------------
 *    SSD1963
 *--------------*/
#ifndef USE_SSD1963
#  define USE_SSD1963         0
#endif

#if USE_SSD1963
#  define SSD1963_HOR_RES     LV_HOR_RES
#  define SSD1963_VER_RES     LV_VER_RES
#  define SSD1963_HT          531
#  define SSD1963_HPS         43
#  define SSD1963_LPS         8
#  define SSD1963_HPW         10
#  define SSD1963_VT          288
#  define SSD1963_VPS         12
#  define SSD1963_FPS         4
#  define SSD1963_VPW         10
#  define SSD1963_HS_NEG      0   /*Negative hsync*/
#  define SSD1963_VS_NEG      0   /*Negative vsync*/
#  define SSD1963_ORI         0   /*0, 90, 180, 270*/
#  define SSD1963_COLOR_DEPTH 16
#endif

/*----------------
 *    R61581
 *--------------*/
#ifndef USE_R61581
#  define USE_R61581          0
#endif

#if USE_R61581
#  define R61581_HOR_RES      LV_HOR_RES
#  define R61581_VER_RES      LV_VER_RES
#  define R61581_HSPL         0       /*HSYNC signal polarity*/
#  define R61581_HSL          10      /*HSYNC length (Not Implemented)*/
#  define R61581_HFP          10      /*Horitontal Front poarch (Not Implemented)*/
#  define R61581_HBP          10      /*Horitontal Back poarch (Not Implemented */
#  define R61581_VSPL         0       /*VSYNC signal polarity*/
#  define R61581_VSL          10      /*VSYNC length (Not Implemented)*/
#  define R61581_VFP          8       /*Vertical Front poarch*/
#  define R61581_VBP          8       /*Vertical Back poarch */
#  define R61581_DPL          0       /*DCLK signal polarity*/
#  define R61581_EPL          1       /*ENABLE signal polarity*/
#  define R61581_ORI          0       /*0, 180*/
#  define R61581_LV_COLOR_DEPTH 16    /*Fix 16 bit*/
#endif

/*------------------------------
 *  ST7565 (Monochrome, low res.)
 *-----------------------------*/
#ifndef USE_ST7565
#  define USE_ST7565          0
#endif

#if USE_ST7565
/*No settings*/
#endif  /*USE_ST7565*/

/*------------------------------
 *  GC9A01 (color, low res.)
 *-----------------------------*/
#ifndef USE_GC9A01
#  define USE_GC9A01          0
#endif

#if USE_GC9A01
/*No settings*/
#endif  /*USE_GC9A01*/

/*------------------------------------------
 *  UC1610 (4 gray 160*[104|128])
 *  (EA DOGXL160 160x104 tested)
 *-----------------------------------------*/
#ifndef USE_UC1610
#  define USE_UC1610          0
#endif

#if USE_UC1610
#  define UC1610_HOR_RES         LV_HOR_RES
#  define UC1610_VER_RES         LV_VER_RES
#  define UC1610_INIT_CONTRAST   33   /* init contrast, values in [%] */
#  define UC1610_INIT_HARD_RST   0    /* 1 : hardware reset at init, 0 : software reset */
#  define UC1610_TOP_VIEW        0    /* 0 : Bottom View, 1 : Top View */
#endif  /*USE_UC1610*/

/*-------------------------------------------------
 *  SHARP memory in pixel monochrome display series
 *      LS012B7DD01 (184x38  pixels.)
 *      LS013B7DH03 (128x128 pixels.)
 *      LS013B7DH05 (144x168 pixels.)
 *      LS027B7DH01 (400x240 pixels.) (tested)
 *      LS032B7DD02 (336x536 pixels.)
 *      LS044Q7DH01 (320x240 pixels.)
 *------------------------------------------------*/
#ifndef USE_SHARP_MIP
#  define USE_SHARP_MIP       1
#endif

#if USE_SHARP_MIP
// uint8_t rev_byte(uint8_t b){
static const uint8_t lsbLUT[] = {0x00, 0x80, 0x40, 0xC0, 0x20, 0xA0, 0x60, 0xE0, 0x10, 0x90, 0x50, 0xD0, 0x30, 0xB0, 0x70, 0xF0,
                                  0x08, 0x88, 0x48, 0xC8, 0x28, 0xA8, 0x68, 0xE8, 0x18, 0x98, 0x58, 0xD8, 0x38, 0xB8, 0x78, 0xF8,
                                  0x04, 0x84, 0x44, 0xC4, 0x24, 0xA4, 0x64, 0xE4, 0x14, 0x94, 0x54, 0xD4, 0x34, 0xB4, 0x74, 0xF4,
                                  0x0C, 0x8C, 0x4C, 0xCC, 0x2C, 0xAC, 0x6C, 0xEC, 0x1C, 0x9C, 0x5C, 0xDC, 0x3C, 0xBC, 0x7C, 0xFC,
                                  0x02, 0x82, 0x42, 0xC2, 0x22, 0xA2, 0x62, 0xE2, 0x12, 0x92, 0x52, 0xD2, 0x32, 0xB2, 0x72, 0xF2,
                                  0x0A, 0x8A, 0x4A, 0xCA, 0x2A, 0xAA, 0x6A, 0xEA, 0x1A, 0x9A, 0x5A, 0xDA, 0x3A, 0xBA, 0x7A, 0xFA,
                                  0x06, 0x86, 0x46, 0xC6, 0x26, 0xA6, 0x66, 0xE6, 0x16, 0x96, 0x56, 0xD6, 0x36, 0xB6, 0x76, 0xF6,
                                  0x0E, 0x8E, 0x4E, 0xCE, 0x2E, 0xAE, 0x6E, 0xEE, 0x1E, 0x9E, 0x5E, 0xDE, 0x3E, 0xBE, 0x7E, 0xFE,
                                  0x01, 0x81, 0x41, 0xC1, 0x21, 0xA1, 0x61, 0xE1, 0x11, 0x91, 0x51, 0xD1, 0x31, 0xB1, 0x71, 0xF1,
                                  0x09, 0x89, 0x49, 0xC9, 0x29, 0xA9, 0x69, 0xE9, 0x19, 0x99, 0x59, 0xD9, 0x39, 0xB9, 0x79, 0xF9,
                                  0x05, 0x85, 0x45, 0xC5, 0x25, 0xA5, 0x65, 0xE5, 0x15, 0x95, 0x55, 0xD5, 0x35, 0xB5, 0x75, 0xF5,
                                  0x0D, 0x8D, 0x4D, 0xCD, 0x2D, 0xAD, 0x6D, 0xED, 0x1D, 0x9D, 0x5D, 0xDD, 0x3D, 0xBD, 0x7D, 0xFD,
                                  0x03, 0x83, 0x43, 0xC3, 0x23, 0xA3, 0x63, 0xE3, 0x13, 0x93, 0x53, 0xD3, 0x33, 0xB3, 0x73, 0xF3,
                                  0x0B, 0x8B, 0x4B, 0xCB, 0x2B, 0xAB, 0x6B, 0xEB, 0x1B, 0x9B, 0x5B, 0xDB, 0x3B, 0xBB, 0x7B, 0xFB,
                                  0x07, 0x87, 0x47, 0xC7, 0x27, 0xA7, 0x67, 0xE7, 0x17, 0x97, 0x57, 0xD7, 0x37, 0xB7, 0x77, 0xF7,
                                  0x0F, 0x8F, 0x4F, 0xCF, 0x2F, 0xAF, 0x6F, 0xEF, 0x1F, 0x9F, 0x5F, 0xDF, 0x3F, 0xBF, 0x7F, 0xFF};
//     return lsbLUT[b];
// }
#  define SHARP_MIP_HOR_RES             LV_HOR_RES
#  define SHARP_MIP_VER_RES             LV_VER_RES
#  define SHARP_MIP_SOFT_COM_INVERSION  1
#  define SHARP_MIP_REV_BYTE(b)         (lsbLUT[b])  /*Architecture / compiler dependent byte bits order reverse*/
#endif  /*USE_SHARP_MIP*/

/*-------------------------------------------------
 *  ILI9341 240X320 TFT LCD
 *------------------------------------------------*/
#ifndef USE_ILI9341
#  define USE_ILI9341       0
#endif

#if USE_ILI9341
#  define ILI9341_HOR_RES       LV_HOR_RES
#  define ILI9341_VER_RES       LV_VER_RES
#  define ILI9341_GAMMA         1
#  define ILI9341_TEARING       0
#endif  /*USE_ILI9341*/

/*-----------------------------------------
 *  Linux frame buffer device (/dev/fbx)
 *-----------------------------------------*/
#ifndef USE_FBDEV
#  define USE_FBDEV           0
#endif

#if USE_FBDEV
#  define FBDEV_PATH          "/dev/fb0"
#endif

/*-----------------------------------------
 *  FreeBSD frame buffer device (/dev/fbx)
 *.........................................*/
#ifndef USE_BSD_FBDEV
#  define USE_BSD_FBDEV		0
#endif

#if USE_BSD_FBDEV
# define FBDEV_PATH		"/dev/fb0"
#endif

/*-----------------------------------------
 *  DRM/KMS device (/dev/dri/cardX)
 *-----------------------------------------*/
#ifndef USE_DRM
#  define USE_DRM           0
#endif

#if USE_DRM
#  define DRM_CARD          "/dev/dri/card0"
#  define DRM_CONNECTOR_ID  -1	/* -1 for the first connected one */
#endif

/*********************
 *  INPUT DEVICES
 *********************/

/*--------------
 *    XPT2046
 *--------------*/
#ifndef USE_XPT2046
#  define USE_XPT2046         0
#endif

#if USE_XPT2046
#  define XPT2046_HOR_RES     480
#  define XPT2046_VER_RES     320
#  define XPT2046_X_MIN       200
#  define XPT2046_Y_MIN       200
#  define XPT2046_X_MAX       3800
#  define XPT2046_Y_MAX       3800
#  define XPT2046_AVG         4
#  define XPT2046_X_INV       0
#  define XPT2046_Y_INV       0
#  define XPT2046_XY_SWAP     0
#endif

/*-----------------
 *    FT5406EE8
 *-----------------*/
#ifndef USE_FT5406EE8
#  define USE_FT5406EE8       0
#endif

#if USE_FT5406EE8
# define FT5406EE8_I2C_ADR   0x38                  /*7 bit address*/
#endif

/*---------------
 *  AD TOUCH
 *--------------*/
#ifndef USE_AD_TOUCH
#  define USE_AD_TOUCH        0
#endif

#if USE_AD_TOUCH
/*No settings*/
#endif


/*---------------------------------------
 * Mouse or touchpad on PC (using SDL)
 *-------------------------------------*/
/*DEPRECATED: Use the SDL driver instead. */
#ifndef USE_MOUSE
#  define USE_MOUSE           0
#endif

#if USE_MOUSE
/*No settings*/
#endif

/*-------------------------------------------
 * Mousewheel as encoder on PC (using SDL)
 *------------------------------------------*/
/*DEPRECATED: Use the SDL driver instead. */
#ifndef USE_MOUSEWHEEL
#  define USE_MOUSEWHEEL      0
#endif

#if USE_MOUSEWHEEL
/*No settings*/
#endif

/*-------------------------------------------------
 * Touchscreen, mouse/touchpad or keyboard as libinput interface (for Linux based systems)
 *------------------------------------------------*/
#ifndef USE_LIBINPUT
#  define USE_LIBINPUT           0
#endif

#ifndef USE_BSD_LIBINPUT
#  define USE_BSD_LIBINPUT       0
#endif

#if USE_LIBINPUT || USE_BSD_LIBINPUT
/*If only a single device of the same type is connected, you can also auto detect it, e.g.:
 *#define LIBINPUT_NAME   libinput_find_dev(LIBINPUT_CAPABILITY_TOUCH, false)*/
#  define LIBINPUT_NAME   "/dev/input/event0"        /*You can use the "evtest" Linux tool to get the list of devices and test them*/

#endif  /*USE_LIBINPUT || USE_BSD_LIBINPUT*/

/*-------------------------------------------------
 * Mouse or touchpad as evdev interface (for Linux based systems)
 *------------------------------------------------*/
#ifndef USE_EVDEV
#  define USE_EVDEV           0
#endif

#ifndef USE_BSD_EVDEV
#  define USE_BSD_EVDEV       0
#endif

#if USE_EVDEV || USE_BSD_EVDEV
#  define EVDEV_NAME   "/dev/input/event0"        /*You can use the "evtest" Linux tool to get the list of devices and test them*/
#  define EVDEV_SWAP_AXES         0               /*Swap the x and y axes of the touchscreen*/

#  define EVDEV_CALIBRATE         0               /*Scale and offset the touchscreen coordinates by using maximum and minimum values for each axis*/

#  if EVDEV_CALIBRATE
#    define EVDEV_HOR_MIN         0               /*to invert axis swap EVDEV_XXX_MIN by EVDEV_XXX_MAX*/
#    define EVDEV_HOR_MAX      4096               /*"evtest" Linux tool can help to get the correct calibraion values>*/
#    define EVDEV_VER_MIN         0
#    define EVDEV_VER_MAX      4096
#  endif  /*EVDEV_CALIBRATE*/
#endif  /*USE_EVDEV*/

/*-------------------------------------------------
 * Full keyboard support for evdev and libinput interface
 *------------------------------------------------*/
#  ifndef USE_XKB
#    define USE_XKB           0
#  endif

#if USE_LIBINPUT || USE_BSD_LIBINPUT || USE_EVDEV || USE_BSD_EVDEV
#  if USE_XKB
#    define XKB_KEY_MAP       { .rules = NULL, \
                                .model = "pc101", \
                                .layout = "us", \
                                .variant = NULL, \
                                .options = NULL } /*"setxkbmap -query" can help find the right values for your keyboard*/
#  endif  /*USE_XKB*/
#endif  /*USE_LIBINPUT || USE_BSD_LIBINPUT || USE_EVDEV || USE_BSD_EVDEV*/

/*-------------------------------
 *   Keyboard of a PC (using SDL)
 *------------------------------*/
/*DEPRECATED: Use the SDL driver instead. */
#ifndef USE_KEYBOARD
#  define USE_KEYBOARD        0
#endif

#if USE_KEYBOARD
/*No settings*/
#endif

#endif  /*LV_DRV_CONF_H*/

#endif /*End of "Content enable"*/
