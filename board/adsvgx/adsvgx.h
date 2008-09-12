/*
 *  linux/include/asm-arm/arch-pxa/adsvgx.h
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Updated for u-boot by Robert Whaley 12/07/2006
 *
 */

#ifndef ASM_ARCH_ADSVGX_H
#define ASM_ARCH_ADSVGX_H

#define ADSVGX_CPLD_BASE		PXA_CS2_PHYS
#define ADSVGX_CPLD_VBASE		0xf1000000
#define ADSVGX_p2v( x )		((x) - ADSVGX_CPLD_BASE + ADSVGX_CPLD_VBASE)
#define ADSVGX_v2p( x )		((x) - ADSVGX_CPLD_VBASE + ADSVGX_CPLD_BASE)

#define _ADSVGX_CPLD_SPI_TX         (ADSVGX_CPLD_BASE + 0x20000)
#define _ADSVGX_CPLD_SPI_RX         (ADSVGX_CPLD_BASE + 0x20004)
#define _ADSVGX_CPLD_SPI_EN         (ADSVGX_CPLD_BASE + 0x20008)
#define _ADSVGX_CPLD_DDR            (ADSVGX_CPLD_BASE + 0x40000)
#define _ADSVGX_CPLD_DRR            (ADSVGX_CPLD_BASE + 0x40004)
#define _ADSVGX_CPLD_DWR            (ADSVGX_CPLD_BASE + 0x40004)
#define _ADSVGX_CPLD_CR0            (ADSVGX_CPLD_BASE + 0x60000)
#define _ADSVGX_CPLD_CR1            (ADSVGX_CPLD_BASE + 0x60004)
#define _ADSVGX_CPLD_CR2            (ADSVGX_CPLD_BASE + 0x60008)
#define _ADSVGX_CPLD_CR3            (ADSVGX_CPLD_BASE + 0x6000C)
#define _ADSVGX_CPLD_IRQ_EN         (ADSVGX_CPLD_BASE + 0x80000)
#define _ADSVGX_CPLD_IRQ_FL         (ADSVGX_CPLD_BASE + 0x80008)
#define _ADSVGX_CPLD_PCMCIA_CR      (ADSVGX_CPLD_BASE + 0xA0000)
#define _ADSVGX_CPLD_PCMCIA_IRQ_EN  (ADSVGX_CPLD_BASE + 0xA0004)
#define _ADSVGX_CPLD_PCMCIA_IRQ_FL  (ADSVGX_CPLD_BASE + 0xA0008)
#define _ADSVGX_CPLD_PCMCIA_SR      (ADSVGX_CPLD_BASE + 0xA000C)
#define _ADSVGX_CPLD_UART1          (ADSVGX_CPLD_BASE + 0xC0000)
#define _ADSVGX_CPLD_UART2          (ADSVGX_CPLD_BASE + 0xC0020)
#define _ADSVGX_CPLD_UART3          (ADSVGX_CPLD_BASE + 0x00000)
#define _ADSVGX_CPLD_UART4          (ADSVGX_CPLD_BASE + 0x00020)
#define _ADSVGX_CPLD_CAN_ADDR       (ADSVGX_CPLD_BASE + 0xE0000)
#define _ADSVGX_CPLD_CAN_DATA       (ADSVGX_CPLD_BASE + 0xE0004)
#define _ADSVGX_CPLD_CAN2_ADDR      (ADSVGX_CPLD_BASE + 0xE0008)
#define _ADSVGX_CPLD_CAN2_DATA      (ADSVGX_CPLD_BASE + 0xE000C)

#ifndef __ASSEMBLY__
#define ADSVGX_CPLD_SPI_TX          (*((volatile u_char *)(_ADSVGX_CPLD_SPI_TX)))
#define ADSVGX_CPLD_SPI_RX          (*((volatile u_char *)(_ADSVGX_CPLD_SPI_RX)))
#define ADSVGX_CPLD_SPI_EN          (*((volatile u_char *)(_ADSVGX_CPLD_SPI_EN)))
#define ADSVGX_CPLD_DDR             (*((volatile u_char *)(_ADSVGX_CPLD_GPDR)))
#define ADSVGX_CPLD_DRR             (*((volatile u_char *)(_ADSVGX_CPLD_GPSR)))
#define ADSVGX_CPLD_DWR             (*((volatile u_char *)(_ADSVGX_CPLD_GPSR)))
#define ADSVGX_CPLD_CR0             (*((volatile u_char *)(_ADSVGX_CPLD_CR0)))
#define ADSVGX_CPLD_CR1             (*((volatile u_char *)(_ADSVGX_CPLD_CR1)))
#define ADSVGX_CPLD_CR2             (*((volatile u_char *)(_ADSVGX_CPLD_CR2)))
#define ADSVGX_CPLD_CR3             (*((volatile u_char *)(_ADSVGX_CPLD_CR3)))
#define ADSVGX_CPLD_REV             (*((volatile u_char *)(_ADSVGX_CPLD_REV)))
#define ADSVGX_CPLD_IRQ_EN          (*((volatile u_char *)(_ADSVGX_CPLD_IRQ_EN)))
#define ADSVGX_CPLD_IRQ_FL          (*((volatile u_char *)(_ADSVGX_CPLD_IRQ_FL)))
#define ADSVGX_CPLD_PCMCIA_CR       (*((volatile u_char *)(_ADSVGX_CPLD_PCMCIA_CR)))
#define ADSVGX_CPLD_PCMCIA_IRQ_EN   (*((volatile u_char *)(_ADSVGX_CPLD_PCMCIA_IRQ_EN)))
#define ADSVGX_CPLD_PCMCIA_IRQ_FL   (*((volatile u_char *)(_ADSVGX_CPLD_PCMCIA_IRQ_FL)))
#define ADSVGX_CPLD_PCMCIA_SR       (*((volatile u_char *)(_ADSVGX_CPLD_PCMCIA_SR)))
#define ADSVGX_CPLD_UART1           (*((volatile u_char *)(_ADSVGX_CPLD_UART1)))
#define ADSVGX_CPLD_UART2           (*((volatile u_char *)(_ADSVGX_CPLD_UART2)))
#define ADSVGX_CPLD_UART3           (*((volatile u_char *)(_ADSVGX_CPLD_UART3)))
#define ADSVGX_CPLD_UART4           (*((volatile u_char *)(_ADSVGX_CPLD_UART4)))
#define ADSVGX_CPLD_CAN_ADDR        (*((volatile u_char *)(_ADSVGX_CPLD_CAN_ADDR)))
#define ADSVGX_CPLD_CAN_DATA        (*((volatile u_char *)(_ADSVGX_CPLD_CAN_DATA)))
#define ADSVGX_CPLD_CAN2_ADDR       (*((volatile u_char *)(_ADSVGX_CPLD_CAN2_ADDR)))
#define ADSVGX_CPLD_CAN2_DATA       (*((volatile u_char *)(_ADSVGX_CPLD_CAN2_DATA)))
#endif

