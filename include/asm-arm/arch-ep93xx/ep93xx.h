/* Cirrus LogicEP9312 header file
 * From Cirrus Spec Revision 0.14
 * Copyright 2002 2003
 * Adam Bezanson, Network Audio Technologies, Inc.
 * bezanson@netaudiotech.com
 * 12/26/02
 */

/* UART 1 REGISTERS */
#define rUART1_DR			(*(volatile unsigned *)0x808c0000)
#define rUART1_RSR			(*(volatile unsigned *)0x808c0004)
#define rUART1_LCR_H			(*(volatile unsigned *)0x808c0008)
#define rUART1_LCR_M			(*(volatile unsigned *)0x808c000c)
#define rUART1_LCR_L			(*(volatile unsigned *)0x808c0010)
#define rUART1_CR   			(*(volatile unsigned *)0x808c0014)
#define rUART1_FR   			(*(volatile unsigned *)0x808c0018)
#define rUART1_IIR   			(*(volatile unsigned *)0x808c001c)

/* SYSCON Regs */
#define SysconCLKSET1 		(*(volatile unsigned *)0x80930020)
#define SysconCLKSET1VAL 	0x02a4a3d7
#define SysconCLKSET2 		(*(volatile unsigned *)0x80930024)
#define SysconCLKSET2VAL 	0x700CC317
#define SysconDEVCFG 		(*(volatile unsigned *)0x80930080)
#define SysconCHIP_ID		(*(volatile unsigned *)0x80930094)
#define SysconCFG 		(*(volatile unsigned *)0x8093009C)
#define SysconSWLock		(*(volatile unsigned *)0x809300C0)
#define SysconPwrCnt            (*(volatile unsigned *)0x80930004)
#define SysconPwrCnt_USH_EN	(1 << 28)
#define SysconScrReg0		(*(volatile unsigned *)0x80930040)
#define SysconScrReg1		(*(volatile unsigned *)0x80930044)

#define SYSCON_SW_UNCLOCK do { SysconSWLock |= 0xAA; } while(0)

/* Security regs */
#define SecurityExtensionID	(*(volatile unsigned *)0x80832714)

/*-----------------------------------------------------------------------------
 * SYSCON_CLKSET1
 *-----------------------------------------------------------------------------*/
#define SYSCON_CLKSET1_PLL1_X2IPD_SHIFT     0
#define SYSCON_CLKSET1_PLL1_X2IPD_MASK      0x0000001f
#define SYSCON_CLKSET1_PLL1_X2FBD2_SHIFT    5
#define SYSCON_CLKSET1_PLL1_X2FBD2_MASK     0x000007e0
#define SYSCON_CLKSET1_PLL1_X1FBD1_SHIFT    11
#define SYSCON_CLKSET1_PLL1_X1FBD1_MASK     0x0000f800
#define SYSCON_CLKSET1_PLL1_PS_SHIFT        16
#define SYSCON_CLKSET1_PLL1_PS_MASK         0x00030000
#define SYSCON_CLKSET1_PCLKDIV_SHIFT        18
#define SYSCON_CLKSET1_PCLKDIV_MASK         0x000c0000
#define SYSCON_CLKSET1_HCLKDIV_SHIFT        20
#define SYSCON_CLKSET1_HCLKDIV_MASK         0x00700000
#define SYSCON_CLKSET1_nBYP1                0x00800000
#define SYSCON_CLKSET1_SMCROM               0x01000000
#define SYSCON_CLKSET1_FCLKDIV_SHIFT        25
#define SYSCON_CLKSET1_FCLKDIV_MASK         0x0e000000

/* Timer STUFF for interrupts.c */
#define Timer1Load 		(*(volatile unsigned *)0x80810000)
#define Timer1Value 	(*(volatile unsigned *)0x80810004)
#define Timer1Control 	(*(volatile unsigned *)0x80810008)



/* Ethernet MAC, Info Copied from Cirrus Logic Verisuite info */
/*
*  15.2 PCI Operational Registers
*  (The addresses shown are relative to the IO base address register)
*/
#define OpReg_RxCTL     (*(volatile unsigned *)0x80010000)  /* 4-RW Receive  Control */
#define OpReg_TxCTL     (*(volatile unsigned *)0x80010004)  /* 1-RW Transmit Control */
#define OpReg_TestCTL   (*(volatile unsigned *)0x80010008)  /* 1-RW Test Control */
#define OpReg_MIICmd    (*(volatile unsigned *)0x80010010)  /* 2-RW MII(Media Independent Intf) Command */
#define OpReg_MIIData   (*(volatile unsigned *)0x80010014)  /* 2-RW MII Data */
#define OpReg_MIISts    (*(volatile unsigned *)0x80010018)  /* 1-RO MII Status */

#define OpReg_SelfCTL   (*(volatile unsigned *)0x80010020)  /* 1-RW Self Control for LED interface */
#define OpReg_IntEn     (*(volatile unsigned *)0x80010024)  /* 4-RW Interrupt Enable */
#define OpReg_IntStsP   (*(volatile unsigned *)0x80010028)  /* 4-RW Interrupt Status Preserve */
#define OpReg_IntStsC   (*(volatile unsigned *)0x8001002C)  /* 4-RO Interrupt Status Clear */

