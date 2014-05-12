//*****************************
// Galileo Button Game
//*****************************

//set some constants
int gameDuration = 30000; //duration in milliseconds
int numButtons = 3; //number of buttons per player
# define numCombinations 7
//where it's all plugged in: 
int p1[3] = {2, 4, 6}; //player 1 button pins
int p1LED[3] = {3, 5, 7}; //player 1 LED pins
int p2[3] = {8, 10, 12}; //player 2 button pins
int p2LED[3] = {9, 11, 13}; //player 2 LED pins

int p1Score = 0; //hold scores
int p2Score = 0;

long lastP1Score = 0; //holds time of last score
long lastP2Score = 0;

int currentPattern[2];//holds the pattern number that each player must hit (corresponds to a row in buttonCombinations array)

const int buttonCombinations [numCombinations][3] = { //holds all sets of possible button combinations
	{1, 0, 0},
	{0, 1, 0},
	{0, 0, 1},
	{1, 1, 0},
	{1, 0, 1},
	{0, 1, 1},
	{1, 1, 1}
};

int presses [2][3] = { //holds the state of both players' buttons, 0 is not pressed, 1 is pressed
	{0, 0, 0},
	{0, 0, 0}
};

void setup(){
	Serial.begin(9600);
	//set LEDs as outputs, buttons as inputs
	for (int i = 0; i < 3; i++){ //player 1 buttons
		pinMode(p1[i], INPUT_PULLUP);
		}
	for (int i = 0; i < 3; i++){ //player 2 buttons
		pinMode(p2[i], INPUT_PULLUP);
		}
	for (int i = 0; i < 3; i++){ //player 1 LEDs
		pinMode(p1LED[i], OUTPUT);
		}	
	for (int i = 0; i < 3; i++){ //player 2 LEDs
		pinMode(p2LED[i], OUTPUT);
		}	
}

void loop()
{
	standby(); //stay in standby mode and flash LEDs until someone hits a button to start a game
	readButtons(); //check if any of the buttons are pressed

	for (int i=0; i<numButtons; i++){ //this loop checks if any buttons have been updated as being pressed, if so, it triggers the flag to begin the game
		if (presses[0][i] == 1 || presses[1][i] == 1) {
			gamePlay();
			return;
		}
	}
} 

void standby(){ //default mode for when a game isn't being played
	if ((millis()/1000)%2 == 0){ //change every second
		for (int i=0; i<numButtons; i++){ //turn only player 1's LEDs on
			digitalWrite (p1LED[i], HIGH);
			digitalWrite (p2LED[i], LOW);
		}
	}
	else{
		for (int i=0; i<numButtons; i++){ //turn only player 2's LEDs on
			digitalWrite (p1LED[i], LOW);
			digitalWrite (p2LED[i], HIGH);
		}
	}
}

void gamePlay(){ 
	Serial.println("GamePlay!");
	p1Score = 0;
	p2Score = 0;
	long startGameTime = millis();
	currentPattern[0] = (int)randomNumber(numCombinations-1); //assign first pattern for player 1
	delay(1); //since our "randomNumber" is a function of time, waiting 1 millisecond means player 2 will get a different pattern than player 1
	currentPattern[1] = (int)randomNumber(numCombinations-1); //assign first pattern to player 2

/*  //Just printing stuff...
	Serial.print("Player 1: Pattern No. ");
	Serial.println(currentPattern[0]);
	for(int i=0; i< 3 ; i++){
		Serial.print(buttonCombinations[currentPattern[0]][i]);
		Serial.print(" ");
	}
		Serial.println(" ");


	Serial.print("Player 2: Pattern No. ");
	Serial.println(currentPattern[1]);
	for(int i=0; i< 3 ; i++){
		Serial.print(buttonCombinations[currentPattern[1]][i]);
		Serial.print(" ");
	}
*/

	while (millis() < startGameTime + gameDuration){ //this is where the game happens
		updateLED(); 
		readButtons();
		if (checkP1Buttons() && millis() > lastP2Score + 200){ //player 1 gets the right buttons and it has been enough time since the last point
			lastP1Score = millis();
			p1Score++; //player 1 gets a point!
			Serial.print("Player 1 now has ");
			Serial.println(p1Score);
			int nextPattern = (int)randomNumber(numCombinations); //go find the next pattern
			while(nextPattern == currentPattern[0]){ //if its the same pattern as before, go find another one
			    nextPattern = (int)randomNumber(numCombinations);
			}
			currentPattern[0] = nextPattern; //change the pattern
		}
		if (checkP2Buttons() && millis() > lastP2Score + 200){ //player 2 gets the right buttons and it has been enough time since the last point
			lastP2Score = millis();
			p2Score++; //player 2 gets a point!
			Serial.print("Player 2 now has ");
			Serial.println(p2Score);
			int nextPattern = (int)randomNumber(numCombinations);
			while(nextPattern == currentPattern[1]){ 
			    nextPattern = (int)randomNumber(numCombinations);
			}
			currentPattern[1] = nextPattern; //change the pattern
		}
	}
	winner(); //gets here after game has been played, calculates winner and flashes their colors
}

