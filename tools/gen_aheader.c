#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define	IV_SIZE			16
#define	CERT_SIZE		288
#define	SIGE_SIZE		128

#define HEAD_SIGNATURE		0xB0087006

typedef struct {
	unsigned char		iv[IV_SIZE];
	unsigned int		boot_signature;
	unsigned int		load_address;
	unsigned int		run_address;
	unsigned int		firm_size;
	unsigned int		nand_offset;
	unsigned int		image_type;	/* see below */
	unsigned char		board_name[16];
	unsigned char		reserved[40];
	unsigned char 		certificate[CERT_SIZE];
	unsigned char 		signature[SIGE_SIZE];
} atxx_image_header_t;

#define IH_TYPE_INVALID         0       /* Invalid Image                */
#define IH_TYPE_STANDALONE      1       /* Standalone Program           */
#define IH_TYPE_KERNEL          2       /* OS Kernel Image              */
#define IH_TYPE_RAMDISK         3       /* RAMDisk Image                */
#define IH_TYPE_MULTI           4       /* Multi-File Image             */
#define IH_TYPE_FIRMWARE        5       /* Firmware Image               */
#define IH_TYPE_SCRIPT          6       /* Script file                  */
#define IH_TYPE_FILESYSTEM      7       /* Filesystem Image (any type)  */
#define IH_TYPE_FLATDT          8       /* Binary Flat Device Tree Blob */
#define IH_TYPE_KWBIMAGE        9       /* Kirkwood Boot Image          */

static void usage(void)
{
	printf("\n");
	printf( "Usage: makeboot -i <infile> -o <outfile>\n"
		"	-l <load_address> -r <run_address> -n <nand_offset>\n"
		"	[-t image_type] [-s] [-e]\n\n");
	printf("	-i: assign input file name\n");
	printf("	-o: assign output file name\n");
	printf("	-l: assign address to load\n");
	printf("	-r: assign address to run\n");
	printf("	-n: assign nand offset to store\n");
	printf("	-t: assign the image type: 1 -- standalone, 2 -- kernel, 3 -- ramdisk\n");
	printf("	-s: enable security\n");
	printf("	-e: enable encryption\n");
	printf("\n");
}

static int read_file(const char *name, char *buf, int size)
{
	FILE *fpr;

	fpr = fopen(name, "rb");
	if (fpr == NULL) {
		printf("Failed to open %s\n", name);
		return -1;
	}

	if (fread(buf, 1, size, fpr) != size) {
		printf("Failed to read %s\n", name);
		return -1;
	}

	fclose(fpr);

	return 0;
}

int main(int argc, char *argv[])
{
	FILE *fpr, *fpw;
	char *infile = NULL, *outfile = NULL;
	int opt, args, i;
	unsigned char buffer;

	int security = 0, encryption = 0;
	atxx_image_header_t hd;

	args = 0;
	memset(&hd, 0, sizeof(hd));
	hd.image_type = IH_TYPE_STANDALONE;

	while((opt = getopt(argc, argv, "i:o:b:l:r:n:t:s:e:h")) != -1 ) {
		switch(opt) {
		case 'i':
			infile = optarg;
			printf("input file: %s\n", infile);
			args++;
			break;
		case 'o':
			outfile = optarg;
			printf("output file: %s\n", outfile);
			args++;
			break;
		case 'l':
			hd.load_address = strtoll(optarg, NULL, 0);
			printf("load_address = 0x%08x\n", hd.load_address);
			args++;
			break;
		case 'r':
			hd.run_address = strtoll(optarg, NULL, 0);
			printf("run_address = 0x%08x\n", hd.run_address);
			args++;
			break;
		case 'n':
			hd.nand_offset = strtoll(optarg, NULL, 0);
			printf("nand_offset = 0x%08x\n", hd.nand_offset);
			args++;
			break;
		case 't':
			hd.image_type = strtoll(optarg, NULL, 0);
			printf("image_type = 0x%08x\n", hd.image_type);
			break;
		case 's':
			security = 1;
			printf("security is enabled\n");
			break;
		case 'e':
			encryption = 1;
			printf("encryption is enabled\n");
			break;
		case 'b':
			strcpy(hd.board_name, optarg);
			printf("board %s\n", hd.board_name);
			break;
		case 'h':
		default:
			usage();
			return -1;
		}
	}

	if (args < 5) {
		usage();
		return -1;
	}

	fpr = fopen(infile, "rb");
	if (fpr == NULL) {
		printf("Failed to open %s\n", infile);
		return -1;
	}

	fpw = fopen(outfile, "wb");
	if (fpw == NULL) {
		printf("Failed to open %s\n", outfile);
		return -1;
	}

	/* Get file size */
	fseek(fpr, 0, SEEK_END);
	hd.firm_size = ftell(fpr);
	fseek(fpr, 0, SEEK_SET);

	/*write the IV and certificate to the security_header*/
	if (security) {
		if (read_file("certificate.bin", hd.certificate, CERT_SIZE) != 0) {
			return -1;
		}

		if (read_file("signature.bin", hd.signature, CERT_SIZE) != 0) {
			return -1;
		}
	}

	if (encryption) {
		if (read_file("iv.bin", hd.iv, IV_SIZE) != 0) {
			return -1;
		}
	}

	hd.boot_signature = HEAD_SIGNATURE;

	/* write header */
	fwrite(&hd, sizeof(hd), 1, fpw);

	i = 0;
	/* write body */
	fread(&buffer,  1, 1, fpr);
	while(!feof(fpr)) {
		i++;
		fwrite(&buffer, 1, 1, fpw);
		fread(&buffer,  1, 1, fpr);
	}

	fclose(fpr);
	fclose(fpw);

	printf("Done!\n");

	return 0;
}
