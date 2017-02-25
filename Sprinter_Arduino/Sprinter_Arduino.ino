
#include <pt.h>   // include protothread library

#define m_0 10
#define m_1 11
#define m_2 12
#define m_3 13

#define p1_1 5
#define p1_2 4
#define p1_3 3
#define p1_4 2

#define p2_1 6
#define p2_2 7
#define p2_3 8
#define p2_4 9

int last_pad_player_1 = 0;
int last_pad_player_2 = 0;
int pad_time =400;
int per_pad_delay = 5;
int player_1[4] = {0, 0, 0, 0};
int player_2[4] = {0, 0, 0, 0};

int randNumber_1 = 0;
int randNumber_2 = 0;

int Player_1_score = 0;
int Player_2_score = 0;

int del = 10000;
unsigned long start_time,start_time2;
unsigned long current_time,current_time2;
unsigned long time_diff;

// each protothread needs one of these
static struct pt pt1, pt2; 

bool isPlayer1Next;
bool isPlayer2Next;

bool isPlay,isdelay;
bool isCheckCommand;
String string;
char command;
void setup() {
  Serial.begin(9600);
  pinMode(m_0, OUTPUT);
  pinMode(m_1, OUTPUT);
  pinMode(m_2, OUTPUT);
  pinMode(m_3, OUTPUT);

// output pads 1-4 for the Player-01
  pinMode(p1_1, INPUT);  // pad 1
  pinMode(p1_2, INPUT);  // pad 2
  pinMode(p1_3, INPUT);  // pad 3
  pinMode(p1_4, INPUT);  // pad 4

// output pads 1-4 for the Player-02
  pinMode(p2_1, INPUT);  // pad 1
  pinMode(p2_2, INPUT);  // pad 2
  pinMode(p2_3, INPUT);  // pad 3
  pinMode(p2_4, INPUT);  // pad 4

  isPlayer1Next = true;
  isPlayer2Next = true;
  isPlay = false;
  isdelay=false;
  isCheckCommand = true;
  PT_INIT(&pt1);  // initialise the two
  PT_INIT(&pt2);  // protothread variables
}



/* This function toggles the LED after 'interval' ms passed */
static int protothread1(struct pt *pt, int interval) {
  static unsigned long timestamp = 0;
  PT_BEGIN(pt);
  while(1) { // never stop 
    /* each time the function is called the second boolean
    *  argument "millis() - timestamp > interval" is re-evaluated
    *  and if false the function exits after that. */
    
    PT_WAIT_UNTIL(pt, millis() - timestamp > interval );
    timestamp = millis(); // take a new timestamp
    performPlayer1();
  }
  PT_END(pt);
}
/* exactly the same as the protothread1 function */
static int protothread2(struct pt *pt, int interval) {
  static unsigned long timestamp = 0;
  PT_BEGIN(pt);
  while(1) {
    PT_WAIT_UNTIL(pt, millis() - timestamp > interval );
    timestamp = millis();
    performPlayer2();
    //Serial.println("th2");
  }
  PT_END(pt);
}

void loop() {

   if (Serial.available() > 0)
   { 
    {string = "";}   
    while(Serial.available() > 0)
    {
      command = Serial.read();
      string += command;
      delay(1);
    }
    isdelay = true;
   }
   delay(5);
   if(isdelay)
   {
    
    String tempst = "start";
    String tempstop = "stop";
    isdelay = false;
   
    if(isCheckCommand==true&&string==tempst)
    {
    Serial.println(string);
    isPlay = true;
    isCheckCommand = false;
    }

    if(isCheckCommand==false&&string==tempstop)
    {
    Serial.println(string);
    isPlay = false;
    isCheckCommand = true;
    }
   }
  if(isPlay==true)
  {
   protothread1(&pt1, 10); // schedule the two protothreads
   protothread2(&pt2, 9); // by calling them infinitely
  }else{

      mux_4_16(0);
      isPlayer1Next = true;
      isPlayer2Next = true;
      Player_1_score = 0;
      Player_2_score = 0;
      randNumber_1 = 0;
      randNumber_2 = 0;
    }
}

void readData()
{
  if (Serial.available() > 0) 
    {string = "";}   
    while(Serial.available() > 0)
    {
      command = Serial.read();
      string += command;
      delay(1);
    }
    String tempst = "start";
    String tempstop = "stop";
    if(isCheckCommand==true&&string==tempst)
    {
    Serial.println(string);
    isPlay = true;
    isCheckCommand = false;
    }

    if(isCheckCommand==false&&string==tempstop)
    {
    Serial.println(string);
    isPlay = false;
    isCheckCommand = true;
    }
  }

void performPlayer1()
{
  if(isPlayer1Next)
  {
   randNumber_1 = random(5,9);
  while(randNumber_1 == last_pad_player_1)
    randNumber_1 = random(5,9) ;  

  while(((last_pad_player_1 == 5) && (randNumber_1==7)) || ((last_pad_player_1 == 7) && (randNumber_1==5)))
    randNumber_1 = random(5,9);
    
  last_pad_player_1 = randNumber_1;

   start_time = millis();
   current_time = millis();
   isPlayer1Next = false;
  }
  current_time = millis();
  if(current_time-start_time > 4000)
  {
    isPlayer1Next = true;
    }
  mux_4_16(randNumber_1);
    
  current_time = millis();
  if((digitalRead(p1_1)==HIGH) && (randNumber_1 == 5))
    {
      Player_1_score+=1;
      Serial.print("p1:");
      Serial.print(Player_1_score);
      Serial.print("\n");    
      mux_4_16( 0);
      isPlayer1Next = true;
     // break;
    }
  if((digitalRead(p1_2)==HIGH) && (randNumber_1==6))   
    {
      Player_1_score+=1;
      Serial.print("p1:");
      Serial.print(Player_1_score);
      Serial.print("\n");
      mux_4_16( 0);
      isPlayer1Next = true;

     // break;  
    }

  if((digitalRead(p1_3)==HIGH) && (randNumber_1  == 7))   
    {
      Player_1_score+=1;
      Serial.print("p1:");
      Serial.print(Player_1_score);
      Serial.print("\n");
      mux_4_16( 0);
      isPlayer1Next = true;
     // break;
    }
  if((digitalRead(p1_4) ==HIGH) && (randNumber_1  == 8))   
    {
      Player_1_score+=1;
      Serial.print("p1:");
      Serial.print(Player_1_score);
      Serial.print("\n");
     mux_4_16( 0);
     isPlayer1Next = true;

 //    break;
    }
 
   }


