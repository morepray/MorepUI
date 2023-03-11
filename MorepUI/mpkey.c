#include "mpkey.h"
#include "oled.h"

static pmpkey_t head = NULL, tail = NULL;
static uint8_t ITPeriod = 1; // Ts = 1ms

void MpKey_Init(pmpkey_t key)
{
    key->status = Release;
    key->next = NULL;
    if (head == NULL)
    {
        head = key;
        tail = key;
    }
    else
    {
        // 使用next连接每一个Key
        tail->next = key;
        tail = tail->next;
    }
    key->isPressed = 0;
    key->isHoled = 0;
}

mpkey_t Key1, Key2;

__weak void MpKeyScanLoop(pmpkey_t key)
{
    if (key == &Key1)
        key->value = HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin);
    if (key == &Key2)
        key->value = HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin);
    key->prevalue = key->value;
}

__weak void MpKeyPressCallback(mpkey_t *key)
{
    key->isPressed = 1;
}
__weak void MpKeyLongHoldCallback(mpkey_t *key)
{
}
__weak void MpKeyFirstlongHoldCallback(mpkey_t *key)
{
    key->isHoled = 1;
}
__weak void MpKeylongReleaseCallback(mpkey_t *key)
{
}

void MpKeyLoop(void)
{
    // 轮询按键
    for (pmpkey_t key = head; key != NULL; key = key->next)
    {
        // 每一个按键都扫描
        MpKeyScanLoop(key);
        // key->value未按下, !key->value按下
        if (key->value)
            key->PressTime = 0;
        if ((!key->prevalue) & (!key->value))
        {
            key->PressTime += ITPeriod;
        }

        switch (key->status)
        {
        case Release:
            if (!key->value)
                key->status = Prepress;
            break;

        case Prepress:
            if (key->value)
                key->status = Release;
            else if (key->PressTime > ClickMinTime)
            {
                key->status = Prelong;
            }
            break;

        case Prelong:
        {
            if (key->value)
            {
                key->status = Release;
                MpKeyPressCallback(key);
            }
            else if (key->PressTime > LongpressMinTime)
            {
                key->status = Longpress;
                key->LongHoldToggle = LongpressToggleTime;
                MpKeyFirstlongHoldCallback(key);
            }
        }
        break;

        case Longpress:
            if (key->LongHoldToggle > 0)
                key->LongHoldToggle -= ITPeriod;
            else
            {
                key->LongHoldToggle = LongpressToggleTime;
                MpKeyLongHoldCallback(key);
            }
            if (key->value)
            {
                key->status = Release;
                MpKeylongReleaseCallback(key);
            }
            break;

        default:
#ifdef USE_HAL_DRIVER
            Error_Handler();
#endif
            break;
        }
    }
}
