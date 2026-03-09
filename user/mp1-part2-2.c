#include "kernel/types.h"
#include "user/user.h"
#include "user/threads.h"

#define NULL 0
static struct thread *g_t3 = 0;
static struct thread *g_t7 = 0;
static struct thread *t6 = 0;
static struct thread *t9 = NULL;
void f9_target(void *arg) {
    for (int i = 0; i < 3; i++) {
        printf("thread 9: %d\n", i + 900);
        thread_yield();
    }
    thread_exit();
}

void f_waiter(void *arg) {
    int id = (int)(unsigned long)arg;
    printf("thread %d: joining thread 9\n", id);
    
    thread_join(t9); 
    
    printf("thread %d: woke up!\n", id);
    thread_exit();
}

void s5(int signo)
{
    int i = 5;
    while(1) {
        if(signo) printf("handler 5: %d\n", i*5);
        else printf("handler 5: %d\n", i*5+1);
        i++;
        if(i == 8) {
            thread_exit();
        }
        thread_yield();
    }
}

void s4(int signo)
{
    printf("handler 4: %d\n", signo);
}


void s3(int signo)
{
    printf("handler 3: %d\n", signo);
}

void s2(int signo)
{   
    int i = 22;
    while(1) {
        if(signo) printf("handler 2: %d\n", i*2);
        else printf("handler 2: %d\n", i*2+1);
        i++;
        if(i == 26) {
            return;
        }
        thread_yield();
    }
}
void f8(void *arg)
{
    int i = 8000;
    while(1) {
        printf("thread 8: %d\n", i++);
        if (i == 8002) {
            if (g_t7) {
                printf("thread 7: resuming\n");             // Print "thread 3: resuming"
                thread_resume(g_t7);  // Resume thread 3
            }
            thread_exit();
        }
        thread_yield();
    }
}
void f7(void *arg)
{
    int i = 700;
    while(1) {
        printf("thread 7: %d\n", i++);
        if (i == 701) {
            g_t7 = get_current_thread();
            printf("thread 7: suspending\n"); 
            thread_suspend(g_t7);
        }
        if (i == 704){
            thread_exit();
        }
        thread_yield();
    }
}
void f6(void *arg) {
    int i = 60;
    while(1) {
        printf("thread 6: %d\n", i++);
        if (i == 70) {
            thread_exit();
        }
        thread_yield();
    }
}
void f5(void *arg)
{
    thread_register_handler(1, s5);

    int i = 10;
    while(1) {
        printf("thread 5: %d\n", i++);
        if (i == 17) {
            if (g_t3) {
                printf("thread %d: resuming\n", g_t3->ID);             // Print "thread 3: resuming"
                thread_resume(g_t3);  // Resume thread 3
            }
            thread_exit();
        }
        thread_yield();
    }
}

void f4(void *arg)
{
    thread_register_handler(0, s4);

    int i = 1000;
    while(1) {
        printf("thread 4: %d\n", i++);
        if (i == 1011) {
            thread_exit();
        }
        thread_yield();
    }
}

void f3(void *arg)
{
    thread_register_handler(0, s3);

    int i = 10000;

    struct thread *t4 = thread_create(f4, NULL);
    thread_add_runqueue(t4);
    struct thread *t5 = thread_create(f5, NULL);
    thread_add_runqueue(t5);
    t6 = thread_create(f6, NULL);
    thread_add_runqueue(t6);
    
    while(1) {
        printf("thread 3: %d\n", i++);
        if (i == 10003) {
            g_t3 = get_current_thread();
            printf("thread %d: suspending\n", g_t3->ID);             // "thread 3: suspending"
            thread_suspend(g_t3);
            thread_kill(t4, 1);
        }
        if (i == 10004) {
            thread_kill(t5, 1);
        }
        if (i == 10005) {
            thread_exit();
        }
        thread_yield();
    }
}

void f2(void *arg)
{
    int i = 0;
    while(1) {
        printf("thread 2: %d\n", i++);
        if (i == 4) {
            thread_exit();
        }
        thread_yield();
    }
}

void f1(void *arg)
{
    thread_register_handler(0, s2);
    int i = 100;

    struct thread *t2 = thread_create(f2, NULL);
    thread_add_runqueue(t2);
    struct thread *t3 = thread_create(f3, NULL);
    thread_add_runqueue(t3);
    
    struct thread *t7 = thread_create(f7, NULL);
    struct thread *t8 = thread_create(f8, NULL);
    t9 = thread_create(f9_target, NULL);
    struct thread *t10 = thread_create(f_waiter, (void*)10);
    struct thread *t11 = thread_create(f_waiter, (void*)11);

    while(1) {
        printf("thread 1: %d\n", i++);
        if (i == 101) {
            thread_kill(t2, 0);
        }
        if (i == 104) {
            thread_kill(t3, 0);
            thread_join(t6);
        }
        if (i == 109){
            thread_add_runqueue(t7);
            thread_add_runqueue(t8);
            thread_join(t7);
        }
        if (i == 111) {
            thread_add_runqueue(t9);
            thread_add_runqueue(t10);
            thread_add_runqueue(t11);
            printf("thread 1: joining thread 9\n");
            thread_join(t9);
            printf("thread 1: woke up!\n");
        }
        if (i == 113) {
            thread_exit();
        }
        thread_yield();
    }
}

int main(int argc, char **argv)
{
    printf("mp1-part2-2\n");
    struct thread *t1 = thread_create(f1, NULL);
    thread_add_runqueue(t1);
    thread_start_threading();
    printf("\nexited\n");
    exit(0);
}