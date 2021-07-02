#ifndef FILESYSTEM
#include "fs.c"
#endif

#define STACKSIZE 4096

// unused currently
typedef struct Hook {
	char hook_name[16];
	void (* callback)();
} Hook;

typedef struct Process {
	short pid;
	void *stack;
	void (* callback)(struct Process);
} Process;

static int eqProcess(Process a, Process b){
	return a.pid == b.pid && a.stack == b.stack && a.callback == b.callback;
}

#define equatableProcess
defineList(Process)

List(Process) processTable = NULL;
short currentPid = 0;

short new_process(void (* fnptr)(Process)){
	Process next = {
		pid: listLen(Process)(processTable) + 1,
		stack: malloc(STACKSIZE),
		callback: fnptr
	};
	if(processTable == NULL){
		processTable = allocate_list(Process);
		processTable->value = next;
		processTable->next = NULL;
	} else {
		append(Process)(processTable, next);
	}
	// this should also prepare the stack and get a PID of the new process
	return next.pid;
}

void core_process(Process process){
	(void) process;
	// this process should handle all stuff related to system services
}

int is_there_hook(){
	// TO BE DONE
	return 0;
}

#include "processes.c"

int init(){
	terminal_cat("Init started - looping forever\n");
	new_process(&core_process);
	new_process(&bash_process);
	while(1){
		if(is_there_hook()){
			// hook callbacks WIP
		} else {
			currentPid = processTable->value.pid;
			processTable->value.callback(processTable->value);
			if(processTable->next != NULL){
				List(Process) head = processTable;
				List(Process) last = processTable;
				while(last->next != NULL){
					last = last->next;
				}
				processTable = processTable->next;
				last->next = head;
				head->next = NULL;
			}
		}
	}
	die("Init loop died");
	return 1;
}
