#include "../include/HAL.h"
#include <stdint.h>

void HAL_ReadVoltages(float data[N_CELLS]) {}

void HAL_ReadTemperatures(float data[N_CELLS]) {}

void HAL_SetSDC(bool closed) {}

void HAL_SetLED(uint8_t r, uint8_t g, uint8_t b) {}

void HAL_SendCanMsg(uint16_t id, const uint8_t data[N_CELLS]) {}

void HAL_RecvCanMsg(uint16_t *id, uint8_t data[N_CELLS]) {}

uint32_t HAL_GetMS() {}
