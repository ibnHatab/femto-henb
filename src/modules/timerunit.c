
#include <linux/module.h>
#include <linux/init.h>

static int __init timerunit_init(void)
{
	printk(KERN_INFO "Hello world from timerunit_ \n");
	return 0;
}

static void __exit timerunit_exit(void)
{
	printk(KERN_INFO "Goodbye world from timerunit_ \n");
}

static void timerunit_cb(void)
{
	printk(KERN_INFO " VOILA! I got into mod1fun \n");
}

EXPORT_SYMBOL(timerunit_cb);

module_init(timerunit_init);
module_exit(timerunit_exit);

MODULE_AUTHOR("VKI");
MODULE_DESCRIPTION("Timer unit callback module");
MODULE_LICENSE("GPL v2");
