#include <sys/select.h>
#include <err.h>
#include <fcntl.h>
#include <termios.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>

#define BUF_SIZE 32
#define CMD_COORDS              0x01

int fd = -1;
int quit = 0;

int
init(char* device)
{
	struct termios opt;

	fd = open(device, O_RDWR | O_NOCTTY| O_NDELAY);
	if (fd < 0) {
		perror("open");
		return 0;
	}

	tcflush(fd, TCIOFLUSH);

	if (tcgetattr(fd, &opt) < 0) {
		perror("tcgetattr");
		return 0;
	}

	/* 57600 baud */
	cfsetispeed(&opt, B57600);
	cfsetospeed(&opt, B57600);

	opt.c_cflag |= CLOCAL | CREAD;

	/* 8 bits, no parity, 1 stop bit */
	opt.c_cflag &= ~(PARENB | PARODD | CSTOPB | CSIZE);
	opt.c_cflag |= CS8;

	/* no hardware/software flow control */
	opt.c_cflag &= ~CRTSCTS;
	opt.c_iflag &= ~(IXON | IXOFF | IXANY);
	opt.c_iflag |= IGNBRK;

	/* raw input */
	opt.c_lflag = 0;
	opt.c_oflag = 0;

	opt.c_cc[VTIME] = 1;
	opt.c_cc[VMIN] = 60;

	if (tcsetattr(fd, TCSANOW, &opt) < 0) {
		perror("tcsetattr");
		return 0;
	}
	return 1;
}

void
done()
{
	close(fd);
}

void
sigint(int)
{
	quit++;
}

int
main(int argc, char* argv[])
{
	fd_set fds;
	char tmp_buf[BUF_SIZE];
	int cur_pos = 0;
	unsigned int cur_cmd = 0;
	unsigned int cmd_len = 0;

	if (argc != 2) {
		fprintf(stderr, "usage: touch_test device\n");
		return 1;
	}

	if (!init(argv[1]))
		return 1;

	signal(SIGINT, sigint);
	while (!quit) {
		FD_ZERO(&fds);
		FD_SET(fd, &fds);
		if (select(fd + 1, &fds, (fd_set*)NULL, (fd_set*)NULL, NULL) < 0) 
			break;

		if(!FD_ISSET(fd, &fds)) 
			continue;

		if (cmd_len == 0) {
			cur_pos = 0;
			if (!(read(fd, &cur_cmd, 1)))
				break;
			if (cur_cmd != CMD_COORDS) {
				printf("got unsupported command 0x%x, ignored\n", cur_cmd);
			} else {
				cmd_len = 4;
			}
			continue;
		} else {
			if (!(read(fd, &tmp_buf[cur_pos++], 1)))
				break;
			if (cur_pos != cmd_len)
				continue;
		}

		/* got a complete command */
		uint16_t x, y;
		x = tmp_buf[0] | (tmp_buf[1] << 8);
		y = tmp_buf[1] | (tmp_buf[2] << 8);
		printf("touch: x=%x, y=%x\n", x, y);
		cur_pos = 0;
	}

	done();
	return 0;
}
