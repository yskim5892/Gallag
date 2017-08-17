#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <conio.h>

#define RED SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),0x000c ) //빨간색 
#define LIGHT_GREEN SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),0x000a) // 연두
#define WHITE SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),0x000f) // 흰색
#define SKY SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x000b) //하늘색
#define YELLOW SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x000e) //노란색
#define PURPLE SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x000d) //형광
#define BLUE SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_INTENSITY) //짙은 파랑(잘 안보이니 비추)

#define MAKING

#define UP 72
#define DOWN 80
#define LEFT 75
#define RIGHT 77
#define ENTER 13
#define SPACE_BAR 32
#define PAUSE 112
#define PLUS 43
#define MINUS 45

#define startdelay 500

//적/적 미사일 종류
#define basic 1
#define advanced 2
#define developed 4
#define developed_hp1 3

//아이템 종류
#define multiplex 1
#define shield 2
#define bomb 3
#define guided 4
#define freeze 5

#define shield_hp 5 //쉴드의 체력 
#define freeze_time 80 //freeze 지속 턴
#define bomb_power 2 //폭탄이 폭발했을 때 피해 반경

#define hrz 67
#define vtc 22

#define SS 'O' //SHIP SHAPE
#define BMS '^' //BASIC MISSILE SHAPE
#define BMBS '*' //BOMB SHAPE

#define BES 'X' //BASIC ENEMY SHAPE
#define AES 'H' //ADVANCED ENEMY SHAPE
#define DES 'W' //DEVELOPED ENEMY SHAPE
#define EAMS '|' //ENEMY ADVANCED MISSILE SHAPE
#define EDMS 'o' //ENEMY DEVELOPED MISSILE SHAPE

#define MIS '2' //MULTIPLEX ITEM SHAPE
#define SIS '}' //SHIELD ITEM SHAPE
#define BIS '+' //BOMD ITEM SHAPE
#define GIS '$' //GUIDED ITEM SHAPE
#define FIS '@' //FREEZE ITEM SHAPE

#define BS1 'O' //BOSS SHAPE1
#define BS2 'X' //BOSS SHAPE2
#define BS3 ' ' //BOSS SHAPE3

#define developed_stage 4 //developed type이 최초로 등장하는 스테이지
#define boss_stage 3
#define boss_size1 30 //boss 가로 사이즈
#define boss_size2 5 //boss 세로 사이즈

int ship=40; //아군 배의 위치
unsigned int direct=1; //적의 이동 방향(오른쪽은 1, 왼쪽은 -1)
int win_lose=0;
int iflose=0;
int stage=1;
int turn=1;
int SHIELD=0;
int MULTIPLEX=1;
int BOMB=0;
int bomb_shoot=0;
int life=5;
HANDLE hOut;
int delay=30; //게임 속도(숫자가 작을수록 빠름)

int missile[hrz][vtc]={0};
int enemy[hrz][vtc]={0};
int enemymissile[hrz][vtc]={0};
int item[hrz][vtc]={0};
int boss_hp;
int boss_x, boss_y;
int enemy_num=0;
int FREEZE=0;

void CursorView(char show);//커서가 보이지 않게 하려면 CursorView(0);
void gotoxy(int x, int y); //커서를 x, y 좌표로 이동시킴
void stageshow(); //스테이지 보여주는 함수
void allshow();

void stageinitialize();
void enemygenerate();
void drawUI();
void oneturn();
void enemymove();
void shipmove();
void missilemove();
void enemymissilemove();
void bossmove();
void bossoneturn(void);

void win(); //이기면 나오는 문구
void lose(); //지면 나오는 문구
void start(); //시작할 때 나오는 문구
void tutorial();

int main(void)
{
    hOut=GetStdHandle(STD_OUTPUT_HANDLE);
	WHITE;
	CursorView(0);
	srand(time(NULL));
	start();
	ship=40;
	while(life>0)
	{
		turn=1;
		stageshow();
		if(stage==developed_stage)
		{
			system("cls");
			gotoxy(5, 12); printf("%c	이 적은 유도탄 미사일을 발사합니다. 또한 미사일을 두 번 맞아야 죽습니다", DES);
			gotoxy(5, 13); printf("유도탄을 피하기 위해선 움직이고 있어야 합니다.");
			Sleep(1000);
			while(getch()!=ENTER)
				;
			system("cls");
		}
		drawUI();
		stageinitialize();
		if(stage%boss_stage==0)
		{
			boss_x=25;
			boss_y=1;
			boss_hp=100+50*(stage/boss_stage);
		}
		else
		{
			enemygenerate();
		}

		while(win_lose==0)//턴 시작
		{
			Sleep(delay);
			allshow();
			missilemove(); if(FREEZE==0) enemymissilemove();

			if(stage%boss_stage==0)
			{
				bossoneturn();
			}
			else
			{
				oneturn();
			}
			if(win_lose==1)
			{
				FREEZE=0;
				if(iflose==0)
					stage+=1;
				else
				{
					iflose=0;
					life-=1;
					SHIELD=0;
					MULTIPLEX=1;
				}
				win_lose=0;
				ship=40;
				direct=1;
				break;
			}
			turn+=1;
			if(FREEZE!=0)
				FREEZE-=1;
		}
	}
	return 0;
}

void gotoxy(int x, int y)
{
	COORD Pos={x-1, y-1};
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos);
}

