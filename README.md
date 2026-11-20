# Fault-Tolerant Traffic Light Controller

A four-way traffic light control system built using two ATmega32 microcontrollers with a Master-Backup architecture.

## Overview

This project aims to implement a fault-tolerant traffic light controller using an active-standby architecture.

The Master controls the traffic lights during normal operation, while the Backup monitors its heartbeat signal and automatically takes control if the Master fails.

## Features

- Four-way traffic light control
- Dual ATmega32 microcontrollers
- Master-Backup architecture
- Heartbeat-based failure detection
- Hardware output selection using 74HC244
- Manual control return mechanism

## Technologies

- C++
- ATmega32
- Proteus
- 74HC244

## Timing Configuration

| Parameter | Value |
|---|---:|
| CPU Frequency | 1 MHz |
| Green Light | 15 seconds |
| Yellow Light | 3 seconds |
| Heartbeat Interval | ~50 ms |
| Failure Detection | ~150 ms |

## Project Status

**Currently Under Development**

