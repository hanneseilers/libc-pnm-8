/*
 ============================================================================
 Name        : libpnm.c
 Author      : H. Eilers
 Version     : 0.0.1a
 Description : 8-bit pnm library to read and write pnm images.
 ============================================================================
 */

#include "libpnm.h"
#include <stdio.h>

/* GLOBAL VARIABLES */

uint8_t pnm_init_done = false;
pnm_file pnm_file_data;

// function pointers
uint8_t	(*pnm_fread)(void);					// reading one byte from file
void	(*pnm_fwrite)(uint8_t);				// writing one byte to file
uint8_t	(*pnm_fopen)(uint8_t[], uint8_t);	// open file
uint8_t	(*pnm_fclose)(void);				// close file
void	(*pnm_fseek)(uint32_t);				// file seek offset


/* FUNCTIONS */


/**
 * Initiates pnm functionality
 * pnm_init( fopen, fclose, fread, fwrite, fseek )
 * expects function pointers for following functions:
 *
 * uint8_t fopen(uint8_t filename, uint8_t rw_flag)		open file with filename and rw flag ('r' = read, 'w'=write, 'a'=append), returns true if successful
 * uint8_t fclose(void)									close opened file, returns true if successful
 * uint8_t fread(void)									read one byte from file
 * void fwrite(uint8_t data)							write one byte to file
 * void fseek(uint32_t offset)							seeks to offset position inside file
 *
 */
void pnm_init( uint8_t (*fopen)(uint8_t[], uint8_t), uint8_t (*fclose)(void), uint8_t (*fread)(void), void (*fwrite)(uint8_t), void (*fseek)(uint32_t)  ){
	pnm_fopen = fopen;
	pnm_fclose = fclose;
	pnm_fread = fread;
	pnm_fwrite = fwrite;
	pnm_fseek = fseek;

	pnm_init_done = true;
}

/**
 * Sets name of file to use.
 */
void pnm_set_file(uint8_t filename[]){
	pnm_file_data.fname = filename;
}

/**
 * Sets file size.
 * Necessary if pnm_create() wasn't called before reading file.
 */
void pnm_set_file_size(uint8_t width, uint8_t height){
	pnm_file_data.width = width;
	pnm_file_data.height = height;
}

/**
 * Checks if ready
 * Returns true if library initiated and filename set
 */
uint8_t _pnm_cir(void){
	return pnm_init_done && pnm_file_data.fname != NULL;
}



/**
 * Creates a new empty pnm file. If the file already exists, it will be deleted and recreated
 * width:	image width in px
 * height:	image height in px
 * color:	set pnm colorspace, curently only PNM_GRAY is available
 */
uint8_t pnm_create(uint16_t width, uint16_t height, uint8_t colorspace){
	if( _pnm_cir() ){

		// set file size
		pnm_set_file_size(width, height);

		// CREATE NEW FILE
		switch (colorspace) {
			case PNM_GRAY:
				if( pnm_fopen( pnm_file_data.fname, 'w' ) ){
					_pnm_writes((uint8_t*) "P5" );
					pnm_fwrite('\n' );
					_pnm_write_itoa( width );
					pnm_fwrite('\n');
					_pnm_write_itoa(height);
					pnm_fwrite('\n');
					_pnm_writes( (uint8_t*) "255" );
					pnm_fwrite('\n');

					// set all pixels white
					for( uint16_t y=0; y<height; y++ ){
						for( uint16_t x=0; x<width; x++ ){
							pnm_fwrite( PNM_BUFFER_FILLER );
						}
					}

					pnm_fclose();
					return true;
				}
				break;
		}

	}

	return false;
}

/**
 * Gets image values of one section of the image.
 * If section is greater than image borders, all remaining entries of the section will be set as 0xff.
 * width:	Width of the section to get
 * height:	Height of the section to get
 * x:		Left position of section
 * y:		Top position of section
 * buffer:	2 dimensional array in size of [height][width]
 */
uint8_t pnm_get(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t *buffer)
{
	if( _pnm_cir() && x < pnm_file_data.width && y < pnm_file_data.height ){

		// open file
		pnm_fopen( pnm_file_data.fname, 'r' );

		uint8_t vx = x;
		uint8_t i = 0, j = 0;

		// iterate through file and fill buffer
		while( y < pnm_file_data.height && j < height  ){

			// seek for next line start
			pnm_fseek( PNM_HEADER_OFFSET + pnm_file_data.width*y + x );

			// get a single line
			while( vx < pnm_file_data.width && i < width ){
				uint8_t c = pnm_fread();
				*((uint8_t*) buffer + j*width + i ) = c;
				i++;
				vx++;
			}

			// check if to fill array
			for( ; i<width; i++ ){
				*((uint8_t*) buffer + j*width + i ) = PNM_BUFFER_FILLER;
			}

			// set variables
			vx = x;
			i = 0;
			y++;
			j++;
		}

		// check if to fill array
		for( ; j<height; j++ ){
			for( i=0; i < width; i++ ){
				*((uint8_t*) buffer + j*width + i ) = PNM_BUFFER_FILLER;
			}
		}

		pnm_fclose();
		return true;

	}
	return false;
}

/**
 * Writes a 2 dimension array [y][x] to the image
 * If buffer is larger than image borders, the remaining buffer
 * will be not be added to the image.
 * buffer:	Pointer to 2 dimensional array
 * width:	Width of buffer
 * height:	Height of buffer
 * x:		Left position where to insert the buffer
 * y:		Top position where to insert the buffer
 */
void pnm_write(uint8_t *buffer, uint16_t x, uint16_t y, uint16_t width, uint16_t height){

	if( _pnm_cir() && x < pnm_file_data.width && y < pnm_file_data.height ){

		// open file
		pnm_fopen( pnm_file_data.fname, 'a' );

		uint8_t vx = x;
		uint8_t i = 0, j = 0;

		// iterate through file and write buffer
		while( y < pnm_file_data.height && j < height  ){

			// seek for next line start
			pnm_fseek( PNM_HEADER_OFFSET + pnm_file_data.width*y + x );

			// write a single line
			while( vx < pnm_file_data.width && i < width ){
				pnm_fwrite( *((uint8_t*) buffer + j*width + i ) );
				i++;
				vx++;
			}

			// set variables
			vx = x;
			i = 0;
			y++;
			j++;
		}

		pnm_fclose();

	}

}



/* ----- INTERNAL FUNCTION ----- */

/**
 * Writes a integer number as ascii string
 */
void _pnm_write_itoa(uint16_t number){
	uint8_t s[6];
	_itoa(number, s);

	uint8_t i;
	for( i=0; s[i]!='\0' && i<6; i++ ){
		pnm_fwrite( s[i] );
	}
}

/**
 * Writes a string
 */
void _pnm_writes(uint8_t *data){
	while( *data ){
		pnm_fwrite( *data );
		data++;
	}
}

/* itoa:  convert n to characters in s */
void _itoa(uint16_t n, uint8_t s[]){
	int i = 0;
	do {       /* generate digits in reverse order */
		s[i++] = n % 10 + '0';   /* get next digit */
	} while ((n /= 10) > 0);     /* delete it */
	s[i] = '\0';
	_reverse(s);
}

/* reverse:  reverse string s in place */
void _reverse(uint8_t s[]){
	uint8_t i, j;
	uint8_t c;

	for (i = 0, j = strlen((char*)s)-1; i<j; i++, j--) {
		c = s[i];
		s[i] = s[j];
		s[j] = c;
	}
}

