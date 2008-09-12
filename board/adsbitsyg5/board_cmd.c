/*
 *  board_cmd.c
 *
 *  Author:		jl.shan@vantrontech.com.cn
 *  Created:	May 9, 2007
 *  Copyright:	Applied Data Systems, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * 
 *
 */
#include <common.h>
#include <command.h>
#include <linux/types.h>
#include <asm/io.h>
#include <asm/arch/mx31-regs.h>

extern u32 pll_clock(enum plls pll);
extern u32 get_main_clock(enum main_clocks clk);
extern u32 get_peri_clock(enum peri_clocks clk);
extern int setup_clock(u32 core, u32 ahb, u32 ipg);

static int do_clock_setup ( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);
extern unsigned int g_clock_src ;
extern void serial_stop (void);

static int do_clock_setup ( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    u32 i, data[3];
    int ret;

	if (argc == 1)
		goto print_clock;

	if (g_clock_src == FREQ_27MHZ) {
		printf("Error: clock setup is not supported for 27MHz source\n\n");
		return -1;
	}
	for (i = 0;  i < 3;  i++) {
		data[i] = simple_strtoul (argv[i+1], NULL, 10);
	}

	/*core clock ahb_div ipg_div*/
	ret = setup_clock(data[0], data[1], data[2]);

    if (ret != 0) {
        printf("Failed to setup clock: %d\n", ret);
        return -1;
    }
    printf("\n<<<New clock setting>>>\n");

    // Now printing clocks
print_clock:
    printf("\nMPLL\t\tUPLL\t\tSPLL\n");
    printf("========================================\n");
    printf("%-16d%-16d%-16d\n\n", pll_clock(MCU_PLL), pll_clock(USB_PLL),
                pll_clock(SER_PLL));
    printf("CPU\t\tAHB\t\tIPG\t\tIPG_PER\n");
    printf("========================================================\n");
    printf("%-16d%-16d%-16d%-16d\n\n",
                get_main_clock(CPU_CLK),
                get_main_clock(AHB_CLK),
                get_main_clock(IPG_CLK),
                get_main_clock(IPG_PER_CLK));

    printf("NFC\t\tUSB\t\tIPU-HSP\n");
    printf("========================================\n");
    printf("%-16d%-16d%-16d\n\n",
                get_main_clock(NFC_CLK),
                get_main_clock(USB_CLK),
                get_main_clock(HSP_CLK));

    printf("UART1-5\t\tSSI1\t\tSSI2\t\tCSI\n");
    printf("===========================================");
    printf("=============\n");

    printf("%-16d%-16d%-16d%-16d\n\n", 
                get_peri_clock(UART1_BAUD),
                get_peri_clock(SSI1_BAUD),
                get_peri_clock(SSI2_BAUD),
                get_peri_clock(CSI_BAUD));

    printf("FIRI\t\tSIM\t\tMSTICK1\t\tMSTICK2\n");
    printf("===========================================");
    printf("=============\n");

    printf("%-16d%-16d%-16d%-16d\n\n", 
                get_peri_clock(FIRI_BAUD),
                get_peri_clock(SIM_BAUD),
                get_peri_clock(MSTICK1_CLK),
                get_peri_clock(MSTICK2_CLK));
    printf("IPG_PERCLK as baud clock for: UART1-5, I2C, SIM, OWIRE, SDHC");
    if (((readl(EPIT1_BASE_ADDR) >> 24) & 0x3) == 0x2) {
        printf(", EPIT");
    }
    if (((readl(GPT1_BASE_ADDR) >> 6) & 0x7) == 0x2) {
        printf("GPT,");
    }
    if (((readl(PWM_BASE_ADDR) >> 16) & 0x3) == 0x2) {
        printf("PWM,");
    }
    printf("\n");
	return 0;
}

U_BOOT_CMD(
	clock,     4,     0,      do_clock_setup,
	"clock      - Setup/Display clock.\n",
	"[<core clock in MHz> [:<AHB-to-core divider>[:<IPG-to-AHB divider>]]] \n\n\
If a divider is zero or no divider is specified, the optimal divider values \n\
will be chosen. Examples:\n\
   [clock]         -> Show various clocks\n\
   [clock 532]     -> Core=532  AHB=133           IPG=66.5\n\
   [clock 399]     -> Core=399  AHB=133           IPG=66.5\n\
   [clock 532:8]   -> Core=532  AHB=66.5(Core/8)  IPG=66.5\n\
   [clock 532:8:2] -> Core=532  AHB=66.5(Core/8)  IPG=33.25(AHB/2)\n"
);

