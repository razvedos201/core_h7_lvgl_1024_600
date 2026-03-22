/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma2d.h"
#include "i2c.h"
#include "ltdc.h"
#include "memorymap.h"
#include "gpio.h"
#include "fmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "string.h"
#include "bsp_sdram.h"
//#include "../../Drivers/lvgl/lvgl/lvgl.h"
#include "lvgl.h"
#include "GT911.h"
#include "stm32h7xx_ll_exti.h"
#include "BMP.h"
#include "demos/widgets/lv_demo_widgets.h"


/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
#define QSPI_BUFF_SIZE 		100
#define LTDC_FRAMEBUFFER_ADDR  ((uint32_t)0xC0000000)

uint8_t QSPI_RD[QSPI_BUFF_SIZE], QSPI_WR[QSPI_BUFF_SIZE];
/*
uint8_t writebuf[] = "Hello world from QSPI";
uint16_t rd[1024], rw[1024];
*/

static lv_disp_draw_buf_t draw_buf;
#define LVGL_BUF_LINES  60  /* 1/10 screen height */
#define LVGL_BUF_SIZE   (PIXELS_W * LVGL_BUF_LINES)  /* 1024*60 = 61440 pixels = 120KB */
static lv_color_t buf1[LVGL_BUF_SIZE];  /* AXI SRAM - fast internal RAM */
static lv_color_t buf2[LVGL_BUF_SIZE];  /* AXI SRAM - fast internal RAM */

static uint8_t BMP_BUFF[1152000] __attribute__((section(".extram")));

static lv_disp_drv_t disp_drv;        /*Descriptor of a display driver*/
static lv_indev_drv_t indev_drv;      /*Descriptor of a input device driver*/

extern LTDC_HandleTypeDef hltdc;
extern SDRAM_HandleTypeDef hsdram1;
static uint16_t temp_fbuf[PIXELS_W*PIXELS_H] __attribute__((section(".extram")));

uint32_t temp;

// Touch Controller GT911
GT911Touch_TypeDef GT911Touch[5];
uint8_t TouchCount = 0;
volatile uint8_t TouchIRQFlag = 0;

BMP_TypeDef BMP_INFO;
unsigned int bytesRead;
uint32_t i = 0, j = 0;
uint8_t SD_BUFF[12288];
//uint32_t BMP_BUFF[4096];
uint32_t l=0;
uint32_t data;
uint32_t BMP_SIZE;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

//Конвертор цветов
uint16_t Color_To_565(uint8_t r, uint8_t g, uint8_t b)
{
	return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3);
}

void my_disp_flush(lv_disp_drv_t * disp, const lv_area_t * area, lv_color_t * color_p)
{
	/* Wait for previous DMA2D transfer to finish before starting new one */
	while(DMA2D->CR & DMA2D_CR_START) {}
	/* Flush D-Cache so DMA2D reads correct data from RAM */
	uint32_t size = (area->x2 - area->x1 + 1) * (area->y2 - area->y1 + 1) * 2;
	SCB_CleanDCache_by_Addr((uint32_t *)color_p, size);
	LTDC_Color_Fill(area->x1, area->y1, area->x2, area->y2, (uint16_t *)color_p);
	lv_disp_flush_ready(disp);
}
/*void my_disp_flush(lv_disp_drv_t * disp, const lv_area_t * area, lv_color_t * color_p)
{
    uint16_t *fb = (uint16_t *)LTDC_FRAMEBUFFER_ADDR;

    for(int y = area->y1; y <= area->y2; y++) {
        memcpy(&fb[y * PIXELS_W + area->x1],
               color_p,
               (area->x2 - area->x1 + 1) * 2);

        color_p += (area->x2 - area->x1 + 1);
    }

    lv_disp_flush_ready(disp);
}*/
void my_touchpad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    if(TouchIRQFlag) {
      TouchIRQFlag = 0;
      TouchCount = GT911_ReadTouch(&GT911Touch[0]);
    }
    if(TouchCount) {
      data->state = LV_INDEV_STATE_PRESSED;
      data->point.x = GT911Touch[0].XCoordinate;
      data->point.y = GT911Touch[0].YCoordinate;
    } else {
      data->state = LV_INDEV_STATE_RELEASED;
    }
}
void EXTI3_IRQHandler(void)
{
	LL_EXTI_ClearFlag_0_31(GT911_INT_Pin);
	NVIC_ClearPendingIRQ(EXTI3_IRQn);
	TouchIRQFlag = 1;
}

