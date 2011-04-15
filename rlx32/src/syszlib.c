/****************************************************************************
; *
; *	File		: syszlib.c
; *
; *	Description : ZLIB loader
; *
; *	Copyright © Realtech VR 1995 - 2003 - http://www.v3x.net
; *
; *  This is UNPUBLISHED PROPRIETARY SOURCE CODE of Realtech VR;
; *  the contents of this file may not be disclosed to third parties,
; *  copied or duplicated in any form, in whole or in part, without the
; *  prior written permission of Realtech VR.
; *
; **************************************************************************/
#include <stdio.h>
#include "_rlx32.h"
#include "sysresmx.h"
#include "systools.h"

#define NO_DEFLATE

#include "zlib.h"

extern SYS_FILEIO FIO_gzip;

gzFile lib_gzfopen (SYS_FILEHANDLE fd, const char *mode);
int lib_gzclose (gzFile file);
int	filewad_closeFP(SYS_WAD *resource, SYS_FILEHANDLE fp);

#define DEF_MEM_LEVEL 8
#define OS_CODE  0x00

/* bigger buffer ? */
#define Z_BUFSIZE 16384

/* gzip flag byte */
#define ASCII_FLAG   0x01 /* bit 0 set: file probably ascii text */
#define HEAD_CRC     0x02 /* bit 1 set: header CRC present */
#define EXTRA_FIELD  0x04 /* bit 2 set: extra field present */
#define ORIG_NAME    0x08 /* bit 3 set: original file name present */
#define COMMENT      0x10 /* bit 4 set: file comment present */
#define RESERVED     0xE0 /* bits 5..7: reserved */

typedef struct gz_stream
{
    z_stream stream;
    int      z_err;   /* error code for last stream operation */
    int      z_eof;   /* set if end of input file */
    SYS_FILEHANDLE  file;   /* .gz file */
    Byte     *inbuf;  /* input buffer */
    Byte     *outbuf; /* output buffer */
    uLong    crc;     /* crc32 of uncompressed data */
    char     *msg;    /* error message */
    int      transparent; /* 1 if input file is not a .gz file */
    char     mode;    /* 'w' or 'r' */
    int32_t     startpos; /* start of compressed data in file (header skipped) */
} gz_stream;

#define F_READ	(uInt)FIO_std.fread
#define F_WRITE FIO_std.fwrite
#define F_TELL	FIO_std.ftell
#define F_CLOSE FIO_std.fclose
#define F_SEEK  FIO_std.fseek
#define F_PUTC	FIO_std.fputc
#define F_FFLUSH FIO_std.fflush
#undef  F_OPEN
#define F_OPEN	FIO_std.fopen
#define F_ERROR(x) x
#define F_REWIND(f) FIO_std.fseek(f, 0, SEEK_SET)

#define ALLOC(size) MM_heap.malloc(size)
#define TRYFREE(p) {if (p) MM_heap.free(p); p = 0;}

static voidpf  _zcalloc(
    voidpf opaque,
    unsigned items,
    unsigned size)
{
    if (opaque) items += size - size; /* make compiler happy */
    return (voidpf)ALLOC(items * size);
}

static void  _zcfree (
    voidpf opaque,
    voidpf ptr)
{
    TRYFREE(ptr);
    if (opaque) return; /* make compiler happy */
}

static int gz_magic[2] = {0x1f, 0x8b}; /* gzip magic header */


 /* ===========================================================================
 * Cleanup then free the given gz_stream. Return a zlib error code.
   Try freeing in the reverse order of allocations.
 */
static int destroy (gz_stream *s)
{
    int err = Z_OK;

    if (!s)
		return Z_STREAM_ERROR;

    TRYFREE(s->msg);

    if (s->stream.state != NULL) {
	if (s->mode == 'w') {
#ifdef NO_DEFLATE
	    err = Z_STREAM_ERROR;
#else
	    err = deflateEnd(&(s->stream));
#endif
	} else if (s->mode == 'r') {
	    err = inflateEnd(&(s->stream));
	}
    }

    if (s->file != (SYS_FILEHANDLE )NULL)
	{
		FIO_std.fclose(s->file);
    }

    if (s->z_err < 0)
		err = s->z_err;

    TRYFREE(s->inbuf);
    TRYFREE(s->outbuf);
    TRYFREE(s);
    return err;
}


