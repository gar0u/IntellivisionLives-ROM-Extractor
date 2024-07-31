// @originalSource	http://mercatopo-en.blogspot.com/2012/01/how-t0-extract-legal-roms-from.html
// @originalAuthor	Fabio Battaglia

// @modifiedBy		William Hutton <williamhutton@gmail.com>
// @version		2014.0709

// @description		Modified to send a little more output to STDOUT to understand the program
//			execution.
//
//			Used the 'bless' hex editor in Ubuntu 12.04 LTS to examine 'gameroms.rez' file
//			and find the corret 'FOOTER_OFFSET' for the PlayStation 2 "Intellivision Lives"
//			disc.


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// #define FOOTER_OFFSET 0x14e800
#define FOOTER_OFFSET 0x152800

// WARNING: Packing this structure is _important_
// Otherwise the compiler might "optimize" it increasing
// the size.
typedef struct __attribute__((__packed__)) {
	char fname[80];
	unsigned short int size;
	char data[26];
} game_entry;

int main (int argc, char *argv[]) {
	long current_game_offset = 0;
	int str_len, idx, current_game = 0;
	game_entry g_ent;
	unsigned char game_buffer[0xFFFF]; // That's 65kb worth of data!
	FILE *fd = NULL, *dest = NULL;

	if (argc < 2) {
		fprintf(stdout, "%s path/to/gameroms.rez\n", argv[0]);
		return -1;
	}

        printf("Trying to open file: %s\n", argv[1]);

	fd = fopen(argv[1], "r");
	if (!fd) return -1;

        int whileI = 0;

	while (1) {
		whileI++;
		printf("Pass %i.  Looking for footers...\n", whileI);

		fseek(fd, FOOTER_OFFSET + sizeof(game_entry) * current_game, SEEK_SET);
		fread(&g_ent, sizeof(game_entry), 1, fd);

		if (strncmp(g_ent.fname, "NTV", 3) != 0) {
		   	printf("Found 'NTV'.  Breaking out of while loop...\n");
			break;
		}

		fseek(fd, current_game_offset, SEEK_SET);
		fread(game_buffer, g_ent.size, 1, fd);

		str_len = strlen(g_ent.fname);

		printf("Found a game named: %s\n", g_ent.fname);

		for (idx = str_len; idx >= 0; idx--)
			if (g_ent.fname[idx] == '\\') break;

		fprintf(stdout, "Extracting game: %s - 0x%.4X bytes\n", g_ent.fname + idx + 1, g_ent.size);

		dest = fopen(g_ent.fname + idx + 1, "w");
		fwrite(game_buffer, g_ent.size, 1, dest);
		fclose(dest);

		current_game++;
		current_game_offset += g_ent.size;
	}

	fclose(fd);

	return 1;
}
