#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <scsi/scsi.h>

typedef struct my_scsi_ioctl_command {
        unsigned int inlen;  /* _excluding_ scsi command length */
        unsigned int outlen;
        unsigned char data[1];  /* was 0 but that's not ISO C!! */
                /* on input, scsi command starts here then opt. data */
} My_Scsi_Ioctl_Command;

#define OFF (2 * sizeof(unsigned int))

#ifndef SCSI_IOCTL_SEND_COMMAND
#define SCSI_IOCTL_SEND_COMMAND 1
#endif

#define INQUIRY_CMD     0x12
#define INQUIRY_CMDLEN  6
#define INQUIRY_REPLY_LEN 96


int main(int argc, char * argv[])
{
    int s_fd, res, to;
    unsigned char inq_cdb [INQUIRY_CMDLEN] = {INQUIRY_CMD, 0, 0, 0, INQUIRY_REPLY_LEN, 0};
    unsigned char *inqBuff = (unsigned char *) malloc(OFF + sizeof(inq_cdb) + 512);
    unsigned char *buffp = inqBuff + OFF;
    My_Scsi_Ioctl_Command * ishp = (My_Scsi_Ioctl_Command *)inqBuff;

    /* WARN: This is hard-code for my system. Change accordingly */
    char *file_name = "/dev/sda2";
    int oflags = 0;

    if (0 == file_name) {
        printf("Provide a SCSI device name\n");
        return 1;
    }

    s_fd = open(file_name, oflags | O_RDWR);
    if (s_fd < 0) {
        if ((EROFS == errno) || (EACCES == errno)) {
            s_fd = open(file_name, oflags | O_RDONLY);
            if (s_fd < 0) {
                perror("scsi_inquiry: open error");
                return 1;
            }
        }
        else {
            perror("scsi_inquiry: open error");
            return 1;
        }
    }
    /* Don't worry, being very careful not to write to a none-scsi file ... */
    res = ioctl(s_fd, SCSI_IOCTL_GET_BUS_NUMBER, &to);
    if (res < 0) {
        printf("scsi_inquiry: not a scsi device\n");
        return 1;
    }

    ishp->inlen = 0;
    ishp->outlen = INQUIRY_REPLY_LEN;
    memcpy(buffp, inq_cdb, INQUIRY_CMDLEN);
    res = ioctl(s_fd, SCSI_IOCTL_SEND_COMMAND, inqBuff);
    if (0 == res) {
        to = (int)*(buffp + 7);
        printf("    %.8s  %.16s  %.4s, byte_7=0x%x\n", buffp + 8,
               buffp + 16, buffp + 32, to);
    }
    else if (res < 0)
        perror("scsi_inquiry: SCSI_IOCTL_SEND_COMMAND err");
    else
        printf("scsi_inquiry: SCSI_IOCTL_SEND_COMMAND status=0x%x\n", res);

    res = close(s_fd);
    if (res < 0) {
        perror("scsi_inquiry: close error");
        return 1;
    }
    return 0;
}
