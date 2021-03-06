//Code for Test Rig 27/07/2016

#include <stdlib.h>
#include <math.h>

/*
 **HIGH -> means off for normal relays
 **LOW -> means on for normal relays
 ** HIGH -> means on for Solid State relays
 ** LOW -> means off for Solid State relays 
 ** Assumming "hot" -> actuator goes down
 ** Assuming "neutral" -> actuator goes up to orginal position 
 
 Mechanical Relay experiences an arc due to the inductive load when the blender is turned off. Hence Solid State Relay is only used for to avoid the arc. Although Solid Stare relay
 does not turn on when a load is put upon it. Hence the SSR is called to turn on the blender and the load is applied onto the mechanical relay. Hence avoiding the arc brought upon 
 the Mechanical Relay. 
*/

//Distance Calibration Measurments
int TopPosition = 355; // 312
int TopOfCup = 505; //525
int TopOfSmoothie= 540;//560
int BottomOfCup = 600; //610
int BottomOfCleaning = 655; // 578
int CleaningLevel = 570; //505



const int Potentiometer = A1;   //POTENTIOMETER Pin
const int pinButtonBlend = 2;   //BLEND Sequence {Button}
const int pinButtonClean = 3;   //CLEAN Sequence {Button}
const int pinWaterPump = 6;     //Water PUMP 
const int ActuatorHot = 7;      //Actuator to go DOWN
const int ActuatorNeutral = 8;  //Actuator to go UP
const int pinLED_READY = 9;     //LED -->
const int pinLED_IN_USE = 10;   //<--Indicator
const int pinBlender = 12; //Blender {Normal Relay}  /MOV IS USED TO PROTECT FROM ARC FROM AC MODE..
const int pinBlenderSSR = 30; // solid state relay 

const int START= 14;
int ValButtonClean = LOW;
int ValButtonBlend = LOW;

//Bubble Sort Value Initialization
int Read[15] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};  // Array to hold Actuator Portentiometer Reads

// prototypes
void blend_2();

void setup() {

  //Initialization of POTENTIOMETER, BLEND Sequence and CLEAN Sequence 
  pinMode(Potentiometer, INPUT);
  pinMode(pinButtonBlend, INPUT);
  pinMode(pinButtonClean, INPUT);

  Serial.begin(9600);
  Serial.println();

  //Initialization of Actuator to go DOWN and turning it OFF
  pinMode(ActuatorHot, OUTPUT);
  digitalWrite(ActuatorHot, HIGH);
  
  //Initialization of Actuator to go UP
  pinMode(ActuatorNeutral, OUTPUT);

  //Safety precaturions: Removes Hinderance
  Unjam();

  
  if (MeasureDistance() > TopPosition){   //Check if Blender is at the top, if not bring the actuator to the top until TopPosition is reached using the WHILE loop used below
    digitalWrite(ActuatorNeutral, LOW);
  }
  else
  {
    digitalWrite(ActuatorNeutral, HIGH); 
  }

  //Initialization of Blender both Solid State Relay and Mechanical State relay 
  pinMode(pinBlenderSSR, OUTPUT);
  pinMode(pinBlender, OUTPUT);
  
  TurnBlenderOff ();
  
  //Initialization of LED 
  pinMode(pinLED_READY, OUTPUT);
  digitalWrite(pinLED_READY, LOW);
  
  pinMode(pinLED_IN_USE, OUTPUT);
  digitalWrite(pinLED_IN_USE, LOW);

  //Initialization of Water Pump and turning it OFF to start with   
  pinMode(pinWaterPump, OUTPUT);
  digitalWrite(pinWaterPump, HIGH);

  while (MeasureDistance() > TopPosition)
  {
    delay(1);   //move to top position
  }
  
  digitalWrite(ActuatorNeutral, HIGH);
  digitalWrite(pinLED_READY, HIGH);   //Determines Initialization is complete and program is ready

}

//Turns the blender ON, First Solid State Relay then Mechanical State relay, ALWAYS!
void TurnBlenderOn () {
  digitalWrite(pinBlenderSSR, HIGH);    //SSR HIGH = ON
  delay(20);
  digitalWrite(pinBlender, LOW);    //MR LOW = ON     
}