/* ===========================================================================
     Read a byte from a gz_stream; update next_in and avail_in. Return EOF
   for end of file.
   IN assertion: the stream s has been sucessfully opened for reading.
*/
static int get_byte(
    gz_stream *s)
{
    if (s->z_eof) return EOF;
    if (s->stream.avail_in == 0) {
	s->stream.avail_in = F_READ(s->inbuf, 1, Z_BUFSIZE, s->file);
	if (s->stream.avail_in == 0) {
	    s->z_eof = 1;
	    if (F_ERROR(s->file)) s->z_err = Z_ERRNO;
	    return EOF;
	}
	s->stream.next_in = s->inbuf;
    }
    s->stream.avail_in--;
    return *(s->stream.next_in)++;
}


/* ===========================================================================
      Check the gzip header of a gz_stream opened for reading. Set the stream
    mode to transparent if the gzip magic header is not present; set s->err
    to Z_DATA_ERROR if the magic header is present but the rest of the header
    is incorrect.
    IN assertion: the stream s has already been created sucessfully;
       s->stream.avail_in is zero for the first time, but may be non-zero
       for concatenated .gz files.
*/
static void check_header(gz_stream *s)
{
    int method; /* method byte */
    int flags;  /* flags byte */
    uInt len;
    int c;

    /* Check the gzip magic header */
    for (len = 0; len < 2; len++) {
	c = get_byte(s);
	if (c != gz_magic[len]) {
	    if (len != 0) s->stream.avail_in++, s->stream.next_in--;
	    if (c != EOF) {
		s->stream.avail_in++, s->stream.next_in--;
		s->transparent = 1;
	    }
	    s->z_err = s->stream.avail_in != 0 ? Z_OK : Z_STREAM_END;
	    return;
	}
    }
    method = get_byte(s);
    flags = get_byte(s);
    if (method != Z_DEFLATED || (flags & RESERVED) != 0) {
	s->z_err = Z_DATA_ERROR;
	return;
    }

    /* Discard time, xflags and OS code: */
    for (len = 0; len < 6; len++) (void)get_byte(s);

    if ((flags & EXTRA_FIELD) != 0) { /* skip the extra field */
	len  =  (uInt)get_byte(s);
	len += ((uInt)get_byte(s))<<8;
	/* len is garbage if EOF but the loop below will quit anyway */
	while (len-- != 0 && get_byte(s) != EOF) ;
    }
    if ((flags & ORIG_NAME) != 0) { /* skip the original file name */
	while ((c = get_byte(s)) != 0 && c != EOF) ;
    }
    if ((flags & COMMENT) != 0) {   /* skip the .gz file comment */
	while ((c = get_byte(s)) != 0 && c != EOF) ;
    }
    if ((flags & HEAD_CRC) != 0) {  /* skip the header crc */
	for (len = 0; len < 2; len++) (void)get_byte(s);
    }
    s->z_err = s->z_eof ? Z_DATA_ERROR : Z_OK;
}

