/******************************************************************************/
/*                                                                            */
/* ENTITY IMPLEMENTATIONS                                                     */
/*                                                                            */
/******************************************************************************/

// This file contains the actual code for the functions that will implement the
// reliable transport protocols enabling entity "A" to reliably send information
// to entity "B".
//
// This is where you should write your code, and you should submit a modified
// version of this file.
//
// Notes:
// - One way network delay averages five time units (longer if there are other
//   messages in the channel for GBN), but can be larger.
// - Packets can be corrupted (either the header or the data portion) or lost,
//   according to user-defined probabilities entered as command line arguments.
// - Packets will be delivered in the order in which they were sent (although
//   some can be lost).
// - You may have global state in this file, BUT THAT GLOBAL STATE MUST NOT BE
//   SHARED BETWEEN THE TWO ENTITIES' FUNCTIONS. "A" and "B" are simulating two
//   entities connected by a network, and as such they cannot access each
//   other's variables and global state. Entity "A" can access its own state,
//   and entity "B" can access its own state, but anything shared between the
//   two must be passed in a `pkt` across the simulated network. Violating this
//   requirement will result in a very low score for this project (or a 0).
//
// To compile:
//
//     $ gcc entity.c simulator.c -o sim
//
// To run:
//
//     $ ./sim <loss> <corrupt> <interval> <seed> <debug> <window> <input file>
//
//   loss     - packet loss probability        (0.0 = none, 1.0 = all lost)
//   corrupt  - packet corruption probability  (0.0 = none, 1.0 = all corrupted)
//   interval - average time between msgs from layer 5 (10 is a good default)
//   seed     - random number generator seed   (fix for reproducible runs)
//   debug    - debug output level             (0 = none, 1-3 = increasing detail)
//   window   - GBN window size
//   input    - file whose contents are transferred from A to B
//
// Example (perfect channel, window=8):
//
//     $ ./sim 0.0 0.0 10 500 3 8 input.txt
//
// The simulator writes data received at entity "B" to `output.dat`.
// Diff that against the input file to verify correctness.

#include <stdio.h>
#include <string.h>
#include "simulator.h"

/**** A ENTITY ****/

void A_init(int window_size) { }

void A_output(struct msg message) { }

void A_input(struct pkt packet) { }

void A_timerinterrupt() { }


/**** B ENTITY ****/

void B_init(int window_size) { }

void B_input(struct pkt packet) { }

void B_timerinterrupt() { }