#define OpReg_GT        (*(volatile unsigned *)0x80010040)  /* 4-RW General Timer */
#define OpReg_FCT       (*(volatile unsigned *)0x80010044)  /* 4-RO Flow Control Timer */
#define OpReg_FCF       (*(volatile unsigned *)0x80010048)  /* 4-RW Flow Control Format */
#define OpReg_AFP       (*(volatile unsigned *)0x8001004C)  /* 1-RW Address Filter Pointer */
#define OpReg_HashTb    (*(volatile unsigned *)0x80010050)  /* 8-RW Logical Address Filter (Hash Table) */
#define OpReg_IndAd     (*(volatile unsigned *)0x80010050)  /* 6-RW Individual Address, IA */
#define OpReg_IndAd1   (*(volatile unsigned *)0x80010054)  /* 6-RW Individual Address, IA */

#define OpReg_FERMask   (*(volatile unsigned *)0x80010064)  /* 4-RW Cardbus Function Event Mask Register */

#define OpReg_TxCollCnt (*(volatile unsigned *)0x80010070)  /* 2-RO Transmit Collision Count */
#define OpReg_RxMissCnt (*(volatile unsigned *)0x80010074)  /* 2-RO Receive Miss Count */
#define OpReg_RxRuntCnt (*(volatile unsigned *)0x80010078)  /* 2-RO Receive Runt Count */

#define OpReg_BMCTL     (*(volatile unsigned *)0x80010080)  /* 2-RW Bus Master Control */
#define OpReg_BMSts     (*(volatile unsigned *)0x80010084)  /* 1-RO Bus Master Status */
#define OpReg_RxBCA     (*(volatile unsigned *)0x80010088)  /* 4-RO Receive buffer current address */
#define OpReg_TxBCA     (*(volatile unsigned *)0x8001008C)  /* 4-RO Transmit buffer current address */
#define OpReg_RxDBA     (*(volatile unsigned *)0x80010090)  /* 4-RW Receive Descriptor Queue Base Address */
#define OpReg_RxDBL     (*(volatile unsigned *)0x80010094)  /* 2-RW Receive Descriptor Queue Base Length */
#define OpReg_RxDCL     (*(volatile unsigned *)0x80010096)  /* 2-RW Receive Descriptor Queue Current Length */
#define OpReg_RxDCA     (*(volatile unsigned *)0x80010098)  /* 4-RW Receive Descriptor Current Address */
#define OpReg_RxDEQ     (*(volatile unsigned *)0x8001009C)  /* 4-RW Receive Descriptor Enqueue */

#define OpReg_RxSBA     (*(volatile unsigned *)0x800100A0)  /* 4-RW Receive Status Queue Base Address */
#define OpReg_RxSBL     (*(volatile unsigned *)0x800100A4)  /* 2-RW Receive Status Queue Base Length */
#define OpReg_RxSCL     (*(volatile unsigned *)0x800100A6)  /* 2-RW Receive Status Queue Current Length */
#define OpReg_RxSCA     (*(volatile unsigned *)0x800100A8)  /* 4-RW Receive Status Current Address */
#define OpReg_RxSEQ     (*(volatile unsigned *)0x800100AC)  /* 4-RW Receive Status Enqueue */
#define OpReg_TxDBA     (*(volatile unsigned *)0x800100B0)  /* 4-RW Transmit Descriptor Queue Base Address */
#define OpReg_TxDBL     (*(volatile unsigned *)0x800100B4)  /* 2-RW Transmit Descriptor Queue Base Length */
#define OpReg_TxDCL     (*(volatile unsigned *)0x800100B6)  /* 2-RW Transmit Descriptor Queue Current Length */
#define OpReg_TxDCA     (*(volatile unsigned *)0x800100B8)  /* 4-RW Transmit Descriptor Current Address */
#define OpReg_TxDEQ     (*(volatile unsigned *)0x800100BC)  /* 4-RW Transmit Descriptor Enqueue */

#define OpReg_TxSBA     (*(volatile unsigned *)0x800100C0)  /* 4-RW Transmit status Queue Base Address */
#define OpReg_TxSBL     (*(volatile unsigned *)0x800100C4)  /* 2-RW Transmit Status Queue Base Length */
#define OpReg_TxSCL     (*(volatile unsigned *)0x800100C6)  /* 2-RW Transmit Status Queue Current Length */
#define OpReg_TxSCA     (*(volatile unsigned *)0x800100C8)  /* 4-RW Transmit Status Current Address */
#define OpReg_RxBTH     (*(volatile unsigned *)0x800100D0)  /* 4-RW Receive Buffer Threshold */
#define OpReg_TxBTH     (*(volatile unsigned *)0x800100D4)  /* 4-RW Transmit Buffer Threshold */
#define OpReg_RxSTH     (*(volatile unsigned *)0x800100D8)  /* 4-RW Receive Status Threshold */
#define OpReg_TxSTH     (*(volatile unsigned *)0x800100DC)  /* 4-RW Transmit Status Threshold */

#define OpReg_RxDTH     (*(volatile unsigned *)0x800100E0)  /* 4-RW Receive Descriptor Threshold */
#define OpReg_TxDTH     (*(volatile unsigned *)0x800100E4)  /* 4-RW Transmit Descriptor Threshold */
#define OpReg_MaxFL     (*(volatile unsigned *)0x800100E8)  /* 4-RW Maximum Frame Length */
#define OpReg_RxHLen    (*(volatile unsigned *)0x800100EC)  /* 4-RW Receive Header Length */


