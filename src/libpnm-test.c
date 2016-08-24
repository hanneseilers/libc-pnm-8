/*
 * libpnm-test.c
 *
 *  Created on: 16.08.2016
 *      Author: eilers
 */

#include "libpnm.h"

#include <stdio.h>
#include <stdlib.h>

#define SIZE	16

// File pointer used by c file functions
FILE *file;

// Functions
void readImageData();

// Definitions of custom file functions (see below main)
uint8_t _fopen(uint8_t name[], uint8_t rw);
uint8_t _fclose(void);
void _fwrite(uint8_t c);
uint8_t _fread(void);
void _fseek(uint32_t position);


/* Main function */
int main(void){

	printf(">> libc-pnm-8 test <<\n");

	// init library
	pnm_init( _fopen, _fclose, _fread, _fwrite, _fseek );
	printf("> initiated library\n");

	pnm_set_file( (uint8_t*) "test.pgm" );
	printf("> filename set to %s\n", pnm_file_data.fname);

	// create empty white image
	pnm_create( SIZE, SIZE, PNM_GRAY );
	printf("> created image\n");

	// read image data
	readImageData();

	// generate image buffer
	uint8_t buffer[SIZE][SIZE];
	uint8_t i, j, x=0;
	for( i=0; i<SIZE; i++ ){
		for( j=0; j<SIZE; j++ ){
			buffer[i][j] = x++;
		}
	}
	pnm_write( *buffer, 0, 0, SIZE, SIZE );
	printf("> changed image:\n");

	// read image data
	readImageData();

	return 0;

}


/* Read image data */
void readImageData(){
	uint8_t buffer[SIZE][SIZE];
	if( pnm_get( 0, 0, SIZE, SIZE, *buffer ) ){

		printf("> Image data:\n");
		uint8_t i, j;
		for( i=0; i<SIZE; i++ ){
			for( j=0;j<SIZE; j++ ){
				printf( "0x%02x ", buffer[i][j] );
			}
			printf("\n");
		}

	} else {
		printf("> Cannot read image data!\n");
	}
}




// Defining custom functions for mapping generic
// library interface to c file functions

uint8_t _fopen(uint8_t name[], uint8_t rw){
	file = NULL;
	if( rw == 'r' )
		file = fopen( (char*) name, "r" );
	else if( rw == 'w' )
		file = fopen( (char*) name, "w" );
	else if( rw == 'a' )
		file = fopen( (char*) name, "r+" );

	return NULL != file;
}

uint8_t _fclose(void){
	return fclose(file);
}

void _fwrite(uint8_t c){
	putc(c, file);
}

uint8_t _fread(void){
	return (uint8_t) getc(file);
}

void _fseek(uint32_t offset){
	fseek(file, offset, SEEK_SET);
}