void stageshow(void)
{
	int i;
	system("cls");
	gotoxy(35, 12);
	printf("stage %d", stage);
	while(getch()!=ENTER)
		;
	for(i=2; i>=1; i--)
	{
		system("cls");
		gotoxy(35, 12);
		printf("%d초 후 시작", i);
		Sleep(1000);
	}
	system("cls");
}

void allshow()
{
	int i, j;
	for(j=0; j<=vtc-1; j++)
	{
		gotoxy(1, j+1);
		for(i=0; i<=hrz-1; i++)
		{
			if(i==ship-1 && j==vtc-1)
				printf("%c", SS);
			else if(SHIELD!=0 && ship-2<=i && i<=ship && vtc-2<=j && j<=vtc-1)
			{
				if((i==ship-2 || i==ship) && j==vtc-1)
				{
					printf("|");
				}
				else if(ship-2<=i && i<=ship && j==vtc-2)
				{
					printf("-");
				}
			}
			else if(stage%boss_stage==0 && boss_y<=j && j<=boss_y+(boss_size2)-1 && boss_x+1<=i && i<=boss_x+boss_size1){
				if((boss_x+1<=i && i<=boss_x+(boss_size1)/3) || (boss_x+2*((boss_size1)/3)+1<=i && i<=boss_x+boss_size1))
				{
					RED;
					printf("%c", BS1);
				}
				else if(boss_x+(boss_size1)/3+1<=i && i<=boss_x+2*((boss_size1)/3))
				{
					YELLOW;
					printf("%c", BS2);
				}
				else if(boss_y+1<=j && j<=boss_y+3 && (boss_x+13<=i && i<=boss_x+14) || (boss_x+16<=i && i<=boss_x+17))
				{
					WHITE;
					printf("%c", BS3);
				}
				else
				{
					printf(" ");
				}
			}
			else if(missile[i][j]==basic)
			{
				WHITE;
				printf("%c", BMS);
			}
			else if(missile[i][j]==bomb)
			{
				PURPLE;
				printf("%c", BMBS);
			}
			else if(enemymissile[i][j]==advanced)
			{
				printf("%c", EAMS);
			}
			else if(enemymissile[i][j]==developed || enemymissile[i][j]==developed_hp1)
			{
				printf("%c", EDMS);
			}
			else if(item[i][j]==multiplex)
			{
				LIGHT_GREEN;
				printf("%c", MIS);
			}
			else if(item[i][j]==shield)
			{
				LIGHT_GREEN;
				printf("%c", SIS);
			}
			else if(item[i][j]==bomb)
			{
				LIGHT_GREEN;
				printf("%c", BIS);
			}
			else if(item[i][j]==guided)
			{
				LIGHT_GREEN;
				printf("%c", GIS);
			}
			else if(item[i][j]==freeze)
			{
				LIGHT_GREEN;
				printf("%c", FIS);
			}
			else if(enemy[i][j]==basic)
			{
				YELLOW;
				printf("%c", BES);
			}
			else if(enemy[i][j]==advanced)
			{
				RED;
				printf("%c", AES);
			}
			else if(enemy[i][j]==developed || enemy[i][j]==developed_hp1)
			{
				SKY;
				printf("%c", DES);
			}
			else
			{
				printf(" "); //배 또는 적 또는 미사일이 없으면 이전에 있었던 출력을 없앰
			}
			WHITE;
		}
	}
}

void stageinitialize()
{
	int i, j;
	for(i=0; i<=hrz-1; i++)
	{
		for(j=0; j<=vtc-1; j++)
		{
			missile[i][j]=0;
			enemymissile[i][j]=0;
			enemy[i][j]=0; 
			item[i][j]=0; //새로운 스테이지 시작할 시 초기화
		}
	}
}
void enemygenerate()
{
	int i, j;
	enemy_num=0;
	
	for(i=0; i<=(3*stage/2)+10 && i<=hrz-1; i++)
	{
		for(j=0; j<=(stage/3)+4; j++)
		{
			enemy[i][j]=basic;
			enemy_num+=1;
		}
	}
	for(i=(3*stage/2)+15; i<=(3*stage/2)+26 && i<=hrz-1; i++)
	{
		for(j=0; j<=(stage/3)+4; j++)
		{
			enemy[i][j]=advanced;
			enemy_num+=1;
		}
	}
	for(i=(3*stage/2)+31; i<=(3*stage/2)+41 && i<=hrz-1; i++)
	{
		for(j=0; j<=(stage/3)+4; j++)
		{
			enemy[i][j]=basic;
			enemy_num+=1;
		}	
	}
	for(i=(3*stage/2)+15; i<=(3*stage/2)+26 && i<=hrz-1; i++)
	{
		enemy[i][(stage/3)+4]=basic;
	}
	for(i=(3*stage/2+45)/2-2*(stage-developed_stage)+1; i<=(3*stage/2+45)/2+2*(stage-developed_stage)+1; i+=2)
	{
		enemy[i][(stage/3)+5]=developed;
		enemy_num+=2;
	}
}

void drawUI(){
	int i, j;
	for(i=0; i<=hrz; i++)
	{
		gotoxy(i, vtc+1);
		printf("-");
	}
	gotoxy(35, vtc+2);
	printf("life: %d", life);
	gotoxy(35, vtc+3);
	printf("stage: %d", stage);
	gotoxy(35, vtc+4);
	for(j=1; j<=25; j++)
	{
		gotoxy(hrz+1, j);
		printf("|");
	}
}

