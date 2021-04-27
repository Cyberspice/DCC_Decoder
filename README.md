# DCC Decoder

## Introduction

This is an implementation of a Digital Command Control signal decoder
using an ATMEL AVR ATMega328 or an Arduino.

The intention is that this code becomes a toolkit for writing DCC 
decoding software targetting the ATMEL AVR

## DCC_SNIFFER

The branch DCC_SNIFFER is a branch that comprises the software for a
DCC sniffer, it simply decodes incoming DCC packets and then outputs
on the serial the data for each new packet seen. Repeat packets are
not output.

A schematic and PCB layout will be available shortly.

## DCC_ACCESSORY_DECODER_V1

The branch DCC_ACCESSORY_DECODER_V1 is a branch that will eventually
comprise the software for a fully featured DCC accessory decoder. It
will support switching GPIOs in various modes and driving servos. It
will support programming via CVs or via a three button control
interface. It is being developed in conjuction with a schematic and
PCB that provides a sort of 'swiss army knife' DCC accessory decoder.

