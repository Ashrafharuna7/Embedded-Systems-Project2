# STM32 Fan Speed Controller
# Table of Contents
[Introduction](https://github.com/Ashrafharuna7/Embedded-Systems#introduction)

[List of Components used](https://github.com/Ashrafharuna7/Embedded-Systems/blob/main/README.md#list-of-components-used)

[Features](https://github.com/Ashrafharuna7/Embedded-Systems/blob/main/README.md#features)

[Circuit Diagram](https://github.com/Ashrafharuna7/Embedded-Systems/blob/main/README.md#circuit-diagram)

[Calculating the RPM](https://github.com/Ashrafharuna7/Embedded-Systems/blob/main/README.md#calculating-the-rpm)

[Troubleshooting Common issues and during the project](https://github.com/Ashrafharuna7/Embedded-Systems/blob/main/README.md#troubleshooting-common-issues-and-during-the-project)




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
- Below is Table 15 showing the alternative functions of the used pins.![image](https://github.com/user-attachments/assets/b8c79627-e1b0-47e1-b94c-a00ef012f18b)


## Circuit Diagram
The fan is powered using a 12V DC power supply and has its tachometer and PWM connected to PA9 and PA8 respectively. The microcontroller is connected via USB to the laptop. 
The 5-pin Rotary Encoder is connected as follows: 
- CLK pin to PA0
- DT pin to PB3
- SW pin to PB4
- Positive pin to 3.3V
- Negative pin to GND.
 There is a 10kΩ pullup resistor connected to the tachometer and 3.3V to pull the signal **HIGH** when the fan is not pulling it **LOW**. The microcontroller uses UART serial communication to communicate to the laptop. This is done using PA2 to transmit UART to the serial monitor. The circuit digram can be seen below: 
![image](https://github.com/user-attachments/assets/443c0f6c-d24c-4752-b22b-d7d1d494fb73)


The STM32l432KC Microcontroller:
![nucleo_l432kc_2017_10_09](https://github.com/user-attachments/assets/a64d2cd8-6861-476b-8015-f13cac2e5bf3)


## Calculating the RPM
To measure the fan speed, the tachometer signal from the fan was used. When a fan blade completes one full rotation, the tachometer signal transitions from **HIGH to LOW** and **LOW to HIGH**. The fan is an example of a rotary encoder and the time between the pulses tells us the speed. When the fan completes a full rotation it outputs a pulse. The time difference between to consecutive rising or falling edges is used to calculate the rpm. The RPM calculation is as follows: 
- Period = (1000*duration/Clockspeed),period in milliseconds
- rpm = 60000/period

This is done in the captureFanSpeed funtion in the code:![image](https://github.com/user-attachments/assets/25cbcb28-302f-4431-bf65-68e8bc769aca)


## Troubleshooting Common issues and during the project
### No RPM Reading
**Ensure that the tachometer has a pull-up resistor (around 4.7kΩ or more) connecting to 3.3V.**\
While tryng to measure the rpm output, it seemed like some of the readings from the tachometer signal was random. This could be due to not having a sufficient pull-up resistor. The pullup resistor used was only a value of 330Ω which is normally too low. Adding a higher resistor may have stabalised the readings.
  
**Ensure that PA1 is correctly configured as an input capture pin.**\
I had issues when trying to read the tachometer signal as PA1 was not configured as an alternative function for timer2. It is important to make sure that PA1 is configured as an alternative function for timer2 input capture on the correct channel (Channel 2). Going through the datasheets and reference manuals of devices proves to be a big help when setting up configurations.

### PWM not controlling the fan
**Verify the potentiometer connections**\
A big issue that I faced during this project was the wiring of the potentiometer. I connected the potentiometer to the 12V power supply which damaged my board and required a replacement. It is important to draw a schematic before starting to connect anything to prevent such mistakes.

**Check PWM signal using an oscilloscope**\
It was a good idea to check the PWM signal using an oscilloscope to verify if changing the duty cycle changed the speed of the fan.![image](https://github.com/user-attachments/assets/9111bbb5-8c86-491b-bdc1-5145b6e53bfe)


## Conclusion
Overall, the project was successful, and the majority of the implementation worked as expected.
The PMW control of the fan was achieved using Timer 2, and the speed adjustment using the potentiometer was accurately reflected in the PMW duty cycle on the oscilloscope.
The RPM measurement using input capture on PA1 also functioned, allowing for real-time monitoring of the fan's speed.

However, challenges arose along the way.
There were issues with measuring the RPM accurately. 
The fan's tachometer signal required a proper pull-up resistor, which was overlooked, leading to inconsistent readings.

Having tackled these challenges, the objective of the project was successful as it demonstrated PMW-based speed contrl and RPM measurement using an STM32 microcontroller.
Valuable lessons from debugging signal issues and correctly configuring timers and channels were learned and will be used in future embedded systems projects.













    
