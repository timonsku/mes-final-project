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

#define DISP_HOR_RES 400
#define DISP_VER_RES 240

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf1[DISP_HOR_RES * DISP_VER_RES];
static lv_disp_drv_t disp_drv;

bool lvgl_ticker(repeating_timer_t *rt){
  lv_tick_inc(1);
  return true;
}

static lv_style_t selected_style;
static lv_style_t unselected_style;
lv_obj_t *adcButtons[4];

uint8_t selectCounter = 0;
// just any non real value to trigger an update on first run
uint8_t currentSelection = 255;

lv_obj_t* createVoltageLabel(void)
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
    lv_obj_align(label1, LV_ALIGN_CENTER, 0, 10);
    return label1;
}

void initStyles(){
    lv_style_init(&selected_style);
    lv_style_set_text_color(&selected_style, lv_color_white());
    lv_style_set_text_font(&selected_style, &lv_font_montserrat_14);
    lv_style_set_bg_color(&selected_style, lv_color_black());

    lv_style_init(&unselected_style);
    lv_style_set_text_color(&unselected_style, lv_color_black());
    lv_style_set_text_font(&unselected_style, &lv_font_montserrat_14);
    lv_style_set_bg_color(&unselected_style, lv_color_white());
    lv_style_set_border_width(&unselected_style, 2);
    lv_style_set_border_color(&unselected_style, lv_color_black());
}

lv_obj_t* createSelectionBtn(lv_obj_t* src_actor, char* labelText)
{
    lv_obj_t * label;
    lv_obj_t * btn1 = lv_btn_create(src_actor);
    lv_obj_add_style(btn1, &unselected_style, 0);
    lv_obj_add_style(btn1, &selected_style, LV_STATE_PRESSED);
    // lv_obj_add_event_cb(btn1, event_handler, LV_EVENT_ALL, NULL);
    
    // lv_obj_align(btn1, LV_ALIGN_DEFAULT, posX, posY);
    lv_obj_center(btn1);
    lv_obj_add_flag(btn1, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_set_size(btn1, 80, LV_PCT(80));

    label = lv_label_create(btn1);
    lv_label_set_text(label, labelText);
    lv_obj_center(label);
    
    return btn1;
}

void initMenu(){

    initStyles();
    // create flexbox container for buttons
    lv_obj_t * cont_row = lv_obj_create(lv_scr_act());
    lv_obj_set_size(cont_row, 400, 100);
    lv_obj_align(cont_row, LV_ALIGN_TOP_MID, 0, 5);
    lv_obj_set_flex_flow(cont_row, LV_FLEX_FLOW_ROW);

    // populate flexbox with buttons
    for (size_t i = 0; i < 4; i++){
      char labelText[8];
      sprintf(labelText, "ADC %d", i);
      adcButtons[i] = createSelectionBtn(cont_row, labelText);
    }
}

void selectADC(uint8_t adcNum){
    if(adcNum > 3){return;}
    uint8_t selectMsg[3] = {127, adcNum, 255};
    uart_write_blocking(uart0, selectMsg, 3);

    for(size_t i = 0; i < 4; i++){
        if(i == adcNum){
            lv_obj_add_style(adcButtons[i], &selected_style, 0);
        } else {
            lv_obj_add_style(adcButtons[i], &unselected_style, 0);
        }
    }
}

void updateSelection(){
  if(currentSelection != selectCounter){
    selectADC(selectCounter);
    currentSelection = selectCounter;
  }
}

void btn_cb(uint gpio, uint32_t events) {
    if(gpio == 26){
      if(selectCounter < 3){
        selectCounter++;
      } else {
        selectCounter = 0;
      }
    }else if(gpio == 27){
      if(selectCounter > 0){
        selectCounter--;
      } else {
        selectCounter = 3;
      }
    }
    // printf("GPIO %d %d\n", gpio, selectCounter);
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
    lv_disp_draw_buf_init(&draw_buf, buf1, NULL, DISP_HOR_RES * DISP_VER_RES);
    lv_disp_drv_init(&disp_drv);
    disp_drv.flush_cb = sharp_mip_flush;
    disp_drv.rounder_cb = sharp_mip_rounder;
    disp_drv.set_px_cb = sharp_mip_set_px;
    // disp_drv.full_refresh = true;
    disp_drv.draw_buf = &draw_buf; 
    disp_drv.hor_res = DISP_HOR_RES;
    disp_drv.ver_res = DISP_VER_RES;
    lv_disp_drv_register(&disp_drv);

    gpio_init(26);
    gpio_set_function(26, GPIO_FUNC_SIO);
    gpio_set_dir(26, GPIO_IN);
    gpio_pull_up(26);
    gpio_set_irq_enabled_with_callback(26, GPIO_IRQ_EDGE_FALL, true, &btn_cb);

    gpio_init(27);
    gpio_set_function(27, GPIO_FUNC_SIO);
    gpio_set_dir(27, GPIO_IN);
    gpio_pull_up(27);
    gpio_set_irq_enabled_with_callback(27, GPIO_IRQ_EDGE_FALL, true, &btn_cb);

    // lv_obj_t* obj = createCheckmark(10);
    lv_obj_t* voltageLabel = createVoltageLabel();
    initMenu();
    // selectADC(2);
    while(1){
      lv_timer_handler();
      updateSelection();
      uint16_t adcResult;
      if(uart_is_readable(uart0)){
        uint8_t c = uart_getc(uart0);
        if(c == 127){
          //printf("start byte\n");
          // we received a start character
          uint8_t byte1 = uart_getc(uart0);
          uint8_t byte2 = uart_getc(uart0);
          adcResult = byte1 | byte2 << 8;
          printf("ADC result: %d\n", adcResult);
        }
        if(uart_getc(uart0) == 255){
          //message is valid and can be shown to GUI
          
          char str[12];
          sprintf(str, "%5.2fV",(3.333/4096)*adcResult);
          lv_label_set_text(voltageLabel, str);
        }else{
          printf("invalid message\n");
          //message is invalid, skip and wait for next message
        }
        // printf("%c", c);
        
      }
      // for (size_t i = 4; i < 30; i++){
      //   lv_obj_set_pos(obj, sin(time_us_32()/130) * 50, sin(time_us_32()/100) * 80);
        
        // sleep_ms(1);
      // }
      
    }

    return 0;
}
