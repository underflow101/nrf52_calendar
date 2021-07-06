#include "nrf_calendar.h"
#include <stdbool.h>
#include <nrf_rtc.h>
#include <FreeRTOS.h>

String now = "";

TaskHandle_t sm_task;

void print_time();
static bool time_update = true;
static SoftwareTimer timer_counter;
int time_now = 0;

void setup() {
    Serial.begin(115200);
    Serial.println("System start...");
    nrf_cal_init();
    nrf_cal_set_callback(print_time, 2);
    nrf_cal_set_time(2021, 7, 5, 10, 5, 58);

    timer_counter.begin(1000, _timer_handler);
    timer_counter.start();

    xTaskCreate(
        io_sm_task
        ,   "TaskStateMachine"
        ,   4000
        ,   NULL
        ,   2
        ,   &sm_task);
}

void loop() {
    now = String(nrf_cal_get_time_string(true));
    Serial.println(now);
    Serial.println(nrf_time_to_unixtime());
    vTaskDelay(1000);
}

void print_time(void) {
    time_update = true;
}

void _timer_handler(TimerHandle_t xTimerID)
{
    (void) xTimerID;
    time_now++;

    if(time_now == 4) {
        sd_power_mode_set(NRF_POWER_MODE_LOWPWR);
        __WFI();
    }
}

void io_sm_task(void *param)
{
    while(true)
    {
        Serial.println("TESTING...");
        vTaskDelay(1000);
    }
}
