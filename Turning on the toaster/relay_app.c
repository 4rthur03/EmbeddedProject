#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>		/* open */
#include <unistd.h>		/* exit */

#define DEV_NAME "/dev/relay"

int usleep(useconds_t usec);

int main()
{
	int file_desc;
	int bytes_read;
	unsigned char uValue = 1;
	int i;

	file_desc = open(DEV_NAME, O_RDWR);
	if (file_desc < 0) {
		printf("Can't open device file: %s\n", DEV_NAME);
		exit(-1);
	}

	for (i = 0; i < 10; i++) {
		write(file_desc, &uValue, 1);

		usleep(1000L * 1000);

		uValue = !uValue;
	}

	close(file_desc);

	return 1;
}
