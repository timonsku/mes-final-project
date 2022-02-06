/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/malloc.h"
#include <stdlib.h>
#include "pico/binary_info.h"
#include "hardware/spi.h"
#include "lib/lvgl/lvgl.h"
#include "SHARP_MIP.h"

#define SHARPMEM_BIT_WRITECMD (0x80) // 0x01 | 0x80 in LSB format
#define SHARPMEM_BIT_VCOM (0x40)     // 0x02 | 0x40 in LSB format
#define SHARPMEM_BIT_CLEAR (0x20)    // 0x04 | 0x20 in LSB format

#define TOGGLE_VCOM                                                            \
  do {                                                                         \
    _sharpmem_vcom = _sharpmem_vcom ? 0x00 : SHARPMEM_BIT_VCOM;                \
  } while (0);


static const uint8_t set[] = {1, 2, 4, 8, 16, 32, 64, 128},
                     clr[] = {(uint8_t)~1,  (uint8_t)~2,  (uint8_t)~4,
                              (uint8_t)~8,  (uint8_t)~16, (uint8_t)~32,
                              (uint8_t)~64, (uint8_t)~128};

static const uint8_t bitswap[] = {0x00, 0x80, 0x40, 0xC0, 0x20, 0xA0, 0x60, 0xE0, 0x10, 0x90, 0x50, 0xD0, 0x30, 0xB0, 0x70, 0xF0,
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

uint8_t *sharpmem_buffer = NULL;
uint8_t _sharpmem_vcom;

#define WIDTH 400
#define HEIGHT 240

#define DISP_HOR_RES 400
#define DISP_VER_RES 240



static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf1[DISP_HOR_RES * DISP_VER_RES];                        /*Declare a buffer for 1/10 screen size*/
static lv_disp_drv_t disp_drv;

static inline void cs_select() {
    asm volatile("nop \n nop \n nop");
    gpio_put(DISP_CS, 1);  // Active high
    asm volatile("nop \n nop \n nop");
}

static inline void cs_deselect() {
    asm volatile("nop \n nop \n nop");
    gpio_put(DISP_CS, 0);
    asm volatile("nop \n nop \n nop");
}

static bool sharp_display_begin(void) {

  cs_select();

  // Set the vcom bit to a defined state
  _sharpmem_vcom = true;

  sharpmem_buffer = (uint8_t *)malloc((WIDTH * HEIGHT) / 8);

  if (!sharpmem_buffer)
    return false;

  return true;
}

void sharp_display_drawPixel(int16_t x, int16_t y, uint16_t color) {
  if ((x < 0) || (x >= WIDTH) || (y < 0) || (y >= HEIGHT))
    return;

  if (color) {
    sharpmem_buffer[(y * WIDTH + x) / 8] |= set[x & 7];
  } else {
    sharpmem_buffer[(y * WIDTH + x) / 8] &= clr[x & 7];
  }
}

void sharp_display_clearDisplay() {
  memset(sharpmem_buffer, 0xff, (WIDTH * HEIGHT) / 8);
  // Send the clear screen command rather than doing a HW refresh (quicker)
  cs_select();
  uint8_t cmd[2];
	cmd[0] = SHARPMEM_BIT_CLEAR;
  if(_sharpmem_vcom) {
    cmd[0] |= SHARPMEM_BIT_VCOM;
  }
  _sharpmem_vcom = !_sharpmem_vcom;
  cmd[1] = 0x00;
	spi_write_blocking(DISP_SPI_PORT, cmd, 2);
  cs_deselect();
}

void sharp_display_refresh(void) {
  uint16_t i, currentline;

  // Send the write command
  cs_select();
	uint8_t cmd[1];
	cmd[0] = SHARPMEM_BIT_WRITECMD;
  if(_sharpmem_vcom) {
    cmd[0] |= SHARPMEM_BIT_VCOM;
  }
  _sharpmem_vcom = !_sharpmem_vcom;

	spi_write_blocking(DISP_SPI_PORT, cmd, 1);
  // spidev->transfer(_sharpmem_vcom | SHARPMEM_BIT_WRITECMD);

  uint8_t bytes_per_line = WIDTH / 8;
  uint16_t totalbytes = (WIDTH * HEIGHT) / 8;

  for (i = 0; i < totalbytes; i += bytes_per_line) {
    uint8_t line[bytes_per_line + 2];

    // Send address byte
    currentline = ((i + 1) / (WIDTH / 8)) + 1;
    line[0] = bitswap[currentline];
    // copy over this line
    memcpy(line + 1, sharpmem_buffer + i, bytes_per_line);
    // Send end of line
    line[bytes_per_line + 1] = 0x00;
    // send it!
		spi_write_blocking(DISP_SPI_PORT, line, bytes_per_line + 2);
    // spidev->transfer(line, bytes_per_line + 2);
  }

  // Send another trailing 8 bits for the last line
	cmd[0] = 0x00;
	spi_write_blocking(DISP_SPI_PORT, cmd, 1);
  // spidev->transfer(0x00);
  cs_deselect();
}

/**************************************************************************/
/*!
    @brief Clears the display buffer without outputting to the display
*/
/**************************************************************************/
void sharp_display_clearDisplayBuffer() {
  memset(sharpmem_buffer, 0xff, (WIDTH * HEIGHT) / 8);
}


bool lvgl_ticker(repeating_timer_t *rt){
  lv_tick_inc(1);
  return true;
}

static void btn_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * btn = lv_event_get_target(e);
    if(code == LV_EVENT_CLICKED) {
        static uint8_t cnt = 0;
        cnt++;
        printf("btn clicked\n");
        /*Get the first child of the button which is the label and change its text*/
        lv_obj_t * label = lv_obj_get_child(btn, 0);
        lv_label_set_text_fmt(label, "Button: %d", cnt);
    }
}

