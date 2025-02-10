/* C quickstart main file */
#define _POSIX_C_SOURCE 200809L

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include <errno.h>
#include <string.h>

#include <Imlib2.h>

#define pix(x,y) (pixels[((y) * width) + (x)])
#define is_white(x) (((x) & 0xFFFFFF) >= 0x7f7f7f)

/* I am aware there are better ways to do this, but i don't really care. I did
 * this because there is no sextant 135 or 246, and I didn't want to add edge-
 * cases.*/
const char* sextants[] = {" ", "🬀", "🬁", "🬂", "🬃", "🬄", "🬅", "🬆", "🬇", "🬈", "🬉",
                          "🬊", "🬋", "🬌", "🬍", "🬎", "🬏", "🬐", "🬑", "🬒", "🬓", "▌",
                          "🬔", "🬕", "🬖", "🬗", "🬘", "🬙", "🬚", "🬛", "🬜", "🬝", "🬞",
                          "🬟", "🬠", "🬡", "🬢", "🬣", "🬤", "🬥", "🬦", "🬧", "▐", "🬨",
                          "🬩", "🬪", "🬫", "🬬", "🬭", "🬮", "🬯", "🬰", "🬱", "🬲", "🬳",
                          "🬴", "🬵", "🬶", "🬷", "🬸", "🬹", "🬺", "🬻", "█"};

static void die(int erno, char* message, ...);

int
main(int argc, char** argv)
{
	Imlib_Image image;
	Imlib_Load_Error error_return;
	int x, y, sextant, width, height, moreWidth, moreHeight;
	uint32_t *pixels;

	if (argc != 2)
		die(EINVAL, "just give one filename");

	image = imlib_load_image_with_error_return(argv[1], &error_return);

	if (image == NULL)
		die(errno, "failed to open file: %s (imlib error %d)", strerror(errno),
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

			printf(sextants[sextant]);
		}

		if (moreWidth > 0) {
			sextant = 0;

			if (is_white(pix(x,y)))
				sextant |= 0x1;
			if (is_white(pix(x,y+1)))
				sextant |= 0x4;
			if (is_white(pix(x,y+2)))
				sextant |= 0x10;

			printf(sextants[sextant]);
		}

		printf("\n");
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

			printf(sextants[sextant]);
		}

		if (moreWidth > 0) {
			sextant = 0;

			if (is_white(pix(x,y)))
				sextant |= 0x1;
			if (moreHeight > 1)
				if (is_white(pix(x,y+1)))
					sextant |= 0x4;

			printf(sextants[sextant]);
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
	fprintf(stderr, "imgSex: ");
	vfprintf(stderr, message, ap);
	fprintf(stderr, "\n");
	va_end(ap);

	exit(erno);
}
