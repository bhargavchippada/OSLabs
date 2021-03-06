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
Question 2)
Kernel_Thread structure:
1) ulong_t esp :- stack pointer

2) volatile ulong_t numTicks :- used by the scheduler to implement timer based preemption

3) volatile ulong_t totalTime :- total time the thread spent on cpu

4) int priority :- priority for scheduling purpose

5) DEFINE_LINK(Thread_Queue, Kernel_Thread) :- defines previous and next fields used when a kernel thread is on a
thread queue

6) void *stackPage :- points to the kernel thread's stack page

7) struct User_Context *userContext :- if non-null, points to the thread's user context, which is essentially a combined code and data segment allowing the thread to execute a user mode program

8) struct Kernel_Thread *owner:- pointer to the parent thread

9) int affinity :- prefered core = AFFINITY_ANY_CORE -> can run on any core

10) int refCount :- The thread has an implicit self-reference. If the thread is not detached, then its owner also has a reference to it so Wait will work. refCount = detached ? 1 : 2;

11) int detached :- detached indicates whether the parent thread will wait for the child thread to exit. If true, then the parent must call the Join() function to wait for the child. If false, then the parent must not call Join().

12) int pid :- The kernel thread id; also used as process id

/* These fields are used to implement the Join() function */

13) bool alive :- whether or not this thread is dead

14) struct Thread_Queue joinQueue :- queue of the threads to be exited before this

15) int exitCode :- exit code of this thread
/**********************************************************/

16) DEFINE_LINK(All_Thread_List, Kernel_Thread) :- Link fields for list of all threads in the system

17)	#define MAX_TLOCAL_KEYS 128 :- max number of temporary local keys for this thread

18) const void *tlocalData[MAX_TLOCAL_KEYS] :- Array of MAX_TLOCAL_KEYS pointers to thread-local data

19) char threadName[20] :- name of the thread program


2nd Part:
We have written a user program called "check.c" which is spawned as a kernel thread by shell.c before executing it's shell terminal code. check.c thread takes an input and checks it with the password from the Password.txt file and exits with an appropriate code. shell.c exits if exitCode of check.c is not zero else it will execute the rest of its body.

######################################################
Question 3)
In GeekOS, there is just one struct that is common to all kinds of threads. That struct is "Kernel_Thread".
The way you distinguish between a User thread and a Kernel thread is by its fields. Some fields are applicable for User threads whereas some fields are necessary only for Kernel threads. As an example, we take the priority field. A Kernel thread is managed by the process scheduler and so, it carries a priority value. Whereas, a User thread doesn't need a priority value since it doesn't need to be scheduled; it's managed inside the process context itself. A User Thread carries a user context whereas a Kernel thread doesn't. User context is a user mode context which can be attached to a Kernel_Thread, to allow it to execute in user mode. This struct has all information needed to create and manage a user memory space, as well as other kernel resources used by the process (such as semaphores and files).
######################################################
Question 4)
In question 2 the check.c main body calls the function Read_Line which in turn calls the function Get_Key() which is a wrapper function for the syscall SYS_GETKEY, Sys_GetKey calls the function Wait_For_Key() which waits for a keycode to arrive, it uses the keyboard wait queue to sleep until a keycode arrives. When a syscall is made by the thread then it is put to sleep in the wait queue and the scheduler is invoked to run another thread. When an I/O interrupt occurs later the interrupt handler will process it and select the thread which was waiting for it and change its state accordingly, now scheduler thread is invoked again.
######################################################
Question 5)
Fork should do the following:
	a) Clone the calling process, creating an exact copy.
	b) Return -1 for errors, 0 to the new process, and the process ID of the new process to the old process.
Create a wrapper function called Fork() for the syscall SYS_FORK.
Inside the Sys_Fork method, create a new user thread which is a copy of the current_thread kernel_thread data structure and add it to ready threadQueue.
The only difference in the code of the parent program and its fork child will be the return value as described above, their code segment is changed according. 
For the new child thread initialize its kernel_thread structure as follows:
1) ulong_t esp :- pointer to the same place in the copied stack of its parent

2) volatile ulong_t numTicks :- 0

3) volatile ulong_t totalTime :- 0
4) int priority :- priority of parent

5) DEFINE_LINK(Thread_Queue, Kernel_Thread) :- defines previous and next fields used when a kernel thread is on a
thread queue

6) void *stackPage :- points to the copied kernel thread's stack page

7) struct User_Context *userContext :- null

8) struct Kernel_Thread *owner:- pointer to the parent thread

9) int affinity :- affinity of parent

10) int refCount :- 2

11) int detached :- 1

12) int pid :- new pid of this child thread

/* These fields are used to implement the Join() function */

13) bool alive :- true

14) struct Thread_Queue joinQueue :- queue of the threads to be exited before this

15) int exitCode :- exit code of this thread
/**********************************************************/

16) DEFINE_LINK(All_Thread_List, Kernel_Thread) :- Link fields for list of all threads in the system

17)	#define MAX_TLOCAL_KEYS 128 :- max number of temporary local keys for this thread

18) const void *tlocalData[MAX_TLOCAL_KEYS] :- pointer to the copy of tlocalData array of its parent.

19) char threadName[20] :- name of the thread program
######################################################