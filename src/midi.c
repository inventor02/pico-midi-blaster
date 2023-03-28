#include "midi.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#include "pico/stdlib.h"

// *** Welcome to hell! ***
// If you come up with a better way of parsing the binary data than I did, please let me know!
// My way isn't the best I don't think so I'd be very interested to see how you do it.

// uint16_t bytes_to_i16(uint8_t bytes[]) {
  // you may want a helper function to convert some bytes to a uint16
// }

// uint32_t bytes_to_i32(uint8_t bytes[]) {
  // you may want a helper function to convert some bytes to a uint32
// }

// uint32_t varlen_bytes_to_i32(uint8_t bytes[]) {
  // you will need some way of processing MIDI variable lengths
  // see the spec for more details, and a "model" implementation that
  // isn't particularly useful in this context
// }

// Parse a MIDI chunk
int midi_chunk_parse(midi_chunk_t *chunk, uint8_t *contents, uint contents_length) {
  // First check stuff like the header, the length and put them somewhere useful

  // You'll then need to parse all the MIDI events in sequence
  // Each one has certain parameters, see the README

  // Pay attention to "rolling status" (see the spec) - MIDI files allow this!

  // You can get away with handling only NOTE ON and NOTE OFF (as well as END OF FILE) to
  // play very simple files. You might want to get the tempo out of the file as well -
  // it should be a meta event.

  // You need to keep track of how many valid events there are too, and put them in the struct
  // Note the playback code expects only valid events we know how to deal with to be in the
  // midi_chunk_t struct.

  // You should return the number of bytes you have processed so we can parse later chunks
  // if we wanted to.

  // You should also check for "runaway" conditions where for some reason you've blasted
  // past the end of where the chunk should be (remember the length from the start of it).

  // If you do, you can either panic, or just return back and try and salvage the situation.
}

void midi_file_parse(midi_file_t *file, uint8_t contents[], uint contents_length) {
  // Check stuff like the header, length, etc - keep them for later.

  // You'll need the format, number of tracks and the division too. We can only
  // really handle format 0 tracks, so you can panic if the format is something else.

  // Once you've got the header information, you can start using the chunk parser function
  // to extract chunks and save them into your struct, making sure to pass in the correct
  // pointer and update the offser as you go, otherwise you'll keep going forever!

  // You might want some sort of check
}

// If you got this far then you're probably thinking "what the hell is the point of all this,
// we only have one track!"

// You'd be correct. But where's the fun in that ;-)
