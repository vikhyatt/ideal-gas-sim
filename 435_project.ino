#include "LedControl.h"
#include <LiquidCrystal.h>  // include the library code
#include <movingAvg.h>                 


LedControl lc=LedControl(9,8,10,4);  // Pins: DIN,CLK,CS, # of Display connected
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2; // Declare variables to hold pin numbers
LiquidCrystal lcd(rs, en, d4, d5, d6, d7); // Create lcd object to control LCD Display
unsigned long delayTime_const=100;  // Delay between Frames
float tempc; //variable to store temperature in degree Celsius
const int sensor=A5; // Assigning analog pin A5 to variable 'sensor'

float tempf; //variable to store temperature in Fahreinheit

unsigned long long int k = 0;
float pressure = 0;
float vout; //temporary variable to hold sensor reading
// Put values in arrays


struct pos{
  int x;
  int y;
  };

struct vel{
  int vx;
  int vy;
  };

struct particle {
    pos r;
    // int device = r.x / 8
    vel v;
};


const int n = 16;
particle particles[n];


void setup()
{   
   
   randomSeed(80);
   lcd.begin(16,2);
      // Print a message to the LCD.

   int indices[32*8];
   for(int i = 0; i < 32*8; i++){
    indices[i] = i;
    }

   int questionCount = 32*8;
   for (int i=0; i < questionCount; i++) {
       int k = random(0, questionCount);  // Integer from 0 to questionCount-1
       int temp = indices[k];
       indices[k] =  indices[i];
       indices[i] = temp;
      }

   int filtered_indices[n];
   for(int i = 0; i < n; i++){
    filtered_indices[i] = indices[i];
    }

   
   for(int i = 0; i < n; i++)
    {

       int idx = filtered_indices[i];
       int x = idx % 32;
       int y = idx / 32;
       pos r_i =   {x,y};
       int temp_vx = random(0,2);
       int temp_vy = random(0,2);

       if (temp_vx == 0){
        temp_vx = -1;
        }
        if (temp_vy == 0){
        temp_vy = -1;
        }
       vel v_i = {temp_vx,temp_vy}; 
       particles[i] = {r_i,v_i};
    }

  
  Serial.begin(9600);
  lc.shutdown(0,false);  // Wake up displays
  lc.shutdown(1,false);
  lc.shutdown(2,false);  // Wake up displays
  lc.shutdown(3,false);
  lc.setIntensity(0,1);  // Set intensity levels
  lc.setIntensity(1,1);
  lc.setIntensity(2,1);  // Set intensity levels
  lc.setIntensity(3,1);
  lc.clearDisplay(0);  // Clear Displays
  lc.clearDisplay(1);
  lc.clearDisplay(2);  // Clear Displays
  lc.clearDisplay(3);

  
 
}


particle move_particle(particle a){

    
    bool col_x = false;
    bool col_y = false;
    bool y_inc = false;
    bool x_inc = false;

    if (a.r.x == 0){
        if (a.v.vx <= 0){
        a.v.vx = (-1)*a.v.vx; 
        a.r.x += a.v.vx;
        col_x = true;}

        else{ a.r.x += a.v.vx;
        x_inc = true;}
    }

    if (a.r.x == 31){
        if (a.v.vx >= 0){
        a.v.vx = (-1)*a.v.vx; 
        a.r.x += a.v.vx;
        col_x = true;}

        else{ a.r.x += a.v.vx;
        x_inc = true;}
    }
    
    if (a.r.y == 0){
        if (a.v.vy <= 0){
        a.v.vy = (-1)*a.v.vy; 
        a.r.y += a.v.vy;
        col_y = true;}

        else{ a.r.y += a.v.vy;
        y_inc = true;}
    }

    if (a.r.y == 7){
        if (a.v.vy >= 0){
        a.v.vy = (-1)*a.v.vy;
        a.r.y += a.v.vy;
        col_y = true;
        }

        else{ a.r.y += a.v.vy;
        y_inc = true;}
    }

    if ((a.r.x > 0 & a.r.x < 31) & (col_x == false) & (x_inc == false)){
        a.r.x += a.v.vx;
    }

    if ((a.r.y > 0 & a.r.y < 7) & (col_y == false) & (y_inc == false)){
        a.r.y += a.v.vy;
    }
    
  return (a);
}

//  Take values in Arrays and Display them




void display_particles(struct particle particles[], int n)
{
   
  lc.clearDisplay(0);  // Clear Displays
  lc.clearDisplay(1);
  lc.clearDisplay(2);  // Clear Displays
  lc.clearDisplay(3);
  byte bytes[n];
  int devices[n];
  int y_coords[n];
  
  for(int i = 0; i < n; i++)
     {
       int device_i = particles[i].r.x/8;
       int local_x_i = particles[i].r.x - device_i*8;
       bytes[i] = byte(round(pow(2,local_x_i) + 1e-9));
       devices[i] = device_i;
       y_coords[i] = particles[i].r.y;
     }


  for(int device = 0; device < 4; device++) {
    for(int y = 0; y < 8; y++){
       int locs[8];
       byte loc_bytes = B00000000;
       for(int i = 0; i<n; i++){
            if (devices[i] == device){
              if (y_coords[i] == y){
                loc_bytes += bytes[i];
                }}
        }  
        lc.setRow(device,y,loc_bytes);  
      }
    }
    
  }
    //lc.setRow(0,i,invader1a[i]);
  //lc.setRow(device,a.r.y,c);


void loop()
{ 
  
 
    int x_counter = 0;
    int y_counter = 0;
    float x_pressure = 0;
    float y_pressure = 0;
    
    int mass = 1;
    for(int i = 0; i < n; i++)
     { 
      if (particles[i].r.x == 0 && particles[i].v.vx < 0) 
      {
        x_counter = 1;
      }
      if (particles[i].r.x == 31 && particles[i].v.vx > 0) 
      {
        x_counter = 1;
      }
      if (particles[i].r.y == 0 && particles[i].v.vy < 0) 
      {
        y_counter = 1;
      }
      if (particles[i].r.y == 7 && particles[i].v.vy > 0) 
      {
        y_counter = 1;
      } 

      x_pressure += x_counter * 2 * mass * abs(particles[i].v.vx);
      y_pressure += y_counter * 2 * mass * abs(particles[i].v.vy);
      particles[i] = move_particle(particles[i]);
      x_counter = 0;
      y_counter = 0;
     }
     
    pressure = (pressure*k + x_pressure + y_pressure)/(k+1);
    k++;
    
     //set the cursor to column 0, line 1 (line 1 is the 2nd row, since counting begins with 0)
    vout=analogRead(sensor); //Reading the value from sensor
    vout=(vout*500)/1023;
    tempc=vout; // Storing value in Degree Celsius
    //tempf=(vout*1.8)+32; // Converting to Fahrenheit
    //pressure = counter*
    lcd.begin(16,2);
    lcd.print("T(C): ");
    lcd.print(tempc);
    lcd.print(" N:");
    lcd.print(n);
    lcd.setCursor(0, 1);
    lcd.print("P(units):");
    lcd.print(pressure); //Print a message to second line of LCD
    display_particles(particles,n);
    
    
    float delayTime = delayTime_const / pow(tempc,0.5);
    delay(delayTime);
    

}