#define menuitem(key, menu) printf("<%c> %s\n", key, menu)

#include <linux/ctype.h>

static int Run_command(char * cmd)
{
	printf("%s\n", cmd);
	return run_command(cmd, 0);
}

static void copy_to_map(char *name)
{
	if(getenv("filesize"))
	{
		char cmd[128];
		sprintf(cmd, "protect off %s", name);
		Run_command(cmd);

		sprintf(cmd, "erase %s", name);
		Run_command(cmd);

		sprintf(cmd, "cp.b scratch_buffer %s ${filesize}", name);
		Run_command(cmd);
	}
}

static int do_menu ( cmd_tbl_t *cmdtp, int flags, int argc, char *argv[])
{
	int ch, len;
	char cmd[1024];
	extern char console_buffer[];
	char *input  = console_buffer;

	do {
		cmd[0]= 0x00;
		puts("\n");
		menuitem('S', "set ipaddr/serverip");
		menuitem('T', "download file(tftp)");
		menuitem('D', "download file(kermit)");
		menuitem('G', "run program");
		puts("-------------------\n");
		menuitem('B', "copy file to boot");
		menuitem('K', "copy file to zImage");
		menuitem('I', "copy file to ramdisk.gz");
		menuitem('F', "copy file to flashfs1.gz");
		puts("-------------------\n");
		menuitem('E', "Erase Environment.");
		menuitem('R', "Run linux.");
		menuitem('P', "Tftpboot linux.");
		//menuitem('U', "Update U-boot.");
		menuitem('O', "Reboot!");
		menuitem('Q', "Quit!");
		printf("Input Select:");
		ch = toupper(getc());
		printf("\n");
		switch(ch)
		{
			case 'S':
				len = readline ("ip:");
				if(len > 7) /*1.1.1.1*/
				{
					setenv("ipaddr", input);
					sprintf(cmd, "saveenv");
				}

				len = readline("server ip:");
				if(len > 7)
				{
					setenv("serverip", input);
					sprintf(cmd, "saveenv");
				}
				break;

			case 'T':
				len = readline("Input filename to download:");
				if(len > 0)
					sprintf(cmd, "tftp scratch_buffer %s", input);
				break;

			case 'D':
				sprintf(cmd, "loadb scratch_buffer");
				break;
			
			case 'G':
				if(getenv("filesize"))
					sprintf(cmd, "go scratch_buffer");
				break;

			case 'B':
				copy_to_map("boot");
				break;

			case 'K':
				copy_to_map("zImage");
				break;

			case 'I':
				copy_to_map("ramdisk.gz");
				break;

			case 'F':
				copy_to_map("flashfs1");
				break;

			case 'E':
				sprintf(cmd,"protect off 0x%x +0x%x", CFG_ENV_ADDR, CFG_ENV_SIZE);
				Run_command(cmd);
				sprintf(cmd,"erase 0x%x +0x%x", CFG_ENV_ADDR, CFG_ENV_SIZE);
				break;

			case 'R':
				if(getenv("boot_linux"))
					Run_command("run boot_linux");
				else
				{
					Run_command("run hw_config");
					Run_command("cp zImage zImage_buffer");
					Run_command("cp ramdisk.gz initrd");
					sprintf(cmd, "bootm zImage_buffer");
				}
				break;

#if 0
			case 'U':
				{
					 extern u8 __bss_start[], _start[];
					 int size = __bss_start - _start;
					 Run_command("protect off zImage");
					 Run_command("erase zImage");
					 sprintf(cmd, "cp.b 0x%x zImage 0x%x", _start, size);
				}
				break;
#endif
			case 'P':
				Run_command("cp ramdisk.gz initrd");
				Run_command("tftp zImage_buffer zImage");
				Run_command("bootm zImage_buffer");
				break;

			case 'O':
				reset_cpu(0);
				break;
			case 'Q':
				return 0;
		}//~~switch().
		if(strlen(cmd))
			Run_command(cmd);
	} while(1);
	return 0;
}

U_BOOT_CMD(
	menu,     1,     0,      do_menu,
	"menu      - some useful functions combine.\n",
	"menu\n"
);


#if 0