/* ===========================================================================
     Opens a gzip (.gz) file for reading or writing. The mode parameter
   is as in fopen ("rb" or "wb"). The file is given either by file descriptor
   or path name (if fd == -1).
     lib_gz_open return NULL if the file could not be opened or if there was
   insufficient memory to allocate the (de)compression state; errno
   can be checked to distinguish the two cases (if errno is zero, the
   zlib error is Z_MEM_ERROR).
*/
gzFile lib_gzfopen (SYS_FILEHANDLE fd, const char *mode)
{
    int err;
    int level = Z_DEFAULT_COMPRESSION; /* compression level */
    int strategy = Z_DEFAULT_STRATEGY; /* compression strategy */
    char *p = (char*)mode;
    gz_stream *s;
    char fmode[80]; /* copy of mode, without the compression level */
    char *m = fmode;

    if (!mode) return Z_NULL;

    s = (gz_stream *)ALLOC(sizeof(gz_stream));
    if (!s) return Z_NULL;

    s->stream.zalloc = (alloc_func)_zcalloc;
    s->stream.zfree = (free_func)_zcfree;
    s->stream.opaque = (voidpf)0;
    s->stream.next_in = s->inbuf = Z_NULL;
    s->stream.next_out = s->outbuf = Z_NULL;
    s->stream.avail_in = s->stream.avail_out = 0;
    s->file = fd;
    s->z_err = Z_OK;
    s->z_eof = 0;
    s->crc = crc32(0L, Z_NULL, 0);
    s->msg = NULL;
    s->transparent = 0;

    s->mode = '\0';
    do {
        if (*p == 'r') s->mode = 'r';
        if (*p == 'w' || *p == 'a') s->mode = 'w';
        if (*p >= '0' && *p <= '9') {
	    level = *p - '0';
	} else if (*p == 'f') {
	  strategy = Z_FILTERED;
	} else if (*p == 'h') {
	  strategy = Z_HUFFMAN_ONLY;
	} else {
	    *m++ = *p; /* copy the mode */
	}
    } while (*p++ && m != fmode + sizeof(fmode));
    if (s->mode == '\0') return destroy(s), (gzFile)Z_NULL;

    if (s->mode == 'w') {
#ifdef NO_DEFLATE
        err = Z_STREAM_ERROR;
#else
        err = deflateInit2(&(s->stream), level,
                           Z_DEFLATED, -MAX_WBITS, DEF_MEM_LEVEL, strategy);
        /* windowBits is passed < 0 to suppress zlib header */

        s->stream.next_out = s->outbuf = (Byte*)ALLOC(Z_BUFSIZE);
#endif
        if (err != Z_OK || s->outbuf == Z_NULL) {
            return destroy(s), (gzFile)Z_NULL;
        }
    } else {
        s->stream.next_in  = s->inbuf = (Byte*)ALLOC(Z_BUFSIZE);

        err = inflateInit2(&(s->stream), -MAX_WBITS);
        /* windowBits is passed < 0 to tell that there is no zlib header.
         * Note that in this case inflate *requires* an extra "dummy" byte
         * after the compressed stream in order to complete decompression and
         * return Z_STREAM_END. Here the gzip CRC32 ensures that 4 bytes are
         * present after the compressed stream.
         */
        if (err != Z_OK || s->inbuf == Z_NULL) {
            return destroy(s), (gzFile)Z_NULL;
        }
    }
    s->stream.avail_out = Z_BUFSIZE;

    if (s->file == (SYS_FILEHANDLE )NULL) {
        return destroy(s), (gzFile)Z_NULL;
    }
    if (s->mode == 'w') {
        /* Write a very simple .gz header:
         */
		char tx[16];
        sprintf(tx, "%c%c%c%c%c%c%c%c%c%c", gz_magic[0], gz_magic[1],
             Z_DEFLATED, 0 /*flags*/, 0,0,0,0 /*time*/, 0 /*xflags*/, OS_CODE);
		F_WRITE(tx, 1, 10, s->file);
	s->startpos = 10L;
	/* We use 10L instead of ftell(s->file) to because ftell causes an
         * fflush on some systems. This version of the library doesn't use
         * startpos anyway in write mode, so this initialization is not
         * necessary.
         */
    } else {
	check_header(s); /* skip the .gz header */
	s->startpos = (F_TELL(s->file) - s->stream.avail_in);
    }

    return (gzFile)s;
}

/* ===========================================================================
 * Update the compression level and strategy
 */
int lib_gzsetparams (
    gzFile file,
    int level,
    int strategy)
{
    gz_stream *s = (gz_stream*)file;

    if (s == NULL || s->mode != 'w') return Z_STREAM_ERROR;

    /* Make room to allow flushing */
    if (s->stream.avail_out == 0) {

	s->stream.next_out = s->outbuf;
	if (F_WRITE(s->outbuf, 1, Z_BUFSIZE, s->file) != Z_BUFSIZE) {
	    s->z_err = Z_ERRNO;
	}
	s->stream.avail_out = Z_BUFSIZE;
    }

    return deflateParams (&(s->stream), level, strategy);
}

