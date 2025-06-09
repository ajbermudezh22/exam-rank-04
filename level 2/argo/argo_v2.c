
#include "argo.h"
#include <stdio.h>
#include <stdlib.h>
// #include <sys/wait.h>
// #include <stdbool.h>
// #include <string.h>

json	parse_json(FILE *stream);
int	g_error = 0;

void	free_json(json j)
{
	int	i = 0;
	
	if (j.type == MAP)
	{
		while (i < j.map.size)
		{
			free(j.map.data[i].key);
			free_json(j.map.data[i].value);
		}
		free(j.map.data);
	}
	if (j.type == STRING)
		free(j.string);
}

json	parse_number(FILE *stream)
{
	json	json_num;
	int		ret;
	int		integer;

	json_num.type = INTEGER;
	json_num.integer = 0;
	ret = fscanf(stream, "%d", &integer);
	if (ret <= 0 || ret == EOF)
		g_error = 1;
	else
		json_num.integer = integer;
	return (json_num);
}

char	*parse_string(FILE *stream)
{
	char	c;
	char	*res;
	int		size;

	size = 0;
	res = malloc(sizeof(char));
	res[size] = '\0';
	if (!accept(stream, '"'))
	{
		g_error = 1;
		return (res);
	}
	while (peek(stream) != '"' && peek(stream) != EOF)
	{
		size++;
		res = realloc(res, (size + 1) * sizeof(char));
		res[size] = '\0';
		c = getc(stream);
		if (c == '\\')
		{
			if (peek(stream) != '\\' && peek(stream) != '\"')
			{
				g_error = 1;
				return (res);
			}
			c = getc(stream);
		}
		res[size - 1] = c;
	}
	if (!accept(stream, '"'))
		g_error = 1;
	return (res);
}

json	parse_string_json(FILE *stream)
{
	json	json_string;

	json_string.type = STRING;
	json_string.string = parse_string(stream);
	return (json_string);
}

void	read_map(json *json_map,FILE *stream)
{
	if (!accept(stream, '{'))
	{
		g_error = 1;
		return ;
	}
	do
	{
		pair	new_pair;
		new_pair.key = parse_string(stream);
		if (g_error)
		{
			free(new_pair.key);
			g_error = 2;
			return ;
		}
		if (!accept(stream, ':'))
		{
			g_error = 1;
			return ;
		}
		new_pair.value = parse_json(stream);
		if (g_error)
		{
			free(new_pair.key);
			free_json(new_pair.value);
			return ;
		}
		json_map->map.size++;
		json_map->map.data = realloc(json_map->map.data, json_map->map.size * sizeof(pair));
		json_map->map.data[json_map->map.size - 1] = new_pair;
	} while (accept(stream, ','));
	if (!accept(stream, '}'))
		g_error = 1;
}

json	parse_map(FILE *stream)
{
	json	json_map;

	json_map.type = MAP;
	json_map.map.data = NULL;
	json_map.map.size = 0;
	read_map(&json_map, stream);
	return (json_map);
}

json	parse_json(FILE *stream)
{
	if (peek(stream) == '"')
		return (parse_string_json(stream));
	if (peek(stream) == '{')
		return (parse_map(stream));
	if (peek(stream) == '-' || isdigit(peek(stream)))
		return (parse_number(stream));
	json nothing;
	nothing.type = STRING;
	nothing.string = NULL;
	return (nothing);
}

int	argo(json *dst, FILE *stream)
{
	if (!stream)
		return (-1);
	*dst = parse_json(stream);
	if (g_error == 1)
		unexpected(stream);
	if (g_error == 0)
		return (1);
	return (-1);
}
