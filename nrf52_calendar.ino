#include "nrf_calendar.h"

String now = "";

void setup() {
    Serial.begin(115200);
    Serial.println("System start...");
    nrf_cal_init();
    nrf_cal_set_time(2021, 7, 5, 10, 5, 5);
}

void loop() {
    now = String(nrf_cal_get_time_string(true));
    Serial.println(now);
    delay(1000);
}
