#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>		/* open */
#include <unistd.h>		/* exit */

#define DEV_NAME "/dev/ADC"

int usleep(useconds_t usec);

int main(int argc, const char *argv[])
{
	if (argc < 2){
		printf("Error! Required 2 arguements.\n");
		exit(-1);
	}
	
	int file_desc;
	int bytes_read;
	unsigned char uValue = atoi(argv[1]);
	int i;

	file_desc = open(DEV_NAME, O_RDWR); // Open the Driver

	if (file_desc < 0) {
		printf("Can't open device file: %s\n", DEV_NAME);
		exit(-1);
	}
	
	write(file_desc, &uValue, 1);

	//for (i = 0; i < 10; i++) {
	//	write(file_desc, &uValue, 1);

	//	usleep(1000L * 1000);

		//uValue = !uValue;
	//}

	close(file_desc);

	return 1;
}
