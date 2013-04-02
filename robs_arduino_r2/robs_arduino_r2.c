/*
Setup and global variables:
*/

//array to store the latest correct read tag.
byte stored_Tag[10];

/*
  Pin to connect to the circuit
  Setting the pin LOW will tune the coil
  meaning it will respond as a high signal to the reader
  Setting the pin to HIGH will detune coil
  meaning the reader will see it as a low signal
*/
int coil_pin = 2;

//Read or not read
int read;
int sent;

void setup(){
  // Just initialize the stored tag to this to be able to compare to something.
  stored_Tag = {0,0,0,0,0,0,0,0,0,0};
  // No tag is read.
  read = 0;
  sent = 0;
  // Sender initialize
  // Configure of terminal.
  Serial.begin(9600);  
  // Set pin as output
  pinMode(coil_pin, OUTPUT);    
  // Start it as low
  digitalWrite(coil_pin, LOW);
  
  // Receiver initialize
  Serial.flush(); //Flushing serial for correct first read.
  Serial.begin(9600);
}

/*
Receiver code:
*/

// Function to read tag, we recieve the array we write the info to
void readTag(byte* pt){
  byte* pt_tag = pt;
  // Delay for data to arrive into the serial buffer.
  delay(200);

  // Transfering data from serial in the loop
  for(int i = 0; i < 14; i++){
    // Read the incoming number on serial RX
    *pt_tag = Serial.read();
    // Check if the ascii sign is a number or letter.
    if((47 < *pt_tag) && (*pt_tag < 58)){
      // It's a number and we adjust it to normal HEX number
      *pt_tag -= 48;
    }
    else{
      // It's a letter so we adjust it to HEX letter.
      *pt_tag -= 55;
    }
    // Increment the pointer.
    pt_tag++;
  }
  // Flush the serial so we dont have residual data when we read it again.
  Serial.flush();
}

// Function to print out the tag data.
void printTag(byte* pt){
  byte* print_tag = pt;

  Serial.println("New Tag");
  // Printing the tag data
  for(int i = 1; i < 11; i++){
    Serial.print(print_tag[i], HEX);
    // Add space to make it more readable.
    Serial.print(" ");
  }
  // We print out to mark the end of the tag.
  Serial.println();
  Serial.println("End of Tag");
}

// Function that checks if the read tag is correct => checking that the read is succesful.
boolean crcCheck(byte* pt){
  byte* crc_tag = pt;
  
  // This is an xor(^) operation done between the numbers.
  byte CRC1 = (crc_tag[1] ^ crc_tag[3] ^ crc_tag[5] ^ crc_tag[7] ^ crc_tag[9]);
  byte CRC2 = (crc_tag[2] ^ crc_tag[4] ^ crc_tag[6] ^ crc_tag[8] ^ crc_tag[10]);
  
  // Compare the Checksum with the checksum recieved when the tag is read.
  return ((CRC1 == crc_tag[11]) && (CRC2 == crc_tag[12]));
}

//Used to se if it's a new tag or an old one that has been reread.
boolean newTagCheck(byte* pt){
  byte* new_Tag = pt;
  // If everyone are same, this stay false
  boolean difference = false;
  // Iterate through the UID of the tag and compare it to the stored one.
  for(int i=1; i<11; i++){
    // Check if the specific tag number is identical.
    if(new_Tag[i] != stored_Tag[i-1]){
      // If it isn't identical, return difference true and stop the for-loop
      difference = true;
      break;
    }
  }
  // Return true if all tag numbers were identical.
  return difference;
}

// Used to store the read tag.
void storeTag(byte* pt){
  byte* save_Tag = pt;
  // Transfer the newley readtag to the stored_tag array.
  for(int i=1; i<11; i++){
    stored_Tag[i-1] = save_Tag[i];
  }
}