/**
 * Create a button with a label and react on click event.
 */
void lv_example_get_started_1(void)
{
    lv_obj_t * btn = lv_btn_create(lv_scr_act());     /*Add a button the current screen*/
    lv_obj_set_pos(btn, 10, 10);                            /*Set its position*/
    lv_obj_set_size(btn, 120, 50);                          /*Set its size*/
    lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_ALL, NULL);           /*Assign a callback to the button*/

    lv_obj_t * label = lv_label_create(btn);          /*Add a label to the button*/
    lv_label_set_text(label, "Button");                     /*Set the labels text*/
    lv_obj_center(label);
}

void lv_example_obj_1(void)
{
    lv_obj_t * obj1;
    obj1 = lv_obj_create(lv_scr_act());
    lv_obj_set_size(obj1, 100, 50);
    lv_obj_align(obj1, LV_ALIGN_CENTER, -60, -30);

    static lv_style_t style_shadow;
    lv_style_init(&style_shadow);
    lv_style_set_shadow_width(&style_shadow, 10);
    lv_style_set_shadow_spread(&style_shadow, 5);
    lv_style_set_shadow_color(&style_shadow, lv_palette_main(LV_PALETTE_BLUE));

    lv_obj_t * obj2;
    obj2 = lv_obj_create(lv_scr_act());
    lv_obj_add_style(obj2, &style_shadow, 0);
    lv_obj_align(obj2, LV_ALIGN_CENTER, 60, 30);
}

int main() {
    stdio_init_all();
		sleep_ms(3000);
    printf("Hello, Sharp Memory Display test!\n");
    printf("Initializing SPI...\n");

    // This example will use SPI0 at 0.5MHz.
    spi_init(DISP_SPI_PORT, 1000 * 1000);
    gpio_set_function(DISP_SCK, GPIO_FUNC_SPI);
    gpio_set_function(DISP_MOSI, GPIO_FUNC_SPI);
    // Make the SPI pins available to picotool
    bi_decl(bi_2pins_with_func(DISP_MOSI, DISP_SCK, GPIO_FUNC_SPI));

    // Chip select is active-high, so we'll initialise it to a driven-low state
    gpio_init(DISP_CS);
    gpio_set_dir(DISP_CS, GPIO_OUT);
    gpio_put(DISP_CS, 0);
    // Make the CS pin available to picotool
    bi_decl(bi_1pin_with_name(DISP_CS, "SPI CS"));
    // printf("init display\n");
    // sharp_display_begin();
		// sleep_ms(1000);
		// printf("clear display\n");
		
		// sharp_display_clearDisplay();
    sleep_ms(2000);

    lv_init();
    struct repeating_timer timer;
    add_repeating_timer_ms(1, lvgl_ticker, NULL, &timer);
    lv_disp_draw_buf_init(&draw_buf, buf1, NULL, DISP_HOR_RES * DISP_VER_RES);  /*Initialize the display buffer.*/
    lv_disp_drv_init(&disp_drv);          /*Basic initialization*/
    disp_drv.flush_cb = sharp_mip_flush;    /*Set your driver function*/
    disp_drv.rounder_cb = sharp_mip_rounder;
    disp_drv.set_px_cb = sharp_mip_set_px;
    disp_drv.draw_buf = &draw_buf;        /*Assign the buffer to the display*/
    disp_drv.hor_res = DISP_HOR_RES;   /*Set the horizontal resolution of the display*/
    disp_drv.ver_res = DISP_VER_RES;   /*Set the vertical resolution of the display*/
    lv_disp_drv_register(&disp_drv);      /*Finally register the driver*/
    printf("lv_disp_drv setup\n");
    lv_example_get_started_1();
    while(1){
      lv_timer_handler();
      sleep_ms(5);
    }
		
    // while (1) {
		// 	printf("drawing line\n");
    //   // for (size_t i = 0; i < WIDTH; i++)
    //   // {
    //     for (size_t j = 0; j < HEIGHT; j++){
    //       sharp_display_drawPixel(10, j, 0);
    //     }
    //   // }
      
			
		// 	sharp_display_refresh();
				
    //     // printf("Temp. = %f\n", (temp / 340.0) + 36.53);

    //     sleep_ms(100);
    // }

    return 0;
}
