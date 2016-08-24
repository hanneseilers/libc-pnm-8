# libc-pnm-8
libpnm-avr is a C-library for 8 bit systems to read, write and manipulate protable anymap images (file-endings: .pbm, .pgm, .ppm). It is designed to work smoothly with any kind of hardware to store the image data by using a general hardware access interface. It was mainly designed for 8 bit AVR microcontrollers.

# Installation
Include the following files into your workspace and compile them together with your project.

* libpnm.h
* libpnm.c

# Usage
Include the libpnm.h into your code:

    #include "libpnm.h"
    
You need to init the function and tell her the function to use for opening, closing, reading, writing and seeking through files:

    pnm_init( _fopen, _fclose, _fread, _fwrite, _fseek );

Maybe you need to write your own wrapper functions to make them fit to libpnm function headers. See libpnm.h for function details or take a closer look at libpnm-test.c for code examples.

Next you need to set the name of the file you want to read/write. It's currently only possible to work with one file at once.

    pnm_set_file( (uint8_t*) "test.pgm" );

Where "test.pgm" should be replaced with your file name.

If the image isn't existing, you need to create it first, so it has the correct header and size. You cannot change the file size once it is set.

    pnm_create( width, height, PNM_GRAY );

 This initiates the file with a white background. The last argument is the file color space. Currently only gray is implemented.
 If you want to use an existing file, you need the set the file size:
 
    pnm_set_file_size( width, height );
    
 To read the image data you need to define an area to read (depending on you available system memory) and create a 2 dimensional buffer where libpnm read the image data to:
 
     pnm_get( x, y, width, height, *buffer );

Where x and y defining the left-top-position where to start reading the image. If your buffer size (width/height) exceeds the image boundaries, the rest will be filled with white color.

You can also write data from an buffer to the image:

    pnm_write( *buffer, x, y, width, height );