/* ===========================================================================
   Reads a int32_t in LSB order from the given gz_stream. Sets z_err in case
   of error.
*/
static uLong getLong (gz_stream *s)
{
    uLong x = (uLong)get_byte(s);
    int c;

    x += ((uLong)get_byte(s))<<8;
    x += ((uLong)get_byte(s))<<16;
    c = get_byte(s);
    if (c == EOF) s->z_err = Z_DATA_ERROR;
    x += ((uLong)c)<<24;
    return x;
}



/* ===========================================================================
     Reads the given number of uncompressed bytes from the compressed file.
   lib_gzread returns the number of bytes actually read (0 for end of file).
*/
int lib_gzread (gzFile file, voidp buf, size_t len)
{
    gz_stream *s = (gz_stream*)file;
    Bytef *start = (Bytef*)buf; /* starting point for crc computation */
    Byte  *next_out; /* == stream.next_out but not forced far (for MSDOS) */

    if (s == NULL || s->mode != 'r') return Z_STREAM_ERROR;

    if (s->z_err == Z_DATA_ERROR || s->z_err == Z_ERRNO) return -1;
    if (s->z_err == Z_STREAM_END) return 0;  /* EOF */

    next_out = (Byte*)buf;
    s->stream.next_out = (Bytef*)buf;
    s->stream.avail_out = (uInt)len;

    while (s->stream.avail_out != 0) {

	if (s->transparent) {
	    /* Copy first the lookahead bytes: */
	    uInt n = s->stream.avail_in;
	    if (n > s->stream.avail_out) n = s->stream.avail_out;
	    if (n > 0) {
		sysMemCpy(s->stream.next_out, s->stream.next_in, n);
		next_out += n;
		s->stream.next_out = next_out;
		s->stream.next_in   += n;
		s->stream.avail_out -= n;
		s->stream.avail_in  -= n;
	    }
	    if (s->stream.avail_out > 0) {
		s->stream.avail_out -= F_READ(next_out, 1, s->stream.avail_out,
					     s->file);
	    }
	    len -= s->stream.avail_out;
	    s->stream.total_in  += (uLong)len;
	    s->stream.total_out += (uLong)len;
            if (len == 0) s->z_eof = 1;
	    return (int)len;
	}
        if (s->stream.avail_in == 0 && !s->z_eof) {

            s->stream.avail_in = F_READ(s->inbuf, 1, Z_BUFSIZE, s->file);
            if (s->stream.avail_in == 0) {
                s->z_eof = 1;
		if (F_ERROR(s->file)) {
		    s->z_err = Z_ERRNO;
		    break;
		}
            }
            s->stream.next_in = s->inbuf;
        }
        s->z_err = inflate(&(s->stream), Z_NO_FLUSH);

	if (s->z_err == Z_STREAM_END) {
	    /* Check CRC and original size */
	    s->crc = crc32(s->crc, start, (uInt)(s->stream.next_out - start));
	    start = s->stream.next_out;

	    if (getLong(s) != s->crc) {
		s->z_err = Z_DATA_ERROR;
	    } else {
	        (void)getLong(s);
                /* The uncompressed length returned by above getlong() may
                 * be different from s->stream.total_out) in case of
		 * concatenated .gz files. Check for such files:
		 */
		check_header(s);
		if (s->z_err == Z_OK) {
		    uLong total_in = s->stream.total_in;
		    uLong total_out = s->stream.total_out;

		    inflateReset(&(s->stream));
		    s->stream.total_in = total_in;
		    s->stream.total_out = total_out;
		    s->crc = crc32(0L, Z_NULL, 0);
		}
	    }
	}
	if (s->z_err != Z_OK || s->z_eof) break;
    }
    s->crc = crc32(s->crc, start, (uInt)(s->stream.next_out - start));

    return (int)(len - s->stream.avail_out);
}

