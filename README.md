# STM32 Fan Speed Control using a Rotary Encoder




## Introduction
The aim of this project is to control the speed of a PC fan using **PWM** on an STM32L432KC microcontroller. 
The fan speed is controlled using a rotary encoder, a tachometer signal is used to measure RPM and the encoder's push button toggles a low-power mode.
The project demonstrates real-time speed control and feedback, useful for embedded systems.

## List of Components used
- STM32L432KC Microcontroller
- 12V DC PC Fan
- Rotary Encoder (with push button)
- Oscilloscope
- 10kΩ Pullup resistor
- 12V Power supply

## Peripherals
- Digital I/O: Used to toggle the mode of the fan to low power using the switch on the encoder.
- PWM: Used to control the fan speed by varying the duty cycle of the signal sent to the fan
- Serial Communications (UART): Sends real-time RPM data to the computer for monitoring
- Timers: Used for capturing pulse signals from the fans tachometer to calculate RPM

## Features
- Timer 1 CH1 is used on **PA8** as **PWM Output Mode** to control the fan's speed PWM output.
- Timer 1 CH2 is used on **PA9** as **Input Capture Mode** to capture the fan's tachometer signal for RPM measurement.
- Timer 2 CH1 is used on **PA0** as **Encoder Mode** for the Rotary Encoder's **CLK** pin.
- Timer 2 CH2 is used on **PB3** as **Encoder Mode** for the Rotary Encoder's **DT** pin.
- GPIO Input is used on **PB4** for the switch on the Rotary Encoder to toggle a low power mode.
- USART2_TX is used on **PA2** to transmit UART to the serial monitor.
- Below is Table 15 showing the alternative functions of the used pins.
![image](https://github.com/user-attachments/assets/cbf11e7d-d9d2-445a-a652-ce790eecefd7)
![image](https://github.com/user-attachments/assets/300efeb3-e7df-4fd3-9096-f3cb0374b08c)



## Circuit Diagram
The fan is powered using a 12V DC power supply and has its tachometer and PWM connected to PA9 and PA8 respectively. The microcontroller is connected via USB to the laptop. 
The 5-pin Rotary Encoder is connected as follows: 
- CLK pin to PA0
- DT pin to PB3
- SW pin to PB4
- Positive pin to 3.3V
- Negative pin to GND.

There is a 10kΩ pullup resistor connected to the tachometer and 3.3V to pull the signal **HIGH** when the fan is not pulling it **LOW**. The microcontroller uses UART serial communication to communicate to the laptop. This is done using PA2 to transmit UART to the serial monitor. The circuit digram can be seen below: 
![image](https://github.com/user-attachments/assets/72ad520d-3ad8-4812-a951-ea096a05ce32)





The STM32l432KC Microcontroller with used Pins:
![nucleo_l432kc_2017_10_09 (1)](https://github.com/user-attachments/assets/078e9f90-ffc1-4efe-aba1-01d981389d41)

## Encoder Mode
One of the operation modes for the timers on the STM32 is **encoder mode**. The purpose of this mode is to increment or decrement the timer counter when a transitiion occurs on the rotary encoder. The increment or decrement depends on the clockwise or counter-clockwise movement of the encoder.


## How the Rotary Encoder Works
Inside a rotary encoder, there's a circular disc with evenly spaced out disc with evenly spaced slots. This disc is attached to the turnknob. The disc connects to a pin called "C" which serves as the common ground. The encoder also has two other important pins, A and B, which help determine what direction the knob is turning.

Because of the way the slots are arranged, pins A and B do not touch the common ground at the same time. One always touches just before the other. This creates two separate signals that are 90 degrees out of phase. 

So how do we figure out which way the knob is turning? This is done by watching the state of pin B at the exact moment pin A changes its state.
If pin B's state is **different** from pin A when pin A changes state, then we know the knob is being turned clockwise.![image](https://github.com/user-attachments/assets/b130e347-4b95-4a86-a876-065a60ef6dc4)

If pin B's state is **the same** from pin A when pin A changes state, then we know the knob is being turned counter-clockwise.![image](https://github.com/user-attachments/assets/3bfa9c64-b070-4d49-aba6-88864c46d988)

This method of tracking is called **Quadrature Encoding**

## Calculating the RPM
To determine the speed of the fan, we used the STM32's Timer 1 in input capture mode. The fan provides a tachometer signal from one of its pins. The fan generates two pulses when it completes one full rotation. By measuring the frequency of this signal, the fan speed can be calculated.


## Lessons Learned
- Always verify peripheral pin mappings with the reference manual and datasheet before writing code.
- Use an oscilloscope when debugging analog or fast-changing digital signals to validate software assumptions.
- Pull-up resistors are essential for open-drain signals like the tachometer
- Input capture modes and encoder modes can be combined effectively with PWM to create interactive control systems.

**Debugging the PWM signal and tachometer signal using an oscilloscope**\
It was a good idea to check the PWM signal and tachometer signal using an oscilloscope to verify if changing the duty cycle changed the speed of the fan. This also confirmed that the tachometer signal needed a pullup resistor as the initial signal was unstable.

Before pullup resistor was added:

![image](https://github.com/user-attachments/assets/6c420289-266d-40ec-afa2-69b1cd4b2342)

After pullup resistor was addded:

![image](https://github.com/user-attachments/assets/39c0787c-6aa0-4504-baae-0ca294c53269)


## Conclusion
This project successfully demonstrated how to use the STM32L432KC microcontroller to control andd monitor the speed of a DC fan. 
Using PWM generation, rotary encoder input, and input capture modes, we were able to implement real-time speed control with feedback.
The rotary encoder offered a user-friendly interface to adjust fan speed precisely. 
The tachometer signal from the fan was captured using Timer 1 in input capture mode to calculate RPM values based on pulse timing.
The encoder's push button was also utilised to toggle a low-power mode for energy efficiency.

A major learning outcome involved correctly setting up timer channels and configuring GPIO pins for alternate functions.
Additionally, using an oscilloscope to debug PWM and tachometer signals helped identify the need for a pull-up resistor, which stabalised the tachometer output.
Despite challenges such as unexpected RPM readings and timer conflicts, reviewing the datasheet and reference manual allowed for resolution. 
These experiences provided hands-on understanding of embedded peripherals such as timers, PWM generation, serial communication, and signal conditioning.
The project serves as a practical example of integrating multiple peripherals in embedded systems and highlights the importance of debugging, hardware awareness and good desing practiceas.












    
