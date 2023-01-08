#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
	FILE *exe, *target;
	char mz_magic[2], buffer;
	int ne_offset, i, buffer16;

	if (argc != 3) {
		printf("Usage: mdos4fix <source> <target>\n");
		exit(1);
	}

	exe = fopen(argv[1], "rb");
	if (errno) {
		perror("Cannot open source file");
		exit(1);
	}

	fread(mz_magic, 1, 2, exe);
	if (mz_magic[0] != 'M' || mz_magic[1] != 'Z') {
		printf("Not an MZ executable\n");
		exit(1);
	}

	fseek(exe, 0x3C, SEEK_SET);
	fread(&ne_offset, 2, 1, exe);
	if (ne_offset == 0) {
		printf("Not an NE executable\n");
		exit(1);
	}

	target = fopen(argv[2], "wb");
	if (errno) {
		perror("Cannot open target file");
		exit(1);
	}
	fseek(exe, 0, SEEK_SET);

	for (i = 0; i < ne_offset; i++) {
		fread(&buffer, 1, 1, exe);
		fwrite(&buffer, 1, 1, target);
	}
	fseek(target, 0x3c, SEEK_SET);
	buffer16 = 0;
	fwrite(&buffer16, 2, 1, target);
	fseek(target, 0x04, SEEK_SET);
	buffer16 = ne_offset / 512 + (ne_offset % 512 == 0 ? 0 : 1);
	fwrite(&buffer16, 2, 1, target);

	fclose(target);
	fclose(exe);
}
