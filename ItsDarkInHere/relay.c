#include <linux/module.h> /* Needed by all modules */
#include <linux/kernel.h> /* Needed for KERN_INFO */
#include <linux/init.h> /* Needed for the macros */
#include <linux/sched/signal.h> // tty stuff for Log function
#include <linux/sched.h> // tty stuff for Log function
#include <linux/tty.h>	// tty stuff for Log function
#include <asm/io.h>	// ioremap, ioread32, iowrite32
#include <asm/delay.h>	// udelay
#include <linux/mfd/ti_am335x_tscadc.h>

#define DEV_NAME "KernelDriver"
#define SUCCESS 0

void Log(char *Fmt, ...);
void initPins(void);
void msleep(int ms);
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *filp, char *buffer,	size_t length, loff_t * offset);
static ssize_t device_write(struct file *filp, const char *buff, size_t len, loff_t * off);


int Major;
static int Device_Open = 0;
char localBuffer[20];
unsigned int uvGPIO2;

//TODO fill in the following file_operations table as needed
static struct file_operations fops = {
	.read = device_read,
	.write = device_write,
	.open = device_open,
	.release = device_release
};

static int __init startDriver(void)
{
	Log(" ");
	Log(" ");
	Log("------------------------------------------------------------------ ");
	Log(" ");
	Log(" ");


	Log("Loading module");
	
	Major = register_chrdev(0, DEV_NAME, &fops);

	Log("led: Major = %d", Major);

	initPins();

	return SUCCESS;
}

static void __exit endDriver(void)
{
	unregister_chrdev(Major, DEV_NAME);

	printk(KERN_INFO "Goodbye for now\n");

	// Remember to deallocate memory and deinitialize pins, etc. as needed
	// free();

	iounmap(uvGPIO2); 

}

module_init(startDriver);
module_exit(endDriver);


static int device_open(struct inode *inode, struct file *file)
{
	unsigned long uValue;

	Log("device_open");

	if (Device_Open)
		return -EBUSY;

	Device_Open++;
	try_module_get(THIS_MODULE);

	//TODO - do the stuff that needs to happen when the driver is opened
	
	return SUCCESS;
}

/* 
 * Called when a process closes the device file.
 */
static int device_release(struct inode *inode, struct file *file)
{
	unsigned long uValue;

	Device_Open--;		/* We're now ready for our next caller */

	/* 
	 * Decrement the usage count, or else once you opened the file, you'll
	 * never get get rid of the module. 
	 */
	module_put(THIS_MODULE);

	//TODO - do the stuff that needs to happen when the driver is closed

	
	Log("device_release");

	return SUCCESS;
}

void initPins(void)
{
	unsigned int uValue, uvaddr;
	
	//TODO initialize any Beaglebone pins, as needed
	uvaddr = ioremap(0x44E10000, 0x1000);	// Control Module
	
	// Setup alternate function modes
	uValue = ioread32(uvaddr + 0x894);
	iowrite32(uValue | 0x1F, uvaddr + 0x894); // GPIO2_3 - Pull-up if doest work 0x17, mode 7

	uValue = ioread32(uvaddr + 0x890);
	iowrite32(uValue | 0x1F, uvaddr + 0x890); // GPIO2_2 - Pull-up if doest work 0x17, mode 7
	// Initialize GPIO2 directions
	uvGPIO2 = ioremap(0x481AC000, 0x1000); // GPIO2
	uValue = ioread32(uvGPIO2 + (0x134)); // OE
	iowrite32(uValue &= ~((1<<3)|(1<<2)), uvGPIO2 + 0x134); // GPIO2_3 and GPIO2_2 output
	
	iounmap(uvaddr);

}

static ssize_t device_read(struct file *filp, /* see include/linux/fs.h */
			   char *buffer,	/* buffer to fill with data */
			   size_t length, /* length of the buffer */
			   loff_t * offset) {
	/*
	 * Number of bytes actually written to the user's buffer
	 */
	int bytes_written = 0;

		// Copy data from local buffer in kernel space to user buffer in user space
	put_user(localBuffer, buffer++);

	//TODO Write code to handle transfering the desired data to the user
	
	//TODO update bytes_written as needed
	
	return bytes_written;
}

static ssize_t device_write(struct file *filp, const char *buff, size_t len, loff_t * off) {
	int Ret;
	unsigned long temp;

	// Copy data from user space to local buffer in kernel space
	if (len > 20)
		return 0;

	Ret = copy_from_user(localBuffer, buff, len);

	if (Ret)
		return Ret;	// Error, not all bytes written
	
	// TODO: Write code to handle the data sent to the driver
	if (localBuffer[0] == 1) {
		temp = ioread32(uvGPIO2 + 0x138);
		temp = temp | ((1<<3) | (1<<2));
		iowrite32(temp, uvGPIO2 + 0x13C);
	}
	if (localBuffer[0] == 0) {
		temp = ioread32(uvGPIO2 + 0x138);
		temp = temp & ~((1<<3)|(1<<2));
		iowrite32(temp, uvGPIO2 + 0x13C);
	}

	return len;	// Successfully wrote len bytes
}

void msleep(int ms)
{
	int i;

	for (i = 0; i < ms; i++)
		udelay(1000);
}

//TODO - Recognize that Log is a function that we have added to our driver to make it easier to print debug information to the terminal. It uses a variable number of parameters like printf.
// Example:  Log("The value of register %d is %x", reg, regValue);
void Log(char *Fmt, ...)
{
	char OutStr[128];
	struct tty_struct *my_tty;

	va_list ap;

	va_start(ap, Fmt);

	vsprintf(OutStr, Fmt, ap);

	va_end(ap);

	my_tty = current->signal->tty;

	if (my_tty) {
	
		((my_tty->driver)->ops->write) (my_tty, OutStr, strlen(OutStr));	
	
		((my_tty->driver)->ops->write) (my_tty, "\015\012", 2);  // CR LF
	}
	
	printk(KERN_ALERT "%s\n", OutStr);
}

