/* The MIT License (MIT)
 *
 * Copyright (c) 2016 Cyril Schumacher
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <process.h>

#include "yshell.h"

/**
 * Converts the {@code char *} to {@code char **}.
 * @param {char *}  str         The string to convert.
 * @param {size_t}  str_len     The string length.
 * @param {char}    delimiter   The delimiter.
 * @return {char **} The array.
 */
char **to_char_array(char *str, size_t str_len, char delimiter) {
	char **str_arr = (char **) malloc(2 * sizeof(char *));
	if (str_arr == NULL)
		return NULL;

	char *buf = "";
	int i, j = 0;
	for (i = 0; i < str_len; i++) {
		char *cur = (char *) calloc(2, sizeof(char));
		cur[0] = str[i];

		if (cur[0] == delimiter) {
			if (strcmp(buf, "") != 0) {
				str_arr[j] = (char *) calloc(strlen(buf), sizeof(char));
				strcpy(str_arr[j], buf);
				buf = "";
				j++;
			}
		} else {
			size_t tmp_len = strlen(buf) + 1;
			char *tmp = (char *) calloc(tmp_len, sizeof(char));
			strcpy(tmp, buf);
			strcat(tmp, cur);
			buf = (char *) calloc(0, tmp_len);
			strcpy(buf, tmp);
		}
	}

	str_arr[j] = NULL;
	return str_arr;
}

/**
 * Count all elements in an array.
 * @param {char **} The array.
 * @return {size_t} The number of elements in array.
 */
size_t arr_count(char **arr) {
	char *cur = NULL;
	size_t count = 0;

	do {
		cur = arr[count];
		count++;
	} while (cur != NULL);

	return (count - 1);
}

/**
 * Exits the yShell.
 * @param {int}     argc The argument count.
 * @param {void*}   argv The argument vector.
 * @return {int} Returns the error code.
 */
int yshell_exit(int argc, void *argv) {
	exit(0);
	return SUCCESS;
}

/**
 * Prints message.
 * @param {int}     argc The argument count.
 * @param {void*}   argv The argument vector.
 * @return {int} Returns the error code.
 */
int yshell_print(int argc, void *argv) {
	const char **words = argv;

	char *msg = "";
	for (int i = 0; words[i] != NULL; i++) {
		const char *curr = words[i];
		const size_t buf_len = (sizeof(char) * strlen(msg)) + (sizeof(char) * strlen(curr)) + (sizeof(char) * 2);
		char buf[buf_len];

		strcpy(buf, msg);
		strcat(buf, curr);
		if (words[i + 1] != NULL)
			strcat(buf, " ");

		free(msg);
		const size_t msg_len = sizeof(char) * strlen(buf) + sizeof(char);
		msg = (char *) malloc(msg_len);
		if (msg == NULL)
			return ERROR;

		strcpy(msg, buf);
		free(buf);
	}

	printf("%s\n", msg);
	return SUCCESS;
}

/**
 * Adds new command.
 * @param {P_YSHELL_COMMANDS*}  stack   The stack.
 * @param {char*}               name    The name.
 * @param {yShellCommandAction} action  The action.
 * @return {int} Returns zero if the command has added, otherwise, less zero if a error occurs.
 */
int add_cmd(P_YSHELL_COMMANDS *stack, char *name, yShellCommandAction action) {
	P_YSHELL_COMMANDS new_stack = (P_YSHELL_COMMANDS) malloc(sizeof(YSHELL_COMMANDS));
	if (new_stack == NULL)
		return ERROR;

	YSHELL_COMMAND cmd = {.name = name, .action = action};
	new_stack->cmd = cmd;
	new_stack->next = (*stack);

	(*stack) = new_stack;
	return SUCCESS;
}

/**
 * Initializes yShell commands.
 * @return {P_YSHELL_CMD_STACK} The commands.
 */
P_YSHELL_COMMANDS init_cmd_stack() {
	P_YSHELL_COMMANDS stack = NULL;
	add_cmd(&stack, "exit", &yshell_exit);
	add_cmd(&stack, "print", &yshell_print);

	return stack;
}

/**
 * Execute the command.
 * @param {P_YSHELL_COMMANDS}   stack       The commands.
 * @param {P_YSHELL_USER_INPUT} user_input  The user input.
 */
void exec_command(P_YSHELL_COMMANDS stack, P_YSHELL_USER_INPUT user_input) {
	P_YSHELL_COMMANDS curr = stack;
	while (curr != NULL) {
		if (strcmp(user_input->cmd_name, curr->cmd.name) == 0) {
			curr->cmd.action(user_input->cmd_params_len, user_input->cmd_params);
		}

		curr = curr->next;
	}
}

/**
 * Analyzes the input.
 * @param {char[]} input The input.
 * @return {P_YSHELL_USER_INPUT} The user input.
 */
P_YSHELL_USER_INPUT analyze_input(char input[]) {
	// remove newline character
	char *format = strtok(input, "\n");

	P_YSHELL_USER_INPUT user_input = (P_YSHELL_USER_INPUT) malloc(sizeof(YSHELL_USER_INPUT));

	// obtains the command name.
	char *token = strtok(format, " ");
	user_input->cmd_name = (char *) malloc(sizeof(char *));
	strcpy(user_input->cmd_name, token);

	// converts the letters of command name to lowercase.
	for (int i = 0; user_input->cmd_name[i]; i++)
		user_input->cmd_name[i] = (char) tolower(user_input->cmd_name[i]);

	// obtains the command parameters.
	user_input->cmd_params = (char **) malloc(2 * sizeof(char **));
	token = strtok(NULL, " ");

	size_t i = 0;
	while (token != NULL) {
		user_input->cmd_params[i] = (char *) malloc(sizeof(char *));
		strcpy(user_input->cmd_params[i], token);

		token = strtok(NULL, " ");
		i++;
	}

	user_input->cmd_params[i] = NULL;
	user_input->cmd_params_len = i;
	return user_input;
}

/**
 * Entry point.
 * @param {int}     argc    The argument count.
 * @param {char**}  argv    The argument vector.
 * @return {int} Returns the error code.
 */
int main(int argc, char **argv) {
	// initializes commands.
	const P_YSHELL_COMMANDS stack = init_cmd_stack();

	// initializes path.
	strcpy(g_path, "/");

	// obtains the input.
	const int INPUT_LENGTH = 255;

	char *result;
	do {
		printf("%s> ", g_path);

		// obtains the input.
		char *input = (char *) malloc(INPUT_LENGTH);
		result = fgets(input, INPUT_LENGTH, stdin);
		if (result != NULL) {
			// analyzes the input and execute the command.
			P_YSHELL_USER_INPUT user_input = analyze_input(input);
			exec_command(stack, user_input);
		}
	} while (result != NULL);

	return 0;
}