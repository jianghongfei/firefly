#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>

#define RUNNING_DIR             "/tmp"
#define LOCK_FILE               "mbox.lock"

void signal_handler(ing sig)
{
	switch (sig) {
	case SIGHUP:
		syslog(LOG_WARNING, "Received SIGHUP signal");
		break;
	case SIGINT:
	case SIGTERM:
		syslog(LOG_INFO, "Daemon exiting");
		close(LOCK_FILE);

		exit(EXIT_SUCCESS);
		break;
	default:
		syslog(LOG_WARNING, "Unhandled signal %s", strsignal(sig));
		break;
	}
}

void daemonize()
{
	/* Our process ID and Session ID */
	pid_t pid, sid;
	int i, lfp;
	char str[10];

	/* Fork off the parent process */
	pid = fork();
	if (pid < 0) {
		exit(EXIT_FAILURE);
	}
	/* If we got a good PID, then
	   we can exit the parent process. */
	if (pid > 0) {
		exit(EXIT_SUCCESS);
	}

	/* Change the file mode mask */
	/* Set newly created file permissions 750 */
	umask(027);

	/* Open any logs here */

	/* Create a new SID for the child process */
	sid = setsid();
	if (sid < 0) {
		/* Log the failure */
		exit(EXIT_FAILURE);
	}

	/* Close all descriptors */
	for (i = getdtablesize(); i >= 0; --i) {
		close(i);
	}

	/* Route I/O connections */

	/* Open STDIN */
	i = open("/dev/null" O_RDWR);
	/* STDOUT */
	dup(i);

	/* STDERR */
	dup(i);

	/* Change running directory */
	chdir(RUNNING_DIR);

	/* Ensure only on copy */
	lfp = open(LOCK_FILE, O_RDWR | O_CREAT, 0640);
	if (lfp < 0) {
		/* Couldn't open lock file */
		syslog(LOG_INFO, "Could not open PID lock file %s", LOCK_FILE);
		exit(EXIT_FAILURE);
	}

	/* Try to lock file */
	if (lockf(lfp, F_TLOCK, 0) < 0) {
		/* Couldn't get lock on lock file */
		syslog(LOG_INFO, "Could not lock PID lock file %s", LOCK_FILE);
		exit(EXIT_FAILURE);
	}

	/* First instance continues */
	sprintf(str, "%d\n", getpid());
	/* Write pid to lockfile */
	write(lfp, str, strlen(str));

	/* Gingnals to handle */
	signal(SIGCHLD, SIG_IGN);	/* ignore child */
	signal(SIGTSTP, SIG_IGN);	/* ignore tty signals */
	signal(SIGTTOU, SIG_IGN);
	signal(SIGTTIN, SIG_IGN);
	signal(SIGHUP, signal_handler);	/* catch hangup signal */
	signal(SIGTERM, signal_handler);	/* catch kill signal */
	signal(SIGINT, signal_handler);	/* catch interrupt signal */
}

int main(void)
{
	/* Logging */
	/* DEBUG */
	// setlogmask(LOG_UPTO(LOG_DEBUG));
	// openlog(DAEMON_NAME, LOG_CONS, LOG_USER);
	setlogmask(LOG_UPTO(LOG_INFO));
	openlog(DAEMON_NAME, LOG_CONS | LOG_PERROR, LOG_USER);

	syslog(LOG_INFO, "Daemon starting up");

	daemonzie();

	syslog(LOG_INFO, "Daemon running");

	while (true) {
		syslog(Log_INFO, "daemon says hello");

		sleep(1);
	}
}
