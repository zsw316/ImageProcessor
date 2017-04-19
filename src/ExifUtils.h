#pragma once
#include <string>
#include <libexif/exif-data.h>

#define FILE_BYTE_ORDER EXIF_BYTE_ORDER_INTEL
/* start of JPEG image data section */
static const unsigned int image_data_offset = 20;

/* raw EXIF header data */
static const unsigned char exif_header[] = {
	0xff, 0xd8, 0xff, 0xe1
};
/* length of data in exif_header */
static const unsigned int exif_header_len = sizeof(exif_header);

/* Remove spaces on the right of the string */
void trim_spaces(char *buf);

/* Show the tag name and contents if the tag exists */
// static void show_tag(ExifData *d, ExifIfd ifd, ExifTag tag);

std::string get_tag(ExifData *d, ExifIfd ifd, ExifTag tag);

ExifEntry *init_tag(ExifData *exif, ExifIfd ifd, ExifTag tag);

/* Create a brand-new tag with a data field of the given length, in the
 * given IFD. This is needed when exif_entry_initialize() isn't able to create
 * this type of tag itself, or the default data length it creates isn't the
 * correct length.
 */
ExifEntry *create_tag(ExifData *exif, ExifIfd ifd, ExifTag tag, size_t len, ExifFormat format);


/*
exif:GPSLatitude: 39/1, 5943/100, 0/1
exif:GPSLatitudeRef: N
exif:GPSLongitude: 116/1, 1992/100, 0/1
exif:GPSLongitudeRef: E
*/