/***************************************************************************/
/***************************************************************************/

/*
*       OpReg_RxCTL     0x0000   4-RW Receive  Control
*/
#define RxCTL_PauseAccept               (1L<<0x14)      /*  */
#define RxCTL_RxFlowControlEn1          (1L<<0x13)      /*  */
#define RxCTL_RxFlowControlEn0          (1L<<0x12)      /*  */
#define RxCTL_BufferCRC                 (1L<<0x11)      /*  */
#define RxCTL_SerRxON                   (1L<<0x10)      /*  */

#define RxCTL_RuntCRCA                  (1L<<0x0D)      /*  */
#define RxCTL_RuntA                     (1L<<0x0C)      /*  */
#define RxCTL_PromiscuousA              (1L<<0x0B)      /*  */
#define RxCTL_BroadcastA                (1L<<0x0A)      /*  */
#define RxCTL_MulticastA                (1L<<0x09)      /*  */
#define RxCTL_IAHashA                   (1L<<0x08)      /*  */

#define RxCTL_IndividualAccept3         (1L<<0x03)      /*  */
#define RxCTL_IndividualAccept2         (1L<<0x02)      /*  */
#define RxCTL_IndividualAccept1         (1L<<0x01)      /*  */
#define RxCTL_IndividualAccept0         (1L<<0x00)      /*  */


/*
*       OpReg_TxCTL     0x0004   1-RW Transmit Control
*/
#define TxCTL_2PartDefDis               (1<<0x07)       /*  */
#define TxCTL_ModBackOffE               (1<<0x06)       /*  */
#define TxCTL_InhibitCRC                (1<<0x05)       /*  */
#define TxCTL_TxPadDis                  (1<<0x04)       /*  */
#define TxCTL_OneColl                   (1<<0x03)       /*  */
#define TxCTL_SendPause                 (1<<0x02)       /*  */
#define TxCTL_PauseBusy                 (1<<0x01)       /*  */
#define TxCTL_SerTxON                   (1<<0x00)       /*  */


/*
*       OpReg_TestCTL   0x0008   1-RW Test Control
*/
#define TestCTL_MACFast                 (1<<0x07)       /*  */
#define TestCTL_MACFDX                  (1<<0x06)       /*  */
#define TestCTL_DisableBackoff          (1<<0x05)       /*  */
#define TestCTL_MIIFast                 (1<<0x04)       /*  */


/*
*       OpReg_MIICmd    0x0010   2-RW MII(Media Independent Intf) Command
*/
#define MIICmd_RegAd_Mask               (0x001F)        /*  */
#define MIICmd_PhyAd_Mask               (0x03E0)        /*  */
#define MIICmd_Opcode_Mask              (0xC000)        /*  */


/** QQQQQQQ */
#define MIICmd_PhyAd_8950               (0x0000)        /* Address the 8950 Phy */
#define MIICmd_Opcode_Read              (0x8000)        /* Read register */
#define MIICmd_Opcode_Write             (0x4000)        /* Write register */



/*
*       OpReg_MIIData   0x0014   2-RW MII Data
*/

/* this place left intentionally blank */

/*
*       OpReg_MIISts    0x0018   1-RO MII Status
*/
#define MIISts_Busy                     (1<<0x00)       /*  */

/*
*       OpReg_SelfCTL   0x0020   1-RW Self Control for LED interface
*/
#define SelfCTL_WakeupViaLAN            (1<<0x07)       /*  */

#define SelfCTL_GPO                     (1<<0x05)       /*  */
#define SelfCTL_PoweredUpWakeupEn       (1<<0x04)       /*  */
#define SelfCTL_PoweredDownWakeupEn     (1<<0x03)       /*  */
#define SelfCTL_MIILoopback             (1<<0x02)       /*  */

#define SelfCTL_RESET                   (1<<0x00)       /*  */


/*
*       OpReg_IntEn     0x0024   4-RW Interrupt Enable
*/
#define IntEn_WakeupViaLANiE            (1L<<0x1e)      /*  */
#define IntEn_RxMissiE                  (1L<<0x1d)      /*  */
#define IntEn_RxBuffersiE               (1L<<0x1c)      /*  */
#define IntEn_RxStsQiE                  (1L<<0x1b)      /*  */
#define IntEn_TxLenErriE                (1L<<0x1a)      /*  */
#define IntEn_EndofChainiE              (1L<<0x19)      /*  */
#define IntEn_TxUnderrunHaltiE          (1L<<0x18)      /*  */

#define IntEn_MissOviE                  (1L<<0x12)      /*  */
#define IntEn_TxCollOviE                (1L<<0x11)      /*  */
#define IntEn_RxRuntOviE                (1L<<0x10)      /*  */

#define IntEn_MIIStsiE                  (1L<<0x0c)      /*  */
#define IntEn_PhyStsiE                  (1L<<0x0b)      /*  */
#define IntEn_TimeoutiE                 (1L<<0x0a)      /*  */
#define IntEn_SWintiE                   (1L<<0x08)      /*  */

