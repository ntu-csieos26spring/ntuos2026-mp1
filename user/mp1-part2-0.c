#include "kernel/types.h"
#include "user/user.h"
#include "user/threads.h"

// Use a global pointer so both f1() and f3() can reference t1.
static struct thread *g_t1 = 0;

#define NULL 0

// Handler for thread 3 (called when thread_kill(t3, 1))
void s3(int signo)
{
    int i = 10;
    while(1) {
        if(signo)
            printf("handler 3: %d\n", i*2);  // e.g., 20, 22, 24
        else
            printf("handler 3: %d\n", i*2+1);

        i++;
        if(i == 13) {
            // After printing three times, leave the handler
            return;
        }
        thread_yield();
    }
}

// Handler for thread 2 (for demonstration)
void s2(int signo)
{
    printf("handler 2: %d\n", signo);
}

void f4(void *arg)
{
    int i = 1000000;
    while(1){
        printf("thread 4: %d\n", i++);
        if(i == 1000004){
            thread_exit();
        }
        thread_yield();
    }
    
}
// Thread 3 function: prints from 10000 up to 10005, then resumes thread 1
void f3(void *arg)
{
    int i = 10000;
    while(1) {
        printf("thread 3: %d\n", i++);
        if(i == 10006) {
            // Here we resume thread 1 before exiting
            if (g_t1) {
                printf("thread %d: resuming\n", g_t1->ID);             // Print "thread 1: resuming"
                thread_resume(g_t1);  // Resume thread 1
            }
            thread_exit();
        }
        thread_yield();
    }
}

// Thread 2 function: prints from 0 up to 9
void f2(void *arg)
{
    // Register a signal handler for demonstration (signal=1)
    thread_register_handler(1, s2);

    int i = 0;
    while(1) {
        printf("thread 2: %d\n", i++);
        if(i == 10) {
            thread_exit();
        }
        thread_yield();
    }
}

// Thread 1 function: prints from 100 to 105, suspends itself at i==103
void f1(void *arg)
{
    int i = 100;
    // int id=1;

    // Register thread 1's handler for signal=1
    thread_register_handler(1, s3);

    // Create and add thread 2, thread 3, thread4
    struct thread *t2 = thread_create(f2, NULL);
    thread_add_runqueue(t2);

    struct thread *t3 = thread_create(f3, NULL);
    thread_add_runqueue(t3);

    struct thread *t4 = thread_create(f4, NULL);

    // Send signal=0 to t2 and signal=1 to t3 (trigger their handlers)
    thread_kill(t2, 0);
    thread_kill(t3, 1);

    while(1) {
        printf("thread 1: %d\n", i);
        if(i == 103){
            // Suspend this thread (g_t1) at i=103
            
            g_t1 = get_current_thread();
            printf("thread %d: suspending\n", g_t1->ID);            
            thread_suspend(g_t1);  // g_t1 points to the same thread
        }
        if (i == 105){
            thread_add_runqueue(t4);
            thread_join(t4);
        }
        if (i == 108) {
            thread_exit();
        }
        i++;
        thread_yield();
    }
}

int main(int argc, char **argv)
{
    printf("mp1-part2-0\n");

    // Create thread 1, store pointer in the global variable g_t1
    g_t1 = thread_create(f1, NULL);

    // Add g_t1 to the run queue and start threading
    thread_add_runqueue(g_t1);
    thread_start_threading();

    printf("\nexited\n");
    exit(0);
}