void oneturn(){
	int i, j, k, l;
	gotoxy(50, vtc+2);
	printf("               ");
	gotoxy(50, vtc+2);
	printf("ENEMIES : %d", enemy_num);

	gotoxy(80, 10);
	printf(" ");
	gotoxy(hrz+2, 10);
	printf("Shield HP:%d", SHIELD);

	gotoxy(80, 11);
	printf(" ");
	gotoxy(hrz+2, 11);
	printf("Bomb:%d", BOMB);

	if(FREEZE<=30)
	{
		RED;
		gotoxy(hrz+4, 13);
		printf("%d", (FREEZE+9)/10);
		WHITE;
	}
	if(FREEZE==0)
	{
		gotoxy(hrz+4, 13);
		printf(" ");
	}
	for(i=0; i<100; i++)
	{
		if(kbhit())  //키보드가 눌러졌으면
			shipmove(); //컨트롤 함수 실행 
	}
	if(turn%2==0)
	{
		if(MULTIPLEX==1)
			missile[ship-1][vtc-2]=basic;//미사일 발사
		if(MULTIPLEX==2)
		{
			missile[ship-2][vtc-2]=basic;
			missile[ship][vtc-2]=basic;
		}
	}
	if(bomb_shoot==1)
	{
		bomb_shoot=0;
		missile[ship-1][vtc-2]=bomb;//폭탄 발사
	}
	for(i=0; i<=hrz-1; i++)
	{
		for(j=0; j<=vtc-1; j++)
		{
			if(turn%4==0)
			{	
				if(enemy[i][j]==advanced)
				{
					if(enemy[i][j+1]==0)
					{
						if(0<=i+direct && i+direct<=hrz-1)
							enemymissile[i+direct][j+1]=enemy[i][j]; //적 미사일 발사
					}
				}
				if(enemy[i][j]==developed || enemy[i][j]==developed_hp1)
				{
					if(turn%30==0)
					{
						if(enemy[i][j+1]==0)
						{
							if(0<=i+direct && i+direct<=hrz-1)
								enemymissile[i+direct][j+1]=enemy[i][j]; //적 미사일 발사
						}
					}
				}
			}
			if(missile[i][j]==basic)
			{
				if(enemy[i][j]==basic || enemy[i][j]==advanced || enemy[i][j]==developed_hp1)//적이 미사일에 맞으면
				{
					missile[i][j]=0;
					enemy[i][j]=0;
					enemy_num-=1;
					if(enemy[i][j]!=developed_hp1)
					{
						if(rand()%200==0 && MULTIPLEX==1)	item[i][j+1]=multiplex;
						if(rand()%75==0)	item[i][j+1]=shield;	
						if(rand()%100==0)	item[i][j+1]=bomb;
#ifndef MAKING
						if(rand()%300==0)	item[i][j+1]=guided;
#endif	
						if(rand()%100==0)	item[i][j+1]=freeze;
					}
				}
				if(enemy[i][j]==developed)
				{
					missile[i][j]=0;
					enemy[i][j]-=1;
					enemy_num-=1;
					if(rand()%100==0 && MULTIPLEX==1)	item[i][j+1]=multiplex;
					if(rand()%30==0)	item[i][j+1]=shield;
					if(rand()%30==0)	item[i][j+1]=bomb;
#ifndef MAKING
					if(rand()%150==0)	item[i][j+1]=guided;
#endif
					if(rand()%40==0)	item[i][j+1]=freeze;
				}
			}
			if(missile[i][j]==bomb)
			{
				if(enemy[i][j]==basic || enemy[i][j]==advanced || enemy[i][j]==developed_hp1)
				{
					Sleep(500);
					for(k=-bomb_power; k<=bomb_power; k++)
					{
						for(l=-bomb_power; l<=bomb_power; l++)
						{
							missile[i-k][j-l]=0;
							enemy[i-k][j-l]=0;
						}
					}
					enemy_num=0;
					for(k=0; k<=hrz-1; k++)
					{
						for(l=0; l<=vtc-1; l++)
						{
							if(enemy[k][l]==basic || enemy[k][l]==advanced || enemy[k][l]==developed_hp1)
								enemy_num+=1;
							if(enemy[k][l]==developed)
								enemy_num+=2;
						}
					}
				}
			}		
		}
	}

	if(item[ship-1][vtc-2]==shield || item[ship-1][vtc-1]==shield) //shield 아이템을 먹으면
	{
		SHIELD+=shield_hp; item[ship-1][vtc-2]=0; item[ship-1][vtc-1]=0;
	}
	if(item[ship-1][vtc-2]==freeze || item[ship-1][vtc-1]==freeze) //freeze 아이템을 먹으면
	{
		FREEZE=freeze_time; item[ship-1][vtc-2]=0; item[ship-1][vtc-1]=0;
	}
	if(item[ship-1][vtc-2]==multiplex || item[ship-1][vtc-1]==multiplex) //multiplex 아이템을 먹으면
	{
		MULTIPLEX=2; item[ship][vtc-2]=0; item[ship-1][vtc-1]=0;
	}
	if(item[ship-1][vtc-2]==bomb || item[ship-1][vtc-1]==bomb) //bomb 아이템을 먹으면
	{
		BOMB+=1; item[ship-1][vtc-2]=0; item[ship-1][vtc-1]=0;
	}

	if(turn%2==0)
		if(FREEZE==0) enemymove();
	if(enemy_num==0)
		win();
	for(i=0; i<=hrz-1; i++)
	{
		if(enemy[i][vtc-1]!=0)//적이 맨 아랫줄로 내려오면
			iflose=1;
	}
	if(enemymissile[ship-1][vtc-2]!=0 || enemymissile[ship-1][vtc-1]!=0)//미사일에 맞으면
	{
		if(FREEZE==0)
		{
			if(SHIELD==0)
				iflose=1;
			else
				SHIELD-=1;
		}
	}
	if(iflose==1)
		lose();
}
void missilemove()
{
	int i, j;
	for(i=0; i<=hrz-1; i++)
	{
		for(j=2; j<=vtc-1; j++)
		{
			if(missile[i][j]==basic || missile[i][j]==0 || missile[i][j]==bomb)
				missile[i][j-1]=missile[i][j];
		}
	}
}

