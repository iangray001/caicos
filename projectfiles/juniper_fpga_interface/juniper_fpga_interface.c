#include "juniper_fpga_interface.h"

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define JUNIPER_PATH "/sys/class/juniper/juniper%d/juniper%d%c/"
#define JUNIPER_PATH_BUFSZ 100

static size_t juniper_fpga_formatpath(int devNo, int partNo, char* subpath, char* buf, int length)
{
	return snprintf(buf, length, JUNIPER_PATH "%s", devNo, devNo, 'a' + partNo, subpath);
}

int juniper_fpga_num_devices()
{
	// TODO: Actually check...
	return 1;
}

int juniper_fpga_num_partitions(int devNo)
{
	// TODO: Actually check...
	return 4;
}

int juniper_fpga_partition_idle(int devNo, int partNo)
{
	// Try and open the file
	// TODO: There must be a better way, no?
	char path[JUNIPER_PATH_BUFSZ];
	char idle;
	int fd;
	int rdAmt;

	juniper_fpga_formatpath(devNo, partNo, "accel_idle", path, JUNIPER_PATH_BUFSZ);

	fd = open(path, O_RDONLY);
	if(fd == -1)
		return JUNIPER_FPGA_FAIL_NOFILE;

	// Read the byte from it
	rdAmt = read(fd, &idle, 1);

	close(fd);

	if(rdAmt != 1)
		return JUNIPER_FPGA_FAIL_DRVERR;

	return idle;
}

int juniper_fpga_partition_start(int devNo, int partNo)
{
	char path[JUNIPER_PATH_BUFSZ];
	char go = '1';
	int fd;
	int wrAmt;

	juniper_fpga_formatpath(devNo, partNo, "accel_start", path, JUNIPER_PATH_BUFSZ);

	fd = open(path, O_WRONLY);
	if(fd == -1)
		return JUNIPER_FPGA_FAIL_NOFILE;

	wrAmt = write(fd, &go, 1);

	close(fd);

	if(wrAmt != 1)
		return JUNIPER_FPGA_FAIL_DRVERR;

	return JUNIPER_FPGA_OK;
}

int juniper_fpga_partition_get_mem_base(int devNo, int partNo, unsigned int* base)
{
	char path[JUNIPER_PATH_BUFSZ];
	unsigned char baseBuf[4];
	int fd;
	int rdAmt;
	unsigned int tmp;

	juniper_fpga_formatpath(devNo, partNo, "accel_mem_base", path, JUNIPER_PATH_BUFSZ);
	fd = open(path, O_RDONLY);
	if(fd == -1)
		return JUNIPER_FPGA_FAIL_NOFILE;

	rdAmt = read(fd, baseBuf, 4);
	close(fd);
	if(rdAmt != 4)
		return JUNIPER_FPGA_FAIL_DRVERR;

	// Pack it.
	tmp = 0;
	tmp |= baseBuf[0];
	tmp |= baseBuf[1] << 8;
	tmp |= baseBuf[2] << 16;
	tmp |= baseBuf[3] << 24;

	*base = tmp;
	return JUNIPER_FPGA_OK;
}

int juniper_fpga_partition_get_retval(int devNo, int partNo, unsigned int* rv)
{
	char path[JUNIPER_PATH_BUFSZ];
	unsigned char baseBuf[4];
	int fd;
	int rdAmt;
	unsigned int tmp;

	juniper_fpga_formatpath(devNo, partNo, "accel_retval", path, JUNIPER_PATH_BUFSZ);
	fd = open(path, O_RDONLY);
	if(fd == -1)
		return JUNIPER_FPGA_FAIL_NOFILE;

	rdAmt = read(fd, baseBuf, 4);
	close(fd);
	if(rdAmt != 4)
		return JUNIPER_FPGA_FAIL_DRVERR;

	// Pack it.
	tmp = 0;
	tmp |= baseBuf[0];
	tmp |= baseBuf[1] << 8;
	tmp |= baseBuf[2] << 16;
	tmp |= baseBuf[3] << 24;

	*rv = tmp;
	return JUNIPER_FPGA_OK;
}

int juniper_fpga_partition_set_mem_base(int devNo, int partNo, unsigned int base)
{
	char path[JUNIPER_PATH_BUFSZ];
	unsigned char baseBuf[4];
	int fd;
	int wrAmt;
	
	// Pack it...
	baseBuf[0] = base & 0xFF;
	baseBuf[1] = (base >> 8) & 0xFF;
	baseBuf[2] = (base >> 16) & 0xFF;
	baseBuf[3] = (base >> 24) & 0xFF;

	juniper_fpga_formatpath(devNo, partNo, "accel_ram_base", path, JUNIPER_PATH_BUFSZ);
	fd = open(path, O_WRONLY);
	if(fd == -1)
		return JUNIPER_FPGA_FAIL_NOFILE;

	wrAmt = write(fd, baseBuf, 4);
	close(fd);
	if(wrAmt != 4)
		return JUNIPER_FPGA_FAIL_DRVERR;

	return JUNIPER_FPGA_OK;
}

int juniper_fpga_partition_get_arg(int devNo, int partNo, int argNo, unsigned int* arg)
{
	char path[JUNIPER_PATH_BUFSZ];
	unsigned char argBuf[4];
	int fd;
	int rdAmt;
	unsigned int tmp;

	// Cheat...
	char fileName[] = "accel_arg0";
	int len = strlen(fileName);
	fileName[len - 1] += argNo;

	juniper_fpga_formatpath(devNo, partNo, fileName, path, JUNIPER_PATH_BUFSZ);

	fd = open(path, O_RDONLY);
	if(fd == -1)
		return JUNIPER_FPGA_FAIL_NOFILE;

	rdAmt = read(fd, argBuf, 4);
	close(fd);
	if(rdAmt != 4)
		return JUNIPER_FPGA_FAIL_DRVERR;

	// Pack it.
	tmp = 0;
	tmp |= argBuf[0];
	tmp |= argBuf[1] << 8;
	tmp |= argBuf[2] << 16;
	tmp |= argBuf[3] << 24;

	*arg = tmp;
	return JUNIPER_FPGA_OK;
}

int juniper_fpga_partition_set_arg(int devNo, int partNo, int argNo, unsigned int arg)
{
	char path[JUNIPER_PATH_BUFSZ];
	unsigned char argBuf[4];
	int fd;
	int wrAmt;

	// Cheat...
	char fileName[] = "accel_arg0";
	int len = strlen(fileName);

	fileName[len - 1] += argNo;
	
	// Pack it...
	argBuf[0] = arg & 0xFF;
	argBuf[1] = (arg >> 8) & 0xFF;
	argBuf[2] = (arg >> 16) & 0xFF;
	argBuf[3] = (arg >> 24) & 0xFF;

	juniper_fpga_formatpath(devNo, partNo, fileName, path, JUNIPER_PATH_BUFSZ);
	fd = open(path, O_WRONLY);
	if(fd == -1)
		return JUNIPER_FPGA_FAIL_NOFILE;

	wrAmt = write(fd, argBuf, 4);
	close(fd);
	if(wrAmt != 4)
		return JUNIPER_FPGA_FAIL_DRVERR;

	return JUNIPER_FPGA_OK;
}
