Guidelines for projects:

General:
- All projects should be hosted on github/gitlab or similars.

- Each student performs commits using his own account.

- We want to see the individual commits.

- The contribution of each wach member in the group to the development
  should be made.  Comment your sections in the source with some
  initials or nickname

  Example:

  //gg: here we clear the list of connections and
  //    do some other stuff
  List_clear(&connections);
  List_pushBack(&connections, stuff_added);



Arduino:
- All projects involvig Arduino should include some sort of serial
  communication with the host.
  The protocol should be binary, and data integrity should be ensured by
  a checksum.

  Meaning: a project of arduino includes a "server" (arduino part),
  and a client (PC part). These are two different programs.
  No cutecom needed for using arduino.

- Use of resources:
  Use at least one interrupt a timer and an I/O port

Example projects

1. Oscilloscope [2]:
   Arduino: timed sampling of up to 8 channels on the ADC,
            and streaming on PC

            Possibility of configuring (from PC)
            - which channel(s) are being sampled
            - the sampling frequency

            Possibility of operating in two modes
            - continuous sampling: (each sample is sent
              to the PC as it becomes ready)
            - buffered mode: a set of samples are stored 
              locally and then sent bulk

            Implementation of "trigger"
            (a condition that decides when to start sampling, in buffered mode)

            Serial communication should be asynchronous (use an interrupt
            on the UART)
   
   PC: host program controlling the oscilloscope.
       Output might be done on text files and displayed with gnuplot.
       

2. Communicator [2]:
   Arduino: two arduinos, with some sort of "transducer" (Led/IR or similar)

            One arduino has the trasmitter circuits, the other the receiver
            circuits. Define a protocol to make the two communicating

            Recall that the signal you get from the sensors is a pure digital
            signal. You might have to implement on it a software serial.

            Communication to/from PC asynchronous (use an ISR to handle comm)

   PC: host program with primitives: sendMessage/receiveMessage

3. Midi Keyboard (for musicans) [2]
   Arduino: Implement a simple keyboard that can monitor for each key,
            the time it took to be pressed (need to use a 2 ways swich)

            For each event, send out a "compliant" midi message through serial

   PC:      write a program that can play the sounds sent through the serial


4. Servo Control (For control engineering students) [2]:
   Arduino: gather a motor with encoder, and an H bridge.
            Implement a closed loop control for the motor.
            The motor can be controlled
            - in open loop (PWM direct)
            - in closed loop (PID, with encoders as reference)

            Serial communication non blocking, using ISR.
            
   PC:      periodically receive state updates from robot
            sends the commands.
            
5. SmartHouse [2-3 w/BT]
   Arduino: Build a smart switch/dimmer/controller that can be controlled
            by bluetooth. We don't use bluetooth, we use a serial first. 

            The arduino should support:
            8 switches/dimmers (simulate tghem with LEDS).
            8 ADC channels
            8 Digital inputs

            Define a protocol to
            - Configure the device (give it a "name"),
              set which channels are used, and what is the name of each channel
              
            - Send commands to the device (based on the "name");
              Upon correct execution of a command, the device should send back
              an "ack" message.
              
            - receive readings from the device (based on the "name");
            
            
   From PC: Implement a "controller" program that can baptize the device,
            and interact with it, from command line

            eg

            $> smart_house_host set_name kitchen_controller
            $> smart_house set_channel_name kitchen_controller switch_0 "top_fridge"
            $> smart_house set_channel_value "top_fridge" 1
            $> smart_house kitchen_controller query_channels 
            
            

           
   
   
-------------------------------------------------------------------------------

1. Semaphores in disastrOS [1]
   implement the following system calls in DisastrOS
   // creates a semaphore in the system, having num semnum
   // the semaphore is accessible throughuot the entire system
   // by its id.
   // on success, the function call returns semnum (>=0);
   // in failure the function returns an error code <0
   - int s=DisastrOS_semOpen(int semnum)

   //releases from an application the given
   // returns 0 on success
   // returns an error code if the semaphore is not owned by the application
   - int DisastrOS_semClose(int semnum)

   //decrements the given semaphore
   //if the semaphore is 0, the caller is put onto wait
   //returns an error code
   - int DisastrOS_semWait(int semnum);

   //increments the given semaphore
   //if the semaphore was at 0, and some other thread was waiting
   //the thread is resumed
   //returns 0 on success, an error code on failure 
   int DisastrOS_semPost(int semnum);


2. Implement a Buddy Allocator that uses a bitmap to store the tree,
   instead of the funky explanatory structures. [1]
   The starting point is the exercise on memory allocators (06_memory_allocators)
   

3. File System: (see past year) [2]
   implement a file system interface using binary files
   - The file system reserves the first part of the file
     to store:
     - a linked list of free blocks
     - linked lists of file blocks
     - a single global directory
     
   - Blocks are of two types
     - data blocks
     - directory blocks

     A data block are "random" information
     A directory block contains a sequence of
     structs of type "directory_entry",
     containing the blocks where the files in that folder start
     and if they are directory themselves

4. Remote Robot Controller (for those that will do LabAI/GI and will build a robot)
   Implement program that connects with the robot [1]
   - (using the low level protocol/example in srrg2_orazio_core)
   - opens the netbook camera (in C)
   - listens on a socket for connections

   When a client connects, it streams back the images and sends
   the joystick command to the robot. Images are displayed in real time on the host.

5. Videogame (jolly) [1]
   Implement a distributed videogame
   - Server Side:
     - the server operates in both TCP and UDP
       TCP Part
       - registerning a new client when it comes online
       - deregistering a client when it goes offline
       - sending the map, when the client requests it
     - UDP part
       - the server receives preiodic upates from the client
         in the form of
	       <timestamp, translational acceleration, rotational acceleration>
         Each "epoch" it integrates the messages from the clients,
         and sends back a state update
       - the server sends to each connected client
         the position of the agents around him

   - Client side
     - the client does the following
     - connects to the server (TCP)
     - requests the map, and gets an ID from the server (TCP)
     - receives udates on the state from the server

     - periodically
       - updates the viewer (provided)
       - reads either keyboard or joystick
       - sends the <UDP> packet of the control to the server

