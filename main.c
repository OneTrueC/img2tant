/* img2tant: renders images as either sextant or octant blocks
 * this code is unlicensed and released to the public domain */
#define _POSIX_C_SOURCE 200809L

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include <errno.h>
#include <fcntl.h>
#include <locale.h>
#include <string.h>
#include <unistd.h>

#include <Imlib2.h>

#include "tants.h"

#define ISOCT (tants == &octants)
#define PIX(x,y) (pixels[((y) * width) + (x)])
#define ISWHITE(x) ((((x) & 0xFFFFFF) >= 0x7f7f7f) ^ inv)

const char* imlib_errs[] = {
	[0] = "Success",
	[-IMLIB_ERR_INTERNAL] = "Internal error (should not happen)",
	[-IMLIB_ERR_NO_LOADER] = "No loader for file format",
	[-IMLIB_ERR_NO_SAVER] = "No saver for file format",
	[-IMLIB_ERR_BAD_IMAGE] = "Invalid image file",
	[-IMLIB_ERR_BAD_FRAME] = "Requested frame not in image",
};

const char* shortopts = ":68hio:v";

static void die(int erno, char* message, ...);
static void usage(void);
static void version(void);

int
main(int argc, char** argv)
{
	const char *(*tants)[];
	char *filename, buf[256];
	FILE* output;
	Imlib_Image image;
	int x, y, fd, opt, len, inv, bits, width, height, moreWidth, moreHeight;
	uint32_t *pixels;

	inv = 0;
	tants = &sextants;
	output = stdout;

	while ((opt = getopt(argc, argv, shortopts)) != -1) {
		switch (opt) {
		case '6':
			tants = &sextants;
			break;
		case '8':
			tants = &octants;
			break;
		case 'i':
			inv = 1;
			break;
		case 'o':
			output = fopen(optarg, "w");
			
			if (output == NULL)
				die(6, "cannot open output file: %s", strerror(errno));
			break;
		case '?':
			usage();
			die(1, "unknown argument");
			break;
		case ':':
			usage();
			die(2, "no output file path given");
			break;
		case 'v':
			version();
			exit(0);
		default:
		case 'h':
			usage();
			exit(0);
		}
	}

	if (optind == argc)
		die(3, "no image filename given");
	else if (optind > argc + 1)
		die(4, "too many positional arguments");

	if (strcmp(argv[optind], "-") == 0) {
		filename = strdup("/tmp/img2tantXXXXXX");
		fd = mkstemp(filename);

		if (fd == -1)
			die(7, "failed to create temporary file: %s", strerror(errno));

		while ((len = read(STDIN_FILENO, buf, 256)) == 256)
			write(fd, buf, len);
		if (len > 0)
			write(fd, buf, len);

		close(fd);
	} else {
		filename = argv[optind];
		fd = 0;
	}

	image = imlib_load_image_with_errno_return(filename, &errno);

	if (fd) {
		unlink(filename);
		free(filename);
	}

	if (image == NULL) {
		if (errno > 0)
			die(5, "failed to open image: %s", strerror(errno));
		else if (errno < 0) {
			if (errno < IMLIB_ERR_BAD_FRAME)
				die(5, "failed to open image: imlib error #%i", -errno);
			else
				die(5, "failed to open image: imlib error: %s",
				    imlib_errs[-errno]);
		}
	}

	imlib_context_set_image(image);

	width = imlib_image_get_width();
	moreWidth = width % 2;

	height = imlib_image_get_height();
	moreHeight = height % (ISOCT ? 4 : 3);

	pixels = imlib_image_get_data_for_reading_only();

	setlocale(LC_ALL, "");

	for (y = 0; y < (height - moreHeight); y += (ISOCT ? 4 : 3)) {
		for (x = 0; x < (width - moreWidth); x += 2) {
			bits = 0;

			if (ISWHITE(PIX(x,y)))
				bits |= 0x1;
			if (ISWHITE(PIX(x+1,y)))
				bits |= 0x2;
			if (ISWHITE(PIX(x,y+1)))
				bits |= 0x4;
			if (ISWHITE(PIX(x+1,y+1)))
				bits |= 0x8;
			if (ISWHITE(PIX(x,y+2)))
				bits |= 0x10;
			if (ISWHITE(PIX(x+1,y+2)))
				bits |= 0x20;
			if (ISOCT) {
				if (ISWHITE(PIX(x,y+3)))
					bits |= 0x40;
				if (ISWHITE(PIX(x+1,y+3)))
					bits |= 0x80;
			}

			fprintf(output, (*tants)[bits]);
		}

		if (moreWidth > 0) {
			bits = 0;

			if (ISWHITE(PIX(x,y)))
				bits |= 0x1;
			if (ISWHITE(PIX(x,y+1)))
				bits |= 0x4;
			if (ISWHITE(PIX(x,y+2)))
				bits |= 0x10;
			if (ISOCT) {
				if (ISWHITE(PIX(x,y+3)))
					bits |= 0x40;
			}

			fprintf(output, (*tants)[bits]);
		}

		fprintf(output, "\n");
	}

	if (moreHeight > 0) {
		for (x = 0; x < (width - moreWidth); x += 2) {
			bits = 0;

			if (ISWHITE(PIX(x,y)))
				bits |= 0x1;
			if (ISWHITE(PIX(x+1,y)))
				bits |= 0x2;
			if (moreHeight > 1) {
				if (ISWHITE(PIX(x,y+1)))
					bits |= 0x4;
				if (ISWHITE(PIX(x+1,y+1)))
					bits |= 0x8;
				if (moreHeight > 2) {
					if (ISWHITE(PIX(x,y+2)))
						bits |= 0x10;
					if (ISWHITE(PIX(x+1,y+2)))
						bits |= 0x20;
				}
			}

			fprintf(output, (*tants)[bits]);
		}

		if (moreWidth > 0) {
			bits = 0;

			if (ISWHITE(PIX(x,y)))
				bits |= 0x1;
			if (moreHeight > 1) {
				if (ISWHITE(PIX(x,y+1)))
					bits |= 0x4;
				if (moreHeight > 2) {
					if (ISWHITE(PIX(x,y+2)))
						bits |= 0x10;
					if (ISWHITE(PIX(x+1,y+2)))
						bits |= 0x20;
				}
			}

			fprintf(output, (*tants)[bits]);
		}
	}

	imlib_free_image_and_decache();

	return 0;
}

static void
die(int erno, char* message, ...)
{
	va_list ap;

	va_start(ap, message);
	fprintf(stderr, "img2tant: ");
	vfprintf(stderr, message, ap);
	fprintf(stderr, "\n");
	va_end(ap);

	exit(erno);
}

static void
usage(void)
{
	puts("Usage: img2tant [-hiv] [-o FILE] IMAGEFILE\n"
	     "\n"
	     "  -6    force the use of sextant blocks\n"
	     "  -8    force the use of octant blocks\n"
	     "  -i    treat white pixels as black and black pixels as white\n"
	     "  -o    send output to file instead of stdout\n"
	     "  -h    print help and exit\n"
	     "  -v    print version and exit");
}

static void
version(void)
{
	puts("img2tant v"VERSION"\n"
	     "written by Drew Marino (OneTrueC)");
}
