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

*Figure 1: Table showing alternative functions of the pins used*



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

*Figure 2: Circuit Diagram of Project*

## Encoder Mode
One of the operation modes for the timers on the STM32 is **encoder mode**. The purpose of this mode is to increment or decrement the timer counter when a transitiion occurs on the rotary encoder. The increment or decrement depends on the clockwise or counter-clockwise movement of the encoder.


## How the Rotary Encoder Works
Inside a rotary encoder, there's a circular disc with evenly spaced out disc with evenly spaced slots. This disc is attached to the turnknob. The disc connects to a pin called "C" which serves as the common ground. The encoder also has two other important pins, A and B, which help determine what direction the knob is turning.

Because of the way the slots are arranged, pins A and B do not touch the common ground at the same time. One always touches just before the other. This creates two separate signals that are 90 degrees out of phase. 

So how do we figure out which way the knob is turning? This is done by watching the state of pin B at the exact moment pin A changes its state.
If pin B's state is **different** from pin A when pin A changes state, then we know the knob is being turned clockwise.![image](https://github.com/user-attachments/assets/b130e347-4b95-4a86-a876-065a60ef6dc4)

*Figure 3: States of Pin A and Pin B showing the knob being turned clockwise*

If pin B's state is **the same** from pin A when pin A changes state, then we know the knob is being turned counter-clockwise.![image](https://github.com/user-attachments/assets/3bfa9c64-b070-4d49-aba6-88864c46d988)

*Figure 4: States of Pin a and Pin B showing the knob being turnedd counter-clockwise*

This method of tracking is called **Quadrature Encoding**

## Calculating the RPM
To determine the speed of the fan, we used the STM32's Timer 1 in input capture mode. The fan provides a tachometer signal from one of its pins. The fan generates two pulses when it completes one full rotation. By measuring the frequency of this signal, the fan speed can be calculated.
```c
int captureFanSpeed()
    {
        // Calculate fan speed based on captured pulse time from FG signal (PA9)
        if (TIM1->SR & TIM_SR_CC2IF)  // Check if capture event occurred (CC1IF flag)
        {
            currentSpeed = TIM1->CCR2; // directly from tach
            duration = currentSpeed - oldSpeed; //period
            

            
            period = 1000*duration/SystemCoreClock;
            rpm = (60000000) / (duration * 2); // 2 pulses per rev, timer at 1MHz
            oldSpeed = currentSpeed;
```

## Low-power Mode
To utilise the rotary encoder even further, a low-power mode is triggered when the SW button is pressed. This sets the fan to 20% PWM duty cycle for a period of time.
This reduces power consumption while keeping the fan operational at a lower speed. Low power modes are essential in embedded systems for energy efficiency, system longevity, and cost efficiency.

## Lessons Learned
- Always verify peripheral pin mappings with the reference manual and datasheet before writing code.
- Use an oscilloscope when debugging analog or fast-changing digital signals to validate software assumptions.
- Pull-up resistors are essential for open-drain signals like the tachometer
- Input capture modes and encoder modes can be combined effectively with PWM to create interactive control systems.

**Debugging the PWM signal and tachometer signal using an oscilloscope**\
It was a good idea to check the PWM signal and tachometer signal using an oscilloscope to verify if changing the duty cycle changed the speed of the fan. This also confirmed that the tachometer signal needed a pullup resistor as the initial signal was unstable.

![image](https://github.com/user-attachments/assets/6c420289-266d-40ec-afa2-69b1cd4b2342)

*Figure 5: Tachometer signal before adding a pull-up resistor*

![image](https://github.com/user-attachments/assets/39c0787c-6aa0-4504-baae-0ca294c53269)

*Figure 6: Tachometer signal after adding a pull-up resistor*

## Ethical Considerations for Embedded Systems
Embedded Systems are specialised computing units within larger systems, often used in critical applications like automotive, medical, and industrial control. 
The STM32L432KC microcontroller, being a power-efficient ARM Cortex-M4, is widely used in Iot and security-sensitive applications.
As the use of embedded systems expands, several ethical considerations must be addressed.

### Security and Risk Mitigation
There is a threat of cyber attacks on embedded systems, which are now central to everything from appliances to vehicles.
Ethically, engineers must prioritse security to prevent harm from potential breaches, whether it's user data theft, system failures, or even life-threatening outcomes in critical systems like automotive or medical devices.
There are three main security princples: Confidntiality, Integrity, and Availability (CIA).

**Confidentiality** - Ensuring that sensitive data is accessible only to authorised parties.

**Integrity** - Preventing unauthorised modification of code or data.

**Availability** - Ensuring system uptime and resistance against denial-of-service attacks.

### Compliance with Legal and Regulatory Standards
Embedded systems must comply with legal requirements.
Ethically, engineers must design systems that comply with regulations ensuring respect for user rights and public safety.

Compliance standards include:

**ISO 26262**: Automotive functional safety standard

**IEC 62433**: Security standard for industrial automation and control system

**GDPR**: Data protection regulation application to IOT devices.

**NIST Guidelines**: Best practices for cybersecurity in embedded systems.


### Firmware Management and Safety
Effective firmware management is not just a technical neccisty but also a ethical obligation. 
Outdated or insecure firmware can expose devices to vulnerabilites.
*"A catastrophe should not be the catalyst for action".*
Manufacturers and developers must ensure regular updates and safe deployment practices to protect users and infrastructure.
This also reinforces the ethical principle of proactive responsibilty.
As engineers, it is important to anticipate risks and act before failures occur, rather than reacting only after the damage is done.




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
The project serves as a practical example of integrating multiple peripherals in embedded systems and highlights the importance of debugging, hardware awareness and good desing practices.

# References
https://www.st.com/resource/en/datasheet/stm32l432kc.pdf - STM32L432KC Datasheet

https://www.st.com/resource/en/reference_manual/rm0432-stm32l4-series-advanced-armbased-32bit-mcus-stmicroelectronics.pdf - STM32L432KC Reference Manual

https://lastminuteengineers.com/rotary-encoder-arduino-tutorial/ - Rotary Encoder












    
