/* This is the code for the 3 slave sequencers*/


#define A PIN_PD2 //also Arduino pin 2
#define B PIN_PD3 //also Arduino pin 3
#define C PIN_PD4 //also Arduino pin 4
#define D PIN_PD5 //also Arduino pin 5
#define E PIN_PD6 //also Arduino pin 6
#define F PIN_PD7 //also Arduino pin 7
#define G PIN_PB0 //also Arduino pin 8
#define H PIN_PB1 //also Arduino pin 9

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



volatile bool spiRead = false;

char inputBuffer[10] = {' ',' ',' ',' ',' ',' ' }; //for serial or SPI input (really only using the first member for the slaves)

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
{0,0,0,0,1,0,0,0}//,

};



void setup() {        

 Serial.begin(115200); //set baud rate to 115200 for usart

  //slave specific stuff
    pinMode(MISO, OUTPUT); // have to send on master in so it set as output
    pinMode(SS, INPUT_PULLUP);
    pinMode(MOSI, INPUT);
    pinMode(SCK, INPUT);

    SPCR |= _BV(SPE); // turn on SPI in slave mode
    SPI.attachInterrupt(); // turn on interrupt



 // Initialize the pins as outputs
 pinMode(A, OUTPUT);  // channel A  
 pinMode(B, OUTPUT);  // channel B
 pinMode(C, OUTPUT);  // channel C 
 pinMode(D, OUTPUT);  // channel D  
 pinMode(E, OUTPUT);  // channel E 
 pinMode(F, OUTPUT);  // channel F
 pinMode(G, OUTPUT);  // channel G 
 pinMode(H, OUTPUT);  // channel H 

}



ISR (SPI_STC_vect)   //Interrupt Service Routine that fires when it gets SPI data
{
  spiRead = true;
  uint8_t readByte = SPDR; //SPDR is an internal register that holds the byte recieved via SPI
  inputBuffer[0] = readByte;
}


void loop (void) 
{

 while(spiRead == false); //this just loops until the interrupt fires when it gets SPI data, then it can run everything else


if (inputBuffer[0] >= '0' && inputBuffer[0] <= '9') //check if it's a digit or letter
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

}  else {
    inputBuffer[0] = 0; //prints a space if its not a valid character
    char2segs((int)inputBuffer[0]);
  }


lightSegments();

spiRead = false; //so it pauses again on the next loop

}




void int2segs (int numberToDisplay)
{

Serial.println (numberToDisplay);


if (numberToDisplay > 9 || numberToDisplay < 0) //bounds check
  {
   return;
  } 


for(int i = 0; i < 8; i++) //fill segment buffer
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

 delay(10);

}






