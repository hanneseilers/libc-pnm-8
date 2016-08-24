/*
 * libpnm.h
 *
 *  Created on: 16.08.2016
 *      Author: eilers
 */

#ifndef LIBPNM_H_
#define LIBPNM_H_

#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

#ifndef false
#define false	0
#endif

#ifndef true
#define true	1
#endif

/* DEFINES */
#define PNM_GRAY			0x00
#define PNM_HEADER_OFFSET	13
#define PNM_BUFFER_FILLER	0xff

/* FUNCTIONS */
void pnm_init(uint8_t (*fopen)(uint8_t[], uint8_t), uint8_t (*fclose)(void), uint8_t (*fread)(void), void (*fwrite)(uint8_t), void (*fseek)(uint32_t));
void pnm_set_file(uint8_t filename[]);
void pnm_set_file_size(uint8_t width, uint8_t height);
uint8_t pnm_create(uint16_t width, uint16_t height, uint8_t colorspace);
uint8_t pnm_get(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t *buffer);
void pnm_write(uint8_t *buffer, uint16_t x, uint16_t y, uint16_t width, uint16_t height);

/* INTERNAL FUNCTIONS */
uint8_t _pnm_cir(void);
void _pnm_write_itoa(uint16_t number);
void _pnm_writes(uint8_t *data);

void _itoa(uint16_t n, uint8_t s[]);
void _reverse(uint8_t s[]);

/* TYPEDEFS */
typedef struct{
	uint8_t *fname;
	uint8_t colorspace;
	uint16_t width;
	uint16_t height;
} pnm_file;

/* GLOBAL VARIABLES */
extern uint8_t pnm_init_done;			// flag if init function was called
extern pnm_file pnm_file_data;			// file data

// function pointers
extern uint8_t	(*pnm_fread)(void);					// reading one byte from file
extern void		(*pnm_fwrite)(uint8_t);				// writing one byte to file
extern uint8_t	(*pnm_fopen)(uint8_t[], uint8_t);	// open file
extern uint8_t	(*pnm_fclose)(void);				// close file
extern void		(*pnm_fseek)(uint32_t);				// file seek offset



#endif /* LIBPNM_H_ */