void updateLED(){ //turn on the appropriate LEDs during the game
	for (int i=0; i<numButtons; i++){
		if(buttonCombinations[currentPattern[0]][i]==0){ //if it should be off, turn it off
			digitalWrite(p1LED[i], LOW);
		}
		else digitalWrite(p1LED[i], HIGH); //otherwise, on!

		if(buttonCombinations[currentPattern[1]][i]==0){
			digitalWrite(p2LED[i], LOW);
		}
		else digitalWrite(p2LED[i], HIGH);
	}	
}

void readButtons(){ //saves current button state into the array called "presses"
	// player1
	for (int i=0; i<numButtons; i++){ //read each button
		presses[0][i] = digitalRead(p1[i]); 
		if(presses[0][i] == 0) presses[0][i] = 1; //want a button press to be represented by a 1 and no press to be a 0
		else presses[0][i] = 0;
		presses[1][i] = digitalRead(p2[i]);
		if(presses[1][i] == 0) presses[1][i] = 1;
		else presses[1][i] = 0;
	}
}

boolean checkP1Buttons() { //will return true if player 1 is holding down the right button combination
	boolean correct = true;
	for (int i=0; i<numButtons; i++){
		if (presses[0][i] != buttonCombinations[currentPattern[0]][i]){
			correct = false;
		}
	}
	if (correct == true) return true;
	else return false;
}


boolean checkP2Buttons() { //will return true if player 2 is holding down the right button combination
	boolean correct = true;
	for (int i=0; i<numButtons; i++){
		if (presses[1][i] != buttonCombinations[currentPattern[1]][i]){
			correct = false;
		}
	}
	if (correct == true) return true;
	else return false;
}

long randomNumber(int a){ //this number isn't random, but it changes every millisecond so it is random enough
	return (millis()%a); //returns a number between 0 and 
 }
 
void winner(){ //figure out who won and flash that player's colors for a few seconds
	long winnerBegin = millis();
	for(int i = 0; i<numButtons; i++){ //turn all LEDs off
		digitalWrite (p1LED[i], LOW);
		digitalWrite (p2LED[i], LOW);
	}

	//printing out winner to the serial monitor
	if (p1Score > p2Score){
		Serial.println("Player 1 wins");
	}
	else if (p2Score > p1Score){
		Serial.println("Player 2 wins");
	}
	else if (p1Score == p2Score){
		Serial.println("tied game");
	}


	while(millis() < winnerBegin + 3000){ // flash winner's LEDs for 3 seconds

		int currentTime = millis()/250;
		if (currentTime%2 == 0){
			for (int i=0; i<numButtons; i++){
				if(p1Score > p2Score){ //player 1 wins
					digitalWrite (p1LED[i], HIGH);
					digitalWrite (p2LED[i], LOW);
				}
				else if(p1Score < p2Score){ //player 2 wins
					digitalWrite (p1LED[i], LOW);
					digitalWrite (p2LED[i], HIGH);
				}
				else if(p1Score == p2Score){ //tied game
					digitalWrite (p1LED[i], HIGH);
					digitalWrite (p2LED[i], HIGH);
				}
			}
		}
		else{
			for (int i=0; i<numButtons; i++){
				digitalWrite (p1LED[i], LOW);
				digitalWrite (p2LED[i], LOW);
			}
		}
	}
}

