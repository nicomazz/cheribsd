/*-
 * SPDX-License-Identifier: BSD-2-Clause-FreeBSD
 *
 * Copyright (c) 2014, 2017 Mark Johnston <markj@FreeBSD.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * Subroutines used for writing compressed user process and kernel core dumps.
 */

#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include "opt_gzio.h"

#include <sys/param.h>

#include <sys/compressor.h>
#include <sys/kernel.h>
#include <sys/linker_set.h>
#include <sys/malloc.h>

MALLOC_DEFINE(M_COMPRESS, "compressor", "kernel compression subroutines");

struct compressor_methods {
	int format;
	void *(* const init)(size_t, int);
	void (* const reset)(void *);
	int (* const write)(void *, void *, size_t, compressor_cb_t, void *);
	void (* const fini)(void *);
};

struct compressor {
	const struct compressor_methods *methods;
	compressor_cb_t cb;
	void *priv;
	void *arg;
};

SET_DECLARE(compressors, struct compressor_methods);

#ifdef GZIO

#include <sys/zutil.h>

struct gz_stream {
	uint8_t		*gz_buffer;	/* output buffer */
	size_t		gz_bufsz;	/* output buffer size */
	off_t		gz_off;		/* offset into the output stream */
	uint32_t	gz_crc;		/* stream CRC32 */
	z_stream	gz_stream;	/* zlib state */
};

static void 	*gz_init(size_t maxiosize, int level);
static void	gz_reset(void *stream);
static int	gz_write(void *stream, void *data, size_t len, compressor_cb_t,
		    void *);
static void	gz_fini(void *stream);

static void *
gz_alloc(void *arg __unused, u_int n, u_int sz)
{

	/*
	 * Memory for zlib state is allocated using M_NODUMP since it may be
	 * used to compress a kernel dump, and we don't want zlib to attempt to
	 * compress its own state.
	 */
	return (malloc(n * sz, M_COMPRESS, M_WAITOK | M_ZERO | M_NODUMP));
}

static void
gz_free(void *arg __unused, void *ptr)
{

	free(ptr, M_COMPRESS);
}

static void *
gz_init(size_t maxiosize, int level)
{
	struct gz_stream *s;
	int error;

	s = gz_alloc(NULL, 1, roundup2(sizeof(*s), PAGE_SIZE));
	s->gz_buffer = gz_alloc(NULL, 1, maxiosize);
	s->gz_bufsz = maxiosize;

	s->gz_stream.zalloc = gz_alloc;
	s->gz_stream.zfree = gz_free;
	s->gz_stream.opaque = NULL;
	s->gz_stream.next_in = Z_NULL;
	s->gz_stream.avail_in = 0;

	error = deflateInit2(&s->gz_stream, level, Z_DEFLATED, -MAX_WBITS,
	    DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY);
	if (error != 0)
		goto fail;

	gz_reset(s);

	return (s);

fail:
	gz_free(NULL, s);
	return (NULL);
}

static void
gz_reset(void *stream)
{
	struct gz_stream *s;
	uint8_t *hdr;
	const size_t hdrlen = 10;

	s = stream;
	s->gz_off = 0;
	s->gz_crc = ~0U;

	(void)deflateReset(&s->gz_stream);
	s->gz_stream.avail_out = s->gz_bufsz;
	s->gz_stream.next_out = s->gz_buffer;

	/* Write the gzip header to the output buffer. */
	hdr = s->gz_buffer;
	memset(hdr, 0, hdrlen);
	hdr[0] = 0x1f;
	hdr[1] = 0x8b;
	hdr[2] = Z_DEFLATED;
	hdr[9] = OS_CODE;
	s->gz_stream.next_out += hdrlen;
	s->gz_stream.avail_out -= hdrlen;
}

