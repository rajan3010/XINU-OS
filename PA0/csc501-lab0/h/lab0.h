extern int IS_SYSCALL_TRACE;

//extern struct pentry *currpptr;

extern void syscall_count_timer_start(struct pentry *process, int syscallid);

extern void syscall_end_timer(struct pentry *process, int syscallid);