#define IntEn_TxStsQiE                  (1L<<0x03)      /*  */
#define IntEn_RxEOFiE                   (1L<<0x02)      /*  */
#define IntEn_RxEOBiE                   (1L<<0x01)      /*  */
#define IntEn_RxHdriE                   (1L<<0x00)      /*  */


/*
*       OpReg_IntStsP   0x0028   4-RW Interrupt Status Preserve
*       OpReg_IntStsC   0x002C   4-RO Interrupt Status Clear
*/
#define IntSts_WakeupViaLAN             (1L<<0x1e)      /*  */
#define IntSts_RxMiss                   (1L<<0x1d)      /*  */
#define IntSts_RxBuffers                (1L<<0x1c)      /*  */
#define IntSts_RxStsSEQ                 (1L<<0x1b)      /*  */
#define IntSts_TxLenErr                 (1L<<0x1a)      /*  */
#define IntSts_EndofChain               (1L<<0x19)      /*  */
#define IntSts_TxUnderrunHalt           (1L<<0x18)      /*  */

#define IntSts_MissOv                   (1L<<0x12)      /*  */
#define IntSts_TxCollOv                 (1L<<0x11)      /*  */
#define IntSts_RxRuntOv                 (1L<<0x10)      /*  */

#define IntSts_MIISts                   (1L<<0x0c)      /*  */
#define IntSts_PhySts                   (1L<<0x0b)      /*  */
#define IntSts_Timeout                  (1L<<0x0a)      /*  */
#define IntSts_SWint                    (1L<<0x08)      /*  */

#define IntSts_Other                    (1L<<0x04)      /*  */
#define IntSts_TxStsQ                   (1L<<0x03)      /*  */
#define IntSts_RxStsQ                   (1L<<0x02)      /*  */



/*
*       OpReg_GT        0x0040   4-RW General Timer
*/
#define GT_Count_Mask                   (0xFFFF0000)    /*  */
#define GT_Period_Mask                  (0x0000FFFF)    /*  */


/*
*       OpReg_FCT       0x0044   4-RO Flow Control Timer
*/
#define FCT_Timer_Mask                  (0x00FFFFFF)    /*  */


/*
*       OpReg_FCF       0x0048   4-RW Flow Control Format
*/
#define FCF_MACCTLType_Mask             (0xFFFF0000)    /*  */
#define FCF_TxPauseTime_Mask            (0x0000FFFF)    /*  */


/*
*       OpReg_AFP       0x004C   1-RW Address Filter Pointer
*/
#define AFP_Mask                        (0x07)  /*  */

#define AFP_IAPrimary                   (0x00)  /* primary IA for Wakeup, Tx pause and Rx pause frames */
#define AFP_IASecondary1                (0x01)  /* secondary IA for Rx pause frames */
#define AFP_IASecondary2                (0x02)  /* secondary IA for qualifying Rx frames */
#define AFP_IASecondary3                (0x03)  /* secondary IA for qualifying Rx frames */

#define AFP_Tx                          (0x06)  /* destination address for Tx */
#define AFP_Hash                        (0x07)  /* hash table */


/*
*       OpReg_HashTb    0x0050   8-RW Logical Address Filter (Hash Table)
*/

/* this place left intentionally blank */


/*
*       OpReg_IndAd     0x0050   6-RW Individual Address, IA
*/

/* this place left intentionally blank */



/*
*       OpReg_FERMask   0x0064   4-RW Cardbus Function Event Mask Register
*/
#define FERMask_Interrupt               (1L<<0x0F)      /*  */


/*
*       OpReg_TxCollCnt 0x0070   2-RO Transmit Collision Count
*/

/* this place left intentionally blank */


/*
*       OpReg_RxMissCnt 0x0074   2-RO Receive Miss Count
*/

/* this place left intentionally blank */


/*
*       OpReg_RxRuntCnt 0x0078   2-RO Receive Runt Count
*/

/* this place left intentionally blank */


/*
*       OpReg_BMCTL     0x0080   2-RW Bus Master Control
*/
#define BMCTL_ManualTrans               (1<<0x0d)       /*  */
#define BMCTL_TimedTrans                (1<<0x0c)       /*  */
#define BMCTL_UnderrunHalt              (1<<0x0b)       /*  */
#define BMCTL_TxChRes                   (1<<0x0a)       /*  */
#define BMCTL_TxDis                     (1<<0x09)       /*  */
#define BMCTL_TxEn                      (1<<0x08)       /*  */

#define BMCTL_EnHeader2                 (1<<0x06)       /*  */
#define BMCTL_EnHeader1                 (1<<0x05)       /*  */
#define BMCTL_EnEOB                     (1<<0x04)       /*  */

#define BMCTL_RxChRes                   (1<<0x02)       /*  */
#define BMCTL_RxDis                     (1<<0x01)       /*  */
#define BMCTL_RxEn                      (1<<0x00)       /*  */


/*
*       OpReg_BMSts     0x0084   1-RO Bus Master Status
*/
#define BMSts_TxAct                     (1<<0x07)       /*  */

#define BMSts_TransPending              (1<<0x04)       /*  */
#define BMSts_RxAct                     (1<<0x03)       /*  */

#define BMSts_QueueID_Mask              (0x07)          /*  */

