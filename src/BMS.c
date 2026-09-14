#include "HAL.h"

#define ISENSE_ID 0x511
#define DIAGNOSTIC_HEARTBEAT_ID 0x1CD
#define FAULTS_ID 0x0B1
#define FAULTS_CLEAR_ID 0x1CF

uint8_t active_faults;
uint8_t latched_faults;
volatile bool fault_clear_requested;

void Init() {
    // This function runs once on startup
    HAL_SetSDC(false);
    latched_faults = active_faults = NO_FAULTS;
}

void Iter() {
    // This function runs periodically at ~20Hz
    struct CAN_FRAME frame;

    active_faults = NO_FAULTS;

    HAL_RecvCanMsg(&frame.id, frame.data);

    float cell_voltages[N_CELLS];
    float cell_temperatures[N_CELLS];

    HAL_ReadVoltages(cell_voltages);
    HAL_ReadTemperatures(cell_temperatures);

    CELL_VoltageFaultDetection(cell_voltages, &active_faults);
    CELL_TemperatureFaultDetection(cell_temperatures, &active_faults);

    switch (frame.id) {
    case ISENSE_ID: {
        int32_t current = 0;
        if (!ISENSE_GetCurrent(&frame, &current)) {
            active_faults |= CURRENT_SENSOR_FAULT;
            break;
        }

        ISENSE_CurrentFaultDetection(current, &active_faults);

        bool is_charging;
        if (ISENSE_IsCharging(&frame, &is_charging) && is_charging) {
            // What to do with charging information
        }
        break;
    }
    case DIAGNOSTIC_HEARTBEAT_ID: {
        break;
    }
    case FAULTS_CLEAR_ID: {
        break;
    }
    }

    latched_faults |= active_faults;

    Manage_Fault(&active_faults, &latched_faults, &fault_clear_requested);

    uint8_t fault_data_packet[CAN_LEN] = {0};
    fault_data_packet[0] = latched_faults;
    fault_data_packet[1] = active_faults;
    HAL_SendCanMsg(FAULTS_ID, fault_data_packet);
}

void RxCan() {
    // Called every time a CAN frame is received on the bus, using an interrupt.
    // Keep in mind, this can be called at any point in the execution of your
    // program. You may not use any HAL_* functions here except HAL_RecvCanMsg,
    // which is how you can pull the message from the bus.
    // An example for pulling a CAN frame is shown below.

    struct CAN_FRAME frame;
    HAL_RecvCanMsg(&frame.id, frame.data);
    if (frame.id == FAULTS_CLEAR_ID) {
        fault_clear_requested = true;
    }
}
