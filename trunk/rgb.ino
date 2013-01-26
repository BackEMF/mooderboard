// Mooderboard Firmware version 1.1.2 (20130126)


//		    GNU GENERAL PUBLIC LICENSE
//                       Version 2, June 1991
//
// Copyright (C) 1989, 1991 Free Software Foundation, Inc.
//                          675 Mass Ave, Cambridge, MA 02139, USA
// Everyone is permitted to copy and distribute verbatim copies
// of this license document, but changing it is not allowed.
//
// For more information see http://www.gnu.org/licenses/gpl.html.


// pin colors
int redPin = 9;  
int grnPin = 10;
int bluPin = 11; 

int faderBrightness=100; // brightness in percentage	
int faderWait = 9;       // internal crossFade delay; increase for slower fades
int faderHold = 0;       // Optional faderHold when a color is complete, before the next crossFade


// default hex color; if assigned then defaults to this after reboot instead of fader

int defaultProgram=0; // 1 for fader, 2 for hex
char defaultHexCode[7]="772200";

//
// ========== no need to edit below this line ==========================================================//
//



// internal var declaration

int faderColor[6][3]    = {	{ 256, 0, 0 },
							{ 128, 128, 0 },
							{ 0, 256, 0 },
							{ 0, 128, 128 },
							{ 0, 0, 256 },
							{ 128, 0, 128 } };
int faderColorRT[6][3];

int currFaderColor=0;
char input[255], backuphexCode[255];
char ser;
char * pEnd;
int program=0;
char * hexCode=defaultHexCode;



// crossfader initial code by....

int DEBUG = 0;      // DEBUG counter; if set to 1, will write values back via serial
int loopCount = 1; // How often should DEBUG report?
int j = 0;          // Loop counter for repeat

//internal variables
int redVal = 0;
int grnVal = 0;
int bluVal = 0;
int prevR = 0;
int prevG = 0;
int prevB = 0;

int calculateStep(int prevValue, int endValue) {
  int step = endValue - prevValue; // What's the overall gap?
  if (step) {                      // If its non-zero,
    step = 1020/step;              //   divide by 1020
  }
  return step;
}

/* The next function is calculateVal. When the loop value, i,
*  reaches the step size appropriate for one of the
*  colors, it increases or decreases the value of that color by 1.
*  (R, G, and B are each calculated separately.) */

int calculateVal(int step, int val, int i) {

  if ((step) && i % step == 0) { // If step is non-zero and its time to change a value,
    if (step > 0) {              //   increment the value if step is positive...
      val += 1;
    }
    else if (step < 0) {         //   ...or decrement it if step is negative
      val -= 1;
    }
  }
  // Defensive driving: make sure val stays in the range 0-255
  if (val > 255) {
    val = 255;
  }
  else if (val < 0) {
    val = 0;
  }
  return val;
}

/* crossFade() converts the percentage colors to a
*  0-255 range, then loops 1020 times, checking to see if
*  the value needs to be updated each time, then writing
*  the color values to the correct pins.
*/

void crossFade() {

  int stepR = calculateStep(prevR, faderColor[currFaderColor][0]/100*faderBrightness);
  int stepG = calculateStep(prevG, faderColor[currFaderColor][1]/100*faderBrightness);
  int stepB = calculateStep(prevB, faderColor[currFaderColor][2]/100*faderBrightness);

  for (int i = 0; i <= 1020; i++) {
	if (Serial.available())
	{
		break;
	}
    redVal = calculateVal(stepR, redVal, i);
    grnVal = calculateVal(stepG, grnVal, i);
    bluVal = calculateVal(stepB, bluVal, i);

    analogWrite(redPin, redVal);   // Write current values to LED pins
    analogWrite(grnPin, grnVal);
    analogWrite(bluPin, bluVal);

    delay(faderWait); // Pause for 'faderWait' milliseconds before resuming the loop

    if (DEBUG) { // If we want serial output, print it at the
      if (i == 0 or i % loopCount == 0) { // beginning, and every loopCount times
        Serial.print("Loop/RGB: #");
        Serial.print(i);
        Serial.print(" | ");
        Serial.print(redVal);
        Serial.print(" / ");
        Serial.print(grnVal);
        Serial.print(" / ");
        Serial.println(bluVal);
      }
      DEBUG += 1;
    }
  }
  // Update current values for next loop
  prevR = redVal;
  prevG = grnVal;
  prevB = bluVal;
  delay(faderHold); // Pause for optional 'wait' milliseconds before resuming the loop
  currFaderColor==5?currFaderColor=0:currFaderColor++;  
}

// play flasher program. triggered by serial command's