void performPlayer2()
{
    if(isPlayer2Next)
    {
    randNumber_2 = random(1,5);
    
  while(((last_pad_player_2 == 1) && (randNumber_2==3)) || ((last_pad_player_2 == 3) && (randNumber_2==1)))
    randNumber_2 = random(1,5);
 
  while(randNumber_2 == last_pad_player_2)
    randNumber_2 = random(1,5) ;  
  
  last_pad_player_2 = randNumber_2;

     start_time2 = millis();
   current_time2 = millis();
   isPlayer2Next = false;
    }
  current_time2 = millis();
  if(current_time2-start_time2 > 4000)
  {
    isPlayer2Next = true;
    }
    
    mux_4_16(randNumber_2);
    
  current_time = millis();
  if((digitalRead(p2_1)==HIGH) && (randNumber_2 == 1))
    {
      Player_2_score+=1;
      Serial.print("p2:");
      Serial.print(Player_2_score);
      Serial.print("\n");    

      mux_4_16( 0);
      isPlayer2Next = true;
     // break;
    }
  if((digitalRead(p2_2)==HIGH) && (randNumber_2==2))   
    {
      Player_2_score+=1;
      Serial.print("p2:");
      Serial.print(Player_2_score);
      Serial.print("\n");    
      mux_4_16( 0);
      isPlayer2Next = true;

     // break;  
    }

  if((digitalRead(p2_3)==HIGH) && (randNumber_2  == 3))   
    {
      Player_2_score+=1;
      Serial.print("p2:");
      Serial.print(Player_2_score);
      Serial.print("\n");    
      mux_4_16( 0);
      isPlayer2Next = true;
     // break;
    }
  if((digitalRead(p2_4) ==HIGH) && (randNumber_2  == 4))   
    {
      Player_2_score+=1;
      Serial.print("p2:");
      Serial.print(Player_2_score);
      Serial.print("\n");    
      mux_4_16( 0);
      isPlayer2Next = true;

 //    break;
    }
  
  }

void mux_4_16(int output)
{
  if(output == 0)
  {
    digitalWrite(m_0, LOW);
    digitalWrite(m_1, LOW);
    digitalWrite(m_2, LOW);
    digitalWrite(m_3, LOW);
   }

   else if(output == 1)
  {
    digitalWrite(m_0, HIGH);
    digitalWrite(m_1, LOW);
    digitalWrite(m_2, LOW);
    digitalWrite(m_3, LOW);
   }

  else if(output == 2)
  {
    digitalWrite(m_0, LOW);
    digitalWrite(m_1, HIGH);
    digitalWrite(m_2, LOW);
    digitalWrite(m_3, LOW);
   }

    else if(output == 3)
  {
    digitalWrite(m_0, HIGH);
    digitalWrite(m_1, HIGH);
    digitalWrite(m_2, LOW);
    digitalWrite(m_3, LOW);
   }

  else if(output == 4)
  {
    digitalWrite(m_0, LOW);
    digitalWrite(m_1, LOW);
    digitalWrite(m_2, HIGH);
    digitalWrite(m_3, LOW);
   }

 else if(output == 5)
  {
    digitalWrite(m_0, HIGH);
    digitalWrite(m_1, LOW);
    digitalWrite(m_2, HIGH);
    digitalWrite(m_3, LOW);
   } 

  else if(output == 6)
  {
    digitalWrite(m_0, LOW);
    digitalWrite(m_1, HIGH);
    digitalWrite(m_2, HIGH);
    digitalWrite(m_3, LOW);
   }

    else if(output == 7)
  {
    digitalWrite(m_0, HIGH);
    digitalWrite(m_1, HIGH);
    digitalWrite(m_2, HIGH);
    digitalWrite(m_3, LOW);
   }

 else if(output == 8)
  {
    digitalWrite(m_0, LOW);
    digitalWrite(m_1, LOW);
    digitalWrite(m_2, LOW);
    digitalWrite(m_3, HIGH);
   }

   else if(output == 9)
  {
    digitalWrite(m_0, HIGH);
    digitalWrite(m_1, LOW);
    digitalWrite(m_2, LOW);
    digitalWrite(m_3, HIGH);
   }

    else if(output == 10)
  {
    digitalWrite(m_0, LOW);
    digitalWrite(m_1, HIGH);
    digitalWrite(m_2, LOW);
    digitalWrite(m_3, HIGH);
   }

   else if(output == 11)
  {
    digitalWrite(m_0, HIGH);
    digitalWrite(m_1, HIGH);
    digitalWrite(m_2, LOW);
    digitalWrite(m_3, HIGH);
   }  

   else if(output == 12)
  {
    digitalWrite(m_0, LOW);
    digitalWrite(m_1, LOW);
    digitalWrite(m_2, HIGH);
    digitalWrite(m_3, HIGH);
   }  
   // rest pins are unused so skipping
  }
