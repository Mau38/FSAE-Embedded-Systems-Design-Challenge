#include "HAL.h"
#include <stddef.h>

bool ISENSE_GetCurrent(const struct CAN_FRAME *frame, int32_t *out_current) {
    if (sizeof(frame->data) != CAN_LEN) {
        return false;
    }
    *out_current = (int32_t)(((uint32_t)(frame->data[2]) << 24) |
                             ((uint32_t)(frame->data[3]) << 16) |
                             ((uint32_t)(frame->data[4]) << 8) |
                             ((uint32_t)(frame->data[5])));
    return true;
}

bool ISENSE_CurrentFaultDetection(const int32_t current, uint8_t *faults) {
    *faults |= current > OVER_CURRENT_LIMIT ? PACK_OVER_CURRENT : NO_FAULTS;
    return true;
}

bool ISENSE_IsCharging(const struct CAN_FRAME *frame, bool *is_charging) {
    int32_t current;
    if (!ISENSE_GetCurrent(frame, &current))
        return false;
    *is_charging = current <= 0;
    return true;
}

bool CELL_IsAllFaultsActive(const uint8_t *faults) {
    return (*faults &
            (CELL_OVER_VOLTAGE | CELL_UNDER_VOLTAGE | CELL_DELTA_EXCEEDED)) ==
           (CELL_OVER_VOLTAGE | CELL_UNDER_VOLTAGE | CELL_DELTA_EXCEEDED);
}

bool CELL_VoltageFaultDetection(const float cell_voltages[N_CELLS],
                                uint8_t *faults) {
    if (N_CELLS == 0)
        return false;
    float peak_cell_voltage = cell_voltages[0];
    float trough_cell_voltage = cell_voltages[0];

    for (size_t cell_index = 0; cell_index < N_CELLS; cell_index++) {
        if (CELL_IsAllFaultsActive(faults)) {
            return true;
        }

        const float cell_voltage = cell_voltages[cell_index];

        if (peak_cell_voltage < cell_voltage)
            peak_cell_voltage = cell_voltage;

        if (trough_cell_voltage > cell_voltage)
            trough_cell_voltage = cell_voltage;

        if (peak_cell_voltage > OVER_VOLTAGE_LIMIT)
            *faults |= CELL_OVER_VOLTAGE;
        if (trough_cell_voltage < UNDER_VOLTAGE_LIMIT)
            *faults |= CELL_UNDER_VOLTAGE;
        if (peak_cell_voltage - trough_cell_voltage > DELTA_VOLTAGE_LIMIT)
            *faults |= CELL_DELTA_EXCEEDED;
    }
    return true;
}

bool CELL_TemperatureFaultDetection(const float cell_temperatures[N_CELLS],
                                    uint8_t *faults) {
    if (N_CELLS == 0)
        return false;

    for (size_t cell_index = 0; cell_index < N_CELLS; cell_index++) {
        if (cell_temperatures[cell_index] > OVER_TEMPERATURE_LIMIT)
            *faults |= CELL_OVER_TEMPERATURE;
    }

    return true;
}

void Manage_Fault(uint8_t *faults, uint8_t *latched_faults,
                  volatile bool *fault_clear_request) {
    if (*latched_faults != NO_FAULTS) {
        HAL_SetSDC(false);
    }
    if (*fault_clear_request) {
        *fault_clear_request = false;
        *latched_faults = NO_FAULTS;
        HAL_SetSDC(true);
    }
}
