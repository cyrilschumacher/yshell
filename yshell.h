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

#ifndef YSHELL_YSHELL_H
#define YSHELL_YSHELL_H

#define ERROR       1
#define SUCCESS     0

/**
 * Path.
 */
char g_path[255];

/**
 * @summary Command action.
 * @param {int} argc The parameters count.
 * @param {void*} argv The parameters.
 * @return {int} Returns the error code.
 */
typedef int (*yShellCommandAction)(int, void *);

/**
 * yShell command.
 */
typedef struct _YSHELL_COMMAND {
	/**
	 * Command name.
	 * @type {char*}
	 */
	char *name;

	/**
	 * Action.
	 * @type {yShellCommandAction}
	 */
	yShellCommandAction action;
} YSHELL_COMMAND, *P_YSHELL_COMMAND;

/**
 * yShell commands.
 */
typedef struct _YSHELL_COMMANDS {
	YSHELL_COMMAND cmd;
	struct _YSHELL_COMMANDS *next;
} YSHELL_COMMANDS, *P_YSHELL_COMMANDS;

/**
 * yShell user input.
 */
typedef struct _YSHELL_USER_INPUT {
	/**
	 * Command name.
	 * @type {char*}
	 */
	char *cmd_name;

	/**
	 * Command parameters.
	 * @type {char**}
	 */
	char **cmd_params;

	/**
	 * Command parameters length.
	 * @type {size_t}
	 */
	size_t cmd_params_len;
} YSHELL_USER_INPUT, *P_YSHELL_USER_INPUT;

#endif