//Turns the blender OFF, First Solid State Relay then Mechanical State relay, ALWAYS!
void TurnBlenderOff () {

  digitalWrite(pinBlenderSSR, LOW);   //SSR LOW = OFF
  delay(50);
  digitalWrite(pinBlender, HIGH);   //MR HIGH = OFF 

}

//Measures current positions of actuator using Potentiometer. Takes 15 readings and returns the 7th read for accuracy  
int MeasureDistance()  
{
  for (int i = 0; i < 15; i++)
  {
    Read [i] = analogRead(Potentiometer);
    delay(1); 
  }
  
  BubbleSort();   //Sort entries 
  return Read[7];
}

//Sorting Algorithm to arrange 15 readings from the potentiometer in ascending order 
void BubbleSort ()      
{
  for (int i = 0; i < 14; i++)
  {
    for (int j = i+1; j>0; j--)
    {
      if (Read[j] < Read[j-1])      
      {
        //Swap adjacent valus inside READ[j]
        int ValRef = Read[j]; // ValRef: Temporary holder value for use during BubbleSort
        Read[j] = Read[j-1];  
        Read[j-1] = ValRef;  
      }
    } 
  }
}

//Up ans Down Pulses created by the actuator based on custom TIME, NUMBER OF CYCLES, DELAY BETWEEN PULSES AND OFFSET BETWEEN GOING DOWN AND UP
void CustomPulses(int Time, int cycles, int Delay, int offset)
{
  digitalWrite(ActuatorNeutral, HIGH);  
  digitalWrite(ActuatorHot, HIGH);  
  
  for(int i = 0; i < cycles; i++)
  {
    digitalWrite(ActuatorHot, LOW);       //Plunge DOWN
    delay (Time + offset);
    digitalWrite(ActuatorHot, HIGH);
  
    delay(Delay);
  
    digitalWrite(ActuatorNeutral, LOW);     //Plunge UP
    delay (Time - offset);
    digitalWrite(ActuatorNeutral, HIGH);
  
    delay(Delay);
  }
  
  digitalWrite(ActuatorNeutral, HIGH);  
  digitalWrite(ActuatorHot, HIGH);  
}

//Unjam function to remove any restrictions 
void Unjam(){
  digitalWrite(ActuatorNeutral, LOW);
  delay(50);
  digitalWrite(ActuatorHot, LOW);
  delay(25);
  digitalWrite(ActuatorNeutral, HIGH);
  delay(25);
  digitalWrite(ActuatorHot, HIGH);
  delay(50);
  digitalWrite(ActuatorNeutral, LOW);
  delay(25);
  digitalWrite(ActuatorHot, LOW);
  delay(25);
  digitalWrite(ActuatorNeutral, HIGH);
  delay(25);
  digitalWrite(ActuatorHot, HIGH);
}

