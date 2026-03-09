#include "kernel/types.h"
#include "user/setjmp.h"
#include "user/threads.h"
#include "user/user.h"
#define NULL 0

static struct thread* current_thread = NULL;
static int id = 1;

//the below 2 jmp buffer will be used for main function and thread context switching
static jmp_buf env_st;
static jmp_buf env_tmp;

struct thread *get_current_thread() {
    return current_thread;
}

struct thread *thread_create(void (*f)(void *), void *arg){
    struct thread *t = (struct thread*) malloc(sizeof(struct thread));
    unsigned long new_stack_p;
    unsigned long new_stack;
    
    new_stack = (unsigned long) malloc(sizeof(unsigned long)*0x100);
    new_stack_p = new_stack + 0x100*8 - 0x2*8;
    
    t->fp = f;
    t->arg = arg;
    t->ID  = id;
    t->buf_set = 0;
    t->stack = (void*) new_stack;  //points to the beginning of allocated stack memory for the thread.
    t->stack_p = (void*) new_stack_p;  //points to the current execution part of the thread.
    id++;
    // part 2
    t->sig_handler[0] = NULL_FUNC;
    t->sig_handler[1] = NULL_FUNC;
    t->signo = -1;
    t->handler_buf_set = 0;
    
    t->suspended = 0;      
    t->join_blocked = 0;   
    
    t->exited = 0;      
    t->waiter_list = NULL; 
    t->next_waiter = NULL; 
    return t;
}

void thread_add_runqueue(struct thread *t){
    if(current_thread == NULL){
        //TODO
    }
    else{
        //TODO
    }
}

void thread_yield(void){
    //TODO
}

void dispatch(void){
    //TO DO   
}

//schedule will follow the rule of FIFO
void schedule(void){
    current_thread = current_thread->next;
    
    //TODO
    while(current_thread->suspended || current_thread->join_blocked) {
    };
}

void thread_exit(void) {
    struct thread *w = current_thread->waiter_list;
    struct thread *ptr = current_thread;
    //TODO (Part2)

    if (current_thread->next != current_thread) {
       //TODO
    } else {
        //TODO
    }
}

void thread_start_threading(void){
    //TODO
}

void thread_register_handler(int signo, void (*handler)(int)){
    current_thread->sig_handler[signo] = handler;
}

void thread_kill(struct thread *t, int signo){
    //TODO
}

void thread_suspend(struct thread *t) {
    //TODO
}

void thread_resume(struct thread *t) {
    //TODO
}

void thread_join(struct thread *t) {
    //TODO
}