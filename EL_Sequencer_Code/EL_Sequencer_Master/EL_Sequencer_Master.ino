/* This is code for the Master EL Sequencer (which is also digit 0)*/

#define A PIN_PD2 //also Arduino pin 2
#define B PIN_PD3 //also Arduino pin 3
#define C PIN_PD4 //also Arduino pin 4
#define D PIN_PD5 //also Arduino pin 5
#define E PIN_PD6 //also Arduino pin 6
#define F PIN_PD7 //also Arduino pin 7
#define G PIN_PB0 //also Arduino pin 8
#define H PIN_PB1 //also Arduino pin 9

#define SLAVE1 PIN_PC2
#define SLAVE2 PIN_PC3
#define SLAVE3 PIN_PC4

#define MASTER 1 


//#define SERIALENTRY 1 //if you want to type stuff into the serial monitor to display, uncomment this

//#define CYCLETHROUGHCHARACTERS 1 //if you want to scroll through each character 

#define SCROLLTHROUGHSTRING 1 //if you want to scroll through whatever is in quote[] uncomment this


// if you want to wire these up to a switch to change betwwen modes, I'll need to do this differently because preprocessor macros can't be changed at runtime

#include <SPI.h>

/*

     C  2
  | ^^^^^^ |
  | |G  H| |
  | |6  7| |
 B|  \  /  |D
 1|  /  \  |3
  | |F  E| |
  | |5  4| |
  | ______ |
     A  0
*/

char quote[] = //you can change this to anything you want, this one is dumb but getting threatened by a microcontroller is hilarious to me
{"All you motherfuckers are gonna pay  You are the ones who are the ball lickers  "  
"Were gonna fuck your mothers while you watch and cry like little whiny bitches  "  
"Once we get to Hollywood and find those Miramax fucks who is makin the movie  "
"were gonna make them eat our shit  then shit out our shit  and then eat that shit "
"thats made up of our shit that we made em eat  Then all you motherfucks are next  "
"Love  Jay and Silent Bob "};


const int scrollSpeed = 50; //change this to scroll faster or slower (you can add a pot and do analogRead() to be able to change it easily)

int scrollLocation = 0; // for scrolling through a quote
int cycleLocation = 0; //for cycling through all the characters

char inputBuffer[15]; //for serial or SPI input (really only need 5, 4 digits and a newline but whatever)

int segBuffer[8] =  {0,0,0,0,0,0,0,0}; //whatever's in here is what's being displayed

int numberSegs [10][8] = 
{
{1,1,1,1,0,0,0,0},//0
{0,1,0,0,0,0,0,0},//1
{1,0,1,0,0,1,0,1},//2
{1,0,1,0,1,0,0,1},//3
{0,0,0,1,0,0,1,1},//4
{1,0,1,0,1,0,1,0},//5
{1,1,1,0,1,1,0,0},//6
{0,0,1,1,0,0,0,0},//7
{1,0,1,0,1,1,1,1},//8
{0,0,1,1,0,0,1,1} //9
};

const int numberOfLetters = 32; //in case you add more letters, just change this so bounds checking still works

int letterSegs [numberOfLetters][8] = 
{
{0,0,0,0,0,0,0,0},//SPACE (also makes it so A=1, B=2, ...)
{0,1,1,1,0,0,1,1},//A
{1,1,1,0,1,1,1,1},//B
{1,1,1,0,0,0,0,0},//C
{1,0,0,1,1,1,0,0},//D
{1,1,1,0,0,1,1,0},//E
{0,1,1,0,0,1,1,0},//F
{1,1,1,0,1,0,0,0},//G
{0,1,0,1,1,1,1,1},//H
{0,0,0,1,0,0,0,0},//I
{1,0,0,1,0,0,0,0},//J
{0,1,0,0,1,0,1,1},//K
{1,1,0,0,0,0,0,0},//L
{0,1,0,1,0,0,1,1},//M
{0,1,0,1,1,0,1,0},//N
{1,1,1,1,0,0,0,0},//O
{0,1,1,0,0,0,1,1},//P
{0,0,1,0,1,0,1,1},//Q
{0,1,1,0,1,0,0,1},//R
{1,0,1,0,1,0,1,0},//S
{1,1,0,0,0,1,1,0},//T
{1,1,0,1,0,0,0,0},//U
{0,0,0,0,0,0,1,1},//V
{0,1,0,1,1,1,0,0},//W
{0,0,0,0,1,1,1,1},//X
{1,0,0,1,0,0,1,1},//Y
{1,0,1,0,0,1,0,1},//Z
{0,0,1,0,0,0,1,1},//°
{0,0,1,0,0,1,0,1},//?
{1,0,0,0,0,0,0,0},//_
{0,0,0,0,1,0,0,0} //,

};



