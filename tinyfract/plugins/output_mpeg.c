#include <ffmpeg/avcodec.h>
#include <ffmpeg/avformat.h>

#include "../plugin.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_COLORS 65536
#define COLOR_CEILING 65536 


typedef struct
{
	float y;
	float cb;
	float cr;
} yuv_t;

typedef struct
{
	AVCodecContext* movie_context;
	AVFrame*        picture;
	FILE*           output_file;
	yuv_t*          colors;
	uint8_t*        outbuf;
} mpeg_t;	


/* Color mapping functions. */
float Rh(float H)
{
	if(H<1/3) return 1-(3*H);	
	else if (H>2/3) return 3*H-2;
	else return 0;
}

float Gh(float H)
{
	if(H<1/3) return (3*H);	
	else if (H>2/3) return 0;
	else return 2-(3*H);
}

float Bh(float H)
{
	if(H<1/3) return 0;	
	else if (H>2/3) return 3-(3*H);
	else return (3*H)-1;
}


float R(float H,float S,float Br)
{
	return Br+Rh(H)*S*(1-Br);
}

float G(float H,float S,float Br)
{
	return Br+Gh(H)*S*(1-Br);
}

float B(float H,float S,float Br)
{
	return Br+Bh(H)*S*(1-Br);
}

/* Constructor and destructor for mpeg output. */
static mpeg_t* constructor_mpeg(const view_dimension_t dimension, char args[])
{
	mpeg_t*  context;
	AVCodec* codec;
	int      i;
	int      outbuf_size;
	int      size;
	float    H;
	float    S;
	float    Br;
	char     des[3];
	int      mod[3];
	float    thres[3];
	uint8_t* picture_buf;
	int      iteration_steps;
	char*    output_args;
	char*    filename;
	

	/* Get memory for the output context. */
	if (!(context=malloc(sizeof(mpeg_t)))) return NULL;

	/* Parse the output args and the safing file name. */
	output_args=strtok(args,"-");
	filename=strtok(NULL,"-");
	#ifdef DEBUG
	fprintf(stderr,"Output args: %s\n",output_args);
	fprintf(stderr,"Safing file: %s\n",filename);
	#endif

	
	/* Check if parameters were given. */
	if(output_args==NULL)
	{
		fprintf(stderr,"Please insert output parameters\n");
		exit(EXIT_FAILURE);
	}

	/* Check safing file was given. */
	if(filename==NULL)
	{
		fprintf(stderr,"Please insert a safing file for the image.\n");
		exit(EXIT_FAILURE);
	}

	/* Scan the argument string. */
	sscanf(output_args,"%c%d,%f%c%d,%f%c%d,%f",
		&des[0],&mod[0],&thres[0],
		&des[1],&mod[1],&thres[1],
		&des[2],&mod[2],&thres[2]);
	
	
	/* Just for help now. */
	iteration_steps=mod[0]*mod[1]*mod[2];


	/* Alloc memory for colors. */
	context->colors=malloc(sizeof(yuv_t)*iteration_steps);

	/* Allocate colors */
	for(i=0;i<iteration_steps;i++)
	{
		int x=i;
		int d;
		
		for (d=0;d<3;d++)
		{
			if (thres[d]<0 || thres[d]>=1)
			{
				fprintf(stderr,"Illegal output format %s.\n",output_args);
				exit(EXIT_FAILURE);
			}
			switch (des[d])
			{
				case 'h':
				case 'H':
					H=(float)(x%mod[d])/mod[d]+thres[d];
					if (H>1) H=H-1;
					x=x/mod[d];
					break;
				case 's':
				case 'S':
					S=(float)(x%mod[d])/mod[d]+thres[d];
					if (S>1) S=S-1;
					x=x/mod[d];
					break;
				case 'b':
				case 'B':
					Br=(float)(x%mod[d])/mod[d]+thres[d];
					if (Br>1) Br=Br-1;
					x=x/mod[d];
					break;
					
				default: 
					fprintf(stderr,"Illegal output format %s.\n",args);
					exit(EXIT_FAILURE);
			}	
		}

		/* Now we write the color information, the RGB Value is translatet into YCbCr. */
		context->colors[i].y=0.299*(R(H,S,Br)/255)+0.587*(G(H,S,Br)/255)+0.114*(B(H,S,Br)/255);
		context->colors[i].cb=(((B(H,S,Br)/255)-context->colors[i].y)/1.772)+0.5;
		context->colors[i].cr=(((R(H,S,Br)/255)-context->colors[i].y)/1.402)+0.5;
	}

	/* Find the mpeg1 video encoder. */
	codec=avcodec_find_encoder(CODEC_ID_MPEG1VIDEO);

	/* Get the context. */
	context->movie_context=avcodec_alloc_context();

	/* Get memory for the picture. */
	context->picture=avcodec_alloc_frame();

	/* put sample parameters */
	context->movie_context->bit_rate = 400000;
	/* resolution must be a multiple of two */
	context->movie_context->width = 352;  
	context->movie_context->height = 288;
	/* frames per second */
//	context->movie_context->frame_rate = 25;  
//	context->movie_context->frame_rate_base= 1;
	/* emit one intra frame every ten frames */
	context->movie_context->gop_size = 10;
	context->movie_context->max_b_frames=1;

	/* open it */
	if (avcodec_open(context->movie_context,codec) < 0)
	{
		fprintf(stderr, "could not open codec\n");
		exit(1);
	}

	/* the codec gives us the frame size, in samples */

	context->output_file=fopen(filename, "wb");
	if (context->output_file==NULL)
	{
		fprintf(stderr, "could not open %s\n", filename);
		exit(1);
	}
    
	/* alloc image and output buffer */
	outbuf_size=100000;
	handle>outbuf=malloc(outbuf_size);
	size=context->movie_context->width*context->movie_context->height;
	picture_buf=malloc((size * 3) / 2); /* size for YUV 420 */

	context->picture->data[0]=picture_buf;
	context->picture->data[1]=context->picture->data[0]+size;
	context->picture->data[2]=context->picture->data[1] + size / 4;
	context->picture->linesize[0]=context->movie_context->width;
	context->picture->linesize[1]=context->movie_context->width / 2;
	context->picture->linesize[2]=context->movie_context->width / 2;

	/* Return the handle. */
	return context;
}