#ifndef NO_DEFLATE
/* ===========================================================================
     Writes the given number of uncompressed bytes into the compressed file.
   gzwrite returns the number of bytes actually written (0 in case of error).
*/
size_t lib_gzwrite (gzFile file, const voidp buf, size_t len)
{
    gz_stream *s = (gz_stream*)file;

    if (s == NULL || s->mode != 'w') return Z_STREAM_ERROR;

    s->stream.next_in = (Bytef*)buf;
    s->stream.avail_in = (uInt)len;

    while (s->stream.avail_in != 0) {

        if (s->stream.avail_out == 0) {

            s->stream.next_out = s->outbuf;
            if (F_WRITE(s->outbuf, 1, Z_BUFSIZE, s->file) != Z_BUFSIZE) {
                s->z_err = Z_ERRNO;
                break;
            }
            s->stream.avail_out = Z_BUFSIZE;
        }
        s->z_err = deflate(&(s->stream), Z_NO_FLUSH);
        if (s->z_err != Z_OK) break;
    }
    s->crc = crc32(s->crc, (const Bytef *)buf, (uInt)len);

    return (int)(len - s->stream.avail_in);
}

/* ===========================================================================
     Flushes all pending output into the compressed file. The parameter
   flush is as in the deflate() function.
*/
static int do_flush (gzFile file, int flush)
{
    uInt len;
    int done = 0;
    gz_stream *s = (gz_stream*)file;

    if (s == NULL || s->mode != 'w') return Z_STREAM_ERROR;

    s->stream.avail_in = 0; /* should be zero already anyway */

    for (;;) {
        len = Z_BUFSIZE - s->stream.avail_out;

        if (len != 0) {
            if ((uInt)F_WRITE(s->outbuf, 1, len, s->file) != len) {
                s->z_err = Z_ERRNO;
                return Z_ERRNO;
            }
            s->stream.next_out = s->outbuf;
            s->stream.avail_out = Z_BUFSIZE;
        }
        if (done) break;
        s->z_err = deflate(&(s->stream), flush);

	/* Ignore the second of two consecutive flushes: */
	if (len == 0 && s->z_err == Z_BUF_ERROR) s->z_err = Z_OK;

        /* deflate has finished flushing only when it hasn't used up
         * all the available space in the output buffer:
         */
        done = (s->stream.avail_out != 0 || s->z_err == Z_STREAM_END);

        if (s->z_err != Z_OK && s->z_err != Z_STREAM_END) break;
    }
    return  s->z_err == Z_STREAM_END ? Z_OK : s->z_err;
}

int lib_gzflush (gzFile file, int flush)
{
    gz_stream *s = (gz_stream*)file;
    int err = do_flush (file, flush);

    if (err) return err;
    F_FFLUSH(s->file);
    // FIO-
    return  s->z_err == Z_STREAM_END ? Z_OK : s->z_err;
}
#endif /* NO_DEFLATE */

/* ===========================================================================
     Rewinds input file.
*/
int lib_gzrewind (gzFile file)
{
    gz_stream *s = (gz_stream*)file;

    if (s == NULL || s->mode != 'r') return -1;

    s->z_err = Z_OK;
    s->z_eof = 0;
    s->stream.avail_in = 0;
    s->stream.next_in = s->inbuf;
    s->crc = crc32(0L, Z_NULL, 0);

    if (s->startpos == 0) { /* not a compressed file */
		F_REWIND(s->file);
		return 0;
    }

    (void) inflateReset(&s->stream);
    return F_SEEK(s->file, s->startpos, SEEK_SET);
}