U_BOOT_CMD(clko, 2, 0, do_clko,
            "clko 	-	Select clock source for CLKO\n",
            " Default is 1/8 of ARM core\n\
          <0> - display current clko selection \n\
          <1> - mpl_dpdgck_clk (MPLL) \n\
          <2> - ipg_clk_ccm (IPG) \n\
          <3> - upl_dpdgck_clk (UPLL) \n\
          <4> - pll_ref_clk \n\
          <5> - fpm_ckil512_clk \n\
          <6> - ipg_clk_ahb_arm (AHB) \n\
          <7> - ipg_clk_arm (ARM) \n\
          <8> - spl_dpdgck_clk (SPLL) \n\
          <9> - ckih \n\
          <10> - ipg_clk_ahb_emi_clk \n\
          <11> - ipg_clk_ipu_hsp \n\
          <12> - ipg_clk_nfc_20m \n\
          <13> - ipg_clk_perclk_uart1 (IPG_PER)"
           );

static u8* clko_name[] ={
    "NULL",
    "1/8 of mpl_dpdgck_clk (MPLL)",
    "ipg_clk_ccm (IPG)",
    "1/8 of upl_dpdgck_clk (UPLL)",
    "pll_ref_clk",
    "fpm_ckil512_clk",
    "ipg_clk_ahb_arm (AHB)",
    "1/8 of ipg_clk_arm (ARM)",
    "1/8 of spl_dpdgck_clk (SPLL)",
    "ckih",
    "ipg_clk_ahb_emi_clk",
    "ipg_clk_ipu_hsp",
    "ipg_clk_nfc_20m",
    "ipg_clk_perclk_uart1 (IPG_PER)",
};

#define CLKO_MAX_INDEX          (sizeof(clko_name) / sizeof(u8*))

static void do_clko(int argc,char *argv[])
{
    u32 action = 0, cosr;

    if (!scan_opts(argc, argv, 1, 0, 0, (void*) &action,
                   OPTION_ARG_TYPE_NUM, "action"))
        return;

    if (action >= CLKO_MAX_INDEX) {
        printf("%d is not supported\n\n", action);
        return;
    }

    cosr = readl(CCM_BASE_ADDR + CLKCTL_COSR);

    if (action != 0) {
        cosr = (cosr & (~0x1FF)) + action - 1;
        if (action == 1 || action == 3 || action == 7 || action == 8) {
            cosr |= (0x3 << 6); // make it divided by 8
        }
        writel(cosr, CCM_BASE_ADDR + CLKCTL_COSR);
        printf("Set clko to ");
    }

    cosr = readl(CCM_BASE_ADDR + CLKCTL_COSR);
    printf("%s\n", clko_name[(cosr & 0xF) + 1]);
    printf("COSR register[0x%x] = 0x%x\n", 
                (CCM_BASE_ADDR + CLKCTL_COSR), cosr);
}

#ifdef L2CC_ENABLED
/*
 * This command is added for some simple testing only. It turns on/off
 * L2 cache regardless of L1 cache state. The side effect of this is
 * when doing any flash operations such as "fis init", the L2
 * will be turned back on along with L1 caches even though it is off
 * by using this command.
 */
U_BOOT_CMD(L2, 2, 0, do_L2_caches,
            "L2	-	L2 cache",
            "[ON | OFF]"
           );

void do_L2_caches(int argc, char *argv[])
{
    u32 oldints;
    int L2cache_on=0;

    if (argc == 2) {
        if (strcasecmp(argv[1], "on") == 0) {
            HAL_DISABLE_INTERRUPTS(oldints);
            HAL_ENABLE_L2();
            HAL_RESTORE_INTERRUPTS(oldints);
        } else if (strcasecmp(argv[1], "off") == 0) {
            HAL_DISABLE_INTERRUPTS(oldints);
            HAL_CLEAN_INVALIDATE_L2();
            HAL_DISABLE_L2();
            HAL_RESTORE_INTERRUPTS(oldints);
        } else {
            printf("Invalid L2 cache mode: %s\n", argv[1]);
        }
    } else {
        HAL_L2CACHE_IS_ENABLED(L2cache_on);
        printf("L2 cache: %s\n", L2cache_on?"On":"Off");
    }
}
#endif //L2CC_ENABLED

#define IIM_ERR_SHIFT       8
#define POLL_FUSE_PRGD      (IIM_STAT_PRGD | (IIM_ERR_PRGE << IIM_ERR_SHIFT))
#define POLL_FUSE_SNSD      (IIM_STAT_SNSD | (IIM_ERR_SNSE << IIM_ERR_SHIFT))