void destructor_mpeg(mpeg_t* handle)
{
	/* Free the handle and all other space */
	free(handle);
}

/* Blit rectangle from pixelbuffer to X11 viewport. */
void blit_rect_mpeg(mpeg_t* handle, const view_position_t position, const view_dimension_t dimension, pixel_value values[])
{
}


/* Fill rectangle in image with color. */
void fill_rect_mpeg(mpeg_t* handle, const view_position_t position, const view_dimension_t dimension, const pixel_value value)
{
	int x;
	int y;

	/* Put a filled rectangle in the image. */
	/* Dupble for loop */
	for(x=position.x;x<=dimension.width+position.x;x++)
	{
		for(y=position.y;y<=dimension.height+position.y;y++)
		{
			handle->picture->data[0][y*handle->picture->linesize[0]+x]=handle->colors[value].y;
			handle->picture->data[1][y*handle->picture->linesize[1]+x]=handle->colors[value].cb;
			handle->picture->data[2][y*handle->picture->linesize[2]+x]=handle->colors[value].cr;
		}
	}
}

/* Put pixel into the image viewport. */
void put_pixel_mpeg(mpeg_t* handle, const view_position_t position, const pixel_value value)
{
	/* Put a pixel into the image. */
	handle->picture->data[0][position.y*handle->picture->linesize[0]+position.x]=handle->colors[value].y;
	handle->picture->data[1][position.y*handle->picture->linesize[1]+position.x]=handle->colors[value].cb;
	handle->picture->data[2][position.y*handle->picture->linesize[2]+position.x]=handle->colors[value].cr;
	
}

/* Safe the image */
void flush_viewport_mpeg(mpeg_t* handle, button_event_t* position)
{
	/* Add the image to the video stream. */
	handle->outbuf[0] = 0x00;
	handle->outbuf[1] = 0x00;
	handle->outbuf[2] = 0x01;
	handle->outbuf[3] = 0xb7;

	fwrite(outbuf, 1, 4, f);	
	
	/* Tell the main function that the programm is over. */
	position->type=autozoom_quit;

	return;
}


/* Enumerate plugin facilities. */
volatile const plugin_facility_t tinyfract_plugin_facilities[]=
{
	{
		name: "mpeg",
		type: plugin_facility_output,
		facility:
		{
			output:
			{
				constructor:             (const plugin_output_constructor_t*) &constructor_mpeg,
				destructor:              (const plugin_output_destructor_t*) &destructor_mpeg,
				blit_rect_function:      (const plugin_output_blit_rect_function_t*) &blit_rect_mpeg,
				fill_rect_function:      (const plugin_output_fill_rect_function_t*) &fill_rect_mpeg,
				flush_viewport_function: (const plugin_output_flush_viewport_function_t*) &flush_viewport_mpeg,
				put_pixel_function:      (const plugin_output_put_pixel_function_t*) &put_pixel_mpeg,
			}
		}
	},
	{ plugin_facility_end }
};

