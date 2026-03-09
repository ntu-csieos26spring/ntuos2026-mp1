#include "kernel/types.h"
#include "user/user.h"
#include "user/threads.h"

#define NULL 0
static struct thread *g_t2 = 0;
void s3(int signo)
{
    int i = 10;
    while(1) {
        if(signo) printf("handler 3: %d\n", i*2);
        else printf("handler 3: %d\n", i*2+1);
        i++;
        if(i == 15) {
            return;
        }
        thread_yield();
    }
}

void s2(int signo)
{
    printf("handler 2: %d\n", signo);
}
void f5(void *arg)
{
    printf("thread 5: 500\n");
    thread_yield();
    printf("thread 5: 501\n");
    thread_exit();
}
void f6(void *arg)
{
    printf("thread 6: 600\n");
    thread_yield();
    printf("thread 6: 601\n");
    thread_exit();
}
void f4(void *arg)
{
    printf("thread 4\n");
    thread_exit();
}

void f3(void *arg)
{
    thread_register_handler(0, s3);
    thread_register_handler(1, s3);

    int i = 10000;
    while (1) {
        printf("thread 3: %d\n", i++);
        if(i == 10005) {
            thread_exit();
        }
        thread_yield();
    }
}

void f2(void *arg)
{
    thread_register_handler(0, s2);
    thread_register_handler(1, s2);

    int i = 0;
    while(1) {
        printf("thread 2: %d\n",i++);
        if(i==5){
            // Suspend this thread (g_t1) at i=103
            g_t2 = get_current_thread();
            printf("thread %d: suspending\n", g_t2->ID);             // "thread 1: suspending"
            thread_suspend(g_t2);  // g_t1 points to the same thread
        }
        if (i == 10) {
            thread_exit();
        }
        thread_yield();
    }
}

void f1(void *arg)
{
    int i = 100;
    struct thread *t2 = thread_create(f2, NULL);
    thread_add_runqueue(t2);
    struct thread *t3 = thread_create(f3, NULL);
    thread_add_runqueue(t3);
    struct thread *t4 = thread_create(f4, NULL);
    thread_add_runqueue(t4);
    struct thread *t5 = thread_create(f5, NULL);
    struct thread *t6 = thread_create(f6, NULL);
    
    
    while(1) {
        printf("thread 1: %d\n", i++);
        if (i == 102) {
            thread_kill(t2, 1);
        }
        if (i == 105) {
            thread_kill(t3, 0);
        }
        if (i == 106) {
            thread_join(t4); //test join existed
        }
        if (i == 108) {
            thread_add_runqueue(t5);
            thread_add_runqueue(t6);
            thread_join(t5);
            thread_join(t6);
        }
        if (i == 112) {
            if (g_t2) {
                printf("thread %d: resuming\n", g_t2->ID);             // Print "thread 1: resuming"
                thread_resume(g_t2);  // Resume thread 1
            }
            thread_exit();
        }
        thread_yield();
    }
}

int main(int argc, char **argv)
{
    printf("mp1-part2-1\n");
    struct thread *t1 = thread_create(f1, NULL);
    thread_add_runqueue(t1);
    thread_start_threading();
    printf("\nexited\n");
    exit(0);
}