// One-player hit the ball agasint the wall game
//
// Uses 8x8 LED array for display
// Display circuit from Sunfounder
// http://www.sunfounder.com/index.php?c=case_in&a=detail_&id=100&name=Arduino
//
// paddle is potentiometer with wiper read on Analog pin 0
//
//
// basic program structure:
// Initialize
// Loop
//  update ball velocity based on interaction with walls and paddle
//  update ball position based on velocity
//  read and update paddle position
//  store paddle and ball in bitmap
//  push bitmap out to LED array

const int latchPin = 8;//Pin connected to ST_CP of 74HC595
const int clockPin = 12;//Pin connected to SH_CP of 74HC595 
const int dataPin = 11; //Pin connected to DS of 74HC595 
const int analogPin = A0;//the analog input pin attach to potentiometer
int inputValue = 0;//variable to store the value coming from sensor
int ydir = 0; // 0 = down, 1 = up
int xdir = 0; // 0, 1, 2 = left, middle, right
int btick = 4; // counter to set speed of game
int brow=8; // ball row
int bcol=4; // ball column
int col=4; // paddle column
int data[] = {0x01,0x00,0x00,0x00,0x00,0x00,0x40,0x80}; //display buffer
//columns left to right, lsb is bottom, msb is top 

void setup ()
{
  //set pins to output
  pinMode(latchPin,OUTPUT);
  pinMode(clockPin,OUTPUT);
  pinMode(dataPin,OUTPUT);
  digitalWrite(dataPin,LOW); 
  digitalWrite(clockPin,LOW); 
  digitalWrite(latchPin,LOW); //ground latchPin and hold low for as long as you are transmitting
}
void loop()
{
  btick--; // decrement the delay counter
  if(btick==0) // if ready, go process
  {
    btick=2; // reinitialize delay counter
    // first update the ball velocity components
    if(brow<1) // test if ball at bottom?
      {
      // did it hit the paddle?
      if(bcol==col || bcol==col-1 || bcol==col+1 )
        // then turn it around and randomize direction
        {ydir=1; xdir=random(0,3);} // make xdir ~ col+/-1
      else
        {brow=12;}
      } // then go up!
    if(brow>6) // top?
      {ydir=0; xdir=random(0,3);} // go down
    if(bcol<1) // left side?
      {xdir=2;}
    if(bcol>6) //right side?
      {xdir=0;}

    // then update the ball position
    if(ydir==0) // down
      {brow--;}
    else
      {brow++;} // up
   
    switch (xdir) {
    case 0: //left
      bcol--;
      break;
    case 1: //middle
      break;
    case 2: //right
      bcol++;
      break;
    }
  }
  // now update the paddle position
  inputValue = analogRead(analogPin);//read the value from the sensor
  col = map(inputValue+64,0,1023,1,6);//Convert from 0-1023 proportional to the screen position. Added an offset to reducing dithering.
  // screen is 8 pixels wide (0-7), but paddle is 3 wide. So the paddle position is from 1 to 6.  Serial.println(col);
  
  // NOW UPDATE THE BIT MAP
  
  //store paddle in the bit map
  for(int iCol = 0; iCol < 8; iCol++)
  {
    if (iCol!=col) 
    {data[iCol]=0x00;}
    else
    {data[iCol-1]=0x01;
    data[iCol]=0x01;
    data[iCol+1]=0x01;
    iCol++; // skip a position so it doesn't get zeroed on next loop iteration.
    }
  }
  for(int iCol = 0; iCol <8; iCol++)
  {
    if (iCol==bcol)
    {data[iCol] ^= 0x01<<brow;} //left shift a 1 to the ball position and OR with existing
  }
  
  //THIS IS THE display loop
  for(int t = 0;t < 10;t ++)//Show repeated 10 times
  {
    //scan the columns in data
    int dat = 0x80;
    for(int num = 0; num < 8; num++)
    {
        delay(1); //wait for a millisecond    
        shiftOut(dataPin,clockPin,MSBFIRST,data[num]);
        shiftOut(dataPin,clockPin,MSBFIRST,~dat);    
        //return the latch pin high to signal chip that it 
        //no longer needs to listen for information
        digitalWrite(latchPin,HIGH); //pull the latchPin to save the data
        //delay(1); //wait for a microsecond
        digitalWrite(latchPin,LOW); //ground latchPin and hold low for as long as you are transmitting
        dat = dat>>1;  
    }
  }
}

