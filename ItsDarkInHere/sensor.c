#include <linux/module.h> /* Needed by all modules */
#include <linux/kernel.h> /* Needed for KERN_INFO */
#include <linux/init.h> /* Needed for the macros */
#include <linux/sched/signal.h> // tty stuff for Log function
#include <linux/sched.h> // tty stuff for Log function
#include <linux/tty.h>	// tty stuff for Log function
#include <asm/io.h>	// ioremap, ioread32, iowrite32
#include <asm/delay.h>	// udelay
#include <linux/mfd/ti_am335x_tscadc.h>

MODULE_LICENSE("GPL");

#define DEV_NAME "KernelDriver"
#define SUCCESS 0

struct cdev c_dev;
struct class *cl;

void Log(char *Fmt, ...);
void initPins(void);
void msleep(int ms);
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *filp, char *buffer,	size_t length, loff_t * offset);
static ssize_t device_write(struct file *filp, const char *buff, size_t len, loff_t * off);

// Lab 2 changes
void initAnalog(void);

int Major = 240;
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
	
	// Major = register_chrdev(0, DEV_NAME, &fops);
	if (alloc_chrdev_region(&Major, 0, 1, "ADC") < 0)  //$cat /proc/devices
        {

       		 Log("1");

                return -1;

        }
	if ( (cl = class_create(THIS_MODULE, "ADC")) == NULL)	// $ls /sys/class
	{
		Log("2");
		unregister_chrdev_region(Major, 1);
		return -1;
	}

	if ( device_create(cl, NULL, Major, NULL, "ADC") == NULL) // $ls /dev/
	{
		Log("3");
		class_destroy(cl);
		unregister_chrdev_region(Major, 1);
		return -1;
	}

	cdev_init(&c_dev, &fops);

	if ( cdev_add(&c_dev, Major, 1) == -1)
	{
		Log("4");	
		device_destroy(cl, Major);
		class_destroy(cl);
		unregister_chrdev_region(Major, 1);
		return -1;
	}
	
	Log("led: Major = %d", Major);

	initPins();
	// Lab 2 changes
	initAnalog();

	return SUCCESS;
}

static void __exit endDriver(void)
{
	//unregister_chrdev(Major, DEV_NAME);
	cdev_del(&c_dev);
	device_destroy(cl, Major);
	class_destroy(cl);
	unregister_chrdev_region(Major, 1);

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
/*
	Enable TSC_ADC clock and setup TSC_ADC_SS registers

*/
void initAnalog(void)
{
	unsigned int uADC, uvADCaddr;

	// Enable the TSC_ADC clock 
	uvADCaddr = ioremap(0x44E00400, 0x100);
	iowrite32( 2, uvADCaddr + 0xBC); 
	iounmap(uvADCaddr);
	
	uvADCaddr = ioremap(0x44E0D000, 0x1000);
	// TSC_ADC_SS is at GPIO1_5
	//uADC = ioread32(uvADCaddr + REG_CTRL); // Enable the control to be able to write
	iowrite32( (1<<2), uvADCaddr + REG_CTRL);

	//uADC = ioread32(uvADCaddr + REG_STEPCONFIG(0)); // Uses 4-sample averaging and is software-controlled one shot
	iowrite32(STEPCONFIG_AVG(4) | STEPCONFIG_INP(2) | STEPCONFIG_MODE(0), uvADCaddr + REG_STEPCONFIG(0));

	//uADC = ioread32(uvADCaddr + REG_CTRL); // Enable the TSC_ADC_SS
	iowrite32(CNTRLREG_TSCSSENB, uvADCaddr + REG_CTRL);

	iounmap(uvADCaddr);
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
			   size_t length,         /* length of the buffer */
			   loff_t * offset) {
	/*
	 * Number of bytes actually written to the user's buffer
	 */
	int bytes_written = 0;

	int uADC;
	unsigned int uvADCaddr, tmpVal;

	// Copy data from local buffer in kernel space to user buffer in user space
	// put_user(localBuffer, buffer++);

	//TODO Write code to handle transfering the desired data to the user
	
	// Enable step
	uvADCaddr = ioremap(0x44E0D000, 0x1000);
	uADC = ioread32(uvADCaddr + REG_SE);
	iowrite32(uADC | ENB(1), uvADCaddr + REG_SE);
	
	do // Spin-wait until ADC is completed
	{
		tmpVal = ioread32(uvADCaddr + REG_ADCFSM);
		tmpVal = tmpVal & (1<<5);
	} while (tmpVal != 0);

	// Read the FIFO value
	uADC = ioread32(uvADCaddr + REG_FIFO0CNT);
	
	if ( uADC > 0)
	{
		uADC = ioread32(uvADCaddr + REG_FIFO0);
		// Put the low-byte into buffer using put_user()
		put_user(uADC & 0xFF, &(buffer[0]) );
		// Put the high-byte into buffer
		put_user( (( uADC & 0xFF00 ) >> 8) , &(buffer[1]) );
		bytes_written = 2;
	}
	//TODO update bytes_written as needed
	iounmap(uvADCaddr);

	return bytes_written;
}

static ssize_t device_write(struct file *filp, /* see include/linux/fs.h*/
			    const char *buff, /* buffer to write with data*/
			    size_t len, /* lenght of the buffer */
			    loff_t * off) {
	
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

