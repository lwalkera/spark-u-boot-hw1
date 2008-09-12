#include <common.h>
#include <command.h>
#include <exports.h>
#include <asm/types.h>
#include <asm/arch/mx31-regs.h>
#include <asm/arch/mx31_pins.h>
#include <asm/io.h>
#include <asm/arch/iomux.h>
#include <asm/arch/spi.h>

#if defined(CONFIG_PMIC)

static spi_config the_config;

extern int spi_hard_config(spi_config * config);
int pmic_init(void)
{
	/* Initialize the semaphore */
	the_config.module_number = 1;

	the_config.master_mode = 1;
	the_config.bit_rate = 4000000;
	the_config.bit_count = 32;
	the_config.active_high_polarity = 1;
	the_config.active_high_ss_polarity = 1;
	the_config.phase = 0;
	the_config.ss_low_between_bursts = 1;
	the_config.ss_asserted = 2;
	spi_hard_config(&the_config);

	return 0;
};

extern ssize_t spi_send_frame(__u8 *, __u32 , spi_config * );

int spi_send_frame_to_spi(int num_reg, unsigned int *reg_value, int rw)
{
	unsigned int frame_ret = 0;
	unsigned int result = 0;
	unsigned int frame = 0;
	unsigned int send_val;

	if (rw == 1) {
		frame |= 0x80000000;
	} else {
		frame = 0;
	}

	frame |= (*reg_value & 0x0ffffff);
	frame |= ((unsigned int)num_reg & 0x3f) << 0x19;

	send_val = (((frame & 0x000000ff) << 0x18) |
			((frame & 0x0000ff00) << 0x08) |
			((frame & 0x00ff0000) >> 0x08) |
			((frame & 0xff000000) >> 0x18));

	/* use this to launch SPI operation. */
	result = spi_send_frame((unsigned char *)(&send_val),
			(unsigned long)4, &the_config);

	frame_ret = (((send_val & 0x000000ff) << 0x18) |
			((send_val & 0x0000ff00) << 0x08) |
			((send_val & 0x00ff0000) >> 0x08) |
			((send_val & 0xff000000) >> 0x18));

	*reg_value = frame_ret & 0x00ffffff;

	return 0;
};

int pmic_write_reg(int num_reg, unsigned int *reg_value)
{
	return (spi_send_frame_to_spi(num_reg, reg_value, 1));
};

int pmic_read_reg(int num_reg, unsigned int *reg_value)
{
	return (spi_send_frame_to_spi(num_reg, reg_value, 0));
};
#if 1

int do_pmic ( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int reg, rw = 0, value=0;


	if(argc < 3)
	{
		printf("pmic <read/write> <reg> [value]\n");
		return 0;
	}

	if(strstr(argv[1], "write"))
		rw = 1;
	reg = simple_strtoul (argv[2], NULL, 16);

	if(argc > 3)
		value = simple_strtoul (argv[3], NULL, 16);

	pmic_init();

	if(rw)
	{
		/*pmic write*/
		printf("%#x -> [%#x]\n", value, reg);
		pmic_write_reg(reg, &value);
	}
	else
	{
		pmic_read_reg(reg, &value);
		printf("%#x: %#x\n", reg, value);
		/*pmic read*/
	}
	return 0;
}


U_BOOT_CMD(
	pmic,     4,     1,      do_pmic,
	"pmic      - Read/Write pmic register.\n",
	"<read/write> <reg> [value] \n"
);
#endif

#endif/*CONFIG_PMIC*/
