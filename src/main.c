#include "./BMS.c"
#include <stdio.h>

typedef struct {
    uint64_t current_tick_ms;
    uint64_t last_iter_tick;
    uint64_t iter_period_ms;
} TickEngine;

typedef struct CAN_FRAME CanFrame_t;

bool physical_bus_interrupt_pending = false;
CanFrame_t pending_hardware_frame;

void Simulate_Bus_Interrupts(uint64_t current_tick_ms) {
    if (current_tick_ms != 15)
        return;

    pending_hardware_frame.id = ISENSE_ID;
    pending_hardware_frame.data[2] = 1;
    pending_hardware_frame.data[3] = 1;
    pending_hardware_frame.data[4] = 1;
    pending_hardware_frame.data[5] = 1;

    physical_bus_interrupt_pending = true;

    int32_t current;
    ISENSE_GetCurrent(&pending_hardware_frame, &current);
    printf("%d A", current);
}

int main() {
    Simulate_Bus_Interrupts(15);

    return 0;
}