#define BMSts_QueueID_RxData            (0x00)          /*  */
#define BMSts_QueueID_TxData            (0x01)          /*  */
#define BMSts_QueueID_RxSts             (0x02)          /*  */
#define BMSts_QueueID_TxSts             (0x03)          /*  */
#define BMSts_QueueID_RxDesc            (0x04)          /*  */
#define BMSts_QueueID_TxDesc            (0x05)          /*  */



/*
*       OpReg_RxBCA     0x0088   4-RO Receive buffer current address
*/

/* this place left intentionally blank */


/*
*       OpReg_TxBCA     0x008C   4-RO Transmit buffer current address
*/

/* this place left intentionally blank */


/*
*       OpReg_RxDBA     0x0090   4-RW Receive Descriptor Queue Base Address
*/

/* this place left intentionally blank */


/*
*       OpReg_RxDBL     0x0094   2-RW Receive Descriptor Queue Base Length
*/

/* this place left intentionally blank */


/*
*       OpReg_RxDCL     0x0096   2-RW Receive Descriptor Queue Current Length
*/

/* this place left intentionally blank */


/*
*       OpReg_RxDCA     0x0098   4-RW Receive Descriptor Current Address
*/

/* this place left intentionally blank */


/*
*       OpReg_RxDEQ     0x009C   4-RW Receive Descriptor Enqueue
*/
#define RxDEQ_Inc_Mask                  (0x000000FF)    /*  */
#define RxDEQ_Value_Mask                (0xFFFF0000)    /* for reading */


/*
*       OpReg_RxSBA     0x00A0   4-RW Receive Status Queue Base Address
*/

/* this place left intentionally blank */


/*
*       OpReg_RxSBL     0x00A4   2-RW Receive Status Queue Base Length
*/

/* this place left intentionally blank */


/*
*       OpReg_RxSCL     0x00A6   2-RW Receive Status Queue Current Length
*/

/* this place left intentionally blank */


/*
*       OpReg_RxSCA     0x00A8   4-RW Receive Status Current Address
*/

/* this place left intentionally blank */


/*
*       OpReg_RxSEQ     0x00AC   4-RW Receive Status Enqueue
*/
#define RxSEQ_Inc_Mask                  (0x000000FF)    /*  */
#define RxSEQ_Value_Mask                (0xFFFF0000)    /* for reading */


/*
*       OpReg_TxDBA     0x00B0   4-RW Transmit Descriptor Queue Base Address
*/

/* this place left intentionally blank */


/*
*       OpReg_TxDBL     0x00B4   2-RW Transmit Descriptor Queue Base Length
*/

/* this place left intentionally blank */


/*
*       OpReg_TxDCL     0x00B6   2-RW Transmit Descriptor Queue Current Length
*/

/* this place left intentionally blank */


/*
*       OpReg_TxDCA     0x00B8   4-RW Transmit Descriptor Current Address
*/

/* this place left intentionally blank */


/*
*       OpReg_TxDEQ     0x00BC   4-RW Transmit Descriptor Enqueue
*/
#define TxDEQ_Inc_Mask                  (0x000000FF)    /*  */
#define TxDEQ_Value_Mask                (0xFFFF0000)    /* for reading */


/*
*       OpReg_TxSBA     0x00C0   4-RW Transmit status Queue Base Address
*/

/* this place left intentionally blank */


/*
*       OpReg_TxSBL     0x00C4   2-RW Transmit Status Queue Base Length
*/

/* this place left intentionally blank */


/*
*       OpReg_TxSCL     0x00C6   2-RW Transmit Status Queue Current Length
*/

/* this place left intentionally blank */


/*
*       OpReg_TxSCA     0x00C8   4-RW Transmit Status Current Address
*/

/* this place left intentionally blank */


/*
*       OpReg_RxBTH     0x00D0   4-RW Receive Buffer Threshold
*/
#define RxBTH_SoftTh_Mask               (0x000003FF)    /*  */
#define RxBTH_HardTh_Mask               (0x03FF0000)    /*  */


/*
*       OpReg_TxBTH     0x00D4   4-RW Transmit Buffer Threshold
*/
#define TxBTH_SoftTh_Mask               (0x000003FF)    /*  */
#define TxBTH_HardTh_Mask               (0x03FF0000)    /*  */


/*
*       OpReg_RxSTH     0x00D8   4-RW Receive Status Threshold
*/
#define RxSTH_SoftTh_Mask               (0x0000003F)    /*  */
#define RxSTH_HardTh_Mask               (0x003F0000)    /*  */


/*
*       OpReg_TxSTH     0x00DC   4-RW   Transmit Status Threshold
*/
#define TxSTH_SoftTh_Mask               (0x0000003F)    /*  */
#define TxSTH_HardTh_Mask               (0x003F0000)    /*  */


/*
*       OpReg_RxDTH     0x00E0   4-RW Receive Descriptor Threshold
*/
#define RxDTH_SoftTh_Mask               (0x0000003F)    /*  */
#define RxDTH_HardTh_Mask               (0x003F0000)    /*  */


/*
*       OpReg_TxDTH     0x00E4   4-RW Transmit Descriptor Threshold
*/
#define TxDTH_SoftTh_Mask               (0x0000003F)    /*  */
#define TxDTH_HardTh_Mask               (0x003F0000)    /*  */


