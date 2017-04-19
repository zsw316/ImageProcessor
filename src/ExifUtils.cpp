#include <stdio.h>
#include "ExifUtils.h"
#include <assert.h>

/* Remove spaces on the right of the string */
void trim_spaces(char *buf)
{
    char *s = buf-1;
    for (; *buf; ++buf) {
        if (*buf != ' ')
            s = buf;
    }
    *++s = 0; /* nul terminate the string on the first of the final spaces */
}

/* Show the tag name and contents if the tag exists */
// static void show_tag(ExifData *d, ExifIfd ifd, ExifTag tag)
// {
//     /* See if this tag exists */
//     ExifEntry *entry = exif_content_get_entry(d->ifd[ifd],tag);
//     if (entry) {
//         char buf[1024];
// 
//         /* Get the contents of the tag in human-readable form */
//         exif_entry_get_value(entry, buf, sizeof(buf));
// 
//         /* Don't bother printing it if it's entirely blank */
//         trim_spaces(buf);
//         if (*buf) {
//             printf("%s: %s\n", exif_tag_get_name_in_ifd(tag,ifd), buf);
//         }
//     }
// }

std::string get_tag(ExifData *d, ExifIfd ifd, ExifTag tag)
{
	/* See if this tag exists */
	ExifEntry *entry = exif_content_get_entry(d->ifd[ifd],tag);
	if (entry) {
		char buf[1024];

		/* Get the contents of the tag in human-readable form */
		exif_entry_get_value(entry, buf, sizeof(buf));

		trim_spaces(buf);
		return buf;
	}
	return "";
}

ExifEntry *init_tag(ExifData *exif, ExifIfd ifd, ExifTag tag)
{
	ExifEntry *entry;
	/* Return an existing tag if one exists */
	if (!((entry = exif_content_get_entry (exif->ifd[ifd], tag)))) {
	    /* Allocate a new entry */
	    entry = exif_entry_new ();
	    assert(entry != NULL); /* catch an out of memory condition */
	    entry->tag = tag; /* tag must be set before calling
				 exif_content_add_entry */

	    /* Attach the ExifEntry to an IFD */
	    exif_content_add_entry (exif->ifd[ifd], entry);

	    /* Allocate memory for the entry and fill with default data */
	    exif_entry_initialize (entry, tag);

	    /* Ownership of the ExifEntry has now been passed to the IFD.
	     * One must be very careful in accessing a structure after
	     * unref'ing it; in this case, we know "entry" won't be freed
	     * because the reference count was bumped when it was added to
	     * the IFD.
	     */
	    exif_entry_unref(entry);
	}
	return entry;
}

/* Create a brand-new tag with a data field of the given length, in the
 * given IFD. This is needed when exif_entry_initialize() isn't able to create
 * this type of tag itself, or the default data length it creates isn't the
 * correct length.
 */
ExifEntry *create_tag(ExifData *exif, ExifIfd ifd, ExifTag tag, size_t len, ExifFormat format)
{
	void *buf;
	ExifEntry *entry;
	
	/* Create a memory allocator to manage this ExifEntry */
	ExifMem *mem = exif_mem_new_default();
	assert(mem != NULL); /* catch an out of memory condition */

	/* Create a new ExifEntry using our allocator */
	entry = exif_entry_new_mem (mem);
	assert(entry != NULL);

	/* Allocate memory to use for holding the tag data */
	buf = exif_mem_alloc(mem, len);
	assert(buf != NULL);

	/* Fill in the entry */
	entry->data = (unsigned char*)buf;
	entry->size = len;
	entry->tag = tag;
	entry->components = len;
	entry->format = format;

	/* Attach the ExifEntry to an IFD */
	exif_content_add_entry (exif->ifd[ifd], entry);

	/* The ExifMem and ExifEntry are now owned elsewhere */
	exif_mem_unref(mem);
	exif_entry_unref(entry);

	return entry;
}


/*
exif:GPSLatitude: 39/1, 5943/100, 0/1
exif:GPSLatitudeRef: N
exif:GPSLongitude: 116/1, 1992/100, 0/1
exif:GPSLongitudeRef: E
*/