void setup() {        

 Serial.begin(115200); //set baud rate to 115200 for usart

  //Master specific stuff
    pinMode(SS, OUTPUT); 
    pinMode(MISO, INPUT);
    pinMode(MOSI, OUTPUT);
    pinMode(SCK, OUTPUT);

    digitalWrite(SS, HIGH); // disable Slave Select for the master

    SPI.begin();

    SPI.setClockDivider(SPI_CLOCK_DIV128); //bit rate = 16 MHz/128 = 125 kbit/sec slows everthing down because we're only sending 3 bytes
                                           //I'm testing these on Arduinos which have a 16MHz crystal, on your boards there's an 8MHz crystal
                                           //so if it doesn't work, try SPI_CLOCK_DIV64 so it's back to 125 kbit/sec

    pinMode(SLAVE1, OUTPUT);  // goes to chip select (CSN) on slave 1 (these are active LOW signals)
    pinMode(SLAVE2, OUTPUT);  // goes to chip select (CSN) on slave 2
    pinMode(SLAVE3, OUTPUT);  // goes to chip select (CSN) on slave 3 

    digitalWrite(SLAVE1, HIGH); //deselect slaves for now
    digitalWrite(SLAVE2, HIGH);
    digitalWrite(SLAVE3, HIGH);



 pinMode(A, OUTPUT);  // channel A  
 pinMode(B, OUTPUT);  // channel B
 pinMode(C, OUTPUT);  // channel C 
 pinMode(D, OUTPUT);  // channel D  
 pinMode(E, OUTPUT);  // channel E 
 pinMode(F, OUTPUT);  // channel F
 pinMode(G, OUTPUT);  // channel G 
 pinMode(H, OUTPUT);  // channel H 

}



