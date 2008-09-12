#include <linux/types.h>
#include <asm/arch/ep93xx.h>



int interrupt_init (void);
void reset_timer (void);
ulong get_timer (ulong base);
void set_timer (ulong t);
void udelay (unsigned long usec);
void reset_timer_masked (void);
ulong get_timer_masked (void);
void udelay_masked (unsigned long usec);
unsigned long long get_ticks(void);
ulong get_tbclk (void);


int timer_load_val = 0;

/* macro to read the 16 bit timer */
static inline ulong READ_TIMER(void)
{
	return (Timer1Value & 0xffff);
}

static ulong timestamp;
static ulong lastdec;

int interrupt_init (void)
{
        Timer1Control = 0x00; /* use timer 1 with 2KHz and free running */
        if (timer_load_val == 0)
	{
		/*
		 * for 10 ms clock period @ PCLK with 4 bit divider = 1/2
		 * (default) and prescaler = 16. Should be 10390
		 * @33.25MHz and 15625 @ 50 MHz
		 */
	        timer_load_val = 21; /* set to constant just now, until I resolve clocking issues */
	}
	/* auto load, manual update of Timer 1 */
	lastdec = Timer1Load = timer_load_val;
	Timer1Control = 0xC0; /* Enable the timer and periodic mode*/

	return (0);
}

/*
 * timer without interrupts
 */

void reset_timer (void)
{
	reset_timer_masked ();
}

ulong get_timer (ulong base)
{
	return get_timer_masked () - base;
}

void set_timer (ulong t)
{
	timestamp = t;
}

void udelay (unsigned long usec)
{
	ulong tmo;

	tmo = usec / 1000;
	tmo *= (timer_load_val * 100);
	tmo /= 1000;

	tmo += get_timer (0);

	while (get_timer_masked () < tmo)
		/*NOP*/;
}

void reset_timer_masked (void)
{
	/* reset time */
	lastdec = READ_TIMER();
	timestamp = 0;
}

ulong get_timer_masked (void)
{
	ulong now = READ_TIMER();

	if (lastdec >= now) {
		/* normal mode */
		timestamp += lastdec - now;
	} else {
		/* we have an overflow ... */
		timestamp += lastdec + timer_load_val - now;
	}
	lastdec = now;

	return timestamp;
}

void udelay_masked (unsigned long usec)
{
	ulong tmo;

	tmo = usec / 1000;
	tmo *= (timer_load_val * 100);
	tmo /= 1000;

	reset_timer_masked ();

	while (get_timer_masked () < tmo)
		/*NOP*/;
}

/*
 * This function is derived from PowerPC code (read timebase as long long).
 * On ARM it just returns the timer value.
 */
unsigned long long get_ticks(void)
{
	return get_timer(0);
}

/*
 * This function is derived from PowerPC code (timebase clock frequency).
 * On ARM it returns the number of timer ticks per second.
 */
ulong get_tbclk (void)
{
	ulong tbclk;

	tbclk = timer_load_val * 100;
	return tbclk;
}


/*
 * Self-explainatory
 */
void reset_cpu(void)
{
	/* We must generate a 1 to 0 transaction on bit 31 */

	/* Unlock DeviceCfg and write '1' */
	SYSCON_SW_UNCLOCK;
	SysconDEVCFG |= (1 << 31);
	/* Unlock DeviceCfg and write '0' */
	SYSCON_SW_UNCLOCK;
	SysconDEVCFG &= ~(1 << 31);

	/* Dying ... */
	while(1);
}