void flash(int program)
{
	analogWrite(redPin,0);
	analogWrite(grnPin,0);
	analogWrite(bluPin,0);
	int delayms;
	switch(program)
	{
		case 1:
			delayms=40;
			for (int k=0;k<=2;k++ )
			{
				for (int l=0;l<=2;l++ )
				{
					analogWrite(redPin,128);
					analogWrite(grnPin,0);
					analogWrite(bluPin,0);
					delay(delayms);
					analogWrite(redPin,0);
					analogWrite(grnPin,0);
					analogWrite(bluPin,0);
					delay(delayms);
				}
				for (int l=0;l<=2;l++ )
				{
					analogWrite(redPin,64);
					analogWrite(grnPin,64);
					analogWrite(bluPin,0);
					delay(delayms);
					analogWrite(redPin,0);
					analogWrite(grnPin,0);
					analogWrite(bluPin,0);
					delay(delayms);
				}
				for (int l=0;l<=2;l++ )
				{
					analogWrite(redPin,0);
					analogWrite(grnPin,128);
					analogWrite(bluPin,0);
					delay(delayms);
					analogWrite(redPin,0);
					analogWrite(grnPin,0);
					analogWrite(bluPin,0);
					delay(delayms);
				}
				for (int l=0;l<=2;l++ )
				{
					analogWrite(redPin,0);
					analogWrite(grnPin,64);
					analogWrite(bluPin,64);
					delay(delayms);
					analogWrite(redPin,0);
					analogWrite(grnPin,0);
					analogWrite(bluPin,0);
					delay(delayms);
				}
				for (int l=0;l<=2;l++ )
				{
					analogWrite(redPin,0);
					analogWrite(grnPin,0);
					analogWrite(bluPin,128);
					delay(delayms);
					analogWrite(redPin,0);
					analogWrite(grnPin,0);
					analogWrite(bluPin,0);
					delay(delayms);
				}
				for (int l=0;l<=2;l++ )
				{
					analogWrite(redPin,64);
					analogWrite(grnPin,0);
					analogWrite(bluPin,64);
					delay(delayms);
					analogWrite(redPin,0);
					analogWrite(grnPin,0);
					analogWrite(bluPin,0);
					delay(delayms);
				}
				
			}
		break;
		case 2:

			for (int k=0;k<=8;k++ )
			{
				for (int l=0;l<=2;l++ )
				{
					analogWrite(redPin,128);
					delay(25);
					analogWrite(redPin,0);
					delay(25);
				}
				for (int l=0;l<=2;l++ )
				{
					analogWrite(bluPin,128);
					delay(25);
					analogWrite(bluPin,0);
					delay(25);
				}
			}
		break;
		case 3:
			for (int k=0;k<=8;k++ )
			{
				for (int l=0;l<=2;l++ )
				{
					analogWrite(grnPin,128);
					delay(25);
					analogWrite(grnPin,0);
					delay(25);
				}
				for (int l=0;l<=2;l++ )
				{
					analogWrite(bluPin,128);
					delay(25);
					analogWrite(bluPin,0);
					delay(25);
				}
			}
		break;

	}
	
}

// start the default fader loop

void fader()
{
	while (!Serial.available())
	{
		crossFade();
	}
}


// converts string value hex code to decimal and set pwm output.

void sethexcolor(char *hexCode)
{
	char redHex[3];
	char grnHex[3];
	char bluHex[3];
	strcpy(backuphexCode,hexCode);

	redHex[0]=hexCode[0];
	redHex[1]=hexCode[1];
	redHex[2]='\0';
	int redC=strtol(redHex,&pEnd,16);

	grnHex[0]=hexCode[2];
	grnHex[1]=hexCode[3];
	grnHex[2]='\0';
	int grnC=strtol(grnHex,&pEnd,16);

	bluHex[0]=hexCode[4];
	bluHex[1]=hexCode[5];
	bluHex[2]='\0';
	int bluC=strtol(bluHex,&pEnd,16);

	analogWrite(redPin,redC);
	analogWrite(grnPin,grnC);
	analogWrite(bluPin,bluC);
}

void setup()
{
	pinMode(redPin, OUTPUT);
	pinMode(grnPin, OUTPUT);
	pinMode(bluPin, OUTPUT);
	Serial.begin(9600);
	Serial.flush();
	if (defaultProgram==1)
	{
		sethexcolor(hexCode);
		program=1;
	}
	else
	{
		fader();
		program=0;
	}
}

char * key,* val,* val2;
int c=-2, start=0;

void loop() 
{

	Serial.flush();
	if (Serial.available()) // checks if something in serial buffer.
	{
//		Serial.println(c);
		ser = Serial.read();
		Serial.print(ser);

		c++;
		if (ser=='{') 
		{
			c=-1;
			start=1;
//			Serial.println("INSIDE {");
//			Serial.println(c);
		}
		else
		if (start==1 && ser!='}')
		{
			input[c]=ser;
//			Serial.println("--inside counter");
		}
		else
		if (start==1 && ser=='}')
		{
			input[c]='\0';
			Serial.println("");
//			Serial.println("INSIDE }");
//			Serial.println(c);
//			Serial.println(input);
			
			key = strtok (input,",");
			val = strtok (NULL,",");
			val2 = strtok (NULL,",");
			



			// command menu


			// cmd: fader,wait,brightness
			if (strcasecmp(key,"fader")==0)
			{
				program=0;
				Serial.println("--FADER"); 
				faderWait=strtol(val,&pEnd,10);  //string to decimal
				if (val2!=NULL)
				{
					faderBrightness=strtol(val2,&pEnd,10);
				}

				fader();
			}
			else

			// cmd: flash,programnr
			if (strcasecmp(key,"flash")==0)
			{
				Serial.println("--FLASH:1");
				flash(strtol(val,&pEnd,10));
				if (program==1)
				{
					sethexcolor(backuphexCode);
				}
				else
				{
					fader();
				}
			}
			else
				// cmd: hex,html color string
				if (strcasecmp(key,"hex")==0)
				{
					if (strlen(val)==6)
					{
						Serial.print("--HEX:");
						Serial.println(val);
						sethexcolor(val);
						program=1;
					}
				}
			start=0;
		}
		else
		{
			start=0;
		}
	}
}