static lv_obj_t * meter;
static lv_meter_indicator_t * g_indic_min;
static lv_meter_indicator_t * g_indic_hour;
static lv_meter_scale_t * g_scale_min;

static void set_value(void * indic, int32_t v)
{
    lv_meter_set_indicator_end_value(meter, indic, v);
}

static void meter_anim_start(void)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_exec_cb(&a, set_value);
    lv_anim_set_values(&a, 0, 60);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_time(&a, 60000);      /* 60 sec for 1 turn of minute hand */
    lv_anim_set_var(&a, g_indic_min);
    lv_anim_start(&a);

    lv_anim_set_var(&a, g_indic_hour);
    lv_anim_set_time(&a, 720000);     /* 12 min for 1 turn of hour hand */
    lv_anim_set_values(&a, 0, 60);
    lv_anim_start(&a);
}

static void meter_anim_stop(void)
{
    lv_anim_del(g_indic_min, set_value);
    lv_anim_del(g_indic_hour, set_value);
}

static void tab_changed_cb(lv_event_t * e)
{
    lv_obj_t * tv = lv_event_get_current_target(e);
    uint16_t tab_id = lv_tabview_get_tab_act(tv);
    if(tab_id == 0) {
        meter_anim_start();
    } else {
        meter_anim_stop();
    }
}

void lv_example_meter_my(void)
{
    meter = lv_meter_create(lv_scr_act());
    lv_obj_set_size(meter, 300, 300);
    lv_obj_center(meter);

    /*Create a scale for the minutes*/
    /*61 ticks in a 360 degrees range (the last and the first line overlaps)*/
    lv_meter_scale_t * scale_min = lv_meter_add_scale(meter);
    lv_meter_set_scale_ticks(meter, scale_min, 61, 1, 10, lv_palette_main(LV_PALETTE_GREY));
    lv_meter_set_scale_range(meter, scale_min, 0, 60, 360, 270);

    /*Create another scale for the hours. It's only visual and contains only major ticks*/
    lv_meter_scale_t * scale_hour = lv_meter_add_scale(meter);
    lv_meter_set_scale_ticks(meter, scale_hour, 12, 0, 0, lv_palette_main(LV_PALETTE_GREY));               /*12 ticks*/
    lv_meter_set_scale_major_ticks(meter, scale_hour, 1, 2, 20, lv_color_black(), 10);    /*Every tick is major*/
    lv_meter_set_scale_range(meter, scale_hour, 1, 12, 330, 300);       /*[1..12] values in an almost full circle*/

    LV_IMG_DECLARE(img_hand)

    /*Add a the hands from images*/
    lv_meter_indicator_t * indic_min = lv_meter_add_needle_img(meter, scale_min, &img_hand, 5, 5);
    lv_meter_indicator_t * indic_hour = lv_meter_add_needle_img(meter, scale_min, &img_hand, 5, 5);

    /*Create an animation to set the value*/
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_exec_cb(&a, set_value);
    lv_anim_set_values(&a, 0, 60);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_time(&a, 2000);     /*2 sec for 1 turn of the minute hand (1 hour)*/
    lv_anim_set_var(&a, indic_min);
    lv_anim_start(&a);

    lv_anim_set_var(&a, indic_hour);
    lv_anim_set_time(&a, 3600000);    /*24 sec for 1 turn of the hour hand*/
    lv_anim_set_values(&a, 0, 60);
    lv_anim_start(&a);
}

static lv_obj_t * label;

float ser1_data, ser2_data,  m;

static lv_obj_t * slider_label;
static lv_obj_t * meter;

static lv_obj_t * chart;
static lv_chart_series_t * ser;
static lv_chart_series_t * ser2;
static lv_chart_cursor_t * cursor;

