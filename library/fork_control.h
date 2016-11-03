/* library/fork_control.h						*/
/* -------------------------------------------------------------------- */
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef FORK_CONTROL_H
#define FORK_CONTROL_H

#include <unistd.h>
#include "list.h"
#include "boolean.h"

/* Constants */
/* --------- */

/* Enumerated types */
/* ---------------- */
enum fork_control_state {	fork_control_initial,
				fork_control_executing,
				fork_control_done };

/* Type definitions */
/* ---------------- */
typedef struct
{
	char *command_line;
	enum fork_control_state state;
	pid_t running_process_id;
} FORK_CONTROL_PROCESS;

typedef struct
{
	LIST *process_list;
	int processes_in_parallel;
} FORK_CONTROL;

/* Prototypes */
/* ---------- */

FORK_CONTROL *fork_control_new( void );

FORK_CONTROL_PROCESS *fork_control_process_new(
				char *command_line );

void fork_control_append_process(
				LIST *process_list,
				char *command_line );

void fork_control_execute(
				LIST *process_list,
				int processes_in_parallel );

FORK_CONTROL_PROCESS *fork_control_get_next_process(
				LIST *process_list );

void fork_control_execute_process(
				enum fork_control_state *state,
				pid_t *running_process_id,
				char *command_line );

FORK_CONTROL_PROCESS *fork_control_process_seek(
				LIST *process_list,
				pid_t process_id );

boolean fork_control_get_any_processes_pending(
				LIST *process_list );

void fork_control_process_list_display(
				LIST *process_list );

#endif
