#include <bits/stdc++.h>
#include<tchar.h>
#include<Windows.h>
#include<mmsystem.h>
#include <gl/glut.h>
#include<unistd.h>
using namespace std;

#define PI 3.14159
#define GAME_SCREEN 0		
#define MENU_SCREEN 4	

class Tile{
	public:	
	int id;
	int state,style;  //0 for hidden 1 for shown 2 for deleted
	float startx,starty,endx,endy;
	bool deleted;
	Tile(float x,float y,int idd,int stl,int s=0){
		id = idd;
		startx = x;
		starty = y;
		endx = x+200;
		endy = y-200;
		state=s;
		style = stl;
		deleted = false;
	}
	
	void displayTile(){
		if(!deleted and (state==0 or state==1)){
			glBegin(GL_POLYGON);
			if(state==0){glColor3f(1.0,1.0,1.0);}
			else{
				if(style == 1) glColor3f(0.0,0.0,0.0);
				else if(style==2) glColor3f(1.0,0.0,0.0);
				else if(style==3) glColor3f(1.0,1.0,0.0);
				else if(style==4) glColor3f(0.0,1.0,0.0);
				else if(style==5) glColor3f(0.0,0.0,1.0);
				else if(style==6) glColor3f(1.0,0.0,1.0);
				else if(style==7) glColor3f(0.0,1.0,1.0);
				else if(style==8) glColor3f(0.5,0.5,0.5);
			}
			glVertex2f(startx,starty);	
			glVertex2f(endx,starty);	
			glVertex2f(endx,endy);
			glVertex2f(startx,endy);
			glColor3f(0.0,0.0,0.0);
			glEnd();
			
		} else {
			glBegin(GL_POINTS);
			glEnd();
		}
	}
};		

clock_t start,end;

vector<Tile> v;
GLint m_viewport[4];
float mouseX ,mouseY;
float insty,animx;
bool mButtonPressed= false,startGame=false,gameOver=false;		
bool startScreen = true ,nextScreen=false,previousScreen=false;
bool gameQuit = false,instructionsGame = false, optionsGame = false;
int animation=false;
int firstClickId=-1, secondClickId=-1,firstClickStyle, secondClickStyle;
int correct = 0;

int matrix[4][4][4] = {
        { {1, 1, 5, 6}, {2, 2, 6, 5}, {3, 4, 8, 7}, {3, 4, 7, 8} },
        { {1, 7, 6, 4}, {7, 6, 2, 3}, {7, 8, 1, 4}, {8, 7, 3, 2} },
        { {8, 8, 3, 3}, {5, 5, 1, 2}, {6, 6, 4, 1}, {2, 4, 7 ,7} },
        { {1, 3, 5, 7}, {6, 8, 2, 1}, {5, 7, 4, 4}, {8, 6, 2, 3} },
        };

char ch;
int random;
double Score;
long highScore;

void display();
void myinit();
void readFromFile() {
	ifstream file_handle;
	file_handle.open("HighScoreFile.txt",ios::in);
	file_handle>>highScore;
	file_handle.close();
}
void writeIntoFile(int finalScore) {					
	ofstream file_handle;
	file_handle.open("HighScoreFile.txt",ios::out);
	file_handle<<finalScore;
	file_handle.close();
}


void displayRasterTitle(float x,float y,float z,const char *str) {
	const char *c;
	glPushMatrix();
	glTranslatef(x, y,z);
	glLineWidth(5.0);
	for (c=str; *c != '\0'; c++) {
		glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, *c);
	}

	glPopMatrix();
	glLineWidth(1.0);
}


void displayRasterText(float x ,float y ,float z ,const char* stringToDisplay) {
	int length;
	glRasterPos3f(x, y, z);
	length = strlen(stringToDisplay);
	
	for(int i=0 ;i<length ;i++){
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18 ,stringToDisplay[i]); 
	}
}

void displayRasterText2(float x ,float y ,float z ,const char* stringToDisplay) {
	int length;
	glRasterPos3f(x, y, z);
	length = strlen(stringToDisplay);
	
	for(int i=0 ;i<length ;i++){
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24 ,stringToDisplay[i]); 
	}
}

void SetDisplayMode(int modeToDisplay) {
		switch(modeToDisplay){
		case GAME_SCREEN: glClearColor(0,0,0, 1);break;
		case MENU_SCREEN : glClearColor(0.15, 0.22 , 0.24, 1);break;
	}
}

