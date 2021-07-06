#include "nrf_calendar.h"
#include <stdbool.h>
#include <nrf_rtc.h>
String now = "";

extern int just;

void print_time();
static bool time_update = true;

void setup() {
    Serial.begin(115200);
    Serial.println("System start...");
    nrf_cal_init();
    nrf_cal_set_callback(print_time, 2);
    nrf_cal_set_time(2021, 7, 5, 10, 5, 58);
    NVIC_EnableIRQ(CAL_RTC_IRQn);

    Serial.println(just);
}

void loop() {
//    if(just == 777) {
//        Serial.println(just);
////    }
//    if(time_update) {
//        Serial.println("HELL");
//        
//    }
    now = String(nrf_cal_get_time_string(true));
        Serial.println(now);
        Serial.println(nrf_time_to_unixtime());
    delay(1000);
//    Serial.println("Hello????");
//    delay(1000);
}

void print_time(void) {
    time_update = true;
}
