#ifndef NINETAILSX_HELPERS_H
#define NINETAILSX_HELPERS_H

#include <assert.h>
#include <nxcore/primitives.h>

#define Kilobytes(n) (1024*n)
#define Megabytes(n) (1024*Kilobytes(n))
#define Gigabytes(n) (1024*Megabytes((u64)n))
#define Terabytes(n) (1024*Gigabytes((u64)n))

#define internal static
#define global static
#define persist static

#define ArraySize(arr) (sizeof(arr)/sizeof(arr[0]))


#endif