float y_pos=-300,y_pos2=300,colorx = 0.93,colory = 0.41,colorz = 0.42;
int cardState = 1;
void timer(int){
	glutPostRedisplay();
	glutTimerFunc(1000/60,timer,0);
	switch(cardState){
		case 1: if(y_pos<100) {
			y_pos += 5;
			y_pos2 -= 5;
			colorx -= 0.01;colory += 0.01;colorz += 0.01;

			}else cardState = -1;
			break;
		case -1:
			if(y_pos>-700) {
			y_pos -= 5;
			y_pos2 += 5;
			colorx += 0.01;colory -= 0.01;colorz -= 0.01;
			}else cardState = 1;
			break;
	} 
	if(insty<900 and instructionsGame){
		insty += 2;
	}else {
		insty = 0;
	}
	if(startGame and !animation){
		animx+=5;
		if(animx == 1200) animation = true;
	}
			
}
void startScreenDisplay(){	
	glLineWidth(50);
	SetDisplayMode(MENU_SCREEN);
	
	glColor3f(0.5,0.5,0.5);
	glBegin(GL_LINES);               //Border
		glVertex3f(-750 ,-700 ,0.5);
		glVertex3f(-750 ,550 ,0.5);
		
		glVertex3f(-750 ,550 ,0.5);
		glVertex3f(750 ,550 ,0.5);
		
		glVertex3f(750 ,550 ,0.5);
		glVertex3f(750 ,-700, 0.5);
		
		glVertex3f(950 ,-500, 0.5);
		glVertex3f(-950 ,-500 ,0.5);
	glEnd();
	
	glColor3f(1, 1, 1);
	displayRasterText(-100 , -600,0.4 ,"Created by");
	displayRasterText2(-250 ,-650 ,0.4 ,"HITARTH & DEBARJYA");
	
	glBegin(GL_POLYGON);
		glColor3f(colorx,colory,colorz);
		glVertex3f(-950 ,y_pos ,0.5);		//CARD PLOYGON
		glVertex3f(-950 ,y_pos+600 ,0.5);
		glColor3f(0.32, 0.23, 0.47);
		glVertex3f(-550 ,y_pos+600, 0.5);
		glVertex3f(-550 ,y_pos ,0.5);
	glEnd();
	
	glColor3f(0, 0, 0);
	glBegin(GL_POLYGON);
		glColor3f(0.32, 0.23, 0.47);
		glVertex3f(950 ,y_pos2 ,0.5);		//CARD PLOYGON
		glVertex3f(950 ,y_pos2-600 ,0.5);
		glColor3f(colorx,colory,colorz);
		glVertex3f(550 ,y_pos2-600, 0.5);
		glVertex3f(550 ,y_pos2 ,0.5);
	glEnd();
	
	glColor3f(0, 0, 0);
	glBegin(GL_LINES);
		glVertex3f(-750 ,y_pos+300 ,0.5);		//CARD PLOYGON
		glVertex3f(750 ,y_pos2-300 ,0.5);
	glEnd();
	
	glLineWidth(1);

	glColor3f(1, 1, 0);
	glBegin(GL_POLYGON);				//START GAME PLOYGON
		glVertex3f(-200 ,300 ,0.5);
		glVertex3f(-200 ,400 ,0.5);
		glVertex3f(200 ,400 ,0.5);
		glVertex3f(200 ,300, 0.5);
	glEnd();
	
	glBegin(GL_POLYGON);				//INSTRUCTIONS POLYGON
		glVertex3f(-200, 50 ,0.5);
		glVertex3f(-200 ,150 ,0.5);
		glVertex3f(200 ,150 ,0.5);
		glVertex3f(200 ,50, 0.5);
	glEnd();

	glBegin(GL_POLYGON);				//QUIT POLYGON
		glVertex3f(-200 ,-200 ,0.5);
		glVertex3f(-200 ,-100 ,0.5);
		glVertex3f(200, -100 ,0.5);
		glVertex3f(200, -200 ,0.5);
	glEnd();
	
	glColor3f(0 , 0, 1);
	displayRasterTitle(-570,570,0.4,"TILE MASTER");

	if(mouseX>=-100 && mouseX<=100 && mouseY>=150 && mouseY<=200){
		glColor3f(0 ,0 ,1) ;
		if(mButtonPressed){
			startGame = true ;
			gameOver = false;
			mButtonPressed = false;
			start = clock();
		}
	} else
		glColor3f(0 , 0, 0);

	displayRasterText(-100 ,340 ,0.4 ,"Start Game");
	
	if(mouseX>=-100 && mouseX<=100 && mouseY>=30 && mouseY<=80) {
		glColor3f(0 ,0 ,1);
		if(mButtonPressed){
			instructionsGame = true ;
			mButtonPressed = false;
		}
	} else
		glColor3f(0 , 0, 0);
	displayRasterText(-120 ,80 ,0.4 ,"Instructions");
	
	if(mouseX>=-100 && mouseX<=100 && mouseY>=-90 && mouseY<=-40){
		glColor3f(0 ,0 ,1);
		if(mButtonPressed){
			gameQuit = true ;
			mButtonPressed = false;
		}
	}
	else
		glColor3f(0 , 0, 0);
	displayRasterText(-100 ,-170 ,0.4 ,"    Quit");
	
	glColor3f(1 , 1, 1);
	displayRasterText(-500 ,-300 ,0.4 ,"\"We Are A Perfect Match. You And I. We Truly Are!!!\"");
	displayRasterText(-250, -370,0.4,"---One tile to another(2021)");
	
}
void GameScreenDisplay(){
	SetDisplayMode(GAME_SCREEN);
	
	glColor3f(0 , 0, 1);
	displayRasterTitle(-600,570,0.4,"TILE MASTER");
	
	glColor3f(0, 0, 0.5);
	glBegin(GL_POLYGON);				//ENCLOSING SCOREBOARD
		glVertex3f(-1000 ,200,0.5);
		glVertex3f(-1000 ,-200 ,0.5);
		glVertex3f(-400 ,-200 ,0.5);
		glVertex3f(-400,200, 0.5);
	glEnd();
	
	glBegin(GL_POLYGON);				//ENCLOSING SCOREBOARD
		glVertex3f(-1000 ,200,0.5);
		glVertex3f(-900 ,300 ,0.5);
		glVertex3f(-300 ,300 ,0.5);
		glVertex3f(-400,200, 0.5);
	glEnd();
	
	glBegin(GL_POLYGON);				//ENCLOSING SCOREBOARD
		glVertex3f(-300 ,300,0.5);
		glVertex3f(-400 ,200 ,0.5);
		glVertex3f(-400 ,-200 ,0.5);
		glVertex3f(-300,-100, 0.5);
	glEnd();
	
	glColor3f(0, 0, 1);
	glBegin(GL_LINE_LOOP);				//ENCLOSING SCOREBOARD
		glVertex3f(-1000 ,200,0.5);
		glVertex3f(-1000 ,-200 ,0.5);
		glVertex3f(-400 ,-200 ,0.5);
		glVertex3f(-400,200, 0.5);
	glEnd();
	
	glBegin(GL_LINE_LOOP);				//ENCLOSING SCOREBOARD
		glVertex3f(-1000 ,200,0.5);
		glVertex3f(-900 ,300 ,0.5);
		glVertex3f(-300 ,300 ,0.5);
		glVertex3f(-400,200, 0.5);
	glEnd();
	
	glBegin(GL_LINE_LOOP);				//ENCLOSING SCOREBOARD
		glVertex3f(-300 ,300,0.5);
		glVertex3f(-400 ,200 ,0.5);
		glVertex3f(-400 ,-200 ,0.5);
		glVertex3f(-300,-100, 0.5);
	glEnd();
	
	glColor3f(1, 1, 0);
	displayRasterText(-900 ,100 ,0.4 ,"SCOREBOARD");
	
	glColor3f(1 , 1, 1);
	char temp2[40];
	sprintf(temp2,"Fastest Time : %d",highScore);
	displayRasterText(-900 ,0 ,0.4 ,temp2);
	
	char temp[40];
	int  score = -400+(int)((double)clock()-start)*100/CLOCKS_PER_SEC;
	if(score >=0){
		sprintf(temp,"Your Time : %d",score);
		displayRasterText(-900,-100,0.4,temp);
	}
	
	glColor3f(0.2, 0.84, 0.85);
	glBegin(GL_POLYGON);				//ENCLOSING BOX
		glVertex3f(-170 ,-570,0.5);
		glVertex3f(-170 ,520 ,0.5);
		glVertex3f(920 ,520 ,0.5);
		glVertex3f(920,-570, 0.5);
	glEnd();
	
	glLineWidth(5.0);
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_LOOP);				//ENCLOSING BOX
		glVertex3f(-170 ,-570,0.5);
		glVertex3f(-170 ,520 ,0.5);
		glVertex3f(920 ,520 ,0.5);
		glVertex3f(920,-570, 0.5);
	glEnd();
	
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_LOOP);				//ENCLOSING BOX
		glVertex3f(-110 ,-510,0.5);
		glVertex3f(-110 ,460 ,0.5);
		glVertex3f(860 ,460 ,0.5);
		glVertex3f(860,-510, 0.5);
	glEnd();
	
	glBegin(GL_LINES);				//ENCLOSING BOX
		glVertex3f(-110 ,-510,0.5);
		glVertex3f(-170 ,-570 ,0.5);
		glVertex3f(-110 ,460 ,0.5);
		glVertex3f(-170,520, 0.5);
		glVertex3f(860 ,460 ,0.5);
		glVertex3f(920 ,520 ,0.5);
		glVertex3f(860,-510, 0.5);
		glVertex3f(920,-570, 0.5);
	glEnd();
	glLineWidth(1.0);
	
	glColor3f(1,1,1);
	
	for(int i=0;i<v.size();i++){
		if(firstClickId == v[i].id) {
			v[firstClickId].state = 1;
			v[firstClickId].displayTile();
			continue;
		}
		if(mouseX*2>=v[i].startx and mouseX*2<=v[i].endx and mouseY*2>=v[i].endy and mouseY*2<=v[i].starty){
			if(mButtonPressed) {
				mButtonPressed = false;
				if(firstClickId==-1){
					firstClickId = v[i].id;
					firstClickStyle = v[i].style;
				} else {
					secondClickId = v[i].id;
					secondClickStyle = v[i].style;
					if(firstClickId != secondClickId and firstClickStyle == secondClickStyle){
						v[firstClickId].state = 2;
						v[firstClickId].deleted = true;
						v[firstClickId].displayTile();
						v[secondClickId].state = 2;
						v[secondClickId].deleted = true;
						v[secondClickId].displayTile();
						firstClickId = -1;
						secondClickId = -1;
						correct += 2;
						if(correct == 16){
							gameOver = true;
							end = clock();
							Score = ((double)end-start)/CLOCKS_PER_SEC;
						}
					}
					else {
						firstClickId = -1;
						secondClickId = -1;
					}
				}
				
			}
			else{
				v[i].state = 1;
				v[i].displayTile();
			}
		}
		else{

			v[i].state = 0;
			v[i].displayTile();
		}
	}
	
	glColor3f(1,0,0);
	glBegin(GL_POLYGON);				//ANIMATION
		glVertex3f(-1200 ,700 ,0.5);
		glVertex3f(-1200 ,-700 ,0.5);
		glVertex3f(0-animx ,-700 ,0.5);
		glVertex3f(0-animx,700, 0.5);
	glEnd();
	
	glBegin(GL_POLYGON);				//ANIMATION
		glVertex3f(1200 ,700,0.5);
		glVertex3f(1200 ,-700 ,0.5);
		glVertex3f(0+animx ,-700 ,0.5);
		glVertex3f(0+animx,700, 0.5);
	glEnd();
}

