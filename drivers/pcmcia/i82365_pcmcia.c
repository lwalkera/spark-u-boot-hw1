#include <common.h>
#include <config.h>

#ifdef CONFIG_I82365

#include <pcmcia.h>

extern int i82365_init (void);
extern void i82365_exit (void);

int pcmcia_on (void)
{
	u_int rc;

	debug ("Enable PCMCIA " PCMCIA_SLOT_MSG "\n");

	rc = i82365_init();

	if (rc == 0) {
		udelay(200000);
		rc = check_ide_device(0);
	}

	return (rc);
}

int pcmcia_off (void)
{
	printf ("Disable PCMCIA " PCMCIA_SLOT_MSG "\n");

	i82365_exit();

	return 0;
}

#endif /* CONFIG_I82365 */
