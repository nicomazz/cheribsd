# FreeBSD system call object files.
# DO NOT EDIT-- this file is automatically generated.
# $FreeBSD$
MIASM =  \
	cheriabi_syscall.o \
	exit.o \
	fork.o \
	cheriabi_read.o \
	cheriabi_write.o \
	cheriabi_open.o \
	close.o \
	cheriabi_wait4.o \
	cheriabi_link.o \
	cheriabi_unlink.o \
	cheriabi_chdir.o \
	fchdir.o \
	cheriabi_chmod.o \
	cheriabi_chown.o \
	getpid.o \
	cheriabi_mount.o \
	cheriabi_unmount.o \
	setuid.o \
	getuid.o \
	geteuid.o \
	ptrace.o \
	cheriabi_recvmsg.o \
	cheriabi_sendmsg.o \
	recvfrom.o \
	accept.o \
	getpeername.o \
	getsockname.o \
	cheriabi_access.o \
	cheriabi_chflags.o \
	fchflags.o \
	sync.o \
	kill.o \
	getppid.o \
	dup.o \
	getegid.o \
	cheriabi_profil.o \
	cheriabi_ktrace.o \
	getgid.o \
	cheriabi_getlogin.o \
	cheriabi_setlogin.o \
	cheriabi_acct.o \
	cheriabi_sigaltstack.o \
	cheriabi_ioctl.o \
	reboot.o \
	cheriabi_revoke.o \
	cheriabi_symlink.o \
	cheriabi_readlink.o \
	cheriabi_execve.o \
	umask.o \
	cheriabi_chroot.o \
	msync.o \
	vfork.o \
	munmap.o \
	cheriabi_mprotect.o \
	cheriabi_madvise.o \
	mincore.o \
	cheriabi_getgroups.o \
	cheriabi_setgroups.o \
	getpgrp.o \
	setpgid.o \
	cheriabi_setitimer.o \
	cheriabi_swapon.o \
	cheriabi_getitimer.o \
	getdtablesize.o \
	dup2.o \
	fcntl.o \
	select.o \
	fsync.o \
	setpriority.o \
	socket.o \
	connect.o \
	getpriority.o \
	bind.o \
	setsockopt.o \
	listen.o \
	cheriabi_gettimeofday.o \
	getrusage.o \
	getsockopt.o \
	cheriabi_readv.o \
	cheriabi_writev.o \
	cheriabi_settimeofday.o \
	fchown.o \
	fchmod.o \
	setreuid.o \
	setregid.o \
	cheriabi_rename.o \
	flock.o \
	mkfifo.o \
	sendto.o \
	shutdown.o \
	socketpair.o \
	cheriabi_mkdir.o \
	cheriabi_rmdir.o \
	cheriabi_utimes.o \
	adjtime.o \
	setsid.o \
	cheriabi_quotactl.o \
	cheriabi_nlm_syscall.o \
	cheriabi_nfssvc.o \
	cheriabi_lgetfh.o \
	cheriabi_getfh.o \
	cheriabi_sysarch.o \
	rtprio.o \
	setfib.o \
	ntp_adjtime.o \
	setgid.o \
	setegid.o \
	seteuid.o \
	cheriabi_pathconf.o \
	fpathconf.o \
	getrlimit.o \
	setrlimit.o \
	__sysctl.o \
	mlock.o \
	munlock.o \
	cheriabi_undelete.o \
	futimes.o \
	getpgid.o \
	cheriabi_poll.o \
	semget.o \
	semop.o \
	msgget.o \
	msgsnd.o \
	msgrcv.o \
	shmat.o \
	shmdt.o \
	shmget.o \
	cheriabi_clock_gettime.o \
	cheriabi_clock_settime.o \
	cheriabi_clock_getres.o \
	cheriabi_ktimer_create.o \
	ktimer_delete.o \
	cheriabi_ktimer_settime.o \
	cheriabi_ktimer_gettime.o \
	ktimer_getoverrun.o \
	cheriabi_nanosleep.o \
	ffclock_getcounter.o \
	ffclock_setestimate.o \
	ffclock_getestimate.o \
	cheriabi_clock_nanosleep.o \
	cheriabi_clock_getcpuclockid2.o \
	ntp_gettime.o \
	minherit.o \
	rfork.o \
	issetugid.o \
	lchown.o \
	cheriabi_aio_read.o \
	cheriabi_aio_write.o \
	cheriabi_lio_listio.o \
	cheriabi_kbounce.o \
	cheriabi_lchmod.o \
	cheriabi_lutimes.o \
	freebsd11_nstat.o \
	freebsd11_nfstat.o \
	freebsd11_nlstat.o \
	cheriabi_preadv.o \
	cheriabi_pwritev.o \
	fhopen.o \
	modnext.o \
	modstat.o \
	modfnext.o \
	modfind.o \
	kldload.o \
	kldunload.o \
	kldfind.o \
	kldnext.o \
	cheriabi_kldstat.o \
	kldfirstmod.o \
	getsid.o \
	setresuid.o \
	setresgid.o \
	cheriabi_aio_return.o \
	cheriabi_aio_suspend.o \
	cheriabi_aio_cancel.o \
	cheriabi_aio_error.o \
	mlockall.o \
	munlockall.o \
	__getcwd.o \
	sched_setparam.o \
	sched_getparam.o \
	sched_setscheduler.o \
	sched_getscheduler.o \
	sched_yield.o \
	sched_get_priority_max.o \
	sched_get_priority_min.o \
	sched_rr_get_interval.o \
	cheriabi_utrace.o \
	cheriabi_kldsym.o \
	cheriabi_jail.o \
	sigprocmask.o \
	sigsuspend.o \
	sigpending.o \
	cheriabi_sigtimedwait.o \
	cheriabi_sigwaitinfo.o \
	__acl_get_file.o \
	__acl_set_file.o \
	__acl_get_fd.o \
	__acl_set_fd.o \
	__acl_delete_file.o \
	__acl_delete_fd.o \
	__acl_aclcheck_file.o \
	__acl_aclcheck_fd.o \
	extattrctl.o \
	extattr_set_file.o \
	extattr_get_file.o \
	extattr_delete_file.o \
	cheriabi_aio_waitcomplete.o \
	cheriabi_getresuid.o \
	cheriabi_getresgid.o \
	kqueue.o \
	extattr_set_fd.o \
	extattr_get_fd.o \
	extattr_delete_fd.o \
	__setugid.o \
	eaccess.o \
	cheriabi_nmount.o \
	cheriabi___mac_get_proc.o \
	cheriabi___mac_set_proc.o \
	cheriabi___mac_get_fd.o \
	cheriabi___mac_get_file.o \
	cheriabi___mac_set_fd.o \
	cheriabi___mac_set_file.o \
	cheriabi_kenv.o \
	cheriabi_lchflags.o \
	uuidgen.o \
	cheriabi_sendfile.o \
	cheriabi_mac_syscall.o \
	cheriabi___mac_get_pid.o \
	cheriabi___mac_get_link.o \
	cheriabi___mac_set_link.o \
	extattr_set_link.o \
	extattr_get_link.o \
	extattr_delete_link.o \
	cheriabi___mac_execve.o \
	cheriabi_sigaction.o \
	cheriabi_sigreturn.o \
	cheriabi_getcontext.o \
	cheriabi_setcontext.o \
	cheriabi_swapcontext.o \
	cheriabi_swapoff.o \
	__acl_get_link.o \
	__acl_set_link.o \
	__acl_delete_link.o \
	__acl_aclcheck_link.o \
	sigwait.o \
	cheriabi_thr_create.o \
	thr_exit.o \
	thr_self.o \
	thr_kill.o \
	jail_attach.o \
	extattr_list_fd.o \
	extattr_list_file.o \
	extattr_list_link.o \
	ksem_timedwait.o \
	thr_suspend.o \
	thr_wake.o \
	kldunloadf.o \
	audit.o \
	cheriabi_auditon.o \
	getauid.o \
	setauid.o \
	getaudit.o \
	setaudit.o \
	getaudit_addr.o \
	setaudit_addr.o \
	auditctl.o \
	_umtx_op.o \
	cheriabi_thr_new.o \
	cheriabi_sigqueue.o \
	kmq_open.o \
	kmq_setattr.o \
	kmq_timedreceive.o \
	kmq_timedsend.o \
	cheriabi_kmq_notify.o \
	kmq_unlink.o \
	cheriabi_abort2.o \
	thr_set_name.o \
	cheriabi_aio_fsync.o \
	rtprio_thread.o \
	sctp_peeloff.o \
	sctp_generic_sendmsg.o \
	sctp_generic_sendmsg_iov.o \
	sctp_generic_recvmsg.o \
	cheriabi_pread.o \
	cheriabi_pwrite.o \
	cheriabi_mmap.o \
	lseek.o \
	cheriabi_truncate.o \
	ftruncate.o \
	thr_kill2.o \
	shm_open.o \
	shm_unlink.o \
	cpuset.o \
	cpuset_setid.o \
	cpuset_getid.o \
	cpuset_getaffinity.o \
	cpuset_setaffinity.o \
	cheriabi_faccessat.o \
	cheriabi_fchmodat.o \
	cheriabi_fchownat.o \
	cheriabi_fexecve.o \
	cheriabi_futimesat.o \
	cheriabi_linkat.o \
	cheriabi_mkdirat.o \
	cheriabi_mkfifoat.o \
	cheriabi_openat.o \
	cheriabi_readlinkat.o \
	cheriabi_renameat.o \
	cheriabi_symlinkat.o \
	cheriabi_unlinkat.o \
	posix_openpt.o \
	gssd_syscall.o \
	cheriabi_jail_get.o \
	cheriabi_jail_set.o \
	jail_remove.o \
	closefrom.o \
	cheriabi___semctl.o \
	cheriabi_msgctl.o \
	shmctl.o \
	cheriabi_lpathconf.o \
	__cap_rights_get.o \
	cap_enter.o \
	cap_getmode.o \
	pdfork.o \
	pdkill.o \
	pdgetpid.o \
	pselect.o \
	getloginclass.o \
	setloginclass.o \
	rctl_get_racct.o \
	rctl_get_rules.o \
	rctl_get_limits.o \
	rctl_add_rule.o \
	rctl_remove_rule.o \
	posix_fallocate.o \
	posix_fadvise.o \
	cheriabi_wait6.o \
	cap_rights_limit.o \
	cap_ioctls_limit.o \
	cap_ioctls_get.o \
	cap_fcntls_limit.o \
	cap_fcntls_get.o \
	bindat.o \
	connectat.o \
	cheriabi_chflagsat.o \
	accept4.o \
	pipe2.o \
	cheriabi_aio_mlock.o \
	cheriabi_procctl.o \
	cheriabi_ppoll.o \
	cheriabi_futimens.o \
	cheriabi_utimensat.o \
	numa_getaffinity.o \
	numa_setaffinity.o \
	fdatasync.o \
	fstat.o \
	fstatat.o \
	fhstat.o \
	getdirentries.o \
	statfs.o \
	fstatfs.o \
	getfsstat.o \
	fhstatfs.o \
	cheriabi_mknodat.o \
	cheriabi_kevent.o