void GameOverScreen(){
	SetDisplayMode(MENU_SCREEN);
	glColor3f(0,0,0);
	glLineWidth(50);
	
	glColor3f(0.5,0.5,0.5);
	glBegin(GL_LINES);               //Border
		glVertex3f(-750 ,-700 ,0.5);
		glVertex3f(-750 ,550 ,0.5);
		
		glVertex3f(-750 ,550 ,0.5);
		glVertex3f(750 ,550 ,0.5);
		
		glVertex3f(750 ,550 ,0.5);
		glVertex3f(750 ,-700, 0.5);
		
		glVertex3f(950 ,-500, 0.5);
		glVertex3f(-950 ,-500 ,0.5);
	glEnd();
	
	glColor3f(1,1,1);
	displayRasterText(-100 , -600,0.4 ,"Created by");
	displayRasterText2(-250 ,-650 ,0.4 ,"HITARTH & DEBARJYA");
	
	glBegin(GL_POLYGON);
		glColor3f(colorx,colory,colorz);
		glVertex3f(-950 ,y_pos ,0.5);		//CARD PLOYGON
		glVertex3f(-950 ,y_pos+600 ,0.5);
		glColor3f(0.32, 0.23, 0.47);
		glVertex3f(-550 ,y_pos+600, 0.5);
		glVertex3f(-550 ,y_pos ,0.5);
	glEnd();
	
	glColor3f(0, 0, 0);
	glBegin(GL_POLYGON);
		glColor3f(0.32, 0.23, 0.47);
		glVertex3f(950 ,y_pos2 ,0.5);		//CARD PLOYGON
		glVertex3f(950 ,y_pos2-600 ,0.5);
		glColor3f(colorx,colory,colorz);
		glVertex3f(550 ,y_pos2-600, 0.5);
		glVertex3f(550 ,y_pos2 ,0.5);
	glEnd();
	
	glLineWidth(1);
	glColor3f(0, 1, 1);
	glBegin(GL_POLYGON);				//GAME OVER
		glVertex3f(-550 ,810,0.5);
		glVertex3f(-550 ,610 ,0.5);
		glVertex3f(550 ,610 ,0.5);
		glVertex3f(550 ,810, 0.5);
	glEnd();
	
	glColor3f(1, 1, 0);
	glBegin(GL_POLYGON);				//RESTART POLYGON
		glVertex3f(-200, 50 ,0.5);
		glVertex3f(-200 ,150 ,0.5);
		glVertex3f(200 ,150 ,0.5);
		glVertex3f(200 ,50, 0.5);
	glEnd();

	glBegin(GL_POLYGON);				//QUIT POLYGON
		glVertex3f(-200 ,-200 ,0.5);
		glVertex3f(-200 ,-100 ,0.5);
		glVertex3f(200, -100 ,0.5);
		glVertex3f(200, -200 ,0.5);
	glEnd();
		
	glColor3f(0 , 0, 1);
	displayRasterText(-200 ,650 ,0.4 ,"G A M E    O V E R ! ! !");
	double finalScore = Score*100-400;
	char temp[40],temp2[40];
	glColor3f(1 , 1, 1);
	sprintf(temp,"Your time : %d",(int)finalScore);
	displayRasterText(-100 ,340 ,0.4 ,temp);
	if(highScore > finalScore){
		highScore = (int)finalScore;
		writeIntoFile(highScore);
	}
	
	sprintf(temp2,"Fastest time : %d",highScore);
	displayRasterText(-100 ,400 ,0.4 ,temp2);
		
	if(mouseX>=-100 && mouseX<=100 && mouseY>=25 && mouseY<=75){
		glColor3f(0 ,0 ,1);
		if(mButtonPressed){                                                       //Reset game default values
			startGame = true ;
			gameOver=false;
			mButtonPressed = false;
			Score = 0;
			myinit();
			start = clock();
			animation = false;
			animx = 0;
		}
	} else
		glColor3f(0 , 0, 0);
	displayRasterText(-70 ,80 ,0.4 ,"Restart");
		
	if(mouseX>=-100 && mouseX<=100 && mouseY>=-100 && mouseY<=-50){
		glColor3f(0 ,0 ,1);
		if(mButtonPressed){
			exit(0);
			mButtonPressed = false;
		}
	}
	else
		glColor3f(0 , 0, 0);
	displayRasterText(-100 ,-170 ,0.4 ,"    Quit");
	
}
void backButton() {
	if(mouseX <= -450 && mouseX >= -500 && mouseY >= -275 && mouseY <= -250){
			glColor3f(0, 0, 1);
			if(mButtonPressed){
				mButtonPressed = false;
				instructionsGame = false;
				startScreenDisplay();
			}
	}
	else glColor3f(1, 1, 1);
	displayRasterText(-1000 ,-550 ,0, "Back");
}

