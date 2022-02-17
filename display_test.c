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

lv_obj_t* createCheckmark(uint8_t lineWidth)
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

    return obj;
}

lv_obj_t* lv_example_label_1(void)
{
    static lv_style_t label_style;
    lv_style_init(&label_style);
    lv_style_set_text_color(&label_style, lv_color_black());
    lv_style_set_text_font(&label_style, &lv_font_montserrat_48);

    lv_obj_t * label1 = lv_label_create(lv_scr_act());
    lv_obj_add_style(label1, &label_style, 0);
    lv_label_set_long_mode(label1, LV_LABEL_LONG_CLIP);     /*Break the long lines*/
    lv_label_set_recolor(label1, true);                      /*Enable re-coloring by commands in the text*/
    lv_label_set_text(label1, "0.00V");
    lv_obj_set_width(label1, 150);  /*Set smaller width to make the lines wrap*/
    lv_obj_set_style_text_align(label1, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(label1, LV_ALIGN_CENTER, 0, 0);
    return label1;
}



int main() {
    stdio_init_all();
    set_sys_clock_khz(200000, true);
		// sleep_ms(3000);
    printf("Hello, Sharp Memory Display test!\n");
    printf("Initializing SPI...\n");

    spi_init(DISP_SPI_PORT, 2000 * 1000);
    gpio_set_function(DISP_SCK, GPIO_FUNC_SPI);
    gpio_set_function(DISP_MOSI, GPIO_FUNC_SPI);
    // Make the SPI pins available to picotool
    // bi_decl(bi_2pins_with_func(DISP_MOSI, DISP_SCK, GPIO_FUNC_SPI));

    // Chip select is active-high, so we'll initialise it to a driven-low state
    gpio_init(DISP_CS);
    gpio_set_dir(DISP_CS, GPIO_OUT);
    gpio_put(DISP_CS, 0);
    // Make the CS pin available to picotool
    // bi_decl(bi_1pin_with_name(DISP_CS, "SPI CS"));
    uart_init(uart0, 115200);
 
    // Set the GPIO pin mux to the UART - 0 is TX, 1 is RX
    gpio_set_function(0, GPIO_FUNC_UART);
    gpio_set_function(1, GPIO_FUNC_UART);

    lv_init();
    struct repeating_timer lvgl_ticker_timer;
    add_repeating_timer_ms(1, lvgl_ticker, NULL, &lvgl_ticker_timer);
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

    // lv_obj_t* obj = createCheckmark(10);
    lv_obj_t* label = lv_example_label_1();
    
    while(1){

      uint16_t adcResult;
      if(uart_is_readable(uart0)){
        uint8_t c = uart_getc(uart0);
        if(c == 127){
          printf("start byte\n");
          // we received a start character
          uint8_t byte1 = uart_getc(uart0);
          uint8_t byte2 = uart_getc(uart0);
          adcResult = byte1 | byte2 << 8;
          printf("ADC result: %d\n", adcResult);
        }
        if(uart_getc(uart0) == 255){
          //message is valid and can be shown to GUI
          
          char str[12];
          sprintf(str, "%5.2fV",(3.33/1024)*adcResult);
          lv_label_set_text(label, str);
        }else{
          printf("invalid message\n");
          //message is invalid, skip and wait for next message
        }
        // printf("%c", c);
        
      }
      // for (size_t i = 4; i < 30; i++){
      //   lv_obj_set_pos(obj, sin(time_us_32()/130) * 50, sin(time_us_32()/100) * 80);
        lv_timer_handler();
        // sleep_ms(1);
      // }
      
    }

    return 0;
}
