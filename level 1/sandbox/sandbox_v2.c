
#include <stdbool.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int	printf_chld_executed_correct(bool verbose)
{
	if (verbose)
		printf("Nice function!\n");
	return (1);
}

int	printf_chld_executed_incorrect(bool verbose, int exit_code)
{
	if (verbose)
		printf("Bad function: exited with code %d\n", exit_code);
	return (0);
}

int	printf_signal(bool verbose, char *str_signal)
{
	if (verbose)
		printf("Bad function: %s\n", str_signal);
	return (0);
}

int	printf_timeout(bool verbose, unsigned int timeout)
{
	if (verbose)
		printf("Bad function: timed out after %d seconds\n", timeout);
	return (0);
}

bool	flag_timeout = 0;

void	handle_alarm(int sig)
{
	(void)sig;
	flag_timeout = 1;
}

int sandbox(void (*f)(void), unsigned int timeout, bool verbose)
{
	struct sigaction	sa;
	int					cpid;
	int					status;
	int					exit_code;

	sa.sa_handler = handle_alarm;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if (sigaction(SIGALRM, &sa, NULL) == -1)
		return (-1);

	cpid = fork();
	if (cpid == -1)
		return (-1);
	if (cpid == 0)
	{
		f();
		exit(0);
	}
	alarm(timeout);
	while (1)
	{
		if (waitpid(cpid, &status, 0) == cpid)
		{
			if (WIFEXITED(status))
			{
				exit_code = WEXITSTATUS(status);
				if (exit_code == 0)
					return (printf_chld_executed_correct(verbose));
				return (printf_chld_executed_incorrect(verbose, exit_code));
			}
			else if (WIFSIGNALED(status))
				return (printf_signal(verbose, strsignal(WTERMSIG(status))));
		}
		else if (flag_timeout)
		{
			kill(cpid, SIGKILL);
			waitpid(cpid, &status, 0);
			return (printf_timeout(verbose, timeout));
		}
	}
	return (-1);
}