/* ===========================================================================
      Sets the starting position for the next lib_gzread or gzwrite on the given
   compressed file. The offset represents a number of bytes in the
      gzseek returns the resulting offset location as measured in bytes from
   the beginning of the uncompressed stream, or -1 in case of error.
      SEEK_END is not implemented, returns error.
      In this version of the library, gzseek can be extremely slow.
*/
z_off_t lib_gzseek (
    gzFile file,
    z_off_t offset,
    int whence)
{
    gz_stream *s = (gz_stream*)file;

    if (s == NULL || whence == SEEK_END ||
	s->z_err == Z_ERRNO || s->z_err == Z_DATA_ERROR) {
	return -1L;
    }

    if (s->mode == 'w') {
#ifdef NO_DEFLATE
	return -1L;
#else
	if (whence == SEEK_SET) {
	    offset -= s->stream.total_in;
	}
	if (offset < 0) return -1L;

	/* At this point, offset is the number of zero bytes to write. */
	if (s->inbuf == Z_NULL) {
	    s->inbuf = (Byte*)ALLOC(Z_BUFSIZE); /* for seeking */
	    sysMemZero(s->inbuf, Z_BUFSIZE);
	}
	while (offset > 0)  {
	    uInt size = Z_BUFSIZE;
	    if (offset < Z_BUFSIZE) size = (uInt)offset;

	    size = (uInt)lib_gzwrite(file, s->inbuf, size);
	    if (size == 0) return -1L;

	    offset -= size;
	}
	return (z_off_t)s->stream.total_in;
#endif
    }
    /* Rest of function is for reading only */

    /* compute absolute position */
    if (whence == SEEK_CUR) {
	offset += s->stream.total_out;
    }
    if (offset < 0) return -1L;

    if (s->transparent) {
	/* map to F_SEEK */
	s->stream.avail_in = 0;
	s->stream.next_in = s->inbuf;
        if (F_SEEK(s->file, offset, SEEK_SET) < 0) return -1L;

	s->stream.total_in = s->stream.total_out = (uLong)offset;
	return offset;
    }

    /* For a negative seek, rewind and use positive seek */
    if ((uLong)offset >= s->stream.total_out) {
	offset -= s->stream.total_out;
    } else if (lib_gzrewind(file) < 0) {
	return -1L;
    }
    /* offset is now the number of bytes to skip. */

    if (offset != 0 && s->outbuf == Z_NULL) {
	s->outbuf = (Byte*)ALLOC(Z_BUFSIZE);
    }
    while (offset > 0)  {
	int size = Z_BUFSIZE;
	if (offset < Z_BUFSIZE) size = (int)offset;

	size = lib_gzread(file, s->outbuf, (uInt)size);
	if (size <= 0) return -1L;
	offset -= size;
    }
    return (z_off_t)s->stream.total_out;
}

/* ===========================================================================
     Returns the starting position for the next lib_gzread or gzwrite on the
   given compressed file. This position represents a number of bytes in the
   uncompressed data stream.
*/
z_off_t lib_gztell (
    gzFile file)
{
    return lib_gzseek(file, 0L, SEEK_CUR);
}

/* ===========================================================================
     Returns 1 when EOF has previously been detected reading the given
   input stream, otherwise zero.
*/
int lib_gzeof(gzFile file)
{
    gz_stream *s = (gz_stream*)file;

    return (s == NULL || s->mode != 'r') ? 0 : s->z_eof;
}

/* ===========================================================================
   Outputs a int32_t in LSB order to the given file
*/
#ifndef NO_DEFLATE
static void putLong (
    SYS_FILEHANDLE file,
    uLong x)
{
    int n;
    for (n = 0; n < 4; n++) {
        F_PUTC((int)(x & 0xff), file);
        x >>= 8;
    }
}
#endif
/* ===========================================================================
     Flushes all pending output if necessary, closes the compressed file
   and deallocates all the (de)compression state.
*/
int lib_gzclose (gzFile file)
{
#ifndef NO_DEFLATE
    int err;
#endif
    gz_stream *s = (gz_stream*)file;

    if (s == NULL)
		return Z_STREAM_ERROR;

    if (s->mode == 'w') {
#ifdef NO_DEFLATE
	return Z_STREAM_ERROR;
#else
        err = do_flush (file, Z_FINISH);
        if (err != Z_OK)
			return destroy((gz_stream*)file);

        putLong (s->file, s->crc);
        putLong (s->file, s->stream.total_in);
#endif
    }
    return destroy((gz_stream*)file);
}