/* IO bits */

#define ADSVGX_CPLD_SPI_ENABLE     0x01

#define ADSVGX_CPLD_CR0_SW0        0x01
#define ADSVGX_CPLD_CR0_SW1        0x02
#define ADSVGX_CPLD_CR0_SW2        0x04
#define ADSVGX_CPLD_CR0_SW3        0x08

#define ADSVGX_CPLD_CR1_AMP        0x01
#define ADSVGX_CPLD_CR1_CODEC      0x02
#define ADSVGX_CPLD_CR1_BTL        0x04
#define ADSVGX_CPLD_CR1_H_WKUP     0x08
#define ADSVGX_CPLD_CR1_COM3EN     0x10
#define ADSVGX_CPLD_CR1_IRDAON     0x20
#define ADSVGX_CPLD_CR1_COM2EN     0x40
#define ADSVGX_CPLD_CR1_COM1EN     0x80

#define ADSVGX_CPLD_CR2_FW         0x0F
#define ADSVGX_CPLD_CR2_HW         0x30
#define ADSVGX_CPLD_CR2_IRQ_CAN2   0x40

#define ADSVGX_CPLD_CR3_PNLON      0x01
#define ADSVGX_CPLD_CR3_VEECRL     0x02
#define ADSVGX_CPLD_CR3_BLON       0x04
#define ADSVGX_CPLD_CR3_GBLON      0x08
#define ADSVGX_CPLD_CR3_LVDSEN     0x10
#define ADSVGX_CPLD_CR3_SPISEL     0x60
#define ADSVGX_CPLD_CR3_DRESETP    0x80

#define ADSVGX_CPLD_IRQ_SSP        0x01
#define ADSVGX_CPLD_IRQ_CAN        0x02
#define ADSVGX_CPLD_IRQ_ETH        0x04
#define ADSVGX_CPLD_IRQ_EXT        0x08
#define ADSVGX_CPLD_IRQ_UART1      0x10
#define ADSVGX_CPLD_IRQ_UART2      0x20
#define ADSVGX_CPLD_IRQ_UART3      0x40
#define ADSVGX_CPLD_IRQ_UART4      0x80

#define ADSVGX_CPLD_PCMCIA_CR_B_3V 0x01
#define ADSVGX_CPLD_PCMCIA_CR_B_5V 0x02
#define ADSVGX_CPLD_PCMCIA_CR_A_3V 0x04
#define ADSVGX_CPLD_PCMCIA_CR_A_5V 0x08
#define ADSVGX_CPLD_PCMCIA_CR_B_RS 0x10
#define ADSVGX_CPLD_PCMCIA_CR_A_RS 0x20

#define ADSVGX_CPLD_PCMCIA_IRQ_B_DET 0x04
#define ADSVGX_CPLD_PCMCIA_IRQ_A_DET 0x08
#define ADSVGX_CPLD_PCMCIA_IRQ_B_IRQ 0x10
#define ADSVGX_CPLD_PCMCIA_IRQ_A_IRQ 0x20

#define ADSVGX_CPLD_PCMCIA_IRQ_MASK 0x3C

#define ADSVGX_CPLD_PCMCIA_STA_B_IRQ 0x01
#define ADSVGX_CPLD_PCMCIA_STA_A_IRQ 0x02
#define ADSVGX_CPLD_PCMCIA_STA_B_VS1 0x04
#define ADSVGX_CPLD_PCMCIA_STA_B_VS2 0x08
#define ADSVGX_CPLD_PCMCIA_STA_B_DET 0x10
#define ADSVGX_CPLD_PCMCIA_STA_A_DET 0x20
#define ADSVGX_CPLD_PCMCIA_STA_A_VS1 0x40
#define ADSVGX_CPLD_PCMCIA_STA_A_VS2 0x80


#define ADSVGX_ETH_BASE         	PXA_CS4_PHYS
#define ADSVGX_ETH_VBASE		0xf0000000

#endif

