#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>

int	ft_popen(const char *file, char *const argv[], char type)
{
	int pipefd[2];
	pid_t id;

	if (type != 'r' && type != 'w')
		return (-1);
	if (pipe(pipefd) == -1)
		return (-1);
	id = fork();
	if (id == -1)
	{
		close(pipefd[0]);
		close(pipefd[1]);
		return (-1);
	}
	if (id == 0)
	{
		if (type == 'r')
		{
			close(pipefd[0]);
			if (dup2(pipefd[1], STDOUT_FILENO) == -1)
				exit (EXIT_FAILURE);
			close(pipefd[1]);
		}
		else
		{
			close(pipefd[1]);
			if (dup2(pipefd[0], STDIN_FILENO) == -1)
				exit (EXIT_FAILURE);
			close (pipefd[0]);
		}
		execvp(file, argv);
		exit (EXIT_FAILURE);
	}
	if (type == 'r')
	{
		close(pipefd[1]);
		return(pipefd[0]);
	}
	if (type == 'w')
	{
		close(pipefd[0]);
		return (pipefd[1]);
	}
	return (-1);
}
