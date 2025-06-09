#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

//#define DEBUG

// Function to handle the redirection and file descriptor management for each child
int handle_redirection(int i, int n, int fds[2], int tmp_fd) 
{
	if (i == 0) // First command
	{
		dup2(fds[1], STDOUT_FILENO);
		close(fds[0]);
		close(fds[1]);
	}
	else if (i < n - 1) // Middle commands
	{
		dup2(tmp_fd, STDIN_FILENO);
		dup2(fds[1], STDOUT_FILENO);
		close(tmp_fd);
		close(fds[0]);
		close(fds[1]);
	}
	else // Last command
	{
		dup2(tmp_fd, STDIN_FILENO);
		close(tmp_fd);
	}
	return 0;
}

int execute_in_pipe(char **cmds[], int n) 
{
	int fds[2];
	int tmp_fd = -1;
	pid_t pid;

	for (int i = 0; i < n; i++) 
	{
		if (i < n - 1 && pipe(fds) == -1)
		{
			perror("pipe");
			return -1;
		}
		pid = fork();
		if (pid == -1)
		{
			perror("fork");
			return -1;
		}

		if (pid == 0) // Child
		{
			if (handle_redirection(i, n, fds, tmp_fd) == -1)
				exit(EXIT_FAILURE);
			execvp(cmds[i][0], cmds[i]);
			perror("execvp");
			exit(EXIT_FAILURE);
		}
		// Parent closes unused ends
		if (tmp_fd != -1)
			close(tmp_fd);      // Close previous read end
		if (i < n - 1) {
			tmp_fd = fds[0];    // Save current read end for next command
			close(fds[1]);      // Close current write end
		}
	}

	// Wait for all children
	for (int i = 0; i < n; i++)
		wait(NULL);
	return 0;
}

int	calculate_cmds(char **cmds[], int *n)
{
	int	i = 0;

	if (!cmds || ! *cmds[0] || !cmds[0][0])
		return (1);
	while (cmds[i] != NULL)
	{
		i++;
	}
	*n = i;
	return (0);
}

int	picoshell(char **cmds[])
{
	int		n;

	if (calculate_cmds(cmds, &n))
		return (0);
	#ifdef DEBUG
		printf("Commands number %d\n", n);
	#endif
	if (n == 1)
	{
		if (execvp(cmds[0][0], cmds[0]) == -1)
			return (1);
		return (0);
	}
	else
		if (execute_in_pipe(cmds, n) == -1)
			return (1);
	return (0);
}
