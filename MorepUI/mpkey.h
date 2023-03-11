#ifndef __MPKEY_H__
#define __MPKEY_H__

#include "main.h"

#ifdef USE_HAL_DRIVER
#include "gpio.h"
#endif

#define ClickMinTime 30
#define LongpressMinTime 450
#define LongpressToggleTime 300

typedef struct mpkey
{
    uint8_t value : 1;
    uint8_t prevalue : 1;
    uint32_t PressTime;
    uint32_t LongHoldToggle;
    enum
    {
        Release = 0,
        Prepress,
        Prelong,
        Longpress
    } status;
    struct mpkey *next;
    uint8_t isPressed;
    uint8_t isHoled;
} mpkey_t, *pmpkey_t;

extern mpkey_t Key1, Key2;

void MpKey_Init(pmpkey_t key);
void MpKeyScanLoop(pmpkey_t key);
void MpKeyLoop(void);
void MpKeyPressCallback(mpkey_t *key);
void MpKeyLongHoldCallback(mpkey_t *key);
void MpKeyFirstlongHoldCallback(mpkey_t *key);
void MpKeylongReleaseCallback(mpkey_t *key);

#endif
