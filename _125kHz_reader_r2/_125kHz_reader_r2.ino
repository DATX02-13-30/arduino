//array to store the latest correct read tag.
byte stored_Tag[10];

void setup(){
  //Just initialize the stored tag to this to be able to compare to something.
//  stored_Tag = {0,0,0,0,0,0,0,0,0,0};
  Serial.flush();      //Flushing serial for correct first read.
  Serial.begin(9600);
}

//function to read tag, we recieve the array we write the info to
void readTag(byte* pt){
  byte* pt_tag = pt;
  //Delay for data to arrive into the serial buffer.
  delay(200);

  //Transfering data from serial in the loop
  for(int i = 0; i < 14; i++){
    // read the incoming number on serial RX
    *pt_tag = Serial.read();
    //Check if the ascii sign is a number or letter.
    if((47 < *pt_tag) && (*pt_tag < 58)){
      //It's a number and we adjust it to normal HEX number
      *pt_tag -= 48;
    }
    else{
      //It's a letter so we adjust it to HEX letter.
      *pt_tag -= 55;
    }
    //increment the pointer.
    pt_tag++;
  }
  //we flush the serial so we dont have residual data when we read it again.
  Serial.flush();
}

//function to print out the tag data.
void printTag(byte* pt){
  byte* print_tag = pt;

  Serial.println("New Tag");
  //Printing the tag data 
  for(int i = 1; i < 11; i++){
    Serial.print(print_tag[i], HEX);
    //Add space to make it more readable.
    Serial.print(" ");
  }
  //We print out to mark the end of the tag.
  Serial.println();
  Serial.println("End of Tag");
}

//Function that checks if the read tag is correct => checking that the read is succesful.
boolean crcCheck(byte* pt){
  byte* crc_tag = pt;
  
  //This is an xor(^) operation done between the numbers. 
  byte CRC1 = (crc_tag[1] ^ crc_tag[3] ^ crc_tag[5] ^ crc_tag[7] ^ crc_tag[9]);
  byte CRC2 = (crc_tag[2] ^ crc_tag[4] ^ crc_tag[6] ^ crc_tag[8] ^ crc_tag[10]);
  
  //We compare our Checksum with the checksum recieved when we read the tag.
  return ((CRC1 == crc_tag[11]) && (CRC2 == crc_tag[12]));
}

//Used to se if it's a new tag or an old one that has been reread.
boolean newTagCheck(byte* pt){ 
  byte* new_Tag = pt;
  //We use this to count identical parts of the old tag and new tag ids.
  int identical = 0;
  //iterate through the UID of the tag and compare it to the stored one.
  for(int i=1; i<11; i++){
    //Check if the specific tag number is identical.
    if(new_Tag[i] == stored_Tag[i-1]){
      //The specific number is identical. We increment variable identical.
      identical++;
    }
  }
  //we return true if all tag numbers were identical.
  return (identical != 10);
}

//Used to store the read tag.
void storeTag(byte* pt){
  byte* save_Tag = pt;
  //We transfer the newley readtag to the stored_tag array.
  for(int i=1; i<11; i++){
    stored_Tag[i-1] = save_Tag[i];
  }
}

void loop() 
{
  byte tag[14];
  //Checking if there is data on the serial.
  if(Serial.available() > 0){  
    //we call on the read function and save the array to a pointer.
    readTag(tag);
    //we check the checksum and if it's a new tag.
    //If checksum is correct and the tag is new we print it.
    if(crcCheck(tag) && newTagCheck(tag)){
      //We store the new tag.
      storeTag(tag);
      //we send the tag to the print function.
      printTag(tag);
    }
  }
}