void enemymissilemove()
{
	int i, j;
	int tmp[hrz][vtc]={0};
	for(i=0; i<=hrz-1; i++)
	{
		for(j=0; j<=vtc-2; j++)
		{
			if(enemymissile[i][j]==developed || enemymissile[i][j]==developed_hp1)
				tmp[i+(ship-i-1)/(vtc-j-1)][j+1]=enemymissile[i][j];
			if(enemymissile[i][j]==advanced)
			{
				if(tmp[i][j+1]<=enemymissile[i][j])
					tmp[i][j+1]=enemymissile[i][j];
			}
		}
		enemymissile[i][vtc-1]=0;
	}
	for(i=0; i<=hrz-1; i++)
	{
		for(j=0; j<=vtc-1; j++)
			enemymissile[i][j]=tmp[i][j];
	}
	if(turn%2==0)
	{
		for(i=0; i<=hrz-1; i++)
		{
			for(j=vtc-2; j>=1; j--)
				item[i][j+1]=item[i][j];
		}
	}
}

void enemymove()
{
	int i, j;
	int a=0;
	if(direct==1)
	{
		for(j=0; j<=vtc-2; j++)
		{
			if(enemy[hrz-1][j]!=0)//맨 오른쪽에 적이 있으면
				a=1;
		}
		if(a==1)//맨 오른쪽에 적이 있으면
		{
			direct=-1;//방향을 바꾼다
			for(i=0; i<=hrz-1; i++)
			{
				for(j=vtc-1; j>=1; j--)
					enemy[i][j]=enemy[i][j-1];//적을 한칸씩 아래로 움직인다
			}
			for(i=0; i<=hrz-1; i++)
				enemy[i][0]=0;
		}
		else//맨 오른쪽에 적이 없으면
		{
			for(i=hrz-1; i>=1; i--)
			{
				for(j=0; j<=vtc-2; j++)
					enemy[i][j]=enemy[i-1][j];//적을 한칸씩 오른쪽으로 움직인다
			}
			for(j=0; j<=vtc-2; j++)
				enemy[0][j]=0;
		}
	}
	else
	{
		for(j=0; j<=vtc-2; j++)
		{
			if(enemy[0][j]!=0)
				a=1;
		}
		if(a==1)
		{
			direct=1;
			for(i=0; i<=hrz-1; i++)
			{
				for(j=vtc-1; j>=1; j--)
				{
					enemy[i][j]=enemy[i][j-1];
				}
			}
			for(i=0; i<=hrz-1; i++)
				enemy[i][0]=0;
		}
		else
		{
			for(i=1; i<=hrz-1; i++)
			{
				for(j=0; j<=vtc-2; j++)
				{
					enemy[i-1][j]=enemy[i][j];
				}
			}
			for(j=0; j<=vtc-2; j++)
				enemy[hrz-1][j]=0;
		}
	}
}

void shipmove()
{
	switch(getch())
	{
	case LEFT:
		if(ship!=1) 
			ship-=1;
		break;
	case RIGHT:
		if(ship!=hrz-1) 
			ship+=1;
		break;
	case SPACE_BAR:
		if(BOMB!=0)
		{
			bomb_shoot=1;
			BOMB-=1;
		}
		break;
	case PLUS:
		if(delay>20)
			delay-=5;
		break;
	case MINUS:
		if(delay<80)
			delay+=5;
		break;
	case PAUSE:
		while(1)
		{
			if(kbhit())
			{
				if(getch()==PAUSE)
					break;
			}
		}
		break;
	}
}

