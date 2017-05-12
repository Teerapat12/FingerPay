/*4x4 Matrix Keypad connected to Arduino
This code prints the key pressed on the keypad to the serial port*/

#include <Keypad.h>
#include <Wire.h> 
#include <EEPROM.h>
#include "FPS_GT511C3.h"
#include "SoftwareSerial.h"

#include <LiquidCrystal_I2C.h> //ประกาศ Library ของจอ I2C
// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);


const byte numRows= 4; //number of rows on the keypad
const byte numCols= 4; //number of columns on the keypad

//keymap defines the key pressed according to the row and columns just as appears on the keypad
char keymap[numRows][numCols]= 
{
{'1', '2', '3', 'A'}, 
{'4', '5', '6', 'B'}, 
{'7', '8', '9', 'C'},
{'*', '0', '#', 'D'}
};

 int menuNum = 6;
 String foodMenuName[] = {"Thai","Japanese","French","Italian","Texas","Mixed"};
 int foodMenuPrice[] ={50,120,75,83,79,150};

//Code that shows the the keypad connections to the arduino terminals
byte rowPins[numRows] = {13,12,11,10}; //Rows 0 to 3
byte colPins[numCols]= {9,8,7,6}; //Columns 0 to 3

//initializes an instance of the Keypad class
Keypad myKeypad= Keypad(makeKeymap(keymap), rowPins, colPins, numRows, numCols);

FPS_GT511C3 fps(4, 5);


void setup()
{
  Serial.begin(9600);
  lcd.begin();
  // Print a message to the LCD.
  printLine("---Finger Pay---",0);
  printLine("Loading.",1);

  
  //fps.UseSerialDebug = true; // so you can see the messages in the serial debug screen
  fps.Open();
  lcd.print(".");
  fps.SetLED(true);
  
  lcd.print(".");
//  fps.DeleteAll();
  printLine("(Enroll) D P",1);

}

//If key is pressed, this key is stored in 'keypressed' variable
//If key is not equal to 'NO_KEY', then this key is printed out
//if count=17, then count is reset back to 0 (this means no key is pressed during the whole keypad scan process
void printLine(String message,int  l){
  clearLine(l);
  // Now write a message to line 1
  lcd.setCursor (0, l);
  lcd.print(message);
}

void clearLine(int l){
  lcd.setCursor (0, l);
   for (int i = 0; i < 16; ++i)
  {
    lcd.write(' ');
  }
  
  lcd.setCursor(0,l);
}

char mainMenu(){
  char keypressed = myKeypad.getKey();
  char selectedMenu='A';
  printLine("---Finger Pay---",0);
  printLine("(Enroll) A P",1);
  do{
      
      keypressed = myKeypad.getKey();
      if (keypressed != NO_KEY)
      {
        switch(keypressed){
          case 'A': 
            selectedMenu = 'A';
            printLine("(Enroll) A P",1);
            break;
          case 'B':
            selectedMenu = 'B';
            printLine("E (Account) P",1);
            break;
          case 'C':
            selectedMenu = 'C';
            printLine("E A (Payment)",1);
            break;
        }
        Serial.print(keypressed);
        
      }
  } while (keypressed!='D');

  return selectedMenu;
}

void printAccoutBalance(int id){
  printLine("-Balance: ",0);
  lcd.print(EEPROM.read(id));
  lcd.print("--");
  return;
}