static void fuse_op_start(void)
{
    /* Do not generate interrupt */
    writel(0, IIM_BASE_ADDR + IIM_STATM_OFF);
    // clear the status bits and error bits
    writel(0x3, IIM_BASE_ADDR + IIM_STAT_OFF);
    writel(0xFE, IIM_BASE_ADDR + IIM_ERR_OFF);
}

/*
 * The action should be either:
 *          POLL_FUSE_PRGD 
 * or:
 *          POLL_FUSE_SNSD
 */
static int poll_fuse_op_done(int action)
{

    u32 status, error;

    if (action != POLL_FUSE_PRGD && action != POLL_FUSE_SNSD) {
        printf("%s(%d) invalid operation\n", __FUNCTION__, action);
        return -1;
    }

    /* Poll busy bit till it is NOT set */
    while ((readl(IIM_BASE_ADDR + IIM_STAT_OFF) & IIM_STAT_BUSY) != 0 ) {
    }

    /* Test for successful write */
    status = readl(IIM_BASE_ADDR + IIM_STAT_OFF);
    error = readl(IIM_BASE_ADDR + IIM_ERR_OFF);

    if ((status & action) != 0 && (error & (action >> IIM_ERR_SHIFT)) == 0) {
        if (error) {
            printf("Even though the operation seems successful...\n");
            printf("There are some error(s) at addr=0x%x: 0x%x\n",
                        (IIM_BASE_ADDR + IIM_ERR_OFF), error);
        }
        return 0;
    }
    printf("%s(%d) failed\n", __FUNCTION__, action);
    printf("status address=0x%x, value=0x%x\n",
                (IIM_BASE_ADDR + IIM_STAT_OFF), status);
    printf("There are some error(s) at addr=0x%x: 0x%x\n",
                (IIM_BASE_ADDR + IIM_ERR_OFF), error);
    return -1;
}

static void sense_fuse(int bank, int row, int bit)
{
    int addr, addr_l, addr_h, reg_addr;

    fuse_op_start();
    
    addr = ((bank << 11) | (row << 3) | (bit & 0x7));
    /* Set IIM Program Upper Address */
    addr_h = (addr >> 8) & 0x000000FF;
    /* Set IIM Program Lower Address */
    addr_l = (addr & 0x000000FF);

#ifdef IIM_FUSE_DEBUG
    printf("%s: addr_h=0x%x, addr_l=0x%x\n",
                __FUNCTION__, addr_h, addr_l);
#endif
    writel(addr_h, IIM_BASE_ADDR + IIM_UA_OFF);
    writel(addr_l, IIM_BASE_ADDR + IIM_LA_OFF);
    /* Start sensing */
    writel(0x8, IIM_BASE_ADDR + IIM_FCTL_OFF);
    if (poll_fuse_op_done(POLL_FUSE_SNSD) != 0) {
        printf("%s(bank: %d, row: %d, bit: %d failed\n",
                    __FUNCTION__, bank, row, bit);
    }
    reg_addr = IIM_BASE_ADDR + IIM_SDAT_OFF;
    printf("fuses at (bank:%d, row:%d) = 0x%x\n", bank, row, readl(reg_addr));
}

void do_fuse_read(int argc, char *argv[])
{
    int bank, row;

    if (argc == 1) {
        printf("Useage: fuse_read <bank> <row>\n");
        return;
    } else if (argc == 3) {
        if (!parse_num(*(&argv[1]), (unsigned long *)&bank, &argv[1], " ")) {
                printf("Error: Invalid parameter\n");
            return;
        }
        if (!parse_num(*(&argv[2]), (unsigned long *)&row, &argv[2], " ")) {
                printf("Error: Invalid parameter\n");
                return;
            }

        printf("Read fuse at bank:%d row:%d\n", bank, row);
        sense_fuse(bank, row, 0);

    } else {
        printf("Passing in wrong arguments: %d\n", argc);
        printf("Useage: fuse_read <bank> <row>\n");
    }
}

