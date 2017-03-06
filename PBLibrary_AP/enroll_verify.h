#ifndef ENROLL_VERIFY_H
#define ENROLL_VERIFY_H

/* Functions used to interface with the enroll_verify.c example.
 * 
 */

#include <stdint.h>
#include "pb_types.h"
#include "pb_template.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Return number of available image samples, N.
 */ 
int fp_image_num(void);

/**
 * Return 1 if image data is slice data from swipe sendor
 */
int fp_image_is_slice(int n);

/**
 * Return image pixels and attributes to image sample n or 0 if no more
 * samples. N is number 0 and up.
 */ 
const uint8_t* fp_image_get(int n, uint16_t* cols, uint16_t* rows, uint16_t* dpi);

/**
 * Return template data and atrtibutes for pre-enrolled template.
 */
const uint8_t* fp_template_get(uint32_t* size,
                               pb_template_type_t* type,
                               pb_sensor_size_t* sensor_size);

/**
 * Simulate an image capture for sample n. The image attributes form sample n
 * is expected to have been retrieved prior to calling this function using
 * fp_image_get().
 *
 * The buffer should be large enough to contain all pixels of the
 * sample. Returns number of bytes copied or 0 if image sample n does
 * not extist or if buffer is too small.
 *
 * note: except for swipe images the image size should always be same
 * on a real system using one sensor, this may however not be assumed
 * via this API.
 */
uint32_t fp_image_capture(int n, uint8_t* buffer, uint32_t buffer_size);

/*******************************************************************
 *
 * Stuff below kept here just to make main code look cleaner, and
 * is not 
  *
 */

#if !defined(_MSC_VER)
# include <sys/time.h>
#else
# include <winsock2.h>
# define gettimeofday(tv,tz) win_gettimeofday(tv,tz)
int win_gettimeofday(struct timeval * tp, struct timezone * tzp);
#endif
#ifdef MALLSTAT
# include <malloc.h>
#else
# define malloc_trim(pad)
# define malloc_stats()
#endif

#ifdef STM32EVAL
void hw_init(void);
#else
# define hw_init()
#endif

/* PB internal memory test macros */
#ifdef _MEMORY
# include "lib_memory.h"
# define malloc_  LIB_MEM_MALLOC_
# define free_    LIB_MEM_FREE_
# define realloc_ LIB_MEM_REALLOC_
#else
# define malloc_  malloc
# define free_    free
# define realloc_ realloc
# define LIB_MEM_INIT()
# define LIB_MEM_TERMINATE()
# define LIB_MEM_RESET()
# define LIB_MEM_PUSH()
# define LIB_MEM_POP()
# define LIB_MEM_PRINT(title)
# define LIB_MEM_ISVALID(p) 1
#endif
/* For delta time messurement */
#define START_TIME()    gettimeofday (&tv_start, 0);
#define GET_TIME(xxxx)  gettimeofday (&tv_end, 0);                          \
                        timeval_subtract(&tv_diff, &tv_end, &tv_start);     \
                        xxxx = (tv_diff.tv_sec*1000 + tv_diff.tv_usec/1000);


#ifdef __cplusplus
}
#endif

#endif