void bossoneturn(void)
{
	int i, j, k;
	gotoxy(50, vtc+3);
	printf("              ");
	gotoxy(50, vtc+3);
	printf("Boss HP : %d", boss_hp);
		
	gotoxy(80, 10);
	printf(" ");
	gotoxy(hrz+2, 10);
	printf("Shield HP:%d", SHIELD);
		
	gotoxy(80, 11);
	printf(" ");
	gotoxy(hrz+2, 11);
	printf("Bomb:%d", BOMB);

	if(turn%2==0)
	{
		if(MULTIPLEX==1)
			missile[ship-1][vtc-2]=basic;//미사일 발사
		if(MULTIPLEX==2)
		{
			missile[ship-2][vtc-2]=basic;
			missile[ship][vtc-2]=basic;
		}
	}
	if(bomb_shoot==1)
	{
		bomb_shoot=0;
		missile[ship-1][vtc-2]=bomb;//폭탄 발사
	}
	if(turn%4==0)
	{
		for(i=boss_x; i<=boss_x+9; i++)
			enemymissile[i][boss_size2]=advanced; //적 미사일 발사
		for(i=boss_x+20; i<=boss_x+29; i++)
			enemymissile[i][boss_size2]=advanced; //적 미사일 발사
	}
	bossmove(); //보스 이동 함수
	for(i=0; i<100; i++)
	{
		if(kbhit())  //키보드가 눌러졌으면
			shipmove();
	}
	for(i=0; i<=hrz-1; i++)
	{
		for(j=0; j<=boss_size2-1; j++)
		{
			if(missile[i][j]==basic && boss_x<=i && i<=boss_x+boss_size1-1)//적이 미사일에 맞으면
			{
				missile[i][j]=0;
				boss_hp-=1;
			}
			if(missile[i][j]==bomb && boss_x<=i && i<=boss_x+boss_size1-1)//적이 폭탄에 맞으면
			{
				Sleep(500);
				missile[i][j]=0;
				boss_hp-=20;
				if(boss_hp<0)
					boss_hp=0;
			}
		}
	}
	if(enemymissile[ship-1][vtc-2]!=0 || enemymissile[ship-1][vtc-1]!=0)//미사일에 맞으면
	{
		if(SHIELD==0)
			iflose=1;
		else
			SHIELD-=1;
	}
	if(iflose)
	{
		lose();
		return;
	}
	
	if(boss_hp<=0){
		Sleep(500);
		for(i=1; i<=hrz; i++)
		{
			for(j=1; j<=vtc; j++)
			{
				gotoxy(i, j);
				printf(" ");
			}
		}
		for(k=boss_x; k>=1; k--)
		{
			gotoxy(k, boss_y);
			printf("@@@@@@@@@@          @@@@@@@@@@");
			gotoxy(k, boss_y+1);
			printf("          @@@@@@@@@@          ");
			gotoxy(k, boss_y+2);
			printf("@@@@@@@@@@          @@@@@@@@@@");
			gotoxy(k, boss_y+3);
			printf("          @@@@@@@@@@          ");
			gotoxy(k, boss_y+4);
			printf("@@@@@@@@@@          @@@@@@@@@@");
			gotoxy(ship, vtc);
			printf("%c", SS);
			Sleep(100);
			for(i=1; i<=hrz; i++)
			{
				for(j=1; j<=vtc; j++)
				{
					gotoxy(i, j);
					printf(" ");
				}
			}
			gotoxy(k, boss_y);
			printf("          @@@@@@@@@@          ");
			gotoxy(k, boss_y+1);
			printf("@@@@@@@@@@          @@@@@@@@@@");
			gotoxy(k, boss_y+2);
			printf("          @@@@@@@@@@          ");
			gotoxy(k, boss_y+3);
			printf("@@@@@@@@@@          @@@@@@@@@@");
			gotoxy(k, boss_y+4);
			printf("          @@@@@@@@@@          ");
			gotoxy(ship, vtc);
			printf("%c", SS);
			Sleep(100);
			for(i=1; i<=hrz; i++)
			{
				for(j=1; j<=vtc; j++)
				{
					gotoxy(i, j);
					printf(" ");
				}
			}
			boss_y+=1;
			if(boss_y>=vtc-4)
				break;
			turn+=1;
		}
		gotoxy(ship, vtc);
		printf("%c", SS);
		Sleep(1000);
		for(k=vtc; k>=1; k--)
		{
			gotoxy(ship, k);
			printf("%c", SS);
			Sleep(50);
			for(i=1; i<=hrz; i++)
			{
				for(j=1; j<=vtc; j++)
				{
					gotoxy(i, j);
					printf(" ");
				}
			}
		}
		win();
		return;
	}
}

void bossmove()
{
	int a;
	a=rand()%100+(25+10*(stage/boss_stage))*direct;
	if(boss_x==0)
		direct=1;
	if(boss_x==hrz-boss_size1)
		direct=-1;
	if(a<=48 && boss_x!=0)
	{
		boss_x-=1;
		direct=-1;
	}
	else if(a>=52 && boss_x!=hrz-boss_size1)
	{
		boss_x+=1;
		direct=1;
	}
}