void account(){ 
  printLine("----Account----",0);
   char keypressed = myKeypad.getKey();
  char selectedMenu='A';
  printLine("(Balance) D W",1);
  do{
      
      keypressed = myKeypad.getKey();
      if (keypressed != NO_KEY)
      {
        switch(keypressed){
          case 'A': 
            selectedMenu = 'A';
            printLine("(Balance) D W",1);
            break;
          case 'B':
            selectedMenu = 'B';
            printLine("B (Deposit) W",1);
            break;
          case 'C':
            selectedMenu = 'C';
            printLine("B D (Withdraw)",1);
            break;
        }
        Serial.print(keypressed);
        
      }
  } while (keypressed!='D');
Serial.print(selectedMenu);
  int id;

   while(true){
        printLine("Scan finger",1);
        while(fps.IsPressFinger()==false){ delay(100); 
         char keypressed = myKeypad.getKey();
      if (keypressed == '*'){
        return;
      }}
        fps.CaptureFinger(false);
      
       id = fps.Identify1_N();
   
  switch(selectedMenu){
    case 'A':
      //Check Balance
      // Identify fingerprint test

     
        if (id <200)
        {
          Serial.print("Verified ID:");
          Serial.println(id);
         
          printLine("Balance(",1);
          lcd.print(id);
          lcd.print("): ");
          lcd.print(EEPROM.read(id));
          while(myKeypad.getKey()!='D'){delay(100);}
          return;
        }
        else
        {
          Serial.println("Finger not found");
          printLine("Finger not found",1);
          delay(2000);
        }
      
    
      break;
    case 'B':
        if (id <200)
        {
          Serial.print("Verified ID:");
          Serial.println(id);
          printAccoutBalance(id);
          printLine("Deposit(",1);
          lcd.print(id);
          lcd.print("): ");
          int startPosition = 11+String(id).length();
          int lcdPosition = startPosition;
           char keypressed = myKeypad.getKey();  
                  String amount = "";
                do{
                    
                    keypressed = myKeypad.getKey();
                    if (keypressed != NO_KEY && isDigit(keypressed) &&lcdPosition<16)
                    {
                      amount+=keypressed;
                      lcdPosition+=1;
                      lcd.write(keypressed);
                      Serial.print(keypressed);
                      
                    }
                    if(keypressed=='#' &&lcdPosition>startPosition){
                      amount.remove(amount.length()-1,1);
                      lcdPosition-=1;
                     lcd.setCursor(lcdPosition,1);
                      lcd.print(" ");
                      lcd.setCursor(lcdPosition,1);
                    }
                } while (keypressed!='D'|| amount=="");
                int newAmount = amount.toInt()+EEPROM.read(id);
                if(newAmount<0) {printLine("Exceed maximum",1); delay(2000);}
                else {
                  printLine("New balance: ",1);
                  lcd.print(newAmount);
                  EEPROM.write(id,newAmount);
                  delay(4000);
                  return;
                  }
  
        break;
        }
        else
        {
          Serial.println("Finger not found");
          printLine("Finger not found",1);
          delay(2000);
        }
      //Deposit
      break;
    case 'C':
      //Withdraw
       if (id <200)
        {
          Serial.print("Verified ID:");
          Serial.println(id);
          printAccoutBalance(id);
          printLine("Withdraw(",1);
          lcd.print(id);
          lcd.print("): ");
          int startPosition = 12+String(id).length();
          int lcdPosition = startPosition;
           char keypressed = myKeypad.getKey();  
                  String amount = "";
                do{
                    
                    keypressed = myKeypad.getKey();
                    if (keypressed != NO_KEY && isDigit(keypressed) &&lcdPosition<16)
                    {
                      amount+=keypressed;
                      lcdPosition+=1;
                      lcd.write(keypressed);
                      Serial.print(keypressed);
                      
                    }
                    if(keypressed=='#' &&lcdPosition>startPosition){
                      amount.remove(amount.length()-1,1);
                      lcdPosition-=1;
                     lcd.setCursor(lcdPosition,1);
                      lcd.print(" ");
                      lcd.setCursor(lcdPosition,1);
                    }
                } while (keypressed!='D'|| amount=="");
                int newAmount = EEPROM.read(id)-amount.toInt();
                if(newAmount>250) {printLine("Not enough money",1); delay(2000);}
                else {
                  printLine("New balance: ",1);
                  lcd.print(newAmount);
                  EEPROM.write(id,newAmount);
                  delay(4000);
                  return;
                  }
  
        break;
        }
        else
        {
          Serial.println("Finger not found");
          printLine("Finger not found",1);
          delay(2000);
        }
      break;
  }

   }

  delay(500);
  return;
}

void enroll(){
  Enroll();
}

void pay(){
printMenu(0,true);
  int curMenu = 0;
  char keypressed = myKeypad.getKey();
  boolean mode = true; //true = Lower, false=UPPER
   do{
                    
      keypressed = myKeypad.getKey();
      if (keypressed != NO_KEY)
      {
        Serial.println(menuNum);
        if(mode==true){
          if(keypressed=='B'&& curMenu<  menuNum-1){
            curMenu+=1;
            printMenu(curMenu,mode);
          }
          else if(keypressed=='A'&& curMenu>0){            
            mode = false;
            curMenu-=1;
            printMenu(curMenu,mode);
          }
        }
        else if(mode==false){
          if(keypressed=='B'&& curMenu<menuNum-1){
            curMenu+=1;
            mode = true;
            printMenu(curMenu,mode);

          }
          else if(keypressed=='A'&& curMenu>0){
            curMenu-=1;
            printMenu(curMenu,mode);
          }
        }
        
      }
  } while (keypressed!='D');

//  Confirm withFinger.
int id2;
  while(true){
        printLine("-- ",0);
        lcd.print(foodMenuName[curMenu]);
        lcd.print("(");
        lcd.print(foodMenuPrice[curMenu]);
        lcd.print(")--");
        printLine("Scan finger2pay",1);
        while(fps.IsPressFinger()==false){ delay(100); 
         char keypressed = myKeypad.getKey();
      if (keypressed == '*'){
        return;
      }}
        fps.CaptureFinger(false);
      
       id2 = fps.Identify1_N();


         if (id2 <200)
        {
          Serial.print("Verified ID:");
          Serial.println(id2);
                int newAmount = EEPROM.read(id2)-foodMenuPrice[curMenu];
                if(newAmount<0) {printLine("Not enough money",1); delay(2000);}
                else {
                  printLine("New balance: ",1);
                  lcd.print(newAmount);
                  EEPROM.write(id2,newAmount);
                  delay(4000);
                  return;
                  }
  
        break;
        }
        else
        {
          Serial.println("Finger not found");
          printLine("Finger not found",1);
          delay(2000);
        }
  }

//Check money.
//Done.


}

