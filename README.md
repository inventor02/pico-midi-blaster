# Pico MIDI Blaster

A hardware project for [COMP2215](https://www.southampton.ac.uk/courses/modules/comp2215).

## MIDI

The standard for MIDI files, tracks and messages is available for free from the MIDI Manufacturers' Association (if you sign up for an account...) - I am not able to distribute it here but you may find the following brief synopsis helpful in lieu of reading the specification itself.

For what it's worth, the spec is well written and quite easy to understand - this is owing in part to MIDI being a simple protocol, but also to the good structure and language used by the MMA, so it would be amiss of me not to at least acknowledge that.

### MIDI Files

MIDI files are made up of the MIDI header and MIDI tracks. There are various types of MIDI file, but as we only have one piezoelectric buzzer to play the MIDI file, we will limit ourselves to MIDI files of type 0 here - that is, files with only one track.

### MIDI Tracks

MIDI tracks contain a length, as well as one or more MIDI messages.

### MIDI Messages

MIDI messages come in all sorts of shapes and sizes and each MIDI device (or MIDI file-consuming device) does not need to care about all of them.

MIDI messages that the device does not know how to handle should be dropped silently and not throw any sort of error. We do this for most
messages.

Messages contain a status byte in which the four most significant bits denote the message ID and the four least significant bits denote the channel number (1-16). They also contain either one or two data bytes, most significant byte first.

#### Note Off (`0x8N`)

The Note Off message has two data bytes.

1. `0` followed by 7 bits that determine the note number (0-127).
2. `0` followed by 7 bits determining the note off velocity (0-127).

#### Note On (`0x9N`)

The Note On message has two data bytes.

1. `0` followed by 7 bits that determine the note number (0-127).
2. `0` followed by 7 bits determining the note on velocity (0-127).

#### SysEx End of File
