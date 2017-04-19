#include <string>
#include <stdio.h>
#include "ImageHelper.h"
#include "Magick++.h"

using namespace std;
using namespace Magick;

void CImageHelper::Init()
{
	InitializeMagick(NULL);
}

void CImageHelper::LoadImage(const char* srcImage, std::string &strData)
{
	Image imgSrc;
	imgSrc.read(srcImage);

	Blob blobOut;
	imgSrc.write(&blobOut, "jpg");

	strData.assign((char*)blobOut.data(), blobOut.length());
}

bool CImageHelper::CropImage(const char* srcImage, const char* destImage, uint32_t width, uint32_t height, bool bRemoveExif)
{
	Image imgSrc;
	imgSrc.read(srcImage);

	uint32_t srcWidth = imgSrc.baseColumns();
	uint32_t srcHeight = imgSrc.baseRows();

	//locale the start point(x,y)
	double scaleCrop = (double)width / (double)height;
	double scaleSrc = (double)srcWidth / (double)srcHeight;
	
	uint32_t uX = 0, uY = 0;
	uint32_t uWidth = srcWidth, uHeight = srcHeight;

	if (scaleCrop >= scaleSrc)	//width based
	{
		uHeight = (double)srcWidth / scaleCrop;
		uY = (srcHeight - uHeight) / 2;
	}
	else	//height based
	{
		uWidth = (double)srcHeight * scaleCrop;
		uX = (srcWidth - uWidth) / 2;
	}

	// Crop the image to specified size (width, height, xOffset, yOffset)
	try{	

		imgSrc.crop(Geometry(uWidth, uHeight, uX, uY));
		imgSrc.sample(Geometry(width, height));
		
		if (bRemoveExif)
		{
			StripImage(imgSrc.image());
		}

		imgSrc.write(destImage);
		
		debug_log("crop success, src=%dx%d, dest=%dx%d",
			srcWidth, srcHeight, width, height);
	}catch(...)
	{
		error_log("crop failed, catch exception! src=%dx%d, dest=%dx%d",
			srcWidth, srcHeight, width, height);
		return false;
	}

	return true;
}

bool CImageHelper::ZoomImage(const char* srcImage, const char* destImage, double scale, bool bRemoveExif)
{
	Image imgSrc;
	imgSrc.read(srcImage);

	uint32_t srcWidth = imgSrc.baseColumns();
	uint32_t srcHeight = imgSrc.baseRows();
	
	uint32_t uWidth = srcWidth * scale;
	uint32_t uHeight = srcHeight * scale;

	// Crop the image to specified size (width, height, xOffset, yOffset)
	try{			
				
		imgSrc.sample(Geometry(uWidth, uHeight));
		
		if (bRemoveExif)
		{
			StripImage(imgSrc.image());
		}

		imgSrc.write(destImage);
		debug_log("Zoom success, src=%dx%d, zoom=%dx%d",srcWidth, srcHeight, uWidth, uHeight);
	}catch(...)
	{
		error_log("Zoom failed, catch exception! src=%dx%d, zoom=%dx%d", srcWidth, srcHeight, uWidth, uHeight);
		return false;
	}

	return true;
}

void CImageHelper::RotateCounterClockwise(const char* srcImage, const char* destImage, uint32_t uDegree)
{
	Image imgSrc;
	imgSrc.read(srcImage);
	imgSrc.rotate(uDegree);
	imgSrc.write(destImage);
}

void CImageHelper::RemoveExif(std::string &strData)
{
	Blob blobSrc(strData.c_str(), strData.size());
	Image imgSrc(blobSrc);

	StripImage(imgSrc.image());
	Blob blobOut;
	imgSrc.write(&blobOut, "jpg");
	strData.assign((char*)blobOut.data(), blobOut.length());
}

