#include <Arduino.h>
#include <Arduino_FreeRTOS.h>

#define NUM_OF_DISPLAY 2

//Pin connected to Strobe (pin 1) of 4094
int strobePin = A1; ////atmega(pin-25)
//Pin connected to Data (pin 2) of 4094
int dataPin = 13;  //atmega(pin-24)
//Pin connected to Clock (pin 3) of 4094
int clockPin = A0;  //atmega(pin-23)
char shiftOutBuffer[NUM_OF_DISPLAY]={0};
byte segChar[]={

  0b00111111,
  0b00000110,
  0b01011011,
  0b01001111,
  0b01100110,
  0b01101101,
  0b01111101,
  0b00000111,
  0b01111111,
  0b01101111,
  0b00000000,
  



};
int num = 17;
unsigned long period=30000;
void update_display()
{
  int i=0;      
  digitalWrite(strobePin, LOW);
  for(i=NUM_OF_DISPLAY-1;i>-1;i--)
    {     
      shiftOut(dataPin, clockPin, MSBFIRST, segChar[shiftOutBuffer[i]]);             
    }
  digitalWrite(strobePin, HIGH);      
}


void reset_digits()
{
  int i=0;
  for(i=0;i<NUM_OF_DISPLAY;i++)
  {
   shiftOutBuffer[i]=0; 
    
  }
  update_display();

  
}

int segmentshow(int number)
{ int num2=number;
  int i=0;
  reset_digits();
      while(num2 > 0) //do till num greater than  0
    {   
        int mod = num2 % 10;  //split last digit from number
        //printf("%d\n",mod); //print the digit. 
        shiftOutBuffer[i]=mod;
        num2 = num2 / 10;    //divide num by 10. num /= 10 also a valid one 
        i++;
    }
    update_display();
    return 0;
}

class pole
{
public:
  int green_pin;
  int red_pin;
  int yellow_pin;
  int time = 30000; // time in ms
  String color = "red";
  void set_color(String color)
  {
    this->color = color;
    if (color == "green")
    {
      digitalWrite(green_pin, HIGH);
      digitalWrite(red_pin, LOW);
      digitalWrite(yellow_pin, LOW);
    }
    else if (color == "red")
    {
      digitalWrite(green_pin, LOW);
      digitalWrite(red_pin, HIGH);
      digitalWrite(yellow_pin, LOW);
    }
    else if (color == "yellow")
    {
      digitalWrite(green_pin, LOW);
      digitalWrite(red_pin, LOW);
      digitalWrite(yellow_pin, HIGH);
    }
  }
  void set_timer(int time)
  {
    this->time = time;
  }
  int get_timer()
  {
    return time;
  }
  String get_color()
  {
    return color;
  }
};

int driver_pin = A3;
pole traffic_light;
pole pedestrian_light;

// define two tasks for Blink & AnalogRead
void TaskPole(void *pvParameters);
void TaskReadDriverRequest(void *pvParameters);
void TaskRefresh7seg(void *pvParameters);
void TaskBlink(void *pvParameters);
void TaskPrint(void *pvParameters);

