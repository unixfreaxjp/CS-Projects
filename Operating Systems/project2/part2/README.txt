The sys call function in the the LKM source (getprinfo_LKM.c) is sys_getprinfo. The function has a prinfo struct, called kinfo. Most of the fields are added from the current task_struct. I got the UID using current_uid() as in part 1. for finding the old/young children and siblings, I use list_for_each_entry to search through and pick the most appropriate one. -1 is returned if a sibling/child was not found. I assumed that the start time since boot meant the current->real_start_time. The function prints out a syslog message before copying kinfo back to tghe user and then returns 0.

test program:

The test program spawns a bunch of process by forking in two nested loops. Each process waits for keyboard input before making the getprinfo system call (so the information can be printed in order). It is simplest to just hold doun [Enter] until the program completes. Each block of output is tabbed according to the its level in the heirarchy for readability.

The test output file is testoutput.text. It contains a command line log of me loading the module, running the test program, and printing out the system log afterwards.