void CImageHelper::AddExif(const StandardExif &exifAdd, std::string &strData)
{
	ExifData *exif = exif_data_new();

	/* Set the image options */
	exif_data_set_option(exif, EXIF_DATA_OPTION_FOLLOW_SPECIFICATION);
	exif_data_set_data_type(exif, EXIF_DATA_TYPE_COMPRESSED);
	exif_data_set_byte_order(exif, FILE_BYTE_ORDER);

	/* All these tags are created with default values by exif_data_fix() */
	/* Change the data to the correct values for this image. */
	ExifEntry *entry;

	entry = init_tag(exif, EXIF_IFD_0, EXIF_TAG_ORIENTATION);
	exif_set_short(entry->data, FILE_BYTE_ORDER, exifAdd.m_uOrientation);

	if (!exifAdd.m_strDateTimeOriginal.empty())
	{
		entry = init_tag(exif, EXIF_IFD_EXIF, EXIF_TAG_DATE_TIME_ORIGINAL);
		memcpy(entry->data, exifAdd.m_strDateTimeOriginal.c_str(), exifAdd.m_strDateTimeOriginal.size());
	}

	if (!exifAdd.m_strMake.empty())
	{
		entry = create_tag(exif, EXIF_IFD_0, EXIF_TAG_MAKE, exifAdd.m_strMake.size(), EXIF_FORMAT_ASCII);
		memcpy(entry->data, exifAdd.m_strMake.c_str(), exifAdd.m_strMake.size());
	}

	if (!exifAdd.m_strCamera.empty())
	{
		entry = create_tag(exif, EXIF_IFD_0, EXIF_TAG_MODEL, exifAdd.m_strCamera.size(), EXIF_FORMAT_ASCII);
		memcpy(entry->data, exifAdd.m_strCamera.c_str(), exifAdd.m_strCamera.size());
	}

	if (!exifAdd.m_strLatRef.empty())
	{
		entry = create_tag(exif, EXIF_IFD_GPS, (ExifTag)EXIF_TAG_GPS_LATITUDE_REF, exifAdd.m_strLatRef.size(), EXIF_FORMAT_ASCII);
		memcpy(entry->data, exifAdd.m_strLatRef.c_str(), exifAdd.m_strLatRef.size());

		entry = create_tag(exif, EXIF_IFD_GPS, (ExifTag)EXIF_TAG_GPS_LATITUDE, 3 * sizeof(ExifRational), EXIF_FORMAT_RATIONAL);
		exif_set_rational(entry->data, FILE_BYTE_ORDER, exifAdd.m_LatDegrees);
		exif_set_rational(entry->data + 0x08, FILE_BYTE_ORDER, exifAdd.m_LatMinutes);
		exif_set_rational(entry->data + 0x10, FILE_BYTE_ORDER, exifAdd.m_LatSeconds);
	}

	if (!exifAdd.m_strLongRef.empty())
	{
		entry = create_tag(exif, EXIF_IFD_GPS, (ExifTag)EXIF_TAG_GPS_LONGITUDE_REF, exifAdd.m_strLongRef.size(), EXIF_FORMAT_ASCII);
		memcpy(entry->data, exifAdd.m_strLongRef.c_str(), exifAdd.m_strLongRef.size());

		entry = create_tag(exif, EXIF_IFD_GPS, (ExifTag)EXIF_TAG_GPS_LONGITUDE, 3 * sizeof(ExifRational), EXIF_FORMAT_RATIONAL);
		exif_set_rational(entry->data, FILE_BYTE_ORDER, exifAdd.m_LongDegrees);
		exif_set_rational(entry->data + 0x08, FILE_BYTE_ORDER, exifAdd.m_LongMinutes);
		exif_set_rational(entry->data + 0x10, FILE_BYTE_ORDER, exifAdd.m_LongSeconds);
	}	

	/* Get a pointer to the EXIF data block we just created */
	unsigned char *exif_data;
	unsigned int exif_data_len;
	exif_data_save_data(exif, &exif_data, &exif_data_len);

	string strTemp;
	/* Write EXIF header */
	strTemp.assign((const char*)exif_header, exif_header_len);
	/* Write EXIF block length in big-endian order */
	char chTemp[2];
	snprintf(chTemp, sizeof(chTemp), "%c", (exif_data_len+2) >> 8);
	strTemp.append(chTemp, 1);
	snprintf(chTemp, sizeof(chTemp), "%c", (exif_data_len+2) & 0xff);
	strTemp.append(chTemp, 1);
	/* Write EXIF data block */
	strTemp.append((const char*)exif_data, exif_data_len);
	/* Write JPEG image data, skipping the non-EXIF header */
	strTemp.append(strData.c_str() + image_data_offset, strData.size() - image_data_offset);

	/* The allocator we're using for ExifData is the standard one, so use
	 * it directly to free this pointer.
	 */
	free(exif_data);
	exif_data_unref(exif);

	strData = strTemp;
}
	
