#ifndef NINETAILSX_DIBITMAP_H
#define NINETAILSX_DIBITMAP_H

/**
 * NOTE:
 * 			We need to tightly pack the bitmap header in order to ensure that everything aligns
 * 			properly when we cast data we read from a file.
 */
#pragma pack(push)
#pragma pack(1)

/**
 * The bitmap file header which all bitmap files contain.
 */
typedef struct bitmap_file_header
{
	u16 signature;
	u32 fileSize;
	u32 _reserved;
	u32 dataOffset;
} bitmap_file_header;

typedef struct bitmap_color_space_endpoints
{
	u8 colorSpaceEndpoints[36]; // 36 bytes matches spec.
} bitmap_color_space_endpoints;

/**
 * Bitmap Info Header Version 5 info header struct specification.
 */
typedef struct bitmap_info_header_v5
{
	u32 size;
	u32 width;
	u32 height;
	u16 planes;
	u16 bpp;
	u32 compression;
	u32 imageSize;
	u32 xppm;
	u32 yppm;
	u32 colorsUsed;
	u32 importantColors;
	u32 bitmask_red;
	u32 bitmask_green;
	u32 bitmask_blue;
	u32 bitmask_alpha;
	bitmap_color_space_endpoints colorSpaceEndpoints;
	u32 gamma_red;
	u32 gamma_green;
	u32 gamma_blue;
	u32 intent;
	u32 icc_profileData;
	u32 icc_profileSize;
	u32 _reserved;
} bitmap_info;

/**
 * The bitmap section describes a bitmap file.
 */
typedef struct bitmap_header
{
	bitmap_file_header fileHeader;
	bitmap_info_header_v5 infoHeader;
} bitmap_header;

#pragma pack(pop)


/**
 * Describes a device-independent bitmap.
 */
typedef struct dibitmap
{
	bitmap_header* header;
	void* buffer;
	v2i dims; // Quick access to the dimensions of a bitmap.
} dibitmap;

/**
 * Returns a dibitmap struct containing the details of bitmap. The resource parameter
 * is the raw data pulled from the file.
 */
inline dibitmap
GetBitmapFromResource(void* resource)
{
	dibitmap _bitmap = {};
	_bitmap.header = (bitmap_header*)resource;
	_bitmap.buffer = (u8*)resource + _bitmap.header->fileHeader.dataOffset;
	_bitmap.dims = {(i32)_bitmap.header->infoHeader.width, (i32)_bitmap.header->infoHeader.height};
	return _bitmap;
}

#endif