/*Complete Blend Sequence Start to end. Function is called when BlendSequence Button is pushed.
Blend Sequence takes 49s for one iteration without fruits 
*/
void Blend(){ 

  /********* BEGIN MOVING TO CUP POSITION **********/
  digitalWrite(ActuatorHot, LOW);   //Blender is going down
  
  while (MeasureDistance() < TopOfCup - 50)  // check for top of cup
   {
     delay(1);
   }

   while (MeasureDistance() < TopOfCup)  // check for top of cup
   {
      digitalWrite(ActuatorHot, LOW);
      delay(75);
      digitalWrite(ActuatorHot, HIGH);
      delay(75);
   }
    
   
   
   while (MeasureDistance() < TopOfSmoothie + 15)   //MeasureDistance() takes about 15ms to process hence a delay is give evertime a limit is reached 
   {
     digitalWrite(ActuatorHot, LOW);  
     delay(100);
     digitalWrite(ActuatorHot, HIGH);
     delay(100);
   }
  /********* END MOVING TO CUP POSITION **********/

  /********* BEGIN THE BLENDING CYCLE **********/

   // 1. Turn the blender on
   TurnBlenderOn (); // Start the Blender 

   delay(1000); //Blender stays at position for 1s to blend fruits in that region
   
   while (MeasureDistance() < TopOfSmoothie + 10)  
   {
     digitalWrite(ActuatorHot, LOW);
     delay(15);
     digitalWrite(ActuatorHot, HIGH);
     delay(45);
   }
   
   delay(1000);
    
   while (MeasureDistance() < TopOfSmoothie + 15)  
   {
     digitalWrite(ActuatorHot, LOW);
     delay(15);
     digitalWrite(ActuatorHot, HIGH);
     delay(60);
   }
   
   delay(1000);
   
   CustomPulses(40, 5, 220, 8); 

   while(MeasureDistance() < BottomOfCup -25)
   {
     Unjam();
     
     CustomPulses(180, 5, 250, -15);  
     
     if (MeasureDistance() < BottomOfCup)
     {
        CustomPulses(20, 10, 0, 5);
        CustomPulses(50, 8, 250, 15);   
     }
     
     delay(2000);
   }
   
   while(MeasureDistance() < BottomOfCup)
   {
     Unjam();
     
     CustomPulses(150, 5, 250, -25);  
     CustomPulses(10, 20, 0, 2);
     CustomPulses(50, 8, 250, 20);   

     delay(1500); 
   } 
   
   Unjam();

   //Final Stir repated twise
   for(int i = 0; i < 2; i++)
   {
      Unjam();
      
      //Raise blender through cup
      while (MeasureDistance() > TopOfSmoothie + 20)  
      {
        digitalWrite(ActuatorNeutral, LOW);
        delay(15);
        digitalWrite(ActuatorNeutral, HIGH);
      }
      
      while (MeasureDistance() > TopOfSmoothie + 10)  
      {
        digitalWrite(ActuatorNeutral, LOW);
        delay(15);
        digitalWrite(ActuatorNeutral, HIGH);
        delay(30);
      }
      
      delay(3000);

      //Drop blender through cup
      while (MeasureDistance() < BottomOfCup - 50) 
      {
        digitalWrite(ActuatorHot, LOW);
        delay(15);
        digitalWrite(ActuatorHot, HIGH);
      }
  
      while (MeasureDistance() < BottomOfCup - 25)  
      {
        digitalWrite(ActuatorHot, LOW);
        delay(15);
        digitalWrite(ActuatorHot, HIGH);
        delay(15);
      }
      
      while (MeasureDistance() < BottomOfCup)  // check for top of cup
      {
        digitalWrite(ActuatorHot, LOW);
        delay(15);
        digitalWrite(ActuatorHot, HIGH);
        delay(30);
      }
      
      delay(3000); 
  }
/*  
  while(MeasureDistance() > TopOfSmoothie + 10)
  {
    digitalWrite(ActuatorNeutral, LOW);
    delay (15);
    digitalWrite(ActuatorNeutral, HIGH);
  } 

  delay(3000);
  */
  Unjam();

  while(MeasureDistance() > TopOfSmoothie - 18)
  {
    digitalWrite(ActuatorNeutral, LOW);
    delay (15);
    digitalWrite(ActuatorNeutral, HIGH);
  }     

  delay(3000); 
  
  TurnBlenderOff ();  //Turn the Blender OFF 
  
  //Raising the Blender out of smoothie until TopOfSmoothie
  digitalWrite(ActuatorNeutral, LOW);
   
  while(MeasureDistance() > TopOfCup)
  {
   delay (1); 
  } 
   
  Unjam();

  //Few pulses on top of the smoothie to shake some frits off
  CustomPulses(120, 4, 500, -10);
    
  //Bring the blender to the TopPosition
  digitalWrite(ActuatorNeutral, LOW);
   
  while (MeasureDistance()  > TopPosition)
  {
   delay(1);
  }
   
  digitalWrite(ActuatorNeutral, HIGH); 
}

