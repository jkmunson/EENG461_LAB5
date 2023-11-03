---
title:  EENG461, LAB5 Report, Automatic Blinds
author: Mark Clark and Jeremy Munson
geometry: margin=3cm
header-includes:
 - \usepackage{fvextra}
 - \DefineVerbatimEnvironment{Highlighting}{Verbatim}{breaklines,commandchars=\\\{\}}
---

## Objective

In this lab we are to read from a photodiode to determine light levels and drive a stepper motor, as if adjusting some blinds. The stepper motor is driven by an L293D quad half-h module. The photodiode is read by an ADC.

## Introduction

First task set:
* Task 1 Take a light measurement from a photodiode every second
* Task 2 Use it to control a stepper motor such that the brightness affects the amount that it is open in increments.
* Task 3 Every 10% change in brightness should cause one rotation of the stepper motor

Second task set:
* Task 1 Wire up photodiode to ADC and read it with a 1s timer, Conversion Math (to percentage of light range)
* Task 2 Printf the ADCV and the Percentage of light range
* Task 3 Implement the stepper motor in fixed movements such that you can control 1 exact rotation forward or backward (aka draw the rest of the owl) and use the percentage of light to determine how many to do


## Narrative

We already have significant code and utilities prepared from the previous labs. Of note, we re-used existing code for ADC readings, printing over uart, and time-keeping. We call all of the various setup functions for each part, then enter the main loop.


### Main input loop
`uptime_seconds` is updated by a timer each second, and indicates the number of seconds since the time-keeping utility started. At the start of our main loop we do nothing until at least a second has passed since the start of the last loop - this meets the 1-second response requirement of Task 1.

We then read in the current raw ADC value for the `light`. `light` is updated regularly by our existing ADC code, and could be updated mid-loop - which would make the calculations inconsistent. The read into temp prevents this issue.

Next, we dynamically adjust the minimum and maximum ranges for the reading of the light level - a new highest level or a new lowest level will expand the range of the measurement. This allows the program to adjust to different lighting in different rooms, for example. In practice, we were always able to use the entire range of <10% to 100% (in 10% increments) specified in Task 3. 

We then calculate what position, 1 to 10, linearly from the light value. The `target_position` is in the range of 1 to 10, indicating the number of rotations to make relative to 0 rotations.

We print out the information using the printlf() function that we wrote in a previous lab. This meets the requirement of task 2 in the second set.

Finally, we call the calculateRotation() function, which starts the stepper motor movement if needed.
```
while (1) {
		while(last_time == uptime_seconds) {};
		last_time = uptime_seconds;
		
		int temp = light;
		if(temp < adc_lowest) adc_lowest = temp; //If this is a new lowest value ever seen, set a new floor
		if(temp > adc_highest) adc_highest = temp;
		
		int target_position = ((temp - adc_lowest)*100/(adc_highest-adc_lowest) +2) / 10;
		
		printlf("Target Position: %d \t | Light %d0% | raw adc: %d\n", target_position, target_position, temp);
		
		calculateRotation(target_position*360.0f);
	}
```

### Stepper Motor Operation

The stepper motor code was more complex than really needed for this lab, specifically. One of the team members intends to re-use it in their final project, so is working on making a more "general" solution, for arbitrary stepper motors using arbitrary timers. 

The stepper motor driver is connected to pins PC4-PC7. This allows writing the stepper motor states to occur all at once, preventing 



## Concluding Remarks