static void slider_event_cb(lv_event_t * e)
{
    lv_obj_t * slider = lv_event_get_target(e);

    /*Refresh the text*/
    lv_label_set_text_fmt(label, "%"LV_PRId32, lv_slider_get_value(slider));
    lv_obj_align_to(label, slider, LV_ALIGN_OUT_TOP_MID, 0, -15);    /*Align top of the slider*/
}
void lv_example_get_started_my(void)
{
    /*Create a slider in the center of the display*/
    lv_obj_t * slider = lv_slider_create(lv_scr_act());
    lv_obj_set_width(slider, 580);                          /*Set the width*/
    //lv_obj_center(slider);                                  /*Align to the center of the parent (screen)*/
    lv_obj_set_pos(slider, 110, 440);
    lv_obj_add_event_cb(slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);     /*Assign an event function*/

    /*Create a label above the slider*/
    label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, "0");
    lv_obj_align_to(label, slider, LV_ALIGN_OUT_TOP_MID, 0, -15);    /*Align top of the slider*/
}


void tabview(void){
	lv_obj_t * tabview;
	//tabview = lv_tabview_create(lv_scr_act(), LV_DIR_LEFT, 100);
	tabview = lv_tabview_create(lv_scr_act(), LV_DIR_RIGHT, 100);

	/* Instant tab switch - no animation to avoid full-screen redraw */
	lv_obj_set_style_anim_time(tabview, 0, 0);

	/* Pause/resume meter animations on tab switch */
	lv_obj_add_event_cb(tabview, tab_changed_cb, LV_EVENT_VALUE_CHANGED, NULL);

	lv_obj_set_style_bg_color(tabview, lv_palette_darken(LV_PALETTE_INDIGO, 2), 0);

	lv_obj_t * tab_btns = lv_tabview_get_tab_btns(tabview);
	lv_obj_set_style_bg_color(tab_btns, lv_palette_darken(LV_PALETTE_GREY, 3), 0);
	lv_obj_set_style_text_color(tab_btns, lv_palette_lighten(LV_PALETTE_GREY, 5), 0);
	lv_obj_set_style_border_side(tab_btns, LV_BORDER_SIDE_RIGHT, LV_PART_ITEMS | LV_STATE_CHECKED);


	lv_obj_t * tab1 = lv_tabview_add_tab(tabview, "Description");
	lv_obj_t * tab2 = lv_tabview_add_tab(tabview, "Analytics");
	lv_obj_t * tab3 = lv_tabview_add_tab(tabview, "BATTERY");
	lv_obj_t * tab4 = lv_tabview_add_tab(tabview, "LED");
	lv_obj_t * tab5 = lv_tabview_add_tab(tabview, "RS232");
	lv_obj_t * tab6 = lv_tabview_add_tab(tabview, "RS485");
	lv_obj_t * tab7 = lv_tabview_add_tab(tabview, "OUTPUT");
	lv_obj_t * tab8 = lv_tabview_add_tab(tabview, "INPUT");

//Оформление Tab1


	// Создание кнопки btn0
	lv_obj_t * button0 = lv_btn_create(tab1);
	lv_obj_set_size(button0, 100, 50);
	lv_obj_set_pos(button0, 10, 20);

	// Добавляем текст btn0
	lv_obj_t * label0 = lv_label_create(button0);
	lv_label_set_text(label0, "Btn0");
	lv_obj_center(label0);

	// Устанавливаем обводку btn0
	lv_color_t blue0 = lv_palette_main(LV_PALETTE_GREEN);
	lv_obj_set_style_outline_width(button0, 3, LV_PART_MAIN);
	lv_obj_set_style_outline_color(button0, blue0, LV_PART_MAIN);
	lv_obj_set_style_outline_pad(button0, 3, LV_PART_MAIN);

	// Создание кнопки btn1
	lv_obj_t * button1 = lv_btn_create(tab1);
	lv_obj_set_size(button1, 100, 50);
	lv_obj_set_pos(button1, 10, 90);

	// Добавляем текст btn1
	lv_obj_t * label1 = lv_label_create(button1);
	lv_label_set_text(label1, "Btn1");
	lv_obj_center(label1);

	// Устанавливаем обводку btn1
	lv_color_t blue1 = lv_palette_main(LV_PALETTE_YELLOW);
	lv_obj_set_style_outline_width(button1, 3, LV_PART_MAIN);
	lv_obj_set_style_outline_color(button1, blue1, LV_PART_MAIN);
	lv_obj_set_style_outline_pad(button1, 3, LV_PART_MAIN);

	// Создание кнопки btn2
	lv_obj_t * button2 = lv_btn_create(tab1);
	lv_obj_set_size(button2, 100, 50);
	lv_obj_set_pos(button2, 10, 160);

	// Добавляем текст btn2
	lv_obj_t * label2 = lv_label_create(button2);
	lv_label_set_text(label2, "Btn2");
	lv_obj_center(label2);

	// Устанавливаем обводку btn2
	lv_color_t blue2 = lv_palette_main(LV_PALETTE_RED);
	lv_obj_set_style_outline_width(button2, 3, LV_PART_MAIN);
	lv_obj_set_style_outline_color(button2, blue2, LV_PART_MAIN);
	lv_obj_set_style_outline_pad(button2, 3, LV_PART_MAIN);

	// Создание кнопки btn3
	lv_obj_t * button3 = lv_btn_create(tab1);
	lv_obj_set_size(button3, 100, 50);
	lv_obj_set_pos(button3, 10, 230);

	// Добавляем текст btn3
	lv_obj_t * label3 = lv_label_create(button3);
	lv_label_set_text(label3, "Btn3");
	lv_obj_center(label3);

	// Устанавливаем обводку btn3
	lv_color_t blue3 = lv_palette_main(LV_PALETTE_PURPLE);
	lv_obj_set_style_outline_width(button3, 3, LV_PART_MAIN);
	lv_obj_set_style_outline_color(button3, blue3, LV_PART_MAIN);
	lv_obj_set_style_outline_pad(button3, 3, LV_PART_MAIN);

	// Создание кнопки btn4
	lv_obj_t * button4 = lv_btn_create(tab1);
	lv_obj_set_size(button4, 100, 50);
	lv_obj_set_pos(button4, 10, 300);

	// Добавляем текст btn4
	lv_obj_t * label4 = lv_label_create(button4);
	lv_label_set_text(label4, "Btn4");
	lv_obj_center(label4);

	// Устанавливаем обводку btn4
	lv_color_t blue4 = lv_palette_main(LV_PALETTE_ORANGE);
	lv_obj_set_style_outline_width(button4, 3, LV_PART_MAIN);
	lv_obj_set_style_outline_color(button4, blue4, LV_PART_MAIN);
	lv_obj_set_style_outline_pad(button4, 3, LV_PART_MAIN);

//Создание слайдера

	meter = lv_meter_create(tab1);
    lv_obj_set_size(meter, 300, 300);
//  lv_obj_center(meter);
    lv_obj_set_pos(meter, 350, 5);

    /*Create a scale for the minutes*/
    /*61 ticks in a 360 degrees range (the last and the first line overlaps)*/
    lv_meter_scale_t * scale_min = lv_meter_add_scale(meter);
    lv_meter_set_scale_ticks(meter, scale_min, 61, 1, 10, lv_palette_main(LV_PALETTE_GREY));
    lv_meter_set_scale_range(meter, scale_min, 0, 60, 360, 270);

    /*Create another scale for the hours. It's only visual and contains only major ticks*/
    lv_meter_scale_t * scale_hour = lv_meter_add_scale(meter);
    lv_meter_set_scale_ticks(meter, scale_hour, 12, 0, 0, lv_palette_main(LV_PALETTE_GREY));               /*12 ticks*/
    lv_meter_set_scale_major_ticks(meter, scale_hour, 1, 2, 20, lv_color_black(), 10);    /*Every tick is major*/
    lv_meter_set_scale_range(meter, scale_hour, 1, 12, 330, 300);       /*[1..12] values in an almost full circle*/

    /*Add needle lines instead of images (much faster rendering)*/
    g_indic_min = lv_meter_add_needle_line(meter, scale_min, 3, lv_palette_main(LV_PALETTE_RED), -10);
    g_indic_hour = lv_meter_add_needle_line(meter, scale_min, 5, lv_color_black(), -30);
    g_scale_min = scale_min;

    /*Start meter animations (will be paused on tab switch)*/
    meter_anim_start();


//Оформление Tab2

	lv_obj_set_style_bg_color(tab2, lv_palette_lighten(LV_PALETTE_LIGHT_GREEN, 3), 0);
	lv_obj_set_style_bg_opa(tab2, LV_OPA_COVER, 0);

	lv_obj_t * label = lv_label_create(tab2);
	lv_label_set_text(label, "EXAMPLE CHART");

	chart = lv_chart_create(tab2);
	lv_obj_set_size(chart, 550, 350);
	lv_obj_align(chart, LV_ALIGN_CENTER, 0, -10);
	lv_chart_set_axis_tick(chart, LV_CHART_AXIS_PRIMARY_Y, 10, 5, 6, 5, true, 40);
	lv_chart_set_axis_tick(chart, LV_CHART_AXIS_PRIMARY_X, 10, 5, 10, 1, true, 30);
	lv_chart_set_point_count(chart, 200);

	ser = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_RED), LV_CHART_AXIS_PRIMARY_Y);
	ser2 = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_BLUE), LV_CHART_AXIS_PRIMARY_Y);