/*Complete Blend Sequence Start to end. Function is called when BlendSequence Button is pushed
 *Clean sequence time: 11s with 230ml of water **Consistent**
*/
void Clean() 
{       
   digitalWrite(ActuatorHot, LOW);  //Lower Blender 
    
   while (MeasureDistance()  < TopOfCup - 100)  //Lowers blender until top of cup
   {
     delay(1);
   }
   
   while (MeasureDistance()  < BottomOfCleaning - 50) 
   {
      digitalWrite(ActuatorHot, LOW);
      delay(15);
      digitalWrite(ActuatorHot, HIGH);
   }

   
   while (MeasureDistance()  < BottomOfCleaning - 10)  //Gets to a position where the shaft almost touches the 3rd outer  water outlet 
   {
     digitalWrite(ActuatorHot, LOW);
     delay(15);
     digitalWrite(ActuatorHot, HIGH);
   }
  
   while (MeasureDistance()  < BottomOfCleaning)  //Gets to a position where the shaft almost touches the 3rd outer  water outlet 
   {
     digitalWrite(ActuatorHot, LOW);
     delay(15);
     digitalWrite(ActuatorHot, HIGH);
     delay(30);
   }
   
   digitalWrite(pinWaterPump, LOW); //Start Pumping water to clean the shaft
  //1 
   TurnBlenderOn ();  //Turns the blender on at the bottom
   delay(1000);
   TurnBlenderOff ();
   delay(500); 
   //1
   digitalWrite(pinWaterPump, HIGH);
   TurnBlenderOn ();
   delay(1000);
   TurnBlenderOff ();
   delay(500); 
  //3
   TurnBlenderOn ();
   delay(1000);
   TurnBlenderOff ();
   delay(500); 
 digitalWrite(pinWaterPump, LOW);
 
   TurnBlenderOn ();
 digitalWrite(ActuatorNeutral, HIGH);

   digitalWrite(ActuatorNeutral, HIGH);  
   Unjam();
   
   while (MeasureDistance()  > CleaningLevel + 15)  //Positions the blades to where the side nozzles shoots the water at
   {
        digitalWrite(ActuatorNeutral, LOW);
        delay(45);
        digitalWrite(ActuatorNeutral, HIGH);
        delay(15);
   }
   
   digitalWrite(ActuatorNeutral, HIGH);

   //Few pulses in the water jet region 
   CustomPulses(50, 2, 35, -15);
    //digitalWrite(pinWaterPump, HIGH);
   CustomPulses(50, 2, 50, -5);

   TurnBlenderOff ();  //Blender OFF 
 /*1*/ //digitalWrite(pinWaterPump, LOW);
   delay(1000);  //Blender off when Jets are still ON for 1.0s
   
   digitalWrite(pinWaterPump, HIGH);  //Turns water pump OFF

   while (MeasureDistance()  > TopOfCup + 20)  //Positions the blades to where the side nozzles shoots the water at
   {
     digitalWrite(ActuatorNeutral, LOW);
     delay(1);  
   }
     
  //Few pulses on top of the cup to shake some remaining water
  CustomPulse_2(120, 3, 400);

   digitalWrite(ActuatorNeutral, LOW);
   
   while (MeasureDistance()  > TopPosition)
   {
    delay(1);
   }
   
   digitalWrite(ActuatorNeutral, HIGH);
}

void loop() 
{
  //Check if Clean button is pressed
  if (digitalRead(pinButtonClean) == LOW) 
  {  
    delay(5);     
    
    if (digitalRead(pinButtonClean) == LOW) 
    {
      digitalWrite(pinLED_READY, LOW);
      digitalWrite(pinLED_IN_USE, HIGH); 
  
      Clean(); //Time: 00:11:05

      
      digitalWrite(pinLED_IN_USE, LOW);
      digitalWrite(pinLED_READY, HIGH);
    }  
  }

  //Check if Blend button is pressed
  if (digitalRead(pinButtonBlend) == LOW) 
  {   
    delay(5);    
  
    if (digitalRead(pinButtonBlend) == LOW)
    {
      digitalWrite(pinLED_READY, LOW);
      digitalWrite(pinLED_IN_USE, HIGH);  
  
      blend_2(); //Time: 01:10:00 
     
      digitalWrite(pinLED_IN_USE, LOW);
      digitalWrite(pinLED_READY, HIGH);
    }  
  }  

}