/*
*       OpReg_MaxFL     0x00E8   4-RW Maximum Frame Length
*/
#define MaxFL_TxStartTh_Mask            (0x000007FF)    /*  */
#define MaxFL_FrameLen_Mask             (0x07FF0000)    /*  */

/*
*       OpReg_RxHLen    0x00EC   4-RW Receive Header Length
*/
#define RxHLen_1_Mask                   (0x000007FF)    /*  */
#define RxHLen_2_Mask                   (0x07FF0000)    /*  */





/* UART 2 REGISTERS */
#define rUART2_DR			(*(volatile unsigned *)0x808d0000)
#define rUART2_RSR			(*(volatile unsigned *)0x808d0004)
#define rUART2_LCR_H			(*(volatile unsigned *)0x808d0008)
#define rUART2_LCR_M			(*(volatile unsigned *)0x808d000c)
#define rUART2_LCR_L			(*(volatile unsigned *)0x808d0010)
#define rUART2_CR   			(*(volatile unsigned *)0x808d0014)
#define rUART2_FR   			(*(volatile unsigned *)0x808d0018)
#define rUART2_IIR   			(*(volatile unsigned *)0x808d001c)

/* UART 3 REGISTERS */
#define rUART3_DR			(*(volatile unsigned *)0x808e0000)
#define rUART3_RSR			(*(volatile unsigned *)0x808e0004)
#define rUART3_LCR_H			(*(volatile unsigned *)0x808e0008)
#define rUART3_LCR_M			(*(volatile unsigned *)0x808e000c)
#define rUART3_LCR_L			(*(volatile unsigned *)0x808e0010)
#define rUART3_CR   			(*(volatile unsigned *)0x808e0014)
#define rUART3_FR   			(*(volatile unsigned *)0x808e0018)
#define rUART3_IIR   			(*(volatile unsigned *)0x808e001c)

/* Timer STUFF for interrupts.c */
/* These names match what was in interrupts.c already */
#define rTCON                           (*(volatile unsigned *)0x80810048)
#define rTCNTB4                         (*(volatile unsigned *)0x80810040)
#define rTCNTO4                         (*(volatile unsigned *)0x80810044)

/* SYSCON REGS */
#define PLLCFG0                         (*(volatile unsigned *)0x80930004)
#define PLLCFG1                         (*(volatile unsigned *)0x80930008)
#define PLLIDIV                         (*(volatile unsigned *)0x8093000c)
#define FHDIV                           (*(volatile unsigned *)0x80930010)
#define UARTDIV                         (*(volatile unsigned *)0x80930024)
#define WATCHDIV                        (*(volatile unsigned *)0x80930030)

/* Stuff for memsetup.S */
#define RemapReg 0x80820020
#define WatchDog 0x80940000
#define DogDeath 0x0000AA55
#define MiniB_LED 0x80850000
#define PLL_STABILIZE_CNT 0x0280
#define SYSCON_BASE 0x80930000
#define R_RSTCR 0x0
#define R_PLL0CFG 0x4
#define R_PLL1CFG 0x8
#define R_PLLIDIV 0xc
#define R_FHDIV 0x10
#define R_VIDDIV 0x14
#define R_USBDIV 0x18
#define R_IRDADIV 0x1c
#define R_I2SDIV 0x20
#define R_UARTDIV 0x24
#define R_SPIDIV 0x28
#define R_KTDIV 0x2C
#define R_WATCHDIV 0x30
#define R_CHIP_ID 0x34
#define R_TSTCR 0x38
#define R_SYSCONF 0x3C
#define R_LOCK 0x40
#define R_TXDIV 0x44
#define V_RSTCR 0x00040000
#define V_PLL0CFG 0x10809064
#define V_PLL1CFG 0x56809064
#define V_PLLIDIV 0x00008060
#define V_FHDIV 0x001c800a
#define V_VIDDIV 0x00148005
#define V_USBDIV 0x00008004
#define V_IRDADIV 0x800a8004
#define V_I2SDIV 0x800f803c
#define V_UARTDIV 0xf0008019
#define V_SPIDIV 0xc0008032
#define V_KTDIV 0x20018004
#define V_WATCHDIV 0x80048024
#define USB_BASE 0x80020000
#define SDRAM_BASE 0x80060000
#define SMC_BASE 0x80080000


#define GICONFIG 0x04
#define REFRSHTIMR 0x08
#define CFG0 0x10
#define CFG1 0x14
#define CFG2 0x18
#define CFG3 0x1C
#define Cfg0Data 0x00A4AAAA
#define ModeData 0x00022000

/* Ethernet MAC */
#define SelfCTL (*(volatile unsigned *)0x80010020)
#define RxCTL (*(volatile unsigned *)0x80010000)
#define TxCTL (*(volatile unsigned *)0x80010004)
#define AFP (*(volatile unsigned *)0x8001004c)
#define IndAD0 (*(volatile unsigned *)0x80010050)
#define IndAD1 (*(volatile unsigned *)0x80010052)
#define IndAD2 (*(volatile unsigned *)0x80010054)

/* GPIOs */

