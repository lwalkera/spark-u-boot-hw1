#include <common.h>
#include <command.h>
#include <asm/mach-types.h>
#include <asm/io.h>
#include <asm/arch/arc_otg.h>

#ifdef CONFIG_USB_EHCI

/* add to above register address to access Set/Clear functions */
#define ISP1504_REG_SET     0x01
#define ISP1504_REG_CLEAR   0x02

/* ULPIVIEW register bits */
#define ULPIVW_WU       (1 << 31)   /* Wakeup */
#define ULPIVW_RUN      (1 << 30)   /* read/write run */
#define ULPIVW_WRITE        (1 << 29)   /* 0=read  1=write */
#define ULPIVW_SS       (1 << 27)   /* SyncState */
#define ULPIVW_PORT_MASK    0x07    /* Port field */
#define ULPIVW_PORT_SHIFT   24
#define ULPIVW_ADDR_MASK    0xFF    /* data address field */
#define ULPIVW_ADDR_SHIFT   16
#define ULPIVW_RDATA_MASK   0xFF    /* read data field */
#define ULPIVW_RDATA_SHIFT  8
#define ULPIVW_WDATA_MASK   0xFF    /* write data field */
#define ULPIVW_WDATA_SHIFT  0

/*!
 * * read ULPI register 'reg' thru VIEWPORT register 'view'
 * *
 * * @param       reg   register to read
 * * @param       view  the ULPI VIEWPORT register address
 * * @return  return isp1504 register value
 * */
u8 isp1504_read(int reg, volatile u32 * view)
{
	u32 data;
	int count = 500;

	/* make sure interface is running */
	if (!(__raw_readl(view) && ULPIVW_SS)) {
		__raw_writel(ULPIVW_WU, view);
		do {        /* wait for wakeup */
			data = __raw_readl(view);
		} while (data & ULPIVW_WU && count--);
	}

	/* read the register */
	__raw_writel((ULPIVW_RUN | (reg << ULPIVW_ADDR_SHIFT)), view);
	count = 500;

	do {            /* wait for completion */
		data = __raw_readl(view);
	} while (data & ULPIVW_RUN && count--);

	return (u8) (data >> ULPIVW_RDATA_SHIFT) & ULPIVW_RDATA_MASK;
}

/*!
 * * set bits into OTG ISP1504 register 'reg' thru VIEWPORT register 'view'
 * *
 * * @param       bits  set value
 * * @param   reg   which register
 * * @param       view  the ULPI VIEWPORT register address
 * */
void isp1504_set(u8 bits, int reg, volatile u32 * view)
{
	u32 data;
	int count = 500;

	/* make sure interface is running */
	if (!(__raw_readl(view) && ULPIVW_SS)) {
		__raw_writel(ULPIVW_WU, view);
		do {        /* wait for wakeup */
			data = __raw_readl(view);
		} while (data & ULPIVW_WU && count--);
	}

	__raw_writel((ULPIVW_RUN | ULPIVW_WRITE |
				((reg + ISP1504_REG_SET) << ULPIVW_ADDR_SHIFT) |
				((bits & ULPIVW_WDATA_MASK) << ULPIVW_WDATA_SHIFT)),
			view);

	count = 500;
	while (__raw_readl(view) & ULPIVW_RUN && count--)   /* wait for completion */
		udelay(10);
}

/*!
 * * clear bits in OTG ISP1504 register 'reg' thru VIEWPORT register 'view'
 * *
 * * @param       bits  bits to clear
 * * @param   reg   in this register
 * * @param       view  the ULPI VIEWPORT register address
 * */
void isp1504_clear(u8 bits, int reg, volatile u32 * view)
{
	int count = 500;
	__raw_writel((ULPIVW_RUN | ULPIVW_WRITE |
				((reg + ISP1504_REG_CLEAR) << ULPIVW_ADDR_SHIFT) |
				((bits & ULPIVW_WDATA_MASK) << ULPIVW_WDATA_SHIFT)),
			view);

	while (__raw_readl(view) & ULPIVW_RUN && count--)   /* wait for completion */
		udelay(10);
}

static int do_isp1504 ( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int reg;
	u8 value;

	reg = simple_strtoul(argv[1], NULL, 16);

	if(argc == 2)  {/* Read from isp1504)*/
		value = isp1504_read(reg, &UH2_ULPIVIEW);
		printf("Read from %#x - > [%#x]\n", reg, value);
	} else { /* write to isp1504 */
		value = simple_strtoul(argv[2], NULL, 16);
		isp1504_set(value, reg, &UH2_ULPIVIEW);
		isp1504_clear(~value, reg, &UH2_ULPIVIEW);
		printf("Write %#x - > [%#x]\n", value, reg);
	}
	return 0;
}

U_BOOT_CMD( isp1504,     3,     0,      do_isp1504,
		"isp1504      - Read/write isp1504 reigster.\n",
		"isp1504 0x12 # Read from isp1504 0x12\n"
		"isp1504 0x12 0x123 # write 0x123 to 0x12\n"
		);

#endif
