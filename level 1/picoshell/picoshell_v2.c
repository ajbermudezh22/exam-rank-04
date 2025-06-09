#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>

int	calculate_pipes(char **cmds[])
{
	int i = 0;

	if (!cmds || !*cmds[0] || !cmds[0][0])
		return (-1);
	while (cmds[i])
		i++;
	return (i);
}

void	close_fds(int *fd, int tmp_fd)
{
	close(fd[0]);
	close(fd[1]);
	if (tmp_fd != -1)
		close(tmp_fd);
}

void	ft_redir(int *fd, int tmp_fd, int i, int n)
{
	if (i == 0)
	{
		dup2(fd[1], STDOUT_FILENO);
		close(fd[0]);
		close(fd[1]);
	}
	else if (i < n - 1)
	{
		dup2(tmp_fd, STDIN_FILENO);
		dup2(fd[0], STDOUT_FILENO);
		close_fds(fd, tmp_fd);
	}
	else
	{
		dup2(tmp_fd, STDIN_FILENO);
		close(tmp_fd);
	}
}

int	open_pipes(char **cmds, int *fd, int tmp_fd)
{
	if (cmds + 1)
	{
		if (pipe(fd) == -1)
		{
			close_fds(fd, tmp_fd);
			return (1);
		}
	}
	return (0);
}

int	picoshell(char **cmds[])
{
	int	fd[2];
	int	tmp_fd = -1;
	int	i = 0;
	int	pid;
	int	n;

	n = calculate_pipes(cmds);
	if (n == -1)
		return (0);
	if (n == 1)
	{
		execvp(cmds[0][0], cmds[0]);
		return (1);
	}
	while (cmds[i])
	{
		if (open_pipes(cmds[i], fd, tmp_fd) == 1)
			return (1);
		pid = fork();
		if (pid < 0)
		{
			close_fds(fd, tmp_fd);
			return (1);
		}
		if (pid == 0)
		{
			ft_redir(fd, tmp_fd, i, n);
			execvp(cmds[i][0], cmds[i]);
			return (1);
		}
		else
		{
			if (tmp_fd != -1)
				close(tmp_fd);
			if (cmds[i + 1])
			{
				tmp_fd = fd[0];
				close(fd[1]);
			}
		}
		i++;
	}
	wait(NULL);
	return (0);
}
