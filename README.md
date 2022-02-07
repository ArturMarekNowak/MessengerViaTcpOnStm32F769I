# Messenger via TCP on STM32F769I
The idea of this project is to make an application of STM32F7 based TCP server messenger. This project is made so that the authors can get a credit on their college course called 
"Communication standards and systems" oon AGH University of Science and Tehnology.

## Table of contents
* [General info](#general-info)
* [Screenshots](#screenshots)
* [Technologies](#technologies)
* [Setup](#setup)
* [Status](#status)
* [Inspiration](#inspiration)
* [Contact](#contact)

## General info

Project structure is as following:

    .
    ├── ClientPC      		  # Directory containing PC TCP based clients in cpp and python
    └── STM32F769      	          # Directory containing PC TCP based server
		└── Projects
			└── STM32F769I-Discovery
				└── Applications
					└── LwIP
						└── LwIP_HTTP_Server_Socket_RTOS
							└── Src                   # Directory containing main.c and httpserver-socket.c files
							
		
## Screenshots

We have decided to record our project "in action". The videos were published on youtube platform and can be found in the link below:

https://www.youtube.com/watch?v=4M3yUjgGra4&ab

https://www.youtube.com/watch?v=ZocqbPkWzvw

## Technologies
* STM32F769I microcontroller
* TCP protocol
* Cygwin
* Python3
* Keil uVision

## Setup
Open QuickStart.txt file, it contains a meticoulus specification on how to start up a project with hardware. Description is in polish. 

The software part was made with Keil uVision IDE. The easiest way to setup up this project is to open the <em>Project.uvprojx</em> file with your Keil. The <em>Project.uvprojx</em> is in the <em>Src</em> directory. 

Since C++ client contains, unfortunately, both Linux and Windows libraries, we recommend usage of Cygwin on Windows with gcc compiler. Compile the <em>client.cpp</em> file with:

`gcc client.cpp`

When compilation is done, start two client apps on different parts for example:

`./client.exe 192.168.10.10 12345`

and

`./client.exe 192.168.10.10 12346`

The same can be done with python clients. But this time the ports are hardcoded with application so simple commands such as:

`python3 ClientPort12345.py`

and 

`python3 ClientPort12346.py`

are absolutely enough :)

## Status
Project is: _finished_

## Inspiration
Getting a credit on college course called "Communication standards and systems" oon AGH University of Science and Tehnology.

## Contact
Feel free to give us feedback :D
