######################################################
CS 377 - Lab Assignment 6

2 March 2015
Prof D. M. Dhamdhere

By 
Bhargav Chippada 120050053
Ranveer Aggarwal 120050020
######################################################
Question 1) Function calls:

Inside void Hardware_Shutdown():
Out_Byte :- io.h
	Write a byte to an I/O port.

Inside void Main(struct Boot_Info *bootInfo):

Init_BSS() :- mem.h
	Initialize the .bss section of the kernel executable image.
Init_Screen() :- screen.h
	Initialize the screen module
Init_Mem(bootInfo) :- mem.h
	Initialize memory management data structures.
	Enables the use of Alloc_Page() and Free_Page() functions.
Init_CRC32() :- crc32.h
	This routine writes each crc_table entry exactly once,
	with the correct final value.  Thus, it is safe to call
	even on a table that someone else is using concurrently.
Init_TSS() :- tss.h
	Initialize the kernel TSS (Task state segment).  This must be done after the memory and
	GDT(global descriptor table) initialization, but before the scheduler is started.
lockKernel() :- smp.h
	Implement coarse grained kernel locks.  The functions lockKernel and unlockKernel will be called whenever
 	interrupts are enabled or disabled.  This includes in interrupt and iret.
Init_Interrupts(0) :- int.h
	Initialize the interrupt system.
Init_SMP() :- smp.h
	Initialize  SMP (symmetric multiprocessing).
TODO_P(PROJECT_VIRTUAL_MEMORY_A, "initialize virtual memory page tables.") :- projects.h
	PROJECT_VIRTUAL_MEMORY_A, PROJECT_VERBOSITY are defined to be false inside projects.h
	TODO_P will print the message according to the above flags.
	TODO_P internally calls TODO(X) which is defined globally as Print(X).
	This method shouldn't be called with its flag as true unless the corresponding method is initialized.
Init_Scheduler(0, (void *)KERN_STACK) :- kthread.h
	Initialize the scheduler. It creates initial kernel thread context object and stack,
    and make them current. It then creates the idle thread and reaper thread.
Init_Traps() :- trap.h
	Initialize handlers for processor traps.
Init_Local_APIC(0) :- smp.h
	Setup local APIC(Advanced Programmable Interrupt Controller) including calibrating its timer register.
Init_Timer() :- timer.h
	Reprogram the timer to set the frequency. In bochs, it defaults to 18Hz, which is actually pretty reasonable.
Init_Keyboard() :- keyboard.h
	Initialize keyboard.
Init_DMA() :- dma.h
	Initialize the DMA controllers.
Init_IDE() :- ide.h
	Initialize IDE controller.
Init_PFAT() :- pfat.h
	Register pfat filesystem.
Init_GFS2() :- gsf2.h
	Register gsf2 (Global File System 2) filesystem.
Init_GOSFS() :- gosfs.h
	Register gosfs filesystem.
Init_CFS() :- cfs.h
	Register cfs file system.
Init_Alarm() :- alarm.h
	Initialize alarm.
Release_SMP() :- smp.h
	Release SMP (symmetric multiprocessing).

/* Initialize Networking */
Init_Network_Devices() :- net.h
	Initialize network devices. 
Init_ARP_Protocol() :- arp.h
	Initialize arp protocol variables.
Init_IP() :- ip.h
	Initialize IP(internet protocol).
Init_Routing() :- routing.h
	Initialize the routing table with all local routes.
Init_Sockets() :- socket.h
	Initialize socket.
Init_RIP() :- rip.h
	PROJECT_RIP is defined as false in projects.h
	calls TODO_P(PROJECT_RIP, "start threads, setup state, set alarm to timeout routes");

/* End networking subsystem init */

/* Initialize Sound */
Init_Sound_Devices() :- sound.h
	PROJECT_SOUND is defined to be false inside projects.h
	calls TODO_P(PROJECT_SOUND, "Initialize sound card");
/* End sound init */


Mount_Root_Filesystem() :- main.c
	Try mounting root device.
Set_Current_Attr(ATTRIB(BLACK, GREEN | BRIGHT)) :- screen.h
	Set the current character attribute.
Print("Welcome to GeekOS!\n") :- screen.h
	It prints the message.
Set_Current_Attr(ATTRIB(BLACK, GRAY)) :- screen.h
	Set the current character attribute.
Spawn_Init_Process() :- main.c
	Try spawning initial program.


/* it's time to shutdown the system */
Hardware_Shutdown() :- main.c
	Shutdown hardware. 

Inside static void Mount_Root_Filesystem(void):

Mount(ROOT_DEVICE, ROOT_PREFIX, "pfat") :- vfs.h
	* Mount a filesystem on a block device.
		* Params:
			* devname - block device name containing filesystem image
			* pathPrefix - where to mount the filesystem in the overall namespace
			* fstype - filesystem type, e.g. "pfat", "gosfs"
			* Returns: 0 if successful, error code (< 0) if not

Inside static void Spawn_Init_Process(void):
	
Spawn_Foreground(INIT_PROGRAM, INIT_PROGRAM, &initProcess):- user.h
	returns Spawn(program, command, pThread, false);
	 * Spawn a user process.
	 * Params:
	 *   program - the full path of the program executable file
	 *   command - the command, including name of program and arguments
	 *   pThread - reference to Kernel_Thread pointer where a pointer to
	 *     the newly created user mode thread (process) should be
	 *     stored
	 * Returns:
	 *   Zero or an error code
	 *   if the process couldn't be created.  Note that this function
	 *   should return ENOTFOUND if the reason for failure is that
	 *   the executable file doesn't exist.
Join(initProcess) :- kthread.h
	Wait for given thread to die.
	Interrupts must be enabled.
	Returns the thread exit code.

extern void checkPaging(void) : tss.h (but this function is not called in main.c)
	checkPaging function here, in a file that should not be
	modified, so that test code can rely on it being unmodified.
######################################################