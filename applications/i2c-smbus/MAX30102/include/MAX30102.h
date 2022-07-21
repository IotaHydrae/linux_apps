#ifndef __MAX30102_H
#define __MAX30102_H

#define BIT(x) (1 << x)

/**
 * Register Map of MAX30102
 * 0x00 ~ 0xFF
 */

/* -------------- BASIC -------------- */
#define MAX30102_INTERRUPT_STATUS_1 (0x00)
#define MAX30102_INTERRUPT_STATUS_2 (0x01)
#define MAX30102_INTERRUPT_ENABLE_1 (0x02)
#define MAX30102_INTERRUPT_ENABLE_2 (0x03)

/* -------------- FIFO -------------- */
#define MAX30102_FIFO_WRITE_POINTER (0x04)
#define MAX30102_OVERFLOW_COUNTER   (0x05)
#define MAX30102_FIFO_READ_POINTER  (0x06)
#define MAX30102_FIFO_DATA_REGISTER (0x07)

/* -------------- CONFIG -------------- */
#define MAX30102_

#endif /* __MAX30102_H */