void InstructionsScreenDisplay(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	SetDisplayMode(MENU_SCREEN);
	
	glColor3f(0 , 0, 1);
	displayRasterTitle(-500,570,0.4,"INSTRUCTIONS");
	glColor3f(1, 1, 1);
	displayRasterText(-900 ,400 ,0.4 ,"There are 16 tiles in total");
	displayRasterText(-900 ,300 ,0.4 ,"There are 8 pairs of same colour");
	displayRasterText(-900 ,200 ,0.4 ,"Match Tiles with same colour");
	displayRasterText(-900 ,100 ,0.4 ,"When 2 matching tiles are clicked simultaneously they dissappear");
	displayRasterText(-900 ,0.0 ,0.4 ,"Mouse click to select tile");
	displayRasterText2(-900 ,-200 ,0.4 ,"You Get points based on how fast you finished the game");
	displayRasterText2(-900, -270,0.4,"The Objective is to achieve minimum time");
	backButton();
	if(previousScreen)
		nextScreen = false ,previousScreen = false;
	
	glColor3f(0.15, 0.22 , 0.24);
	glBegin(GL_POLYGON);				
		glVertex3f(-900 ,450-insty,0.6);
		glVertex3f(300 ,450-insty ,0.6);
		glVertex3f(300 , -450 ,0.6);
		glVertex3f(-900 ,-450 ,0.6);
	glEnd();
	
	glColor3f(1, 1, 1);
	glBegin(GL_POLYGON);				//T
		glVertex3f(600 ,-300,0.5);
		glColor3f(0.32, 0.23, 0.47);
		glVertex3f(670 ,-300 ,0.5);
		glVertex3f(670 ,300 ,0.5);
		glVertex3f(600 ,300 ,0.5);
	glEnd();
	glColor3f(1,1,1);
	glBegin(GL_POLYGON);
		glVertex3f(870 ,300 ,0.5);
		glVertex3f(870 ,350 ,0.5);
		glVertex3f(400 ,350 ,0.5);
		glColor3f(0.32, 0.23, 0.47);
		glVertex3f(400 ,300 ,0.5);
	glEnd();

}
void display() {

	glClear(GL_COLOR_BUFFER_BIT);   
	glViewport(0,0,1200,700);

	if(startGame && !gameOver)
		GameScreenDisplay();

	else if(instructionsGame)
		InstructionsScreenDisplay();

	else if(gameOver)
		GameOverScreen();

	else if(startScreen){

			startScreenDisplay();
			if(gameQuit || startGame || optionsGame || instructionsGame){
				//startScreen = false;
				
				if(startGame){
					SetDisplayMode(GAME_SCREEN);
					startScreen = false;
					
				} else if(gameQuit) 
					exit(0);

				} else if(instructionsGame) {
					SetDisplayMode(GAME_SCREEN);
					InstructionsScreenDisplay();
				} 
		}
	glFlush();  
	glScalef(1/2 ,1/2 ,0);
	glLoadIdentity();
	glutSwapBuffers();
}
void myinit(){
	glClearColor(0.5,0.5,0.5,0);
	glColor3f(1.0,0.0,0.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
    gluOrtho2D(-1200,1200,-700,700);                   
	glMatrixMode(GL_MODELVIEW);
	int id = 0;
	random = rand()%4;
	v.clear();
	correct = 0;
	readFromFile();
	for(int i=0;i<4;i++){
		for(int j=0;j<4;j++){
			v.push_back(Tile(i*250-100,j*250-300,id,matrix[random][i][j]));
			id++;
		}
	}
}

void toggleGlutWindowMaximizeBox(char *szWindowTitle){
	    long dwStyle;
	    HWND hwndGlut;
	
	    hwndGlut = FindWindow(NULL, szWindowTitle);
	
	    dwStyle = GetWindowLong(hwndGlut, GWL_STYLE);
	    dwStyle ^= WS_MAXIMIZEBOX;
	    SetWindowLong(hwndGlut, GWL_STYLE, dwStyle);
	}

void mouseClick(int buttonPressed ,int state ,int x, int y) {
	if(buttonPressed == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		{
		mButtonPressed = true;
		}
	else 
		mButtonPressed = false;
	display();
}
void idleCallBack() {			
	 display();
}

void passiveMotionFunc(int x,int y) {

	mouseX = float(x)/(m_viewport[2]/1200.0)-600.0; 
	mouseY = -(float(y)/(m_viewport[3]/700.0)-350.0);
	display();
}

void initialiseHighScoreFile(){
   	ifstream ifile;
   	ifile.open("HighScoreFile.txt");
   	if(ifile) {
    	ifile.close();
  	} else {
    	writeIntoFile(0);
   	}	
}

int main(int argc, char** argv) {	
	initialiseHighScoreFile();
	glutInit(&argc, argv);    
	glutInitWindowSize(1200,700);
	glutInitWindowPosition(0 ,0);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
	glutCreateWindow("Match Tiles");  
	toggleGlutWindowMaximizeBox("Match Tiles");
	glutDisplayFunc(display); 
	glutIdleFunc(idleCallBack);
	glGetIntegerv(GL_VIEWPORT ,m_viewport);
	glutPassiveMotionFunc(passiveMotionFunc);
	glBlendFunc(GL_SRC_ALPHA ,GL_ONE_MINUS_SRC_ALPHA);
	glutTimerFunc(0,timer,0);
	glutMouseFunc(mouseClick);
	//sndPlaySound(TEXT("Free Synthwave Loop.wav"), SND_FILENAME|SND_ASYNC|SND_LOOP);
	myinit();
	SetDisplayMode(MENU_SCREEN);
	glutMainLoop();
	return 0;
}
