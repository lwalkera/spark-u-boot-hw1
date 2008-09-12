/*
 *  adssphere.h
 *
 *  Copyright (C) 2005 Robert Whaley <rwhaley@applieddata.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#ifndef ASM_ARCH_ADSSPHERE_H
#define ASM_ARCH_ADSSPHERE_H

#define ADSSPHERE_BASE_PHYS  0xf0000000

#define ADSSPHERE_CR0       (ADSSPHERE_BASE_PHYS+0x00)
#define ADSSPHERE_CR1       (ADSSPHERE_BASE_PHYS+0x04)
#define ADSSPHERE_SR        (ADSSPHERE_BASE_PHYS+0x08)
#define ADSSPHERE_CAN_ADDR  (ADSSPHERE_BASE_PHYS+0x0c)
#define ADSSPHERE_CAN_DATA  (ADSSPHERE_BASE_PHYS+0x10)
#define ADSSPHERE_FW_FW_NUM (ADSSPHERE_BASE_PHYS+0x14)
#define ADSSPHERE_FW_FW_REV (ADSSPHERE_BASE_PHYS+0x18)
#define ADSSPHERE_FW_BD_REV (ADSSPHERE_BASE_PHYS+0x1c)

#define ADSSPHERE_CR0_CF_33 0x01
#define ADSSPHERE_CR0_CF_50 0x02
#define ADSSPHERE_CR0_COMEN 0x04
#define ADSSPHERE_CR0_IRDA  0x08
#define ADSSPHERE_CR0_USB   0x10
#define ADSSPHERE_CR0_PWSAV 0x20

#define ADSSPHERE_CR1_PNL_ON 0x01
#define ADSSPHERE_CR1_PNL_EN 0x04
#define ADSSPHERE_CR1_BL_ON  0x08
#define ADSSPHERE_CR1_AMP_ON 0x10
#define ADSSPHERE_CR1_CDC_ON 0x20
#define ADSSPHERE_CR1_BTL_ON 0x40

#define ADSSPHERE_SR1_SW0 0x01
#define ADSSPHERE_SR1_SW1 0x02
#define ADSSPHERE_SR1_SW2 0x04
#define ADSSPHERE_SR1_SW3 0x08
#define ADSSPHERE_SR1_HP  0x10

#endif /* ASM_ARCH_ADSSPHERE_H */
