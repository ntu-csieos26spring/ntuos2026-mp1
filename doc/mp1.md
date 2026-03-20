<div align="center">
  <h1>💻 Machine Problem 1 - Thread Package</h1>
  <h3>CSIE3310 - Operating Systems</h3>
  <h4>National Taiwan University</h4>
</div>

<hr />

<div align="center">
  <table>
    <tr>
      <td><strong>Total Points:</strong></td>
      <td>100</td>
      <td><strong>Release Date:</strong></td>
      <td>March 9</td>
    </tr>
    <tr>
      <td><strong>Due Date:</strong></td>
      <td>March 23, 23:59:00 (UTC+8)</td>
      <td><strong>TA Hours:</strong></td>
      <td>Wed. 13:00-14:00 & Thu. 9:00-10:00 @CSIE R540 </td>
    </tr>
  </table>
</div>

<hr />

## 📋 Table of Contents


- [📋 Table of Contents](#-table-of-contents)
- [💬 Discussion Policy](#-discussion-policy)
- [📝 Summary](#-summary)
- [💻 Part 1 (40 points)](#-part-1-40-points)
  - [Function Description](#function-description)
  - [Sample Output](#sample-output)
- [💻 Part 2 (60 points)](#-part-2-60-points)
  - [Function Description](#function-description-1)
  - [Sample Output](#sample-output-1)
  - [Note](#note)
- [🧪Test Case Specification/Constraints](#test-case-specificationconstraints)
- [🧪 Run Public Test Cases](#-run-public-test-cases)
- [📤 Submission](#-submission)
- [📊 Grading Policy](#-grading-policy)
- [📂 Appendix](#-appendix)

---

## 💬 Discussion Policy

If you have any questions about this machine problem, please post them on the corresponding NTU COOL discussion board. We have opened a discussion dedicated to MP1. For special requests, you can email [ntuos@googlegroups.com](mailto:ntuos@googlegroups.com).

---

## 📝 Summary

In this MP, you'll try to implement a user-level thread package with the help of `setjmp` and `longjmp`. The threads explicitly yield when they no longer require CPU time. When a thread yields or exits, the next thread should run. The parent thread can send a signal to their children to kill them or trigger their signal handlers.

There are two parts in this MP. In the first part, you'll need to implement the following functions:

- `thread_add_runqueue`
- `thread_yield`
- `dispatch`
- `schedule`
- `thread_exit`
- `thread_start_threading`

In the second part, you'll need to implement the following functions:

- `thread_suspend`
- `thread_resume`
- `thread_kill`
- `thread_join`

The following functions have been implemented for you:

- `thread_create`
- `get_current_thread`
- `thread_register_handler`

Each thread should be represented by a `struct thread` which at least contains a function pointer to the thread's function and a pointer of type `void *` as the function parameters. The function of the thread will take the `void *` as its argument when executed. The struct should contain a pointer to its stack and two `jmp_buf` to store its current state when `thread_yield` is called. It should be enough to use only `setjmp` and `longjmp` to save and restore the context of a thread.

🛠️ Environment Setup

Before starting this machine problem, you must complete the initial environment setup. Please refer to the [Setup Guide](setup.md) for detailed installation steps, including Docker and repository initialization.


---

 You will use the skeleton of `threads.h` and `threads.c` provided in the `xv6/user` folder. Make sure you are familiar with the concept of stack frame and stack pointer taught in System Programming. It is also recommended to check out the appendix.

---

## 💻 Part 1 (40 points)

### Function Description

1. **`struct thread *thread_create(void (*f)(void *), void *arg)`**: Creates a new thread and allocates stack space for it. Note that the address of the stack pointer must be divisible by 8. The function returns the initialized structure.

2. **`void thread_add_runqueue(struct thread *t)`**: Adds an initialized `struct thread` to the runqueue. You'll need to maintain a circular linked list of `struct thread` using the `next` and `previous` fields, which always point to the next to-be-executed thread and the previously executed thread respectively. You should also maintain the static variable `struct thread *current_thread`. Note: insert the new thread at the end of the runqueue so that the newly inserted thread is `current_thread->previous`.

3. **`void thread_yield(void)`**: Suspends the current thread by saving its context to the `jmp_buf` in `struct thread` using `setjmp`. The `setjmp` in xv6 is provided; you only need to add `#include "user/setjmp.h"` to your code. After saving the context, call `schedule()` to determine which thread to run next and then call `dispatch()`. If the thread is resumed later, `thread_yield()` should return to the calling place in the function.

4. **`void dispatch(void)`**: Executes the thread decided by `schedule()`. For a thread that has never run before, initialize it by moving the stack pointer `sp` to the thread's allocated stack. The `sp` can be accessed and modified using `setjmp` and `longjmp` — see `setjmp.h` for where `sp` is stored in `jmp_buf`. For a previously executed thread, restoring context with `longjmp` is enough. If the thread's function returns, remove the thread from the runqueue and dispatch the next one by calling `thread_exit()`.

5. **`void schedule(void)`**: Decides which thread to run next by advancing `current_thread` to `current_thread->next` in the circular linked list.

6. **`void thread_exit(void)`**: Removes the calling thread from the runqueue, frees its stack and `struct thread`, updates `current_thread` to the next thread, and calls `dispatch()`. Consider what happens when the last thread exits — it should return to the main function by some means.

7. **`void thread_start_threading(void)`**: Called by the main function after the first thread is added to the runqueue. Should only return once all threads have exited.

### Sample Output

The output of `mp1-part1-0` should look like the following:

```
$ mp1-part1-0
mp1-part1-0
thread 1: 100
thread 2: 0
thread 3: 10000
thread 1: 101
thread 2: 1
thread 3: 10001
thread 1: 102
thread 2: 2
thread 3: 10002
thread 1: 103
thread 2: 3
thread 3: 10003
thread 1: 104
thread 2: 4
thread 3: 10004
thread 1: 105
thread 2: 5
thread 1: 106
thread 2: 6
thread 1: 107
thread 2: 7
thread 1: 108
thread 2: 8
thread 1: 109
thread 2: 9

exited
```

---

## 💻 Part 2 (60 points)

In this part, you need to implement additional functions related to signal generation and handling. Each thread can install different signal handlers for different signals. Child threads should inherit signal handlers from their parent when created. Note that threads have independent signal handlers — changing a signal handler for one thread does not affect others.

### Function Description

1. **`void thread_suspend(struct thread *t)`**: Mark thread `t` as suspended so scheduler will skip it. If `t == current_thread`, it suspends itself and must yield immediately.

   In order to complete this part, you need to modify:
   - **`void schedule(void)`**: Ensure suspended threads are not selected for execution.
   - **`void dispatch(void)`**: Before dispatching, ensure the selected thread is not suspended. If the next thread is suspended, continue searching for an active thread.

2. **`void thread_resume(struct thread *t)`**: Resumes execution of a suspended thread `*t`, making it schedulable again.

3. **`void thread_register_handler(int signo, void (*handler)(int))`**: Sets the signal handler `handler` on `current_thread` for signal `signo`. If a handler for that signal is already registered, replace it.

4. **`void thread_kill(struct thread *t, int signo)`**: Sends signal `signo` to thread `t` **without** triggering an immediate context switch. If `t` has a corresponding signal handler for `signo`, the handler will execute when `t` is next dispatched. Otherwise, `t` will be killed via `thread_exit()` at the start of t's next dispatch, before executing any of t's code.

   - If a thread returns from its signal handler, it should continue executing the original thread function from where it was interrupted.
   - This function only sends a signal and does not trigger any context switch.

   In order to complete this part, you also need to modify:
   - **`void thread_add_runqueue(struct thread *t)`**: Let the child thread `t` inherit signal handlers from `current_thread`.
   - **`void thread_yield(void)`**: Save context in different `jmp_buf`s depending on whether the thread is executing a signal handler or not. If called from the thread function, save context as in Part 1. If called from a signal handler, save to a separate `jmp_buf` to avoid discarding the thread function's context.
   - **`void dispatch(void)`**: If a signal has arrived, call the corresponding signal handler. If no handler is registered, kill `current_thread` via `thread_exit()`. Handle the case where a signal handler has never run before (initialization needed), and ensure that when a signal handler returns, the thread resumes its original function.

5. **`void thread_join(struct thread *t)`**: Causes the calling thread to block until thread `t` has exited. If `t` has already exited when `thread_join` is called, return immediately without blocking. To implement this:
   - Record `current_thread` as `t`'s waiter using the `waiter` field in `struct thread`.
   - Suspend the calling thread and call `thread_yield()` to give up the CPU.

   In order to complete this part, you also need to modify:
   - **`void thread_exit(void)`**: Before freeing the exiting thread, check if any thread is waiting on it (i.e., `waiter != NULL`). If so, resume the waiter by clearing its `join_blocked` flag.

### Sample Output

The output of `mp1-part2-0` should look like the following:

```
$ mp1-part2-0
mp1-part2-0
thread 1: 100
handler 3: 20
thread 1: 101
handler 3: 22
thread 1: 102
handler 3: 24
thread 3: 10000
thread 1: 103
thread 1: suspending
thread 3: 10001
thread 3: 10002
thread 3: 10003
thread 3: 10004
thread 3: 10005
thread 1: resuming
thread 1: 104
thread 1: 105
thread 4: 1000000
thread 4: 1000001
thread 4: 1000002
thread 4: 1000003
thread 1: 106
thread 1: 107
thread 1: 108

exited
```

The output of `mp1-part2-1` should look like the following:

```
$ mp1-part2-1
mp1-part2-1
thread 1: 100
thread 2: 0
thread 3: 10000
thread 4
thread 1: 101
handler 2: 1
thread 2: 1
thread 3: 10001
thread 1: 102
thread 2: 2
thread 3: 10002
thread 1: 103
thread 2: 3
thread 3: 10003
thread 1: 104
thread 2: 4
thread 2: suspending
handler 3: 21
thread 1: 105
handler 3: 23
thread 1: 106
handler 3: 25
thread 1: 107
handler 3: 27
thread 5: 500
thread 6: 600
handler 3: 29
thread 3: 10004
thread 5: 501
thread 6: 601
thread 1: 108
thread 1: 109
thread 1: 110
thread 1: 111
thread 2: resuming
thread 2: 5
thread 2: 6
thread 2: 7
thread 2: 8
thread 2: 9

exited
```

### Note

In the `mp1-part2-0` test case:

1. Because the signal handler for signal `0` has not been registered when `thread 2` is dispatched, `thread 2` should be killed directly.
2. Because `thread 3` inherited the signal handler for signal `1` from its parent, it should execute that signal handler when dispatched.
3. Because `thread 3` returns from its signal handler, it should continue executing its thread function.
4. `Thread 1` suspends itself, and after `thread 3` finishes its output, `thread 3` resumes `thread 1`.
5. After `thread 1` is resumed, `thread 4` (created by `thread 1`) runs and then `thread 1` continues until it exits.

In the `mp1-part2-1` test case:

1. `Thread 1`, `thread 2`, `thread 3`, and `thread 4` begin running in round-robin.
2. A signal is sent to `thread 2`; its handler executes when `thread 2` is next dispatched.
3. `Thread 2` suspends itself after printing `4`.
4. `Thread 3`'s signal handler runs repeatedly, interleaved with other threads.
5. New threads 5 and 6 are created and run. Once `thread 3` exits, `thread 2` is eventually resumed and finishes.
---
## 🧪Test Case Specification/Constraints

1. The main function creates exactly one thread, i.e, the root.
2. There are two types of signals: `0` and `1`.
3. Parameters of `thread_kill` are always legal — `struct thread *t` has not yet exited, and `signo` is `0` or `1`.
4. The `struct thread *t` parameter in `thread_kill` must be a child thread of `current_thread`.
5. **Signals will only be sent from a parent thread to its child threads. A parent thread will not send signals to the same child thread more than once.**
6. Only `thread_yield()` and `thread_exit()` will be called in signal handlers. Neither `thread_create()` nor `thread_add_runqueue()` will be called in signal handlers.
7. `thread_register_handler()` and `thread_kill()` will not be called from the main thread.
8. `thread_suspend()` / `thread_resume()` are only called with valid, live thread pointers. `thread_resume()` is only called when t is currently suspended.
9. `thread_join` may be called on any valid target thread pointer (not limited to direct child threads), including when the target has already exited.
10. `thread_join` will not be called from within a signal handler.
11. `thread_join` will never be called with `t == current_thread` (no self-join).
12. `thread_join` may be called on a suspended target thread. In this case, the caller remains blocked until that target thread is resumed and exits. `thread_join` returns only when the target thread exits (not when it is resumed).
13. Multiple threads may call `thread_join` on the same target thread.
---


## 🧪 Run Public Test Cases

If you want to run a specific test case:
```bash
./mp.sh qemu
$ mp1-part1-0
```

To run the automated public tests:

```bash
./mp.sh clean
./mp.sh grade
```

For more information on reading grades and logs, refer to the [Workflow Guide](workflow.md).

---

## 📤 Submission

Run `./mp.sh clean` before pushing. Do not commit build artifacts such as `.o`, `.d`, or `.asm` files.

You should only need to modify `user/threads.c` and `user/threads.h`. There is no need to change any other file in the repository.

> [!IMPORTANT]
> The English letters in `<student_id>` must be **lowercase**. E.g., use `r13944062` instead of `R13944062`.

---

## 📊 Grading Policy

**Part 1 (40%):**
- `mp1-part1-0` — 5%
- `mp1-part1-1` — 10%
- `mp1-part1-2` — 10%
- `mp1-part1-3` — 5%
- `mp1-part1-4` — 10%

**Part 2 (60%):**
- `mp1-part2-0` — 10%
- `mp1-part2-1` — 10%
- `mp1-part2-2` — 15%
- `mp1-part2-3` — 10%
- `mp1-part2-4` — 15%

**Penalties:**
- Late penalty: if submission is $n$ days late, score = $\max(\text{raw score} - 20 \times \lceil n \rceil,\ 0)$. No points are awarded if $\lceil n \rceil \geq 5$.
- Execution time limit is 240 seconds (may be extended if needed for correct programs).
- Only the **last submission** will be graded. Previous submissions are ignored.

---

## 📂 Appendix

1. [Function Pointer](https://en.wikipedia.org/wiki/Function_pointer)
2. [Call Stack](https://en.wikipedia.org/wiki/Call_stack)