#define PADR		(*(volatile unsigned *)0x80840000) /* GPIO A Data Register */
#define PBDR		(*(volatile unsigned *)0x80840004) /* GPIO B Data Register */
#define PCDR		(*(volatile unsigned *)0x80840008) /* GPIO C Data Register */
#define PDDR		(*(volatile unsigned *)0x8084000c) /* GPIO D Data Register */
#define PEDR		(*(volatile unsigned *)0x80840020) /* GPIO E Data Register */
#define PFDR		(*(volatile unsigned *)0x80840030) /* GPIO F Data Register */
#define PGDR		(*(volatile unsigned *)0x80840038) /* GPIO G Data Register */
#define PHDR		(*(volatile unsigned *)0x80840040) /* GPIO H Data Register */

#define PADDR		(*(volatile unsigned *)0x80840010) /* GPIO A Direction Register */
#define PBDDR		(*(volatile unsigned *)0x80840014) /* GPIO B Direction Register */
#define PCDDR		(*(volatile unsigned *)0x80840018) /* GPIO C Direction Register */
#define PDDDR		(*(volatile unsigned *)0x8084001c) /* GPIO D Direction Register */
#define PEDDR		(*(volatile unsigned *)0x80840024) /* GPIO E Direction Register */
#define PFDDR		(*(volatile unsigned *)0x80840034) /* GPIO F Direction Register */
#define PGDDR		(*(volatile unsigned *)0x8084003c) /* GPIO G Direction Register */
#define PHDDR		(*(volatile unsigned *)0x80840044) /* GPIO H Direction Register */

/* Static Memory Controller */
#define SMCBCR0		(*(volatile unsigned *)0x80080000) /*  Bank config register 0 */
#define SMCBCR1		(*(volatile unsigned *)0x80080004) /*  Bank config register 1 */
#define SMCBCR2		(*(volatile unsigned *)0x80080008) /*  Bank config register 2 */
#define SMCBCR3		(*(volatile unsigned *)0x8008000C) /*  Bank config register 3 */
#define SMCBCR6		(*(volatile unsigned *)0x80080018) /*  Bank config register 6 */
#define SMCBCR7		(*(volatile unsigned *)0x8008001C) /*  Bank config register 7 */
#define SMCBCRx_IDCY	0x0000000f
#define SMCBCRx_WST1	0x000003e0
#define SMCBCRx_RBLE	0x00000400
#define SMCBCRx_WST2	0x0000f800
#define SMCBCRx_WPERR	0x02000000
#define SMCBCRx_WP	0x04000000
#define SMCBCRx_PME	0x08000000
#define SMCBCRx_MW	0x30000000
#define SMCBCRx_EBIBRK	0x40000000

/* PCMCIA controller */
#define PC1ATTRIB	(*(volatile unsigned *)0x80080020) /*  PC1 Attribute Register */
#define PC1COMMON	(*(volatile unsigned *)0x80080024) /*  PC1 Common Register    */
#define PC1IO		(*(volatile unsigned *)0x80080028) /*  PC1 IO Register        */
#define PC2ATTRIB	(*(volatile unsigned *)0x80080030) /*  PC2 Attribute Register   */
#define PC2COMMON	(*(volatile unsigned *)0x80080034) /*  PC2 Common Register     */
#define PC2IO		(*(volatile unsigned *)0x80080038) /*  PC2 IO Register         */

#define PC_PRECH	0x000000ff
#define PC_HOLD		0x00000f00
#define PC_ACCESS	0x00ff0000
#define PC_WIDTH	0x80000000

#define PCCNTR		(*(volatile unsigned *)0x80080040) /*  PCMCIA control register */
#define PCCNTR_PCEN	0x00000003
#define PCCNTR_PCRST	0x00000004
#define PCCNTR_WEN	0x00000010

#define PCMCIA_IO	0x40000000
#define PCMCIA_ATTR	0x48000000
#define PCMCIA_MEM	0x4C000000

/* Some other (redundant) stuff needed by eCos-derived code */

/* System control registers */
#define EP9312_SYSCON                   0x80930000
#define EP9312_SYSCON_LOCK              0x00c0

/* Clock control registers */
#define EP9312_CLKSET1                  (EP9312_SYSCON + 0x0020)
#define EP9312_CLKSET1_NBYP             0x00800000
#define EP9312_CLKSET1_HCLKDIV_MASK     0x00700000
#define EP9312_CLKSET1_HCLKDIV_SHIFT    20
#define EP9312_CLKSET1_FCLKDIV_MASK     0x0e000000
#define EP9312_CLKSET1_FCLKDIV_SHIFT    25
#define EP9312_CLKSET1_PCLKDIV_MASK     0x000c0000
#define EP9312_CLKSET1_PCLKDIV_SHIFT    18

#define EP9312_CLKSET2                  (EP9312_SYSCON + 0x0024)
#define EP9312_CLKSET2_PLL2_EN          0x00000001
#define EP9312_CLKSET2_PLL2EXCLKSEL     0x00000002
#define EP9312_CLKSET2_PLL2_P_MASK      0x0000007C
#define EP9312_CLKSET2_PLL2_P_SHIFT     2
#define EP9312_CLKSET2_PLL2_M2_MASK     0x00000F80
#define EP9312_CLKSET2_PLL2_M2_SHIFT    7
#define EP9312_CLKSET2_PLL2_M1_MASK     0x0001F000
#define EP9312_CLKSET2_PLL2_M1          12
#define EP9312_CLKSET2_PLL2_PS_MASK     0x000C0000
#define EP9312_CLKSET2_PLL2_PS_SHIFT    18
#define EP9312_CLKSET2_USBDIV_MASK      0xF0000000
#define EP9312_CLKSET2_USBDIV_SHIFT     28

