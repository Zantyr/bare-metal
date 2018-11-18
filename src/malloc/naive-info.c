void log_memory(){
	terminal_cat("Current malloc pointer: ");
	terminal_cat(itoa((int) master_pointer));
	terminal_cat("\n");
}

void malloc_info(){
	terminal_cat("No memory management enabled\n");
}