void blend_2() {
  
  int i;
  /********* BEGIN MOVING TO CUP POSITION **********/
  
  //Blender is going down
  digitalWrite(ActuatorHot, LOW);   
  
  // check for top of cup 
  while (MeasureDistance() < TopOfCup - 100)  
  {
     delay(1);
  }
  
  // check for top of cup
  while (MeasureDistance() < TopOfCup - 10) 
  { 
    digitalWrite(ActuatorHot, LOW);
    delay(30);
    digitalWrite(ActuatorHot, HIGH);
    delay(30);
  }
  
   //MeasureDistance() takes about 15ms to process hence a delay is give evertime a limit is reached 
   while (MeasureDistance() < TopOfSmoothie) 
   {  
     digitalWrite(ActuatorHot, LOW);  
     delay(30);
     digitalWrite(ActuatorHot, HIGH);
     delay(45);
   }
  /********* END MOVING TO CUP POSITION **********/

  /********* BEGIN THE BLENDING CYCLE **********/

   TurnBlenderOn (); // Start the Blender 
   
   digitalWrite(ActuatorHot, HIGH);   
   delay(3000);
  digitalWrite(ActuatorHot, LOW);   
  
   for (i=0; i < 4; i++) 
   {
      while (MeasureDistance() < TopOfSmoothie + ((5*i)+5))  
      {
         digitalWrite(ActuatorHot, LOW);
         delay(15);
         digitalWrite(ActuatorHot, HIGH);
         delay(45);
      }
      delay(1000);
   }
  /* 
  for(int j = 0; j<2; j++)
  {
     while (MeasureDistance() > TopOfSmoothie + ((5*j)+15))  
     {
        digitalWrite(ActuatorNeutral, LOW);
        delay(15);
        digitalWrite(ActuatorNeutral, HIGH);
        delay(45);
     }
   for (i=0; i < 4; i++) 
   {
      while (MeasureDistance() < TopOfSmoothie + ((5*j)+15) + ((5*i)+5))  
      {
         digitalWrite(ActuatorHot, LOW);
         delay(15);
         digitalWrite(ActuatorHot, HIGH);
         delay(45);
      }
      delay(750);
   }
  } 
  */
  
  for (i=0; i < 2; i++) 
  {
     while (MeasureDistance() < BottomOfCup)  
     {
       digitalWrite(ActuatorHot, LOW);
       delay(10);
       digitalWrite(ActuatorHot, HIGH);
       delay(60);
     }
     
     delay(500);
  
     while (MeasureDistance() > TopOfSmoothie + 10)  
     {
        digitalWrite(ActuatorNeutral, LOW);
        delay(30);
        digitalWrite(ActuatorNeutral, HIGH);
        delay(50);
     }
      
     delay(500);
  }

   
  for (i=0; i < 8; i++) 
  {
     while (MeasureDistance() < BottomOfCup)  
     {
       digitalWrite(ActuatorHot, LOW);
       delay(15);
       digitalWrite(ActuatorHot, HIGH);
       delay(30);
     }
     
     delay(500);
  
     while (MeasureDistance() > TopOfSmoothie + 5)  
     {
        digitalWrite(ActuatorNeutral, LOW);
        delay(15);
        digitalWrite(ActuatorNeutral, HIGH);
        delay(30);
     }

     delay(500);
  }


  while (MeasureDistance() > TopOfSmoothie)  
  {
    digitalWrite(ActuatorNeutral, LOW);
    delay(15);
    digitalWrite(ActuatorNeutral, HIGH);
    delay(60);
  }

  delay(3000); 
  
  TurnBlenderOff();

  while (MeasureDistance() > TopOfCup)  
  {
    digitalWrite(ActuatorNeutral, LOW);
    delay(15);
    digitalWrite(ActuatorNeutral, HIGH);
    delay(60);
  }
  
  Unjam();

  //Few pulses on top of the smoothie to shake some frits off
  //CustomPulse_2(120, 4, 400);
   CustomPulses(50, 4, 35, -15); 
  //Bring the blender to the TopPosition
  digitalWrite(ActuatorNeutral, LOW);
   
  while (MeasureDistance()  > TopPosition)
  {
   delay(1);
  }
   
  digitalWrite(ActuatorNeutral, HIGH); 
}

void CustomPulse_2(int Time, int cycles, int Delay)
{
  digitalWrite(ActuatorNeutral, HIGH);  
  digitalWrite(ActuatorHot, HIGH);  
  
  for(int i = 0; i < cycles; i++)
  {
    digitalWrite(ActuatorHot, LOW);       //Plunge DOWN Shortly
    delay (Time);
    digitalWrite(ActuatorHot, HIGH);
  
    delay(Delay - 300);
  
    digitalWrite(ActuatorNeutral, LOW);     //Plunge UP Longer
    delay (Time);
    digitalWrite(ActuatorNeutral, HIGH);
  
    delay(Delay);
  }
  
  digitalWrite(ActuatorNeutral, HIGH);  
  digitalWrite(ActuatorHot, HIGH);  
}