//Оформление Tab3
	lv_obj_set_style_bg_color(tab3, lv_palette_darken(LV_PALETTE_BLUE, 2), 0);
	lv_obj_set_style_bg_opa(tab3, LV_OPA_COVER, 0);
//Оформление Tab4
	lv_obj_set_style_bg_color(tab4, lv_palette_darken(LV_PALETTE_CYAN, 2), 0);
	lv_obj_set_style_bg_opa(tab4, LV_OPA_COVER, 0);
//Оформление Tab5
	lv_obj_set_style_bg_color(tab5, lv_palette_darken(LV_PALETTE_GREEN, 2), 0);
	lv_obj_set_style_bg_opa(tab5, LV_OPA_COVER, 0);
//Оформление Tab6
	lv_obj_set_style_bg_color(tab6, lv_palette_darken(LV_PALETTE_YELLOW, 2), 0);
	lv_obj_set_style_bg_opa(tab6, LV_OPA_COVER, 0);
//Оформление Tab7 (OUTPUT)
	lv_obj_set_style_bg_color(tab7, lv_color_white(), 0);
	lv_obj_set_style_bg_opa(tab7, LV_OPA_COVER, 0);

	lv_obj_t * label_out = lv_label_create(tab7);
	lv_label_set_text(label_out, "Leha zdorovo, kak dela u tebya?");
	lv_obj_set_style_text_color(label_out, lv_palette_main(LV_PALETTE_RED), 0);
	lv_obj_set_style_text_font(label_out, &lv_font_montserrat_40, 0);
	lv_obj_center(label_out);