void win()
{
	win_lose=1;
	system("cls");
	printf("□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□\n");
	printf("□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□\n");
	printf("□□■□□□■□□□■□□■■■□□■■□□□□■□□□□□□□\n");
	printf("□□■□□□■□□□■□□□■□□□■□■□□□■□□□□□□□\n");
	printf("□□■□□□■□□□■□□□■□□□■□■□□□■□□□□□□□\n");
	printf("□□□■□■□■□■□□□□■□□□■□□■□□■□□□□□□□\n");
	printf("□□□■□■□■□■□□□□■□□□■□□■□□■□□□□□□□\n");
	printf("□□□■□■□■□■□□□□■□□□■□□□■□■□□□□□□□\n");
	printf("□□□■□■□■□■□□□□■□□□■□□□■□■□□□□□□□\n");
	printf("□□□□■□□□■□□□□■■■□□■□□□□■■□□□□□□□\n");
	printf("□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□\n");
	printf("□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□\n");
	printf("                      ENTER를  누르세요	       			        \n");
	while(getch()!=ENTER)
		;
}
void lose()
{
	win_lose=1;
	system("cls");
	printf("□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□\n");
	printf("□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□\n");
	printf("□□□■□□□□□□□□□□□□□□□□□□□□□□□□□□□□\n");
	printf("□□□■□□□□□□■■■□□□□■■■■□□□■■□□□□□□\n");
	printf("□□□■□□□□□■□□□■□□■□□□□□□■□□■□□□□□\n");
	printf("□□□■□□□□□■□□□■□□■□□□□□□■□□■□□□□□\n");
	printf("□□□■□□□□□■□□□■□□□■■■□□□■■■■□□□□□\n");
	printf("□□□■□□□□□■□□□■□□□□□□■□□■□□□□□□□□\n");
	printf("□□□■□□□□□■□□□■□□□□□□■□□■□□□□□□□□\n");
	printf("□□□■■■■■□□■■■□□□■■■■□□□□■■■□□□□□\n");
	printf("□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□\n");
	printf("□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□\n");
	printf("                      ENTER를  누르세요	       			        \n");
	while(getch()!=ENTER)
		;
}
void start()
{
	int cursor=1;
	int missile[hrz][vtc]={0};
	int enemy[hrz][vtc]={0};
	int enemymissile[hrz][vtc]={0};
	int item[hrz][vtc]={0};
	int i, j;
	system("cls");
	printf("□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□\n");
	printf("□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□\n");
	printf("□□■■■■■□□□□□□□□■□□□□□□□□■■■■■□□□□□\n");
	printf("□■□□□□□□□□□□□□□■□□□□□□□■□□□□□□□□□□\n");
	printf("□■□□□□□□□□□□□□□■□□□□□□□■□□□□□□□□□□\n");
	printf("□■□□□□□□□□■■□□□■□■□□■□□■□□□□□□□□□□\n");
	printf("□■□□□■■□□■□□■□□■□■□□■□□■□□□■■□□□□□\n");
	printf("□■□□□□■□□■□□■□□■□■□□■□□■□□□□■□□□□□\n");
	printf("□□■■■■■□□□■■□■□■□■■■■□□□■■■■■□□□□□\n");
	printf("□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□\n");
	printf("□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□\n");
	printf("□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□□\n");
	printf("					     made by. K.Y.S							    \n");
	printf("				 메뉴를 선택하세요.									\n");
	printf("				 1. 게임 시작										\n");
	printf("				 2. 게임 설명										\n");
	printf("				 3. 게임 종료										\n");
	while(1)
	{
		gotoxy(24, 2*cursor+13);
		printf("  ");
		gotoxy(24, 2*cursor+17);
		printf("  ");
		gotoxy(24, 2*cursor+15);
		printf("->");
		for(i=0; i<=1000; i++)
		{
			if(kbhit())
			{
				j=getch();
				if(j==UP && cursor!=1)
					cursor-=1;
				if(j==DOWN && cursor!=3)
					cursor+=1;
				break;
			}
		}
		if(j==ENTER)
				break;
	}
	if(cursor==2)
	{
		tutorial();
	}		
	if(cursor==3)
		exit(1);
}

