// this is inlinely dropped into compilation unit...
// BAAD practice

char *bash_string = NULL;
short bash_process_status = 1;

void bash_process(Process process){
	(void) process;
	if(bash_process_status){
		if(!bash_string){
			setupCommands();
			bash_string = malloc(STRSIZE);
			for(short i = 0; i < STRSIZE; i++){
				bash_string[i] = 0;
			}
		}
		setStr(bash_string, terminal_inputLine());
		bashRun(bash_string);
	}
}