// the setup function runs once when you press reset or power the board
void setup()
{ pinMode(A5, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  //digitalWrite(7,HIGH);
  pinMode(driver_pin,INPUT_PULLUP);
  pinMode(strobePin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  
  
  traffic_light.green_pin = 2;
  traffic_light.red_pin = 3;
  traffic_light.yellow_pin = 4;
  pedestrian_light.green_pin = 5;
  pedestrian_light.red_pin = 6;
  pedestrian_light.yellow_pin = 7;
  int i=0;
  for(i=0;i<NUM_OF_DISPLAY;i++)
  {
   shiftOutBuffer[i]=i+0; 
    
  }
  update_display();
  shiftOutBuffer[0]=3;
  shiftOutBuffer[1]=4;
  update_display();
  delay(1000);
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  
  while (!Serial)
  {
    ; // wait for serial port to connect. Needed for native USB, on LEONARDO, MICRO, YUN, and other 32u4 based boards.
  }
  Serial.println("Start");
  // Now set up two tasks to run independently.

  xTaskCreate(
      TaskPole
      , "do tasks for pole"
      , 128 // Stack size
      , NULL
      , 1 // Priority
      , NULL);
  xTaskCreate(
      TaskReadDriverRequest
      , "Driver button read"
      , 128 // Stack size
      , NULL
      , 1 // Priority
      , NULL);
  xTaskCreate(
      TaskRefresh7seg
      , "Refresh the display"
      , 128 // Stack size
      , NULL
      , 1 // Priority
      , NULL);
  xTaskCreate(
      TaskBlink
      , "Blink LED"
      , 128 // Stack size
      , NULL
      , 1 // Priority
      , NULL);
  xTaskCreate(
      TaskPrint
      , "Serial Print"
      , 128 // Stack size
      , NULL
      , 1 // Priority
      , NULL);
  // Now the task scheduler, which takes over control of scheduling individual tasks, is automatically started.
}

void loop()
{
  // Empty. Things are done in Tasks.
  
}

/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/

void TaskPole(void *pvParameters)
{
  (void) pvParameters;
  unsigned long time_now=0;
  time_now=millis();
  
  for (;;)
  {
    /* code */
    period=30000;
    time_now=millis();
    pedestrian_light.set_color("green");
    traffic_light.set_color("red");
    while(1)
      {   


          num=(unsigned long)(millis() - time_now);
          num=period-num;
          if (num<0)
          {
            num=0;
            segmentshow(num/1000);
            time_now=millis();
            break;
            //time is finished for state change
          }
          segmentshow(num/1000);

          
          // Serial.print("time_now is ");
          // Serial.println(time_now);
          // Serial.print("num is ");
          // Serial.println(num);
          // Serial.print("period is ");
          // Serial.println(period);
          vTaskDelay(100  / portTICK_PERIOD_MS);
          
      }
      period=5000;
    pedestrian_light.set_color("yellow");
    traffic_light.set_color("yellow");
        while(1)
      {   


          num=(unsigned long)(millis() - time_now);
          num=period-num;
          if (num<0)
          {
            num=0;
            segmentshow(num/1000);
            time_now=millis();
            break;
            //time is finished for state change
          }
          segmentshow(num/1000);

          
          // Serial.print("time_now is ");
          // Serial.println(time_now);
          // Serial.print("num is ");
          // Serial.println(num);
          // Serial.print("period is ");
          // Serial.println(period);
          vTaskDelay(100  / portTICK_PERIOD_MS);
          
      }
      period=30000;
    pedestrian_light.set_color("red");
    traffic_light.set_color("green");
        while(1)
      {   


          num=(unsigned long)(millis() - time_now);
          num=period-num;
          if (num<0)
          {
            num=0;
            segmentshow(num/1000);
            time_now=millis();
            break;
            //time is finished for state change
          }
          segmentshow(num/1000);

          
          // Serial.print("time_now is ");
          // Serial.println(time_now);
          // Serial.print("num is ");
          // Serial.println(num);
          // Serial.print("period is ");
          // Serial.println(period);
          vTaskDelay(100  / portTICK_PERIOD_MS);
          
      }
      period=5000;
    pedestrian_light.set_color("yellow");
    traffic_light.set_color("yellow");
        while(1)
      {   


          num=(unsigned long)(millis() - time_now);
          num=period-num;
          if (num<0)
          {
            num=0;
            segmentshow(num/1000);
            time_now=millis();
            break;
            //time is finished for state change
          }
          segmentshow(num/1000);

          
          // Serial.print("time_now is ");
          // Serial.println(time_now);
          // Serial.print("num is ");
          // Serial.println(num);
          // Serial.print("period is ");
          // Serial.println(period);
          vTaskDelay(100  / portTICK_PERIOD_MS);
          
      }
    // stopwatch = 0;

    // while (stopwatch < 30000)
    // {
    //   //do nothing
    //   sevseg.setNumber(30000 - stopwatch, 0);
    //   vTaskDelay( 50 / portTICK_PERIOD_MS );
    // }
    // stopwatch = 0;
    // traffic_light.set_color("yellow");
    // while (stopwatch < 30000)
    // {
    //   //do nothing
    //   sevseg.setNumber(30000 - stopwatch, 0);
    //   vTaskDelay( 50 / portTICK_PERIOD_MS );
    // }
    // stopwatch = 0;
    // traffic_light.set_color("red");
    // pedestrian_light.set_color("green");
    // while (stopwatch < 30000)
    // {
    //   //do nothing
    //   sevseg.setNumber(30000 - stopwatch, 0);
    //   vTaskDelay( 50 / portTICK_PERIOD_MS );
    // }
    // stopwatch = 0;
    // pedestrian_light.set_color("red");
    // traffic_light.set_color("yellow");
    // while (stopwatch < 30000)
    // {
    //   //do nothing
    //   sevseg.setNumber(30000 - stopwatch, 0);
    //   vTaskDelay( 50 / portTICK_PERIOD_MS );
    // }
    // vTaskDelay( 50 / portTICK_PERIOD_MS );
    vTaskDelay( 1000 / portTICK_PERIOD_MS );
  }
}

void TaskReadDriverRequest(void *pvParameters)
{
  (void) pvParameters;
  for (;;)
  {
    /* code */
    
    if ((digitalRead(driver_pin) == LOW) && (traffic_light.get_color() == "green"))
    {
      period=period+5000;
      vTaskDelay( 5000 / portTICK_PERIOD_MS );
    }
    vTaskDelay( 100 / portTICK_PERIOD_MS );
  }
}

void TaskRefresh7seg(void *pvParameters)
{ 
  (void) pvParameters;
  
  //vTaskDelay( 10000 / portTICK_PERIOD_MS );
  for (;;)
  { 
    
    
    vTaskDelay( 1000 / portTICK_PERIOD_MS );
  }
}

void TaskBlink(void *pvParameters)  // This is a task.
{
  (void) pvParameters;

  // elapsedMillis elapsedTime;
  unsigned long period2 = 500;
  unsigned long time_now2 = 0;
  for (;;) // A Task shall never return or exit.
  {
    // digitalWrite(A5, HIGH);   // turn the LED on (HIGH is the voltage level)
    // vTaskDelay( 1000 / portTICK_PERIOD_MS ); // wait for one second
    // digitalWrite(A5, LOW);    // turn the LED off by making the voltage LOW
    // vTaskDelay( 1000 / portTICK_PERIOD_MS ); // wait for one second

    if((unsigned long)(millis() - time_now2) > period2){
        time_now2 = millis();
        digitalWrite(A5, !digitalRead(A5));
    }
    // if (elapsedTime>3000)
    // {
    //   /* code */
     vTaskDelay( 100 / portTICK_PERIOD_MS );
    // }
    

  }
}


void TaskPrint(void *pvParameters)  // This is a task.
{
  (void) pvParameters;

  for (;;)
  {
    //Serial.println("printing from independent thread");
    vTaskDelay(1000 / portTICK_PERIOD_MS);

  }
  
  
}









// #include <Arduino.h>
// #include <Arduino_FreeRTOS.h>

// // define two tasks for Blink & AnalogRead
// void TaskBlink( void *pvParameters );
// void TaskAnalogRead( void *pvParameters );

// // the setup function runs once when you press reset or power the board
// void setup() {
  
//   // initialize serial communication at 9600 bits per second:
//   Serial.begin(9600);
  
//   while (!Serial) {
//     ; // wait for serial port to connect. Needed for native USB, on LEONARDO, MICRO, YUN, and other 32u4 based boards.
//   }

//   // Now set up two tasks to run independently.
//   xTaskCreate(
//     TaskBlink
//     ,  "Blink"   // A name just for humans
//     ,  128  // This stack size can be checked & adjusted by reading the Stack Highwater
//     ,  NULL
//     ,  2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
//     ,  NULL );

//   xTaskCreate(
//     TaskAnalogRead
//     ,  "AnalogRead"
//     ,  128  // Stack size
//     ,  NULL
//     ,  1  // Priority
//     ,  NULL );

//   // Now the task scheduler, which takes over control of scheduling individual tasks, is automatically started.
// }

// void loop()
// {
//   // Empty. Things are done in Tasks.
// }

// /*--------------------------------------------------*/
// /*---------------------- Tasks ---------------------*/
// /*--------------------------------------------------*/

// void TaskBlink(void *pvParameters)  // This is a task.
// {
//   (void) pvParameters;

// /*
//   Blink
//   Turns on an LED on for one second, then off for one second, repeatedly.

//   Most Arduinos have an on-board LED you can control. On the UNO, LEONARDO, MEGA, and ZERO 
//   it is attached to digital pin 13, on MKR1000 on pin 6. LED_BUILTIN takes care 
//   of use the correct LED pin whatever is the board used.
  
//   The MICRO does not have a LED_BUILTIN available. For the MICRO board please substitute
//   the LED_BUILTIN definition with either LED_BUILTIN_RX or LED_BUILTIN_TX.
//   e.g. pinMode(LED_BUILTIN_RX, OUTPUT); etc.
  
//   If you want to know what pin the on-board LED is connected to on your Arduino model, check
//   the Technical Specs of your board  at https://www.arduino.cc/en/Main/Products
  
//   This example code is in the public domain.

//   modified 8 May 2014
//   by Scott Fitzgerald
  
//   modified 2 Sep 2016
//   by Arturo Guadalupi
// */

//   // initialize digital LED_BUILTIN on pin 13 as an output.
//   pinMode(A5, OUTPUT);

//   for (;;) // A Task shall never return or exit.
//   {
//     digitalWrite(A5, HIGH);   // turn the LED on (HIGH is the voltage level)
//     vTaskDelay( 500 / portTICK_PERIOD_MS ); // wait for one second
//     digitalWrite(A5, LOW);    // turn the LED off by making the voltage LOW
//     vTaskDelay( 500 / portTICK_PERIOD_MS ); // wait for one second
//   }
// }

// void TaskAnalogRead(void *pvParameters)  // This is a task.
// {
//   (void) pvParameters;
  
// /*
//   AnalogReadSerial
//   Reads an analog input on pin 0, prints the result to the serial monitor.
//   Graphical representation is available using serial plotter (Tools > Serial Plotter menu)
//   Attach the center pin of a potentiometer to pin A0, and the outside pins to +5V and ground.

//   This example code is in the public domain.
// */

//   for (;;)
//   {
//     // read the input on analog pin 0:
//     int sensorValue = analogRead(A0);
//     // print out the value you read:
//     Serial.println(sensorValue);
//     vTaskDelay( 100 / portTICK_PERIOD_MS ); // one tick delay (15ms) in between reads for stability
//   }
// }
