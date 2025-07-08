## Lite Thread Kernel
Lite Thread is a lightweight framework designed to manage task execution in embedded systems on microcontrollers. It is architected based on the Active Object design pattern, offering several key advantages:

- Low code size
- Modular architecture, independent of any specific microcontroller driver library
- High portability, enabling rapid application development across various microcontroller families

Lite Thread is intended to operate in bare-metal (non-RTOS) environments, where it provides an event-driven service layer that sits above the hardware library layer, facilitating clean separation between application logic and low-level hardware operations.

In this model, **Lite Thread** is built to work with a bare-metal project (non-OS), with an event-driven service layer located above the **hardware service** layer.
<div style="text-align: center;">
    <img src="images/event-driven-service.png" width="300"/>
</div>

To approach this model, learners need to have basic knowledge of data structures: Linked list, queue, pool memory, OOP in C. This commit will include the following contents:
- Pool memory
- Post & handling messages mechanisms
- Timer list

### 1. Active Object Design Pattern
<div style="text-align: center;">
    <img src="images/active-object-model.png" width="450"/>
</div>

Active objects (a.k.a. actors) are event-driven, strictly encapsulated software objects endowed with their
own threads of control that communicate with one another asynchronously by exchanging events. The
UML specification further proposes the UML variant of hierarchical state machines (UML statecharts )
with which to model the behavior of event-driven active objects.
Active objects inherently support and automatically enforce the following best practices of concurrent
programming:
- Keep all of the thread's data encapsulated and local, bound to the task itself and hidden from the
rest of the system.
- Communicate among threads asynchronously via intermediary event objects. Using asynchronous
event posting keeps the tasks running truly independently without blocking on each other.
- Threads should spend their lifetime responding to incoming events, so their mainline should consist of
an event loop.
- Threads should process events one at a time (run to completion), thus avoiding any concurrency
hazards within a thread itself.

### 2. Task - Signal Concept
**Event queue**
- The task scheduling model in Lite Thread is based on a priority-driven mechanism, supporting up to 8 distinct priority levels, ranging from lowest to highest. Each priority level is associated with a dedicated Active Object.

- Every task control block (i.e., Active Object) maintains its own message queue to receive and process events independently. In cases where multiple tasks share the same priority level, they are managed collectively under a single Active Object instance.

**Task priority mask**
<div style="text-align: center;">
    <img src="images/active-object-priority-mask.png" width="350"/>
</div>

- When a message is published to the publish-subscribe event bus, the kernel determines which Active Object(s) are subscribed to that message. It then sets the corresponding priority mask bit(s) to "on", indicating that those Active Objects are ready for execution.
- Once an Active Object has processed all of its pending messages, its corresponding priority mask bit is cleared (set to "off"), indicating that it is no longer ready to run.

Once an Active Object has processed all of its pending messages, its corresponding priority mask bit is cleared (set to "off"), indicating that it is no longer ready to run.

**Event loop**
<div style="text-align: center;">
    <img src="images/scheduler.png" width="1000"/>
</div>
The Lite Thread scheduler operates based on the following concept:

- **Step 1**: Upon entering the event loop, the kernel checks the message queues of all Active Objects to determine whether there are any pending messages to process.
- **Step 2**: If no messages are available, the kernel executes all registered Polling Objects — these are simple, sequential code blocks typically equivalent to logic found in traditional main loops. Once polling is complete, the scheduler returns to Step 1.
- **Step 3**: If there are pending messages, the scheduler prioritizes handling them over executing Polling Objects. It scans the priority mask to identify the highest-priority Active Object that is ready. The kernel then retrieves the next message from that Active Object’s queue and dispatches it to the corresponding task for execution.
Each message contains an execution signal and may optionally include application-specific data, depending on the use case.
After the message is processed, the system returns to Step 1 to continue checking for new messages.

Message handling follows a **Run-to-Complete** (RTC) model — meaning that once a message begins execution, it runs to completion without being blocked or preempted by other events (i.e., fully non-blocking).

### 3. Timer service
Timers in event-driven systems operate on the following principle:
- When a timer node is installed, it is added to the timer list, which is implemented as a linked list structure.
- A dedicated timer task, managed by an Active Object, is responsible for continuously traversing this list at runtime. - It checks each timer node to determine whether its expiration time has been reached. If a timer has expired, the timer task will dispatch a message to the corresponding task for execution.
- This timer task runs continuously during runtime, ensuring timely delivery of timer-based events throughout the system.

<div style="text-align: center;">
    <img src="images/timer-task.png" width="600"/>
</div>

- The timer heartbeat is handled by a real timer on the microcontroller (system tick timer, hardware timer, user custom timer,...).

<div style="text-align: center;">
    <img src="images/timer-service.png" width="400"/>
</div>

- During application development, with the convenience of this timer service, the timer can be called and deleted anywhere, even during run-time.

### 4. Usage
In this sample code, built on the Makefile project.
Here are the instructions:
- STM32Cube IDE: **Updating**
- Arduino IDE: **Updating**

### 5. References:
| Topic | Link |
| ------ | ------ |
| Active Object Model | https://www.state-machine.com/doc/AN_Active_Objects_for_Embedded.pdf |
| AK Embedded Base Kit | https://github.com/epcbtech/ak-base-kit-stm32l151 |
|Super Simple Tasker |https://github.com/QuantumLeaps/Super-Simple-Tasker|