#define EP9312_PWRCNT                   (EP9312_SYSCON + 0x0004)
#define EP9312_PWRCNT_UARTBAUD          0x20000000

#define EP9312_DEVCFG                   (EP9312_SYSCON + 0x0080)
#define EP9312_DEVCFG_U1EN              0x00040000
#define EP9312_DEVCFG_U2EN              0x00100000
#define EP9312_DEVCFG_U3EN              0x01000000
#define EP9312_DEVCFG_SWRST             0x80000000  /* 1->0 in this bit resets board */

/* WATCHDOG */
#define EP9312_WATCHDOG                 0x80940000

/* UARTs */
#define EP9312_UART_DATA        0x0000  /* Data/FIFO register */

#define EP9312_UART_SR          0x0004  /* Status register */
#define EP9312_UART_SR_FE       0x0001  /* Framing error */
#define EP9312_UART_SR_PE       0x0002  /* Parity error */
#define EP9312_UART_SR_BE       0x0004  /* Break error */
#define EP9312_UART_SR_OE       0x0008  /* Overrun */

#define EP9312_UART_LCR_H       0x0008  /* Control register High */
#define EP9312_UART_LCR_H_BRK   0x0001 /* Send break */
#define EP9312_UART_LCR_H_PEN   0x0002 /* Enable parity */
#define EP9312_UART_LCR_H_EPS   0x0004 /* Odd/Even parity */
#define EP9312_UART_LCR_H_STP2  0x0008 /* One/Two stop bits */
#define EP9312_UART_LCR_H_FE    0x0010 /* Enable FIFO */
#define EP9312_UART_LCR_H_WLEN5 0x0000 /* Word length - 5 bits */
#define EP9312_UART_LCR_H_WLEN6 0x0020 /* Word length - 6 bits */
#define EP9312_UART_LCR_H_WLEN7 0x0040 /* Word length - 7 bits */
#define EP9312_UART_LCR_H_WLEN8 0x0060 /* Word length - 8 bits */

#define EP9312_UART_LCR_M       0x000C  /* Baud rate 8..15 */
#define EP9312_UART_LCR_L       0x0010  /* Baud rate 0..7 */

#define EP9312_UART_CR          0x0014  /* Control register */
#define EP9312_UART_CR_UARTE    0x0001 /* Enable uart */
#define EP9312_UART_CR_RIE      0x0010 /* Enable Rx interrupt */
#define EP9312_UART_CR_TIE      0x0020 /* Enable Tx interrupt */
#define EP9312_UART_CR_RTIE     0x0040 /* Enable Rx timeout interrupt */
#define EP9312_UART_CR_LBE      0x0080 /* Loopback mode */

#define EP9312_UART_FR          0x0018  /* Flags register */
#define EP9312_UART_FR_CTS      0x0001 /* Clear-to-send status */
#define EP9312_UART_FR_DSR      0x0002 /* Data-set-ready status */
#define EP9312_UART_FR_DCD      0x0004 /* Data-carrier-detect status */
#define EP9312_UART_FR_BUSY     0x0008 /* Transmitter busy */
#define EP9312_UART_FR_RXFE     0x0010 /* Receive FIFO empty */
#define EP9312_UART_FR_TXFF     0x0020 /* Transmit FIFO full */
#define EP9312_UART_FR_RXFF     0x0040 /* Receive FIFO full */
#define EP9312_UART_FR_TXFE     0x0080 /* Transmit FIFO empty */

#define EP9312_UARTIIR          0x001C  /* Interrupt status */
#define EP9312_UARTIIR_MS       0x0001 /* Modem status interrupt */
#define EP9312_UARTIIR_RIS      0x0002 /* Rx interrupt */
#define EP9312_UARTIIR_TIS      0x0004 /* Tx interrupt */
#define EP9312_UARTIIR_RTIS     0x0008 /* Rx timeout interrupt */

#define EP9312_UART_MCR         0x0100  /* Modem control */

#define EP9312_UART1            0x808C0000
#define EP9312_UART2            0x808D0000
#define EP9312_UART3            0x808E0000

/* LED interface - LED0 is green, LED1 is red */
#define EP9312_LED_DATA         0x80840020
#define EP9312_LED_GREEN_ON     0x0001
#define EP9312_LED_RED_ON       0x0002

#define EP9312_LED_DDR          0x80840024
#define EP9312_LED_GREEN_ENABLE 0x0001
#define EP9312_LED_RED_ENABLE   0x0002

/* Timers */
#define EP9312_TIMERS                   0x80810000
#define EP9312_TIMERS_DEBUG_LO          (EP9312_TIMERS+0x0060)
#define EP9312_TIMERS_DEBUG_HI          (EP9312_TIMERS+0x0064)
#define EP9312_TIMERS_DEBUG_HI_RESET    0x00000000
#define EP9312_TIMERS_DEBUG_HI_START    0x00000100

/* Ethernet controller */
#define EP9312_MAC                      0x80010000

#define EP9312_SDRAM_PHYS_BASE          0x00000000

