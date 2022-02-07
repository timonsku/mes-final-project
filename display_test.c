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
#include <math.h>


#define WIDTH 400
#define HEIGHT 240

#define DISP_HOR_RES 400
#define DISP_VER_RES 240



static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf1[DISP_HOR_RES * DISP_VER_RES];
static lv_disp_drv_t disp_drv;

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

lv_obj_t* lv_example_style_9(uint8_t lineWidth)
{
    static lv_style_t style;
    lv_style_init(&style);

    lv_style_set_line_color(&style, lv_color_black());
    lv_style_set_line_width(&style, lineWidth);
    lv_style_set_line_rounded(&style, true);

    /*Create an object with the new style*/
    lv_obj_t * obj = lv_line_create(lv_scr_act());
    lv_obj_add_style(obj, &style, 0);

    static lv_point_t p[] = {{10, 30}, {30, 50}, {100, 0}};
    lv_line_set_points(obj, p, 3);

    lv_obj_center(obj);
    return obj;
}

int main() {
    stdio_init_all();
		sleep_ms(3000);
    printf("Hello, Sharp Memory Display test!\n");
    printf("Initializing SPI...\n");

    spi_init(DISP_SPI_PORT, 12000 * 1000);
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


    lv_init();
    struct repeating_timer timer;
    add_repeating_timer_ms(1, lvgl_ticker, NULL, &timer);
    lv_disp_draw_buf_init(&draw_buf, buf1, NULL, DISP_HOR_RES * DISP_VER_RES);  /*Initialize the display buffer.*/
    lv_disp_drv_init(&disp_drv);          /*Basic initialization*/
    disp_drv.flush_cb = sharp_mip_flush;    /*Set your driver function*/
    disp_drv.rounder_cb = sharp_mip_rounder;
    disp_drv.set_px_cb = sharp_mip_set_px;
    // disp_drv.full_refresh = true;
    disp_drv.draw_buf = &draw_buf;        /*Assign the buffer to the display*/
    disp_drv.hor_res = DISP_HOR_RES;   /*Set the horizontal resolution of the display*/
    disp_drv.ver_res = DISP_VER_RES;   /*Set the vertical resolution of the display*/
    lv_disp_drv_register(&disp_drv);      /*Finally register the driver*/
    printf("lv_disp_drv setup\n");

    lv_obj_t* obj = lv_example_style_9(10);
    while(1){
      
      for (size_t i = 4; i < 30; i++){
        lv_obj_set_pos(obj, sin(time_us_32()/130) * 50, sin(time_us_32()/100) * 80);
        lv_timer_handler();
        sleep_ms(1);
      }

      

      
      
    }


		// // printf("init display\n");
    // sharp_display_begin();
		// // sleep_ms(1000);
		// // printf("clear display\n");
		
		// sharp_display_clearDisplay();
    // sleep_ms(100);
    // while (1) {
		// 	printf("drawing line\n");
    //   // for (size_t i = 0; i < WIDTH; i++)
    //   // {
    //     for (size_t j = 0; j < HEIGHT; j++){
    //       sharp_display_drawPixel(10, j, 0);
    //     }
    //   // }
      
			
		// 	sharp_display_refresh();
				
    // //     // printf("Temp. = %f\n", (temp / 340.0) + 36.53);

    //     sleep_ms(100);
    // }

    return 0;
}