void loop()
{
  if(!read)
  {
      byte tag[14];
      //Checking if there is data on the serial.
      if(Serial.available() > 0){
        // Call on the read function and save the array to a pointer.
        readTag(tag);
        // Check the checksum and if it's a new tag.
        // If checksum is correct and the tag is new, print it.
        if(crcCheck(tag) && newTagCheck(tag)){
          // Store the new tag.
          storeTag(tag);
          // Send the tag to the print function.
          printTag(tag);
          // Stop reading and start transmitting.
          read = 1;
        }
      }
  }
  else
  {
        if(!send)
        {
            // Hex format of the tags UID that we want to spoof.
            byte tag_to_spoof[10] = stored_Tag;
            // This array is used to construct the tag message we want to send, this array will be modified.
            int binary_message[64] = {1,1,1,1,1,1,1,1,1, 0,0,0,0,0 ,0,0,0,0,0, 0,0,0,0,0 ,0,0,0,0,0, 0,0,0,0,0 ,0,0,0,0,0, 0,0,0,0,0 ,0,0,0,0,0, 0,0,0,0,0 ,0,0,0,0,0, 0,0,0,0,0};
            // The UID gets inserted into the binary_message in this array.
            load_UID(tag_to_spoof, binary_message);
            // Create the parity of the message. Number parity and column parity.
            create_parity(tag_to_spoof, binary_message);
            
            // Start transmitting tag.
            send = 1;
        }
        // Calling the transmitt function.
        transmitt_tag(binary_message);
        
  
  }
}




/*
Sender code:
*/




//Does manchester encoding for signal and sets pins.
//Needs clock and signal to do encoding
void set_pin_manchester(int clock_half, int signal)
{
  //manchester encoding is xoring the clock with the signal
  int man_encoded = clock_half ^ signal;
  
  //if it's 1, set the pin LOW (this will tune the antenna and the reader sees this as a high signal)
  //if it's 0, set the pin to HIGH (this will detune the antenna and the reader sees this as a low signal)
  if(man_encoded == 1)
  {
     digitalWrite(coil_pin, LOW);
  }
  else
  {
    digitalWrite(coil_pin, HIGH);
  }
}

//Use this function to do the sending of the spoofed tag.
//It's recieves the array of what it is supposed to send.
void transmitt_tag(int* tag_pointer){
  //we loop and call on set_pin_manchester.
  for(int i = 0; i < 64; i++)
  {
    set_pin_manchester(0, tag_pointer[i]);
    delayMicroseconds(256);
    
    set_pin_manchester(1, tag_pointer[i]);
    delayMicroseconds(256);
  }
}

//Writes the given hex number in binary in the message array.
void write_in_HEX(byte number, int place, int* message){
  //calculate where in the message to write the number.
  //it will point to the least-significant bit.
  int index = 12 + (place * 5);
  //Write in the bits.
  for(int i=0; i<4; i++){
    //read each bit from the given number, starting with least-significant.
    int Bit = bitRead(number,i);
    //Write the bit to the message, writing the least-significant first.
    message[index-i] = Bit;
  }
}

//Used to load the UID into the array.
void load_UID (byte* tag, int* message){
  //we go through the UID and load them into the message array.
  for(int i=0; i<10; i++){
    //Read in a UID number.
    byte UID_num = tag[i];
    //calling on the write function, it takes the hex number, where to write it and the message.
    write_in_HEX(tag[i], i, message);
  }
  //The UID is now inserted into the message.
}

//Decides the even parity for a given number.
int number_parity(byte number){
  //Counter for number of 1 bits found
  int ones = 0;
  //Go through the bits of the given number
  for(int i=0; i<4; i++){
    //If we get a 1, we increment.
    if(bitRead(number,i) == 1){
      ones++;
    }
  }
  //With the number of ones counted and modulo we can say what the parity is.
  return (ones % 2);
}

//Calculates the parity of a column.
int column_parity(int column, byte* tag){
  //Counter for number of 1 bits found
  int ones = 0;
  //Step through the numbers and check a specific bit in each number.
  for(int i=0; i<10; i++){
    //If we get a 1 from the bit read then we increments ones.
    if(bitRead(tag[i], column)){
      ones++;
    }
  }
  //we return what the parity should be.
  return (ones % 2);
}

//Function goes through the binary_message and adds parity to it.
void create_parity(byte* tag, int* binary_message){
  // this index to the parity for the first number.
  int index = 13;
  
  //this loop sets the parity of the numbers.
  for(int i=0; i<10; i++){
    //number_parity will return the parity. Insert it directly into the message.
    binary_message[index+(5*i)] = number_parity(tag[i]);
  }
  
  //setting the index to the last column parity bit in the array.
  index = 62;
  //Column parity.
  for(int i=0; i<4; i++){
    //on each iteration we will evaluate the parity of a column.
    binary_message[index-i] = column_parity(i, tag);
  }
}