//Оформление Tab8
	lv_obj_set_style_bg_color(tab8, lv_palette_darken(LV_PALETTE_PINK, 2), 0);
	lv_obj_set_style_bg_opa(tab8, LV_OPA_COVER, 0);

}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* Enable the CPU Cache */

  /* Enable I-Cache---------------------------------------------------------*/
  SCB_EnableICache();

  /* Enable D-Cache---------------------------------------------------------*/
  SCB_EnableDCache();

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_FMC_Init();
  MX_I2C4_Init();
  MX_LTDC_Init();
  MX_DMA2D_Init();
  /* USER CODE BEGIN 2 */
  SDRAM_InitSequence();
  LTDC_Init();
  GT911_Init();

  //Initialise LVGL UI library
  lv_init();
  lv_disp_draw_buf_init(&draw_buf, buf1, buf2, LVGL_BUF_SIZE);


  lv_disp_drv_init(&disp_drv);
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  disp_drv.hor_res = PIXELS_W;
  disp_drv.ver_res = PIXELS_H;
  lv_disp_drv_register(&disp_drv);

  lv_indev_drv_init(&indev_drv);             /*Basic initialization*/
  indev_drv.type = LV_INDEV_TYPE_POINTER;    /*Touch pad is a pointer-like device*/
  indev_drv.read_cb = my_touchpad_read;      /*Set your driver function*/
  lv_indev_drv_register(&indev_drv);         /*Finally register the driver*/

  /*Create a tabview*/
  //tabview();
  lv_demo_widgets();

  /*Create a spinner*/
//  lv_obj_t * spinner = lv_spinner_create(lv_scr_act(), 5000, 30);
//  lv_obj_set_size(spinner, 330, 330);
//  lv_obj_center(spinner);

//  lv_example_meter_my();
//  lv_example_get_started_my();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  while (1)
  {

	  uint32_t idle_time = lv_timer_handler();
	  if(idle_time > 0) HAL_Delay(idle_time);


    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 60;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