/* Blow fuses based on the bank, row and bit positions (all 0-based)
*/
static int fuse_blow(int bank,int row,int bit)
{
    int addr, addr_l, addr_h, ret = -1;

    fuse_op_start();

    /* Disable IIM Program Protect */
    writel(0xAA, IIM_BASE_ADDR + IIM_PREG_P_OFF);

    addr = ((bank << 11) | (row << 3) | (bit & 0x7));
    /* Set IIM Program Upper Address */
    addr_h = (addr >> 8) & 0x000000FF;
    /* Set IIM Program Lower Address */
    addr_l = (addr & 0x000000FF);

#ifdef IIM_FUSE_DEBUG
    printf("blowing addr_h=0x%x, addr_l=0x%x\n", addr_h, addr_l);
#endif

    writel(addr_h, IIM_BASE_ADDR + IIM_UA_OFF);
    writel(addr_l, IIM_BASE_ADDR + IIM_LA_OFF);
    /* Start Programming */
    writel(0x31, IIM_BASE_ADDR + IIM_FCTL_OFF);
    if (poll_fuse_op_done(POLL_FUSE_PRGD) == 0) {
        ret = 0;
    }

    /* Enable IIM Program Protect */
    writel(0x0, IIM_BASE_ADDR + IIM_PREG_P_OFF);
    return ret;
}

/*
 * This command is added for burning IIM fuses
 */
RedBoot_cmd("fuse_read",
            "read some fuses",
            "<bank> <row>",
            do_fuse_read
           );

RedBoot_cmd("fuse_blow",
            "blow some fuses",
            "<bank> <row> <value>",
            do_fuse_blow
           );

#define         INIT_STRING              "12345678"
static char ready_to_blow[] = INIT_STRING;

void quick_itoa(u32 num, char *a) 
{
    int i, j, k;        
    for (i = 0; i <= 7; i++) {
        j = (num >> (4 * i)) & 0xF;
        k = (j < 10) ? '0' : ('a' - 0xa);
        a[i] = j + k;
    }
}

void do_fuse_blow(int argc, char *argv[])
{
    int bank, row, value, i;

    if (argc == 1) {
        printf("It is too dangeous for you to use this command.\n");
        return;
    } else if (argc == 2) {
        if (strcasecmp(argv[1], "nandboot") == 0) {
            quick_itoa(readl(EPIT_BASE_ADDR + EPITCNR), ready_to_blow);
            printf("%s\n", ready_to_blow);
        }
        return;
    } else if (argc == 3) {
        if (strcasecmp(argv[1], "nandboot") == 0 && 
            strcasecmp(argv[2], ready_to_blow) == 0) {
#if defined(CYGPKG_HAL_ARM_MXC91131) || defined(CYGPKG_HAL_ARM_MX21) || defined(CYGPKG_HAL_ARM_MX27) || defined(CYGPKG_HAL_ARM_MX31)
            printf("No need to blow any fuses for NAND boot on this platform\n\n");
#else
            printf("Ready to burn NAND boot fuses\n");
            if (fuse_blow(0, 16, 1) != 0 || fuse_blow(0, 16, 7) != 0) {
                printf("NAND BOOT fuse blown failed miserably ...\n");
            } else {
                printf("NAND BOOT fuse blown successfully ...\n");
            }
        } else {
            printf("Not ready: %s, %s\n", argv[1], argv[2]);
#endif
        }
    } else if (argc == 4) {
        if (!parse_num(*(&argv[1]), (unsigned long *)&bank, &argv[1], " ")) {
                printf("Error: Invalid parameter\n");
                return;
        }
        if (!parse_num(*(&argv[2]), (unsigned long *)&row, &argv[2], " ")) {
                printf("Error: Invalid parameter\n");
                return;
        }
        if (!parse_num(*(&argv[3]), (unsigned long *)&value, &argv[3], " ")) {
                printf("Error: Invalid parameter\n");
                return;
        }

        printf("Blowing fuse at bank:%d row:%d value:%d\n",
                    bank, row, value);
        for (i = 0; i < 8; i++) {
            if (((value >> i) & 0x1) == 0) {
                continue;
            }
            if (fuse_blow(bank, row, i) != 0) {
                printf("fuse_blow(bank: %d, row: %d, bit: %d failed\n",
                            bank, row, i);
            } else {
                printf("fuse_blow(bank: %d, row: %d, bit: %d successful\n",
                            bank, row, i);
            }
        }
        sense_fuse(bank, row, 0);

    } else {
        printf("Passing in wrong arguments: %d\n", argc);
    }
    /* Reset to default string */
    strcpy(ready_to_blow, INIT_STRING);;
}

/* precondition: m>0 and n>0.  Let g=gcd(m,n). */
int gcd(int m, int n)
{
	int t;
	while(m > 0) {
		if(n > m) {t = m; m = n; n = t;} /* swap */
		m -= n;
	}
	return n;
}
#endif