bool CImageHelper::ShowExif(const char* srcImage)
{
	std::string strData;
	this->LoadImage(srcImage, strData);

	StandardExif oriExifOut;
	bool hasOrient = false;

	ExifData *ed = exif_data_new_from_data((const unsigned char *)strData.c_str(), strData.size());
	if (!ed)
	{
		error_log("not readable or no EXIF info");
		return false;
	}

	//get byte order
	ExifByteOrder order = exif_data_get_byte_order(ed);

	//get orientation
	ExifEntry *entry = exif_content_get_entry(ed->ifd[EXIF_IFD_0],EXIF_TAG_ORIENTATION);
	if (entry)
	{
		oriExifOut.m_uOrientation = exif_get_short(entry->data, order);
		hasOrient = true;
	}
	else
	{
		hasOrient = false;
	}

	oriExifOut.m_strDateTimeOriginal = get_tag(ed, EXIF_IFD_EXIF, EXIF_TAG_DATE_TIME_ORIGINAL);
	if (oriExifOut.m_strDateTimeOriginal.empty())
	{
		oriExifOut.m_strDateTimeOriginal = get_tag(ed, EXIF_IFD_EXIF, EXIF_TAG_DATE_TIME_DIGITIZED);
		if (oriExifOut.m_strDateTimeOriginal.empty())
		{
			oriExifOut.m_strDateTimeOriginal = get_tag(ed, EXIF_IFD_0, EXIF_TAG_DATE_TIME);
		}		
	}

	if (!hasOrient && oriExifOut.m_strDateTimeOriginal.empty())
	{
		error_log("no orientation or date time");
		return false;
	}
	
	oriExifOut.m_strMake = get_tag(ed, EXIF_IFD_0, EXIF_TAG_MAKE);
	oriExifOut.m_strCamera = get_tag(ed, EXIF_IFD_0, EXIF_TAG_MODEL);
	oriExifOut.m_strLatRef = get_tag(ed, EXIF_IFD_GPS, (ExifTag)EXIF_TAG_GPS_LATITUDE_REF);
	oriExifOut.m_strLongRef = get_tag(ed, EXIF_IFD_GPS, (ExifTag)EXIF_TAG_GPS_LONGITUDE_REF);

	entry = exif_content_get_entry(ed->ifd[EXIF_IFD_GPS],(ExifTag)EXIF_TAG_GPS_LATITUDE);
	if (entry)
	{
		oriExifOut.m_LatDegrees = exif_get_rational(entry->data, order);
		oriExifOut.m_LatMinutes = exif_get_rational(entry->data + 0x08, order);
		oriExifOut.m_LatSeconds = exif_get_rational(entry->data + 0x10, order);
	}
	else
	{
		oriExifOut.m_LatDegrees.numerator = 0;
		oriExifOut.m_LatDegrees.denominator = 0;
		oriExifOut.m_LatMinutes.numerator = 0;
		oriExifOut.m_LatMinutes.denominator = 0;
		oriExifOut.m_LatSeconds.numerator = 0;
		oriExifOut.m_LatSeconds.denominator = 0;
	}

	entry = exif_content_get_entry(ed->ifd[EXIF_IFD_GPS],(ExifTag)EXIF_TAG_GPS_LONGITUDE);
	if (entry)
	{
		oriExifOut.m_LongDegrees = exif_get_rational(entry->data, order);
		oriExifOut.m_LongMinutes = exif_get_rational(entry->data + 0x08, order);
		oriExifOut.m_LongSeconds = exif_get_rational(entry->data + 0x10, order);
	}
	else
	{
		oriExifOut.m_LongDegrees.numerator = 0;
		oriExifOut.m_LongDegrees.denominator = 0;
		oriExifOut.m_LongMinutes.numerator = 0;
		oriExifOut.m_LongMinutes.denominator = 0;
		oriExifOut.m_LongSeconds.numerator = 0;
		oriExifOut.m_LongSeconds.denominator = 0;
	}

	exif_data_unref(ed);

	debug_log("======= Exif information ======\n");
	debug_log("Date: %s\n", oriExifOut.m_strDateTimeOriginal.c_str());
	debug_log("Make: %s, Camera: %s\n", oriExifOut.m_strMake.c_str(), oriExifOut.m_strCamera.c_str());
	debug_log("GPS: [%s %dd %dm %.4fs, %s %dd %dm %.4fs]\n", oriExifOut.m_strLatRef.c_str(), oriExifOut.m_LatDegrees.numerator/oriExifOut.m_LatDegrees.denominator,
	oriExifOut.m_LatMinutes.numerator/oriExifOut.m_LatMinutes.denominator, (double)oriExifOut.m_LatSeconds.numerator/(double)oriExifOut.m_LatSeconds.denominator,
	oriExifOut.m_strLongRef.c_str(), oriExifOut.m_LongDegrees.numerator/oriExifOut.m_LongDegrees.denominator,
	oriExifOut.m_LongMinutes.numerator/oriExifOut.m_LongMinutes.denominator, (double)oriExifOut.m_LongSeconds.numerator/(double)oriExifOut.m_LongSeconds.denominator);

	return true;
}
