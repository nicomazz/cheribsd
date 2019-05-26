/*-
 * SPDX-License-Identifier: BSD-2-Clause-FreeBSD
 *
 * Copyright (c) 2019 The FreeBSD Foundation
 *
 * This software was developed by BFF Storage Systems, LLC under sponsorship
 * from the FreeBSD Foundation.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

extern "C" {
#include <sys/types.h>

#include <pthread.h>

#include "fuse_kernel.h"
}

#include <gmock/gmock.h>

#define TIME_T_MAX (std::numeric_limits<time_t>::max())

/* 
 * A pseudo-fuse errno used indicate that a fuse operation should have no
 * response, at least not immediately
 */
#define FUSE_NORESPONSE 9999

#define SET_OUT_HEADER_LEN(out, variant) { \
	(out)->header.len = (sizeof((out)->header) + \
			     sizeof((out)->body.variant)); \
}

/*
 * Create an expectation on FUSE_LOOKUP and return it so the caller can set
 * actions.
 *
 * This must be a macro instead of a method because EXPECT_CALL returns a type
 * with a deleted constructor.
 */
#define EXPECT_LOOKUP(parent, path)					\
	EXPECT_CALL(*m_mock, process(					\
		ResultOf([=](auto in) {					\
			return (in->header.opcode == FUSE_LOOKUP &&	\
				in->header.nodeid == (parent) &&	\
				strcmp(in->body.lookup, (path)) == 0);	\
		}, Eq(true)),						\
		_)							\
	)

extern int verbosity;

/* This struct isn't defined by fuse_kernel.h or libfuse, but it should be */
struct fuse_create_out {
	struct fuse_entry_out	entry;
	struct fuse_open_out	open;
};

/* Protocol 7.8 version of struct fuse_attr */
struct fuse_attr_7_8
{
	__u64	ino;
	__u64	size;
	__u64	blocks;
	__u64	atime;
	__u64	mtime;
	__u64	ctime;
	__u32	atimensec;
	__u32	mtimensec;
	__u32	ctimensec;
	__u32	mode;
	__u32	nlink;
	__u32	uid;
	__u32	gid;
	__u32	rdev;
};

/* Protocol 7.8 version of struct fuse_attr_out */
struct fuse_attr_out_7_8
{
	__u64	attr_valid;
	__u32	attr_valid_nsec;
	__u32	dummy;
	struct fuse_attr_7_8 attr;
};

/* Protocol 7.8 version of struct fuse_entry_out */
struct fuse_entry_out_7_8 {
	__u64	nodeid;		/* Inode ID */
	__u64	generation;	/* Inode generation: nodeid:gen must
				   be unique for the fs's lifetime */
	__u64	entry_valid;	/* Cache timeout for the name */
	__u64	attr_valid;	/* Cache timeout for the attributes */
	__u32	entry_valid_nsec;
	__u32	attr_valid_nsec;
	struct fuse_attr_7_8 attr;
};

/* Output struct for FUSE_CREATE for protocol 7.8 servers */
struct fuse_create_out_7_8 {
	struct fuse_entry_out_7_8	entry;
	struct fuse_open_out	open;
};

union fuse_payloads_in {
	fuse_access_in	access;
	/* value is from fuse_kern_chan.c in fusefs-libs */
	uint8_t		bytes[0x21000 - sizeof(struct fuse_in_header)];
	fuse_flush_in	flush;
	fuse_fsync_in	fsync;
	fuse_fsync_in	fsyncdir;
	fuse_forget_in	forget;
	fuse_interrupt_in interrupt;
	fuse_lk_in	getlk;
	fuse_getxattr_in getxattr;
	fuse_init_in	init;
	fuse_link_in	link;
	fuse_listxattr_in listxattr;
	char		lookup[0];
	fuse_mkdir_in	mkdir;
	fuse_mknod_in	mknod;
	fuse_open_in	open;
	fuse_open_in	opendir;
	fuse_read_in	read;
	fuse_read_in	readdir;
	fuse_release_in	release;
	fuse_release_in	releasedir;
	fuse_rename_in	rename;
	char		rmdir[0];
	fuse_setattr_in	setattr;
	fuse_setxattr_in setxattr;
	fuse_lk_in	setlk;
	fuse_lk_in	setlkw;
	char		unlink[0];
	fuse_write_in	write;
};

struct mockfs_buf_in {
	fuse_in_header		header;
	union fuse_payloads_in	body;
};