void loop (void) 
{

#ifdef SERIALENTRY 

    while (!Serial.available()); //loops until you send something via the serial monitor

    delay(2);    //probably unecessary

    for (int i = 0; i < 10 ; i++)
    {
      inputBuffer[i] = Serial.read();
    }

    delay(2);
#endif


#ifdef CYCLETHROUGHCHARACTERS // if it's uncommented at the top 

for (int i = 0; i < 4; i++ )
{
  if ((cycleLocation + i) <= 9)
    {

      inputBuffer[i] = cycleLocation + i + '0';

    } else if ((cycleLocation + i) > 9 && (cycleLocation + i) <= (numberOfLetters+9))
    {
      inputBuffer[i] = (cycleLocation-10) + i  + 'A'; //the character before A on the acsii chart

    } else {
      inputBuffer[i] = ' ';
    }

}
cycleLocation++;
if (cycleLocation > (numberOfLetters + 12))cycleLocation = 0;
delay(scrollSpeed);

#endif


#ifdef SCROLLTHROUGHSTRING  // if it's uncommented at the top 

for (int i = 0; i < 4; i++ )
{

inputBuffer[i] = quote[(scrollLocation + i)% sizeof(quote)];
if ((scrollLocation + i) > (sizeof(quote)-1) )
{
  inputBuffer[i] = ' ';
}
}

scrollLocation++;

if (scrollLocation > sizeof(quote))scrollLocation = 0;

delay(scrollSpeed);


#endif




uint8_t digit1send = inputBuffer[1]; //I'm sending just one char of the raw serial input to each slave
uint8_t digit2send = inputBuffer[2]; //so all the processing below happens on each board separately and the code is basically the same
uint8_t digit3send = inputBuffer[3]; 


    digitalWrite(SLAVE1, LOW); //selects the slave by pulling its SS (or CSN on these) line low
    delay(15);                     //if the slaves are missing characters, try increasing this delay
    SPI.transfer(digit1send);
    delay(40);
    digitalWrite(SLAVE1, HIGH);


    digitalWrite(SLAVE2, LOW);
    delay(15);
     SPI.transfer(digit2send);
    delay(40);
    digitalWrite(SLAVE2, HIGH);


    digitalWrite(SLAVE3, LOW);
    delay(15);
     SPI.transfer(digit3send);
    delay(40);
    digitalWrite(SLAVE3, HIGH);




if (inputBuffer[0] >= '0' && inputBuffer[0] <= '9') //check if it's a digit or character
 {
  
    inputBuffer[0] -= '0'; //'0' is the same as 48, which is where the numbers start in the ASCII table

    int2segs(inputBuffer[0]);

 } else if (toupper(inputBuffer[0]) >= 'A' && toupper(inputBuffer[0]) <= 'Z' || isPunct(inputBuffer[0]) || isSpace(inputBuffer[0])) 
  {
    inputBuffer[0] = toupper(inputBuffer[0]); // so we don't have to deal with lowercase

    Serial.print("letter   =\t");

    Serial.println(inputBuffer[0]);

    inputBuffer[0] = char2arrayIndex(inputBuffer[0]); 

    Serial.print("index    =\t");

    Serial.println((int)inputBuffer[0]); //that (int) is a cast, so it just converts the char to an int temporarily

    char2segs((int)inputBuffer[0]);

  } else {
    inputBuffer[0] = 0; //prints a space if its not a valid character
    char2segs((int)inputBuffer[0]);
  }

lightSegments();


while (Serial.available()) int trash = Serial.read(); // flush the serial buffer so if you type in too many chars it doesnt run twice
                                                      // the "int trash =" part is there because sometimes compilers optimize things out when the result isn't put anywhere (probably not in this case but whatever)

}




void int2segs (int numberToDisplay)
{

  Serial.println (numberToDisplay);


  if (numberToDisplay > 9 || numberToDisplay < 0) //bounds check
    {
      return;
    } 


  for(int i = 0; i < 8; i++)
    {
      segBuffer[i] = numberSegs[numberToDisplay][i];
    }

}


void char2segs (int charToDisplay)
{

  if (charToDisplay > numberOfLetters || charToDisplay < 0) //bounds check
    {
      return;
    } 


  for(int i = 0; i < 8; i++)
    {
      segBuffer[i] = letterSegs[charToDisplay][i];
    }

}


int char2arrayIndex (char readChar) //converts raw ASCII values to the index of the letter array
{

  switch (readChar) //if you add more charachters, just add a case for it here and return the index
    {
      case ' ':
        return 0;
        break;

      case '°':
        return 27;
        break;
      case '?':
        return 28;
        break;
      case '_':
        return 29;
        break;
      case ',':
        return 30;
        break;

      default:
        return readChar - 64; //this is because the uppercase alphabet in ASCII starts at 65, if you just cast a char to an int it will be the ASCII value (65 for A...)
        break;
    }

}


void lightSegments(void)
{

  Serial.print("segments =\t");

    for (int i = 0; i < 8; i++)
     {
      Serial.print(segBuffer[i]); 

      if (i < 7) Serial.print(", ");
     }

    Serial.println(" \n");
    

      digitalWrite(A, LOW);
      digitalWrite(B, LOW);
      digitalWrite(C, LOW);
      digitalWrite(D, LOW);
      digitalWrite(E, LOW);
      digitalWrite(F, LOW);
      digitalWrite(G, LOW);
      digitalWrite(H, LOW);

      delay(15);// this is where we had some issues with the top/bottom segments staying on, so if they do, this is where you should mess with adding delays

  for (int i = 0; i < 8; i++)
    {

      if(segBuffer[i] == 1)
        {
          digitalWrite(i+2, HIGH);
        }

    } 
  delay(100);

}