void printMenu(int curMenu,boolean mode){
  if(mode==true){
    if(curMenu>0) {printLine(foodMenuName[curMenu-1],0); lcd.print("("); lcd.print(foodMenuPrice[curMenu-1]); lcd.print(")");}
    else printLine("Select Menu",0);
    printLine(foodMenuName[curMenu],1);
    lcd.print("("); lcd.print(foodMenuPrice[curMenu]); lcd.print(")");
    lcd.print("<");
  }
  else{
     if(curMenu>=0) {printLine(foodMenuName[curMenu],0); lcd.print("("); lcd.print(foodMenuPrice[curMenu]); lcd.print(")");}
    else {printLine("Select Menu",0);}

    lcd.print("<");
    printLine(foodMenuName[curMenu+1],1);
    lcd.print("("); lcd.print(foodMenuPrice[curMenu+1]); lcd.print(")");
  }
}

void loop(){
  char selectedMenu = mainMenu();

  switch(selectedMenu){
    case 'A':
      enroll();
      break;
    case 'B':
      account();
      break;
    case 'C':
      pay();
      break;
  }
  Serial.println(selectedMenu);
  lcd.clear();
  delay(500);
}

void Enroll()
{
  // Enroll test
  
  // find open enroll id
  int enrollid = 0;
  bool usedid = true;
  while (usedid == true)
  {
    usedid = fps.CheckEnrolled(enrollid);
    if (usedid==true) enrollid++;
  }
  fps.EnrollStart(enrollid);

  while(true){
  // enroll
  Serial.print("Press finger to Enroll #");
  clearLine(0);
  lcd.print("Enrolling #");
  lcd.print(enrollid);
  clearLine(1);
  lcd.print("Scan finger");
  Serial.println(enrollid);
  while(fps.IsPressFinger() == false){
    delay(100);
      char keypressed = myKeypad.getKey();
      if (keypressed == '*'){
        return;
      }
    };
  clearLine(0);
  lcd.print("Enrolling #");
  lcd.print(enrollid);
  printLine("Remove finger",1);
  bool bret = fps.CaptureFinger(true);
  int iret = 0;
  if (bret != false)
  {
    Serial.println("Remove finger");
    fps.Enroll1(); 
    while(fps.IsPressFinger() == true) delay(100);
    Serial.println("Press same finger again");
    printLine("Press again (2)",1);
    while(fps.IsPressFinger() == false){
    delay(100);
      char keypressed = myKeypad.getKey();
      if (keypressed == '*'){
        return;
      }
    };
    bret = fps.CaptureFinger(true);
    if (bret != false)
    {
      Serial.println("Remove finger");
      printLine("Remove finger",1);
      fps.Enroll2();
      while(fps.IsPressFinger() == true) delay(100);
      Serial.println("Press same finger yet again");
      printLine("Press again (3)",1);
      while(fps.IsPressFinger() == false){
    delay(100);
      char keypressed = myKeypad.getKey();
      if (keypressed == '*'){
        return;
      }
    };
      bret = fps.CaptureFinger(true);
      if (bret != false)
      {
        Serial.println("Remove finger");
        printLine("Remove finger",1);
        iret = fps.Enroll3();
        if (iret == 0)
        {
          Serial.println("Enrolling Successfull");
          printLine("Enrolling Successfull",0);
          printLine("Going back to main menu",1);
          EEPROM.write(enrollid,0);
          delay(4000);
          break;
        }
        else
        {
          Serial.print("Enrolling Failed with error code:");
          clearLine(1);
          lcd.write("Failed code:");
          lcd.print(iret);
          delay(4000);
          Serial.println(iret);
        }
      }
      else  {Serial.println("Failed to capture third finger");  printLine("Failed 3rd finger",1); delay(4000);}
    }
    else {Serial.println("Failed to capture second finger"); printLine("Failed 2nd finger",1); delay(4000);};
  }
  else {Serial.println("Failed to capture first finger"); printLine("Failed 1st finger",1); delay(4000);}
  }
  
}
