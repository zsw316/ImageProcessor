#ifndef IMAGE_HELPER_H
#define IMAGE_HELPER_H

#include <cstring>
#include <stdint.h>
#include <iostream>
#include "def.h"
#include "ExifUtils.h"

struct ImageInfo
{
	uint32_t	m_uWidth;
	uint32_t	m_uHeight;
};

struct StandardExif
{
	uint16_t		m_uOrientation;
	std::string		m_strDateTimeOriginal;
	std::string		m_strMake;
	std::string		m_strCamera;
	std::string		m_strLatRef;
	std::string		m_strLongRef;
	ExifRational	m_LatDegrees;
	ExifRational	m_LatMinutes;
	ExifRational	m_LatSeconds;
	ExifRational	m_LongDegrees;
	ExifRational	m_LongMinutes;
	ExifRational	m_LongSeconds;
};

using namespace std;

class CImageHelper
{
public:
	CImageHelper()
	{
	}

	~CImageHelper()
	{
	}

	void LoadImage(const char* srcImage, std::string &strData);

	bool CropImage(const char* srcImage, const char* destImage, uint32_t width, uint32_t height, bool bRemoveExif = false);

	bool ZoomImage(const char* srcImage, const char* destImage, double scale, bool bRemoveExif = false);

	bool ShowExif(const char* srcImage);

	void GetImageInfo(const std::string &strData, ImageInfo &info);

	void RotateCounterClockwise(const char* srcImage, const char* destImage, uint32_t uDegree);

	void AddExif(const StandardExif &exifAdd, std::string &strData);

	void RemoveExif(std::string &strData);

	void Init();

private:
	
};

#endif	//IMAGE_HELPER_H
