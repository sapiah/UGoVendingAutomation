/*
 * TODO: 
 * LEDs not implelemted
 * PWM on linear actuator to control speed
 * 
 */
#include "global.h"

#ifdef __cplusplus 
extern "C" {
#endif
  #include "actions.h"
  #include "machine.h"
#ifdef __cplusplus 
}
#endif

#ifdef USB_COMMUNICATION
#include "usb_comm.h"
#endif

machine_t machines[NUMBER_OF_MACHINES];

void setup() {
  int i;
  
  blend_actions_init();
  initializing_action_init();
  
  // initialize the machine
  for (i = 0; i < NUMBER_OF_MACHINES; i++) {
    machine_init(&machines[i]);
  }
  
  // initialize the mediator
  mediator_init();

#ifdef USB_COMMUNICATION
  // set up the serial port for communucation
  Serial.begin(9600);
#endif
}

void loop() {
  int i = 0;
#ifdef USB_COMMUNICATION
  // check if there are any messages to process
  usb_communication_process();
#endif

  // initialize the machine
  for (i = 0; i < NUMBER_OF_MACHINES; i++) {
    machine_check_safety_conditions(&machines[i]);
    machine_process(&machines[i]);
  } 
}