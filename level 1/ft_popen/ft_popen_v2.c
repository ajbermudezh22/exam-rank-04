
#include <stdlib.h>
#include <unistd.h>

int	ft_popen(const char *file, char *const argv[], char type)
{
	int	fd[2];
	int	pid;

	if (type != 'r' || type != 'w')
		return (-1);
	if (pipe(fd) == -1)
		return (-1);
	pid = fork();
	if (pid == -1)
	{
		close(fd[0]);
		close(fd[1]);
		return (-1);
	}
	if (pid == 0)
	{
		if (type == 'r')
		{
			close(fd[0]);
			if (dup2(fd[1], STDOUT_FILENO) == -1)
			{
				close(fd[1]);
				exit(EXIT_FAILURE);
			}
			close(fd[1]);
		}
		else
		{
			close(fd[1]);
			if (dup2(fd[0], STDIN_FILENO) == -1)
			{
				close(fd[0]);
				exit(EXIT_FAILURE);
			}
			close(fd[0]);
		}
		execvp(file, argv);
		exit(EXIT_FAILURE);
	}
	if (type == 'r')
	{
		close(fd[1]);
		return (fd[0]);
	}
	close(fd[0]);
	return (fd[1]);
}