static int
gz_write(void *stream, void *data, size_t len, compressor_cb_t cb,
    void *arg)
{
	struct gz_stream *s;
	uint8_t trailer[8];
	size_t room;
	int error, zerror, zflag;

	s = stream;
	zflag = data == NULL ? Z_FINISH : Z_NO_FLUSH;

	if (len > 0) {
		s->gz_stream.avail_in = len;
		s->gz_stream.next_in = data;
		s->gz_crc = crc32_raw(data, len, s->gz_crc);
	} else
		s->gz_crc ^= ~0U;

	error = 0;
	do {
		zerror = deflate(&s->gz_stream, zflag);
		if (zerror != Z_OK && zerror != Z_STREAM_END) {
			error = EIO;
			break;
		}

		if (s->gz_stream.avail_out == 0 || zerror == Z_STREAM_END) {
			/*
			 * Our output buffer is full or there's nothing left
			 * to produce, so we're flushing the buffer.
			 */
			len = s->gz_bufsz - s->gz_stream.avail_out;
			if (zerror == Z_STREAM_END) {
				/*
				 * Try to pack as much of the trailer into the
				 * output buffer as we can.
				 */
				((uint32_t *)trailer)[0] = s->gz_crc;
				((uint32_t *)trailer)[1] =
				    s->gz_stream.total_in;
				room = MIN(sizeof(trailer),
				    s->gz_bufsz - len);
				memcpy(s->gz_buffer + len, trailer, room);
				len += room;
			}

			error = cb(s->gz_buffer, len, s->gz_off, arg);
			if (error != 0)
				break;

			s->gz_off += len;
			s->gz_stream.next_out = s->gz_buffer;
			s->gz_stream.avail_out = s->gz_bufsz;

			/*
			 * If we couldn't pack the trailer into the output
			 * buffer, write it out now.
			 */
			if (zerror == Z_STREAM_END && room < sizeof(trailer))
				error = cb(trailer + room,
				    sizeof(trailer) - room, s->gz_off, arg);
		}
	} while (zerror != Z_STREAM_END &&
	    (zflag == Z_FINISH || s->gz_stream.avail_in > 0));

	return (error);
}

static void
gz_fini(void *stream)
{
	struct gz_stream *s;

	s = stream;
	(void)deflateEnd(&s->gz_stream);
	gz_free(NULL, s->gz_buffer);
	gz_free(NULL, s);
}

struct compressor_methods gzip_methods = {
	.format = COMPRESS_GZIP,
	.init = gz_init,
	.reset = gz_reset,
	.write = gz_write,
	.fini = gz_fini,
};
DATA_SET(compressors, gzip_methods);

#endif /* GZIO */

bool
compressor_avail(int format)
{
	struct compressor_methods **iter;

	SET_FOREACH(iter, compressors) {
		if ((*iter)->format == format)
			return (true);
	}
	return (false);
}

struct compressor *
compressor_init(compressor_cb_t cb, int format, size_t maxiosize, int level,
    void *arg)
{
	struct compressor_methods **iter;
	struct compressor *s;
	void *priv;

	SET_FOREACH(iter, compressors) {
		if ((*iter)->format == format)
			break;
	}
	if (iter == NULL)
		return (NULL);

	priv = (*iter)->init(maxiosize, level);
	if (priv == NULL)
		return (NULL);

	s = malloc(sizeof(*s), M_COMPRESS, M_WAITOK | M_ZERO);
	s->methods = (*iter);
	s->priv = priv;
	s->cb = cb;
	s->arg = arg;
	return (s);
}

void
compressor_reset(struct compressor *stream)
{

	stream->methods->reset(stream->priv);
}

int
compressor_write(struct compressor *stream, void *data, size_t len)
{

	return (stream->methods->write(stream->priv, data, len, stream->cb,
	    stream->arg));
}

int
compressor_flush(struct compressor *stream)
{

	return (stream->methods->write(stream->priv, NULL, 0, stream->cb,
	    stream->arg));
}

void
compressor_fini(struct compressor *stream)
{

	stream->methods->fini(stream->priv);
}
