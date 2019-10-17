#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>		/* open */
#include <unistd.h>		/* exit */

#define DEV_NAME "/dev/ADC"

int usleep(useconds_t usec);

int main()
{
	int file_desc;
	int bytes_read;
	unsigned char uValue[8];
	int i;
	
	unsigned short retVal;

	file_desc = open(DEV_NAME, O_RDWR); // Open the Driver

	if (file_desc < 0) {
		printf("Can't open device file: %s\n", DEV_NAME);
		exit(-1);
	}

	for (i = 0; i < 50; i++) {
		//write(file_desc, &uValue, 1);
		bytes_read = read(file_desc, &uValue, 8);

		//printf("byte_return value: %d\n", bytes_read);
		retVal = uValue[1];
		retVal = retVal << 8;
		retVal = retVal | uValue[0];
		//printf("retVal: %d\n", retVal);
		if (bytes_read > 0) {
			printf("ADC value = %d\n", retVal);
		}

		usleep(1000L * 1000);

		//uValue = !uValue;
	}

	close(file_desc);

	return 1;
}
