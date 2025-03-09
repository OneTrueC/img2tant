/* img2sext: renders images as sextant blocks */
#define _POSIX_C_SOURCE 200809L

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include <errno.h>
#include <string.h>

#include <unistd.h>

#include <Imlib2.h>

#define pix(x,y) (pixels[((y) * width) + (x)])
#define is_white(x) ((((x) & 0xFFFFFF) >= 0x7f7f7f) ^ inv)

/* I am aware there are better ways to do this, but i don't really care. I did
 * this because there is no sextant 135 or 246, and I didn't want to add edge-
 * cases.*/
const char* sextants[] = {" ", "🬀", "🬁", "🬂", "🬃", "🬄", "🬅", "🬆", "🬇", "🬈", "🬉",
                          "🬊", "🬋", "🬌", "🬍", "🬎", "🬏", "🬐", "🬑", "🬒", "🬓", "▌",
                          "🬔", "🬕", "🬖", "🬗", "🬘", "🬙", "🬚", "🬛", "🬜", "🬝", "🬞",
                          "🬟", "🬠", "🬡", "🬢", "🬣", "🬤", "🬥", "🬦", "🬧", "▐", "🬨",
                          "🬩", "🬪", "🬫", "🬬", "🬭", "🬮", "🬯", "🬰", "🬱", "🬲", "🬳",
                          "🬴", "🬵", "🬶", "🬷", "🬸", "🬹", "🬺", "🬻", "█"};
const char* shortopts = ":hio:v";

static void die(int erno, char* message, ...);
static void usage(void);
static void version(void);

int
main(int argc, char** argv)
{
	Imlib_Image image;
	Imlib_Load_Error error_return;
	int x, y, opt, inv, sextant, width, height, moreWidth, moreHeight;
	uint32_t *pixels;
	FILE* output;

	inv = 0;
	output = stdout;

	while ((opt = getopt(argc, argv, shortopts)) != -1) {
		switch (opt) {
		case 'i':
			inv = 1;
			break;
		case 'o':
			output = fopen(optarg, "w");
			
			if (output == NULL)
				die(5, "cannot open output file: %s", strerror(errno));
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

	image = imlib_load_image_with_error_return(argv[optind], &error_return);

	if (image == NULL)
		die(4, "failed to open file: %s (imlib error %d)", strerror(errno),
		    error_return);

	imlib_context_set_image(image);

	width = imlib_image_get_width();
	moreWidth = width % 2;

	height = imlib_image_get_height();
	moreHeight = height % 3;

	pixels = imlib_image_get_data_for_reading_only();

	for (y = 0; y < (height - moreHeight); y += 3) {
		for (x = 0; x < (width - moreWidth); x += 2) {
			sextant = 0;

			if (is_white(pix(x,y)))
				sextant |= 0x1;
			if (is_white(pix(x+1,y)))
				sextant |= 0x2;
			if (is_white(pix(x,y+1)))
				sextant |= 0x4;
			if (is_white(pix(x+1,y+1)))
				sextant |= 0x8;
			if (is_white(pix(x,y+2)))
				sextant |= 0x10;
			if (is_white(pix(x+1,y+2)))
				sextant |= 0x20;

			fprintf(output, sextants[sextant]);
		}

		if (moreWidth > 0) {
			sextant = 0;

			if (is_white(pix(x,y)))
				sextant |= 0x1;
			if (is_white(pix(x,y+1)))
				sextant |= 0x4;
			if (is_white(pix(x,y+2)))
				sextant |= 0x10;

			fprintf(output, sextants[sextant]);
		}

		fprintf(output, "\n");
	}

	if (moreHeight > 0) {
		for (x = 0; x < (width - moreWidth); x += 2) {
			sextant = 0;

			if (is_white(pix(x,y)))
				sextant |= 0x1;
			if (is_white(pix(x+1,y)))
				sextant |= 0x2;
			if (moreHeight > 1) {
				if (is_white(pix(x,y+1)))
					sextant |= 0x4;
				if (is_white(pix(x+1,y+1)))
					sextant |= 0x8;
			}

			fprintf(output, sextants[sextant]);
		}

		if (moreWidth > 0) {
			sextant = 0;

			if (is_white(pix(x,y)))
				sextant |= 0x1;
			if (moreHeight > 1)
				if (is_white(pix(x,y+1)))
					sextant |= 0x4;

			fprintf(output, sextants[sextant]);
		}
	}

	imlib_free_image();

	return 0;
}

static void
die(int erno, char* message, ...)
{
	va_list ap;

	va_start(ap, message);
	fprintf(stderr, "img2sext: ");
	vfprintf(stderr, message, ap);
	fprintf(stderr, "\n");
	va_end(ap);

	exit(erno);
}

static void
usage(void)
{
	puts("Usage: img2sext [-hiv] [-o FILE] IMAGEFILE\n"
	     "\n"
	     "  -i    treat white pixels as black and black pixels as white\n"
	     "  -o    send output to file instead of stdout\n"
	     "  -h    print help and exit\n"
	     "  -v    print version and exit\n"
	     "\n"
	     "Exit Codes:\n"
	     "\n"
	     "  1    unknown argument\n"
	     "  2    no output file path given\n"
	     "  3    no image filename given\n"
	     "  4    failed to open file\n"
	     "  5    cannot open output file\n"
	     "\n"
	     "Behavior is undefined for pixels that are not either white or black");
}

static void
version(void)
{
	puts("img2sext v"VERSION"\n"
	     "written by Drew Marino (OneTrueC)");
}
