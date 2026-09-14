#ifndef _HAL_H_
#define _HAL_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

enum CAN_ERRORS {
    NO_FAULTS = 0b0, /**< No faults recognized. */
    CELL_OVER_VOLTAGE =
        0b000001, /**< Thrown when a single cell’s voltage is above 4.2V. */
    CELL_UNDER_VOLTAGE =
        0b000010, /**< Thrown when a single cell’s voltage is below 2.5v. */
    CELL_OVER_TEMPERATURE =
        0b000100, /**< Thrown when a single cell’s temperature is above 60°C. */
    CELL_DELTA_EXCEEDED = 0b001000, /**< Thrown when the maximum minus minimum
                                      cell voltage is above 0.2V */
    PACK_OVER_CURRENT =
        0b010000, /**< Thrown when the current sensor reads above 200A. */
    CURRENT_SENSOR_FAULT =
        0b100000, /**< Thrown when the current sensor fails to read. */

};

#define CAN_LEN 8 // A CAN message has 8 data bytes
#define N_CELLS 130

#define OVER_VOLTAGE_LIMIT 4.2
#define UNDER_VOLTAGE_LIMIT 2.5
#define DELTA_VOLTAGE_LIMIT 0.2

#define OVER_TEMPERATURE_LIMIT 60

#define OVER_CURRENT_LIMIT 200

struct CAN_FRAME {
    uint16_t id;
    uint8_t data[CAN_LEN];
};

bool ISENSE_GetCurrent(const struct CAN_FRAME *frame, int32_t *out_current);
bool ISENSE_IsCharging(const struct CAN_FRAME *frame, bool *is_charging);

bool ISENSE_CurrentFaultDetection(const int32_t current, uint8_t *faults);

bool CELL_VoltageFaultDetection(const float cell_voltages[N_CELLS],
                                uint8_t *faults);

bool CELL_TemperatureFaultDetection(const float cell_temperatures[N_CELLS],
                                    uint8_t *faults);

void Manage_Fault(uint8_t *faults, uint8_t *latched_faults,
                  volatile bool *fault_clear_request);

// Reads the voltage data for all the cells in volts
void HAL_ReadVoltages(float data[N_CELLS]);

// Reads the temperature data for all the cells in deg C
void HAL_ReadTemperatures(float data[N_CELLS]);

// Sets the state of the Shutdown Circuit
void HAL_SetSDC(bool closed);

// Sets a status LED to an RGB color (0-255 each)
void HAL_SetLED(uint8_t r, uint8_t g, uint8_t b);

// Send a CAN message onto the bus
void HAL_SendCanMsg(uint16_t id, const uint8_t data[CAN_LEN]);

// Read a CAN message from the bus.
// The ID is written into *id and the data into the buffer.
void HAL_RecvCanMsg(uint16_t *id, uint8_t data[CAN_LEN]);

// Gets the time in milliseconds since the controller was powered on.
uint32_t HAL_GetMS(void);

#endif
