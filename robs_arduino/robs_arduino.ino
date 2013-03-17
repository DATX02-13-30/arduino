/* ========================================================================== */
/*                                                                            */
/*   Filename.c                                                               */
/*   (c) 2012 Author                                                          */
/*                                                                            */
/*   Description                                                              */
/*                                                                            */
/* ========================================================================== */

int tagLength = 14;
byte lastTag[14] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0};


void setup(){
  Serial.flush(); //Flushing serial for correct first read.
  Serial.begin(9600);
}
 
//function to read tag, we recieve the array we write the info to
void readTag(byte* inputTag){
   
  byte* updateTag = inputTag;
   
  //Delay for data to arrive into the serial buffer.
  if(Serial.available()>0) {
    delay(200);
    //Transfering data from serial to the tag in the loop
    for(int i = 0; i < tagLength; i++){
      // read the incoming number on serial RX
      *updateTag = Serial.read();
      updateTag++;
      //transfering it to the array.
    }
  }
  //we flush the serial so we dont have residual data when we read it again.
  Serial.flush();
  
}
 
void printTag(byte* inputTag ){
  byte* tag = inputTag;
  Serial.println("New Tag");
  //printing data
  for(int i = 1; i < 11; i++){
    //Check if it's a number or letter
    if((47 < tag[i]) && (tag[i] < 58)){
      Serial.print((tag[i]-48), HEX);
      Serial.print(" ");
    }
    else{
      Serial.print((tag[i]-55), HEX);
      Serial.print(" ");
    }     
  }
  Serial.println("\nEnd of Tag");
}
 
boolean crcCheck(byte* inputTag) {
  byte checkValue1 = 0;
  byte checkValue2 = 0;
  for(int i=1;i<11; i+=2) {
    checkValue1 = checkValue1 ^ inputTag[i];
  }
  for(int i=2;i<11; i+=2) {
    checkValue2 = checkValue2 ^ inputTag[i];
  }
  return (checkValue1 == inputTag[11])/*&&(checkValue2 == inputTag[12]))*/;
}

void saveTag(byte* inputTag)
{
  for(int i = 0; i < tagLength; i++)
  {
    lastTag[i] = inputTag[i];
  }
}

boolean sameTag(byte* inputTag) {
  boolean notSame = false;
  for(int i = 0; i < tagLength; i++)
  {
    if(inputTag[i] != lastTag[i])
    {
      notSame = true;
    }    
  }
  return notSame;
}

void loop()
{
  byte tag[tagLength];
  readTag(tag);
  if(crcCheck(tag) && sameTag(tag)){
    printTag(tag);
    saveTag(tag);
  }
}