union fuse_payloads_out {
	fuse_attr_out		attr;
	fuse_attr_out_7_8	attr_7_8;
	fuse_create_out		create;
	fuse_create_out_7_8	create_7_8;
	/* The protocol places no limits on the size of bytes */
	uint8_t 		bytes[0x20000];
	fuse_entry_out		entry;
	fuse_entry_out_7_8	entry_7_8;
	fuse_lk_out		getlk;
	fuse_getxattr_out	getxattr;
	fuse_init_out		init;
	fuse_listxattr_out	listxattr;
	fuse_open_out		open;
	fuse_statfs_out		statfs;
	/*
	 * The protocol places no limits on the length of the string.  This is
	 * merely convenient for testing.
	 */
	char			str[80];
	fuse_write_out		write;
};

struct mockfs_buf_out {
	fuse_out_header		header;
	union fuse_payloads_out	body;

	/* Default constructor: zero everything */
	mockfs_buf_out() {
		memset(this, 0, sizeof(*this));
	}
};

/* A function that can be invoked in place of MockFS::process */
typedef std::function<void (const struct mockfs_buf_in *in,
			    std::vector<struct mockfs_buf_out*> &out)>
ProcessMockerT;

/*
 * Helper function used for setting an error expectation for any fuse operation.
 * The operation will return the supplied error
 */
ProcessMockerT ReturnErrno(int error);

/* Helper function used for returning negative cache entries for LOOKUP */
ProcessMockerT ReturnNegativeCache(const struct timespec *entry_valid);

/* Helper function used for returning a single immediate response */
ProcessMockerT ReturnImmediate(
	std::function<void(const struct mockfs_buf_in *in,
			   struct mockfs_buf_out *out)> f);

/* How the daemon should check /dev/fuse for readiness */
enum poll_method {
	BLOCKING,
	SELECT,
	POLL,
	KQ
};

/*
 * Fake FUSE filesystem
 *
 * "Mounts" a filesystem to a temporary directory and services requests
 * according to the programmed expectations.
 *
 * Operates directly on the fusefs(4) kernel API, not the libfuse(3) user api.
 */
class MockFS {
	/*
	 * thread id of the fuse daemon thread
	 *
	 * It must run in a separate thread so it doesn't deadlock with the
	 * client test code.
	 */
	pthread_t m_daemon_id;

	/* file descriptor of /dev/fuse control device */
	int m_fuse_fd;
	
	/* The minor version of the kernel API that this mock daemon targets */
	uint32_t m_kernel_minor_version;

	int m_kq;

	/* The max_readahead filesystem option */
	uint32_t m_maxreadahead;

	/* pid of the test process */
	pid_t m_pid;

	/* Method the daemon should use for I/O to and from /dev/fuse */
	enum poll_method m_pm;

	/* Initialize a session after mounting */
	void init(uint32_t flags);

	/* Is pid from a process that might be involved in the test? */
	bool pid_ok(pid_t pid);

	/* Default request handler */
	void process_default(const mockfs_buf_in*,
		std::vector<mockfs_buf_out*>&);

	/* Entry point for the daemon thread */
	static void* service(void*);

	/* Read, but do not process, a single request from the kernel */
	void read_request(mockfs_buf_in*);

	/* Write a single response back to the kernel */
	void write_response(mockfs_buf_out *out);

	public:
	/* pid of child process, for two-process test cases */
	pid_t m_child_pid;

	/* Maximum size of a FUSE_WRITE write */
	uint32_t m_max_write;

	/* 
	 * Number of events that were available from /dev/fuse after the last
	 * kevent call.  Only valid when m_pm = KQ.
	 */
	int m_nready;

	/* Tell the daemon to shut down ASAP */
	bool m_quit;

	/* Create a new mockfs and mount it to a tempdir */
	MockFS(int max_readahead, bool allow_other,
		bool default_permissions, bool push_symlinks_in, bool ro,
		enum poll_method pm, uint32_t flags,
		uint32_t kernel_minor_version);

	virtual ~MockFS();

	/* Kill the filesystem daemon without unmounting the filesystem */
	void kill_daemon();

	/* Process FUSE requests endlessly */
	void loop();

	/* 
	 * Request handler
	 *
	 * This method is expected to provide the responses to each FUSE
	 * operation.  For an immediate response, push one buffer into out.
	 * For a delayed response, push nothing.  For an immediate response
	 * plus a delayed response to an earlier operation, push two bufs.
	 * Test cases must define each response using Googlemock expectations
	 */
	MOCK_METHOD2(process, void(const mockfs_buf_in*,
				std::vector<mockfs_buf_out*>&));

	/* Gracefully unmount */
	void unmount();
};
