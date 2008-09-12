#ifndef __SPI_H_
#define  __SPI_H_

#include <asm/types.h>
#include <asm/arch/iomux.h>

typedef enum _bool{false,true} bool;

#define OFFSET_CSPI_RXDATAREG   0x00
#define OFFSET_CSPI_TXDATAREG   0x04
#define OFFSET_CSPI_CONTROLREG  0x08
#define OFFSET_CSPI_INTREG      0x0C


#define OFFSET_CSPI_DMAREG      0x10
#define OFFSET_CSPI_STATREG     0x14
#define OFFSET_CSPI_PERIODREG   0x18
#define OFFSET_CSPI_TESTREG     0x1C
#define OFFSET_CSPI_RESETREG 	0x0


#define MXC_CSPICONREG_XCH			0x4

#define _reg_CSPI(a)	(a) 
                    

#define SPBA_CPU_SPI            0x07

#define _reg_SPBA(a) (*((volatile unsigned long *) \
                     (IO_ADDRESS(a))))

#define MAX_BIT_COUNT           256

#define MODIFY_REGISTER_32(a,b,c) c = ( ( c & (~(a)) ) | b )

#define MIN_DATA_RATE_SHIFT         2
#define DATA_RATE_SHIFT            16
#define MODE_MASTER_SLAVE_SHIFT     1
#define EXCHANGE_BIT_SHIFT          2
#define SMC_CONTROL_SHIFT           3
#define STATE_ENABLE_DISABLE_SHIFT  0
#define SS_POLARITY_SHIFT           7
#define SS_WAVEFORM_SHIFT           6
#define PHASE_SHIFT                 5
#define POLARITY_SHIFT              4

#define DATA_RATE_MASK            0x7

#define INTERRUPT_IRQ_BIT_SHIFT     9
#define INTERRUPT_IRQ_BIT_SHIFT_0_7 8

#define BIT_COUNT_SHIFT             8
#define RDY_CONTROL_SHIFT          20
#define CHIP_SELECT_SHIFT          24
#define BIT_COUNT_MASK          0x01f

#define BIT_COUNT_SHIFT_0_5        20
#define RDY_CONTROL_SHIFT_0_5       8
#define CHIP_SELECT_SHIFT_0_5      12
#define BIT_COUNT_MASK_0_5     0x0fff

#define BIT_COUNT_SHIFT_0_0             0
#define RDY_CONTROL_SHIFT_0_0         12
#define CHIP_SELECT_SHIFT_0_0      19
#define BIT_COUNT_MASK_0_0          0x01f
#define INTERRUPT_IRQ_BIT_SHIFT_0_0 18

typedef enum {
	SPI_SLAVE = 0,
	SPI_MASTER = 1
} spi_mode_t;

typedef enum {
	SPI_SS_ACTIVE_LOW,
	SPI_SS_ACTIVE_HIGH
} spi_ss_polarity_t; 

typedef enum {
	SPI_LOW_BTN_BURST,
	SPI_PULSE_BTN_BURST
} spi_ss_waveform_t;

typedef enum {
	SPI_PHASE_0,
	SPI_PHASE_1
} spi_phase_t;

typedef enum {
	SPI_POLARITY_ACTIVE_LOW,
	SPI_POLARITY_ACTIVE_HIGH
} spi_polarity_t;

typedef struct {
	int module_number;
	__u32 bit_rate;
	__u32 bit_count;
	bool master_mode;
	bool active_high_polarity;
	bool phase;
	bool active_high_ss_polarity;
	bool ss_low_between_bursts;
	int ss_asserted;
} spi_config;

#endif