static SYS_FILEHANDLE CALLING_C fzip_fopen(const char *filename, const char *mode)
{
	SYS_FILEHANDLE file;
	gzFile fp = 0;
	if (strcmp(mode, "wb")==0)
	{
#ifndef NO_DEFLATE
        file = FIO_std.fopen(filename, mode);
		if (!file)
			return (SYS_FILEHANDLE)NULL;
		fp = lib_gzfopen(file, "wb"); // TODO: may lead a problem ...
#endif
	}
	else
	{
        file = FIO_cur->fopen(filename, mode);
		if (!file)
			return (SYS_FILEHANDLE)NULL;
		file = filewad_get(file);
		fp = lib_gzfopen(file, "rb");
	}
	return (SYS_FILEHANDLE)fp;
}

static int CALLING_C fzip_fclose(SYS_FILEHANDLE fp)
{
    SYS_WAD *resource = filewad_getcurrent();
	if ((!resource)||((resource->mode & SYS_WAD_STATUS_ENABLED)==0))
	{
		return lib_gzclose((gzFile *)fp);
	}
	else
	{
		gz_stream *s = (gz_stream*)fp;
		SYS_ASSERT(s);
		filewad_closeFP(resource, s->file);
		s->file = 0;
		lib_gzclose((gzFile *)fp);
	}
	return 1;
}

static int CALLING_C fzip_fseek(SYS_FILEHANDLE file, long offset, int whence)
{
	return lib_gzseek ((gzFile) file, offset, whence);
}

static size_t CALLING_C fzip_fread(void *ptr, size_t size, size_t n, SYS_FILEHANDLE file)
{
	size_t ret = lib_gzread((gzFile )file, ptr, size * n);
	return ret / size;
}

static int CALLING_C fzip_fgetc(SYS_FILEHANDLE file)
{
	int ptr;
	gzFile fp = (gzFile )file;
	lib_gzread(fp, &ptr, 1);
	return ptr;
}

static size_t CALLING_C fzip_fwrite(const void *ptr, size_t size, size_t n, SYS_FILEHANDLE file)
{
#ifndef NO_DEFLATE
	gzFile fp = (gzFile )file;
	size_t ret = lib_gzwrite(fp, (void*)ptr, size*n);
	return ret / size;
#else
	UNUSED(file);
	UNUSED(n);
	UNUSED(size);
	return 0;
#endif // NO_DEFLATE
}

static long CALLING_C fzip_ftell(SYS_FILEHANDLE file)
{
	return lib_gztell((gzFile)file);
}

static int fzip_fflush( SYS_FILEHANDLE handle )
{
	return 0;
}

static int fzip_fputc( int c, SYS_FILEHANDLE handle )
{
	char c8 = (char)c;
	if (!fzip_fwrite( &c8, sizeof(char), 1, handle))
		return EOF;
	else
		return 1;
}

static int fzip_exists(const char *filename)
{
	return FIO_std.exists(filename);
}

static int fzip_fsize(SYS_FILEHANDLE handle)
{
	return FIO_std.fsize(handle);
}

static char *fzip_fgets( char *buf, int len, SYS_FILEHANDLE handle)
{
	char *b = buf;
    if (buf == NULL || len <= 0) return NULL;

    while (--len > 0 && fzip_fread(buf, 1, sizeof(char), handle) == 1 && *buf++ != '\n') ;
    *buf = '\0';
    return b == buf && len > 0 ? NULL : b;
}

static int fzip_feof( SYS_FILEHANDLE handle )
{
	return 0;
}

SYS_FILEIO FIO_gzip =
{
	fzip_fopen,
	fzip_fclose,
	fzip_fseek,
	fzip_fread,
	fzip_fgetc,
	fzip_fwrite,
	fzip_ftell,
	fzip_feof,
	fzip_fgets,
	fzip_fsize,
	fzip_exists,
	NULL,
	NULL,
	fzip_fputc,
	fzip_fflush,
};

