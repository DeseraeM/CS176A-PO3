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
#define WAIT_MESS 0
#define WAIT_ACK 1
#define BUFFERSIZE 1000

/**** A ENTITY ****/

struct A_send{
    int inital_state;
    struct pkt l_packet[BUFFERSIZE];
    //struct pkt l_packet[SIZE_WINDOW];
    float estimate_rtt;
    int window_size;
    int nextSeq;
    int oldest;
} send_A;

void A_init(int window_size) { 
    send_A.inital_state = WAIT_MESS;
    send_A.oldest = 0;
    send_A.nextSeq = 0;
    send_A.estimate_rtt = 10;
    send_A.window_size = window_size;
}

int g_checksum(struct pkt *packet){
    int checksum = 0;
    checksum += packet->seqnum;
    checksum += packet-> acknum;
    for(int i =0; i < 32; i++){
        checksum += (unsigned char) packet->payload[i];
    }
    return checksum;
};
void A_output(struct msg message) { 
    if( (send_A.nextSeq - send_A.oldest) >= send_A.window_size){
        printf("A_output: Window is full");
        return;
    }
    printf("  A_output: send packet: %s\n", message.data);
    struct pkt packet;
    packet.seqnum = send_A.nextSeq;
    packet.length = message.length;
    memmove(packet.payload, message.data, 32);
    packet.checksum = g_checksum(&packet);
    send_A.l_packet[send_A.nextSeq % send_A.window_size] = packet;
    send_A.inital_state = WAIT_ACK;
    tolayer3_A(packet);
    if(send_A.oldest == send_A.nextSeq){
            starttimer_A(send_A.estimate_rtt);
    }
    send_A.nextSeq += 1; 
}

void A_input(struct pkt packet) {
    if (packet.checksum != g_checksum(&packet)){
    printf(" A_input: packet corrupted. Drop. \n");
    return;
    }
    if( (packet.acknum>= send_A.oldest) && (packet.acknum < send_A.nextSeq)){
        printf("A_output: ACK is vaild ");
        }
    if( (packet.acknum< send_A.oldest) || (packet.acknum >= send_A.nextSeq)){
        printf("A_output: ACK is invaild ");
        return;
    }

    send_A.oldest = packet.acknum + 1;
    printf("A_input: acked.");
    if(send_A.oldest == send_A.nextSeq){
        stoptimer_A();
    }
    else{
        starttimer_A(send_A.estimate_rtt);
    }
    //send_A.seqNum = 1- send_A.seqNum;
    //send_A.inital_state = WAIT_MESS;
 }

void A_timerinterrupt() { 
    for(int i = send_A.oldest; i < send_A.nextSeq; i++){
        tolayer3_A(send_A.l_packet[i % send_A.window_size]);
    }
    starttimer_A(send_A.estimate_rtt);
}


/**** B ENTITY ****/

struct B_recv{
    int seqNum;
    int payload;
} recv_B;


void B_init(int window_size) {
    recv_B.seqNum = 0;
 }
 int send_ACK(int ack){
    struct pkt packet;
    packet.acknum = ack;
    packet.seqnum = 0;
    memset(packet.payload, 0, 32);
    packet.checksum = g_checksum(&packet);

    tolayer3_B(packet);
    
 }

void B_input(struct pkt packet) { 
    if(packet.checksum != g_checksum(&packet)){
        printf("B_input: packet was corrupted. Send ACK for the last packeted.", recv_B.seqNum);
        send_ACK(recv_B.seqNum - 1);
        return;
    }
    if(packet.seqnum != recv_B.seqNum){
        printf("B_input: Not the expected seq.");
        send_ACK(recv_B.seqNum - 1);
        return;
    }
    printf("B_input: recv message: %s", packet.payload);
    struct msg message;
    memmove(message.data,packet.payload, 32);  
    message.length = packet.length;
    tolayer5_B(message);

    printf("B_input: send ACK.");
    send_ACK(recv_B.seqNum); 
    recv_B.seqNum += 1;
}

void B_timerinterrupt(void) {
    printf("B_timerinterrupt: Ignore.");
 }