void tutorial()
{
	int i, j, k;
	system("cls");
	gotoxy(60, 1);
	printf("made by. K.Y.S ");
	gotoxy(12, 5);
	printf("목표는 모든 적을 섬멸하는 것입니다.");
	gotoxy(12, 7);
	printf("그럼 다음 스테이지로 넘어가게 되고, 더 많고 강한 적들이 등장합니다.");
	while(getch()!=ENTER)
		i=1;
	gotoxy(12, 9);
	printf("보스는 %d스테이지마다 등장합니다.", boss_stage);
	while(getch()!=ENTER)
		i=1;
	gotoxy(12, 10);
	printf("유도 미사일을 쏘는 적은 %d스테이지부터 등장합니다.", developed_stage);
	gotoxy(12, 16);
	printf("현재 3종류 아이템과 3종류의 적이 있습니다.");
	while(getch()!=ENTER)
		i=1;
	system("cls");
	for(i=0; i<=(5*stage/2)+10 && i<=hrz-1; i++)
	{
		for(j=0; j<=(stage/3)+4; j++)
			enemy[i][j]=basic;
	}
	for(i=(5*stage/2)+15; i<=(5*stage/2)+30 && i<=hrz-1; i++)
	{
		for(j=0; j<=(stage/3)+4; j++)
			enemy[i][j]=advanced;
	}
	for(i=(5*stage/2)+35; i<=(5*stage/2)+45 && i<=hrz-1; i++)
	{
		for(j=0; j<=(stage/3)+4; j++)
			enemy[i][j]=basic;	
	}
	for(i=(5*stage/2)+15; i<=(5*stage/2)+30 && i<=hrz-1; i++)
		enemy[i][(stage/3)+4]=basic;
	for(i=(5*stage/2+45)/2-3*(stage-developed_stage)+1; i<=(5*stage/2+45)/2+3*(stage-developed_stage)+1; i+=2)
		enemy[i][(stage/3)+5]=developed;
	gotoxy(hrz/2-4, 10);
	printf("-게임 화면-");
	allshow();
	for(i=1; i<=hrz; i++)
	{
		gotoxy(i, vtc+1);
		printf("-");
	}
	for(j=1; j<=25; j++)
	{
		gotoxy(hrz+1, j);
		printf("|");
	}
	gotoxy(35, vtc+2);
	printf("life: %d", life);
	gotoxy(35, vtc+3);
	printf("stage: %d", stage);
	while(getch()!=ENTER)
		i=1;
	gotoxy(43, vtc+2); printf("현재 라이프");
	while(getch()!=ENTER)
		i=1;
	gotoxy(44, vtc+3); printf("현재 스테이지");
	while(getch()!=ENTER)
		i=1;
	gotoxy(43, vtc+2); printf("           ");
	gotoxy(44, vtc+3); printf("             ");

	gotoxy(ship, vtc-1); printf("|"); gotoxy(ship, vtc-2); printf("|"); 
	gotoxy(ship-30, vtc-3); printf("당신의 배입니다. 방향키를 눌러 좌우로 움직일 수 있습니다.");
	while(getch()==ENTER)
	{
		shipmove(); allshow();
	}
	gotoxy(ship, vtc-1); printf(" "); gotoxy(ship, vtc-2); printf(" "); 
	gotoxy(ship-30, vtc-3); printf("Enter를 눌러서 다음 단계로 진행할 수 있습니다.           ");
	while(1)
	{
		for(i=0; i<=1000; i++)
		{
			if(kbhit())
			{
				shipmove(); allshow();
			}
		}
		j=getch();
		if(j==ENTER)
			break;
	}
	ship=40;
	gotoxy(ship-30, vtc-3); printf("                                                  ");
	for(i=1; i<=hrz; i++)
	{
		gotoxy(i, vtc);
		printf(" ");
	}
	allshow();
	gotoxy(4, 6); printf("|"); gotoxy(4, 7); printf("|"); gotoxy(1, 8); printf("이 적은 미사일을 발사하지 않습니다.");
	while(getch()!=ENTER)
		i=1;
	gotoxy(4, 6); printf(" "); gotoxy(4, 7); printf(" "); gotoxy(1, 8); printf("                                   ");
	gotoxy(22, 5); printf("|"); gotoxy(22, 6); printf("|"); gotoxy(22, 7); printf("|"); gotoxy(3, 8); printf("이 적은 미사일을 발사합니다.");
	while(getch()!=ENTER)
		i=1;
	gotoxy(22, 5); YELLOW; printf("%c", BES); WHITE; gotoxy(22, 6); printf(" "); gotoxy(22, 7); printf(" "); gotoxy(3, 8); printf("                             ");
	gotoxy(ship-30, vtc-3); printf("적의 미사일에 맞으면 라이프가 1 감소합니다.");
	while(getch()!=ENTER)
		i=1;
	gotoxy(1, vtc-3); printf("또한 적이 미사일에 맞으면 일정 확률로 아이템을 떨구며 파괴됩니다.");
	while(getch()!=ENTER)
		i=1;
	ship=3;
	for(k=0; k<=16; k++)
	{
		Sleep(100);
		if(k==0)
			missile[ship-1][vtc-2]=basic;//미사일 발사
		for(i=0; i<=hrz-1; i++)
		{
			for(j=0; j<=vtc-1; j++)
			{
				if(enemy[i][j]==0 && missile[i][j]==0 && enemymissile[i][j]==0 && item[i][j]==0 &&(ship!=i+1 || j!=vtc-1))
				{
					gotoxy(i+1, j+1);
					printf(" "); //배 또는 적 또는 미사일이 없으면 이전에 있었던 출력을 없앰
				}
			}
		}
		allshow(); missilemove();
	}
	Sleep(200);
	gotoxy(3, 5);
	printf(" ");
	item[2][4]=shield;
	for(k=0; k<=5; k++)
	{
		Sleep(100);
		turn=0;
		enemymissilemove();//아이템 이동
		allshow();
		for(i=0; i<=hrz-1; i++)
		{
			for(j=0; j<=vtc-1; j++)
			{
				if(enemy[i][j]==0 && missile[i][j]==0 && enemymissile[i][j]==0 && item[i][j]==0 &&(ship!=i+1 || j!=vtc-1))
				{
					gotoxy(i+1, j+1);
					printf(" "); //배 또는 적 또는 미사일이 없으면 이전에 있었던 출력을 없앰
				}
			}
		}
	}
	gotoxy(3, 12); printf("|"); gotoxy(3, 13); printf("|");
	gotoxy(1, 14); printf("적을 죽였을 때 일정 확률로 나오는 쉴드 아이템입니다. 획득하면 쉴드가 생깁니다.");
	fflush(stdin);
	fflush(stdin);
	fflush(stdin);
	fflush(stdin);
	while(getch()!=ENTER)
		i=1;
	gotoxy(hrz+1, 14); printf("              ");
	gotoxy(hrz+1, 14); printf("|");
	missile[2][3]=0;
	allshow();
	item[2][10]=0;
	allshow();

	for(k=0; k<=16; k++)
	{
		Sleep(100);
		if(k==0)
			missile[ship-1][vtc-2]=basic;//미사일 발사
		for(i=0; i<=hrz-1; i++)
		{
			for(j=0; j<=vtc-1; j++)
			{
				if(enemy[i][j]==0 && missile[i][j]==0 && enemymissile[i][j]==0 && item[i][j]==0 &&(ship!=i+1 || j!=vtc-1))
				{
					gotoxy(i+1, j+1);
					printf(" "); //배 또는 적 또는 미사일이 없으면 이전에 있었던 출력을 없앰
				}
			}
		}
		allshow(); missilemove();
	}
	Sleep(200);
	gotoxy(3, 5);
	printf(" ");
	item[2][4]=freeze;
	for(k=0; k<=5; k++)
	{
		Sleep(100);
		turn=0;
		enemymissilemove();//아이템 이동
		allshow();
		for(i=0; i<=hrz-1; i++)
		{
			for(j=0; j<=vtc-1; j++)
			{
				if(enemy[i][j]==0 && missile[i][j]==0 && enemymissile[i][j]==0 && item[i][j]==0 &&(ship!=i+1 || j!=vtc-1))
				{
					gotoxy(i+1, j+1);
					printf(" "); //배 또는 적 또는 미사일이 없으면 이전에 있었던 출력을 없앰
				}
			}
		}
	}
	gotoxy(3, 12); printf("|"); gotoxy(3, 13); printf("|");
	gotoxy(1, 14); printf("적을 죽였을 때 일정 확률로 나오는 프리즈 아이템입니다. 획득하면 일정 시간동안 적이 멈춥니다.");
	fflush(stdin);
	fflush(stdin);
	fflush(stdin);
	fflush(stdin);
	while(getch()!=ENTER)
		i=1;
	gotoxy(hrz+1, 14); printf("                                           ");
	gotoxy(hrz+1, 14); printf("|");
		
	for(k=0; k<=10; k++)
	{
		Sleep(100);
		turn=0;
		enemymissilemove();//아이템 이동
		allshow();
		for(i=0; i<=hrz-1; i++)
		{
			for(j=0; j<=vtc-1; j++)
			{
				if(enemy[i][j]==0 && missile[i][j]==0 && enemymissile[i][j]==0 && item[i][j]==0 &&(ship!=i+1 || j!=vtc-1))
				{
					gotoxy(i+1, j+1);
					printf(" "); //배 또는 적 또는 미사일이 없으면 이전에 있었던 출력을 없앰
				}
			}
		}
	}
	gotoxy(hrz+4, 13); printf("3");
	gotoxy(hrz+3, 13); printf(">");
	for(i=-1; i<=2; i++)
	{
		gotoxy(hrz+i, 13); printf("-");
	}
	gotoxy(25, 13); printf("freeze아이템의 남은 시간이 표시됩니다.");
	Sleep(1000);
	gotoxy(hrz+4, 13); printf("2");
	Sleep(1000);
	gotoxy(hrz+4, 13); printf("1");
	item[2][vtc-1]=0;
	missile[2][3]=0;
	allshow();
	gotoxy(25, 13); printf("                                                   ");

	Sleep(700);
	for(k=0; k<=16; k++)
	{
		Sleep(100);
		if(k==0)
			missile[ship-1][vtc-2]=basic;//미사일 발사
		for(i=0; i<=hrz-1; i++)
		{
			for(j=0; j<=vtc-1; j++)
			{
				if(enemy[i][j]==0 && missile[i][j]==0 && enemymissile[i][j]==0 && item[i][j]==0 &&(ship!=i+1 || j!=vtc-1))
				{
					gotoxy(i+1, j+1);
					printf(" "); //배 또는 적 또는 미사일이 없으면 이전에 있었던 출력을 없앰
				}
			}
		}
		allshow(); missilemove();
	}
	gotoxy(3, 5);
	printf(" ");
	item[2][4]=multiplex;
	for(k=0; k<=5; k++)
	{
		Sleep(100);
		turn=0;
		enemymissilemove();
		allshow();
		for(i=0; i<=hrz-1; i++)
		{
			for(j=0; j<=vtc-1; j++)
			{
				if(enemy[i][j]==0 && missile[i][j]==0 && enemymissile[i][j]==0 && item[i][j]==0 &&(ship!=i+1 || j!=vtc-1))
				{
					gotoxy(i+1, j+1);
					printf(" "); //배 또는 적 또는 미사일이 없으면 이전에 있었던 출력을 없앰
				}
			}
		}
	}
	gotoxy(3, 12); printf("|"); gotoxy(3, 13); printf("|");
	gotoxy(1, 14); printf("적을 죽였을 때 일정 확률로 나오는 다중발사 아이템입니다. 획득하면 미사일을 2개씩 쏩니다.");
	fflush(stdin);
	fflush(stdin);
	fflush(stdin);
	fflush(stdin);
	while(getch()!=ENTER)
		i=1;
	gotoxy(hrz+1, 14); printf("              ");
	gotoxy(hrz+1, 14); printf("|");
	missile[2][3]=0;
	allshow();
	item[2][10]=0;
	allshow();

	fflush(stdin);
	fflush(stdin);
	fflush(stdin);
	fflush(stdin);
	while(getch()!=ENTER)
		i=1;
	system("cls");
	gotoxy(5, 13); printf("폭탄 아이템은 %c 이렇게 생겼으며 SPACE BAR를 눌러 발사합니다.", BIS);
	fflush(stdin);
	fflush(stdin);
	fflush(stdin);
	fflush(stdin);
	while(getch()!=ENTER)
		i=1;
	system("cls");
	gotoxy(30, 12); printf("게임 시작!!");
	while(getch()!=ENTER)
		i=1;
	system("cls");
}

void CursorView(char show)
{
    HANDLE hConsole;
    CONSOLE_CURSOR_INFO ConsoleCursor;

    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    ConsoleCursor.bVisible = show;
    ConsoleCursor.dwSize = 1;

    SetConsoleCursorInfo(hConsole , &ConsoleCursor);
}