#ifndef COMMON_H
#define COMMON_H

/*
 * Some useful macros.
 */
#define VARCOPY(dest,src) memcpy(&dest,&src,sizeof(dest));
#define VARZERO(dest)     memset(&dest,0,sizeof(dest));
#define Re(z) (z).real_part
#define Im(z) (z).imaginary_part


/*
 * Some useful typedefs. Useful, if we ever have to change it, maybe when
 * we want to use an arbitrary precision calculating library later.
 */
typedef unsigned int ordinal_number_t;
typedef double       real_number_t;

typedef struct
{
	real_number_t real_part;
	real_number_t imaginary_part;
} complex_number_t;



typedef struct
{
	unsigned int width;
	unsigned int height;
	unsigned int fill;
} view_dimension_t;

typedef struct
{
	int x;
	int y;
} view_position_t;

typedef unsigned int pixel_value;

/*typedef struct
{
	unsigned int hue;
	unsigned int saturation;
	unsigned int brightness;
} pixel_properties;
*/

#endif

