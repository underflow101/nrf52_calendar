#include "nrf_calendar.h"
#include "nrf.h"

int just = 0;
 
static struct tm time_struct, m_tm_return_time; 
static time_t m_time, m_last_calibrate_time = 0;
static float m_calibrate_factor = 0.0f;
static uint32_t m_rtc_increment = 60;
static void (*cal_event_callback)(void) = 0;
 
void nrf_cal_init()
{
//    NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
//    NRF_CLOCK->TASKS_HFCLKSTART = 1;
//    while(NRF_CLOCK->EVENTS_HFCLKSTARTED == 0);
    
    // Select the 32 kHz crystal and start the 32 kHz clock
    NRF_CLOCK->LFCLKSRC = CLOCK_LFCLKSRC_SRC_Xtal << CLOCK_LFCLKSRC_SRC_Pos;
    NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;
    NRF_CLOCK->TASKS_LFCLKSTART = 1;
    while(NRF_CLOCK->EVENTS_LFCLKSTARTED == 0);
    
    // Configure the RTC for 1 minute wakeup (default)
    //CAL_RTC->PRESCALER = 0xFFF;
    CAL_RTC->PRESCALER = 32767;
    CAL_RTC->EVTENSET = RTC_EVTENSET_COMPARE0_Msk;
    CAL_RTC->INTENSET = RTC_INTENSET_COMPARE0_Msk;
    CAL_RTC->CC[0] = m_rtc_increment * 8;
    CAL_RTC->TASKS_START = 1;

    NVIC_SetPriority(CAL_RTC_IRQn, CAL_RTC_IRQ_Priority);
    NVIC_EnableIRQ(CAL_RTC_IRQn);
}

void nrf_cal_set_callback(void (*callback)(void), uint32_t interval)
{
    // Set the calendar callback, and set the callback interval in seconds
    cal_event_callback = callback;
    m_rtc_increment = interval;
    m_time += CAL_RTC->COUNTER / 8;
    CAL_RTC->TASKS_CLEAR = 1;
    CAL_RTC->CC[0] = interval * 8;
}
 
void nrf_cal_set_time(uint32_t year, uint32_t month, uint32_t day, uint32_t hour, uint32_t minute, uint32_t second)
{
    static time_t uncal_difftime, difftime, newtime;
    time_struct.tm_year = year - 1900;
    time_struct.tm_mon = month;
    time_struct.tm_mday = day;
    time_struct.tm_hour = hour;
    time_struct.tm_min = minute;
    time_struct.tm_sec = second;
    newtime = mktime(&time_struct);
    CAL_RTC->TASKS_CLEAR = 1;  
    
    // Calculate the calibration offset 
    if(m_last_calibrate_time != 0)
    {
        difftime = newtime - m_last_calibrate_time;
        uncal_difftime = m_time - m_last_calibrate_time;
        m_calibrate_factor = (float)difftime / (float)uncal_difftime;
    }
    
    // Assign the new time to the local time variables
    m_time = m_last_calibrate_time = newtime;
}    

struct tm *nrf_cal_get_time()
{
    time_t return_time;
    return_time = m_time + CAL_RTC->COUNTER / 8;
    m_tm_return_time = *localtime(&return_time);
    return &m_tm_return_time;
}

struct tm *nrf_cal_get_time_calibrated()
{
    time_t uncalibrated_time, calibrated_time;
    if(m_calibrate_factor != 0.0f)
    {
        uncalibrated_time = m_time + CAL_RTC->COUNTER / 8;
        calibrated_time = m_last_calibrate_time + (time_t)((float)(uncalibrated_time - m_last_calibrate_time) * m_calibrate_factor + 0.5f);
        m_tm_return_time = *localtime(&calibrated_time);
        return &m_tm_return_time;
    }
    else return nrf_cal_get_time();
}

char *nrf_cal_get_time_string(bool calibrated)
{
    static char cal_string[80];
    strftime(cal_string, 80, "%x - %H:%M:%S", (calibrated ? nrf_cal_get_time_calibrated() : nrf_cal_get_time()));
    return cal_string;
}

int nrf_time_to_unixtime() {
    struct tm unix_converter;
    unix_converter.tm_year = time_struct.tm_year;
    unix_converter.tm_mon = time_struct.tm_mon - 1;
    unix_converter.tm_mday = time_struct.tm_mday;
    unix_converter.tm_hour = time_struct.tm_hour;
    unix_converter.tm_min = time_struct.tm_min;
    unix_converter.tm_sec = time_struct.tm_sec + CAL_RTC->COUNTER / 8;
    int newtime = mktime(&unix_converter);
    
    return newtime;
}
 
void CAL_RTC_IRQHandler(void)
{
    if(CAL_RTC->EVENTS_COMPARE[0])
    {
        //just = 777;
        CAL_RTC->EVENTS_COMPARE[0] = 0;
        CAL_RTC->TASKS_CLEAR = 1;
        
        m_time += m_rtc_increment;
        
//       if(cal_event_callback) {
//           cal_event_callback();
//       }
       cal_event_callback();
    }
}
