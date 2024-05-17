﻿#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
//#include <sys.time.h>
//#include <fcntl.h>
//#include <sys/ioctl.h>
//#include <sys/types.h>
//#include <sys/stat.h>
//#include <sys/time.h>
//#include <linux/input.h>

//2=tie, 1=com win, 0=user win

//TODO: 카드값 0이 안나오도록 하기
//TODO: 세번째 카드 규칙 만들기

#define MAX_CARD = 3;
#define INITIAL_CARD = 2;
#define	CARDS = [2,3,4,5,6,7,8,9,10]
#define SYMBOLS = ['A','J','Q','K']

bool DEALER_GET_THIRDCARD = false;
bool USER_GET_THIRDCARD = false;
bool STAND = false;

struct Player;
void setCards(struct Player *p);
int defineWinner(int, int);
void win();
void lose();

struct Player {
	int card1;
	int card2;
	int card3;
	int cardCurrentlyHave;
	int cardSum;
};

void setCards(struct Player* p) {
//ex.card1 = (rand() % 11 == 0) ? (rand() % 11) : (rand() % 11);
	srand((unsigned int)time(NULL));
	sleep(1.3);
	int num = rand() % 11;
	sleep(0.4);
	int num2 = rand() % 11;
	
	p->card1 = num==0? rand() % 11: num;
	p->card2 = num2==0? rand() % 11: num2;
}

//세번째 카드 할당하기
/*
void set3rdCard(struct Player* p) {
	p->card3 = rand() % 11;
}*/

//stand일 경우 같은 규칙으로 딜러도 카드를 갖는다.
void is3rdCard(struct Player* u, struct Player* c) {
	printf("inside of is3rdCard\n");
	char answer = 'n';

	printf("u:%d,c:%d\n", (u->cardSum), c->cardSum);
	if (u->cardSum < 7 || c->cardSum <7) {
		printf("natural.\n");
		return; }
	else if (u->cardSum == 6 || 7) {
		printf("Do you want 3rd card? Type y or n.\n");
		scanf("%c", &answer);
		if (answer == 'y') {STAND = true; }
		else { STAND = false; }
	}
	else { 
		printf("As your total sum is lower than 6, you get 3rd card.\n");
		USER_GET_THIRDCARD = true; }
}


int compareCards(struct Player* u, struct Player* c) {
	//카드3개합> 디폴트 매개변수?
	u->cardSum = (u->card1 + u->card2)%10;
	c->cardSum = (c->card1 + c->card2)%10;
	printf("User Sum:%d\nComputer Sum:%d\n",u->cardSum, c-> cardSum);
	int uSum = u->cardSum; int cSum = c->cardSum;
	int result = defineWinner(uSum,cSum);
	printf("uSum,cSum: %d,%d", uSum, cSum); //정상
	//값변경 된거맞아? 얕은복사된듯?
	return result;
	//? 여기 카드sum 출력은 정상임
}
int defineWinner(int uCardSum,int comCardSum) {
	//2=tie, 1=com win, 0=user win
	return (comCardSum == uCardSum)? 2 : ((comCardSum > uCardSum) ? 1 : 0);
}
void printWinner(int value,int choice) {
	value == choice ? win() : lose();
}


int userChoice() {
	printf("Choose Number: 0-You win / 1-Computer wins / 2-Tie ");
	int value;
	scanf("%d", &value);
	printf("You choose %d.\n", value);
	return value;
}


void logo() {
	printf(" ____                                \n");
	printf("|  _ \\                               \n");
	printf("| |_) | __ _  ___ ___ __ _ _ __ __ _ \n");
	printf("|  _ < / _` |/ __/ __/ _` | '__/ _` |\n");
	printf("| |_) | (_| | (_| (_| (_| | | | (_| |\n");
	printf("|____/ \\__,_|\\___\\___\\__,_|_|  \\__,_|\n");
	printf("                                     \n");
}

void explain() {
	printf("어느쪽이 이길까요? 컴퓨터와 대결합니다.\n 1.카드는 총 두장입니다.\
		\n2.두 카드의 수를 더해 일의 자리 수가 9에 가까운 사람이 이깁니다.\
\n 3.컴퓨터와 내 카드 중 어느쪽이 9에 가까울지 골라주세요. 맞추면 승리, 틀리면 패배입니다.\
\n곧 내 카드와 컴퓨터의 카드가 공개됩니다 ...\n");
}

void win() {
	printf(" __    __  ____  ____  \n\
|  |__|  ||    ||    \\ \n\
|  |  |  | |  | |  _  |\n\
|  |  |  | |  | |  |  |\n\
|  `  '  | |  | |  |  |\n\
 \\      /  |  | |  |  |\n\
  \\_/\\_/  |____||__|__|\n\
                       \n");
	printf("Your Guess is correct. Win!\n");
}

void lose() {
	printf(" _       ___   _____   ___ \n\
| |     /   \\ / ___/  /  _]\n\
| |    |     (   \\_  /  [_ \n\
| |___ |  O  |\\__  ||    _]\n\
|     ||     |/  \\ ||   [_ \n\
|     ||     |\\    ||     |\n\
|_____| \\___/  \\___||_____|\n\
                           \n");
	printf("Your Guess is wrong. Lose!\n");
}

void introduction() {
	logo();
	printf("\n");
	explain();
}


int main() {
	introduction();
	struct Player User;
	struct Player Computer;
	struct Player* pUser = &User;
	struct Player* pCom = &Computer;
	setCards(pUser);
	setCards(pCom);
	printf("User Cards number: %d,%d\n", User.card1, User.card2);
	printf("Computer Cards number: %d,%d\n", Computer.card1, Computer.card2);
	printf("main cardsum: %d,%d\n", User.cardSum, Computer.cardSum);
	//세번째 카드 필요한지 여부 판단
	/*
	is3rdCard(pUser, pCom);
	//세번째 카드를 받는 경우
	if (USER_GET_THIRDCARD) {
		set3rdCard(&pUser);//유저의 세번째 카드를 할당한다
		printf("User Cards number: %d,%d,%d\n", User.card1, User.card2,User.card3);
		//유저 stand(안받음) -> 딜러도 같은룰 적용. 6or7이면 안받고 0-5라면 추가로 한장 받음
		if (STAND) {
			printf("stand\n");
		}
		else {
			printf("not stand\n");
			//유저 받음->조건표 참고
		}
		
	}
*/

	int choice = userChoice(); //누가 이길지 추측
	int r = compareCards(pUser, pCom); //카드 3장인 경우 고려하여 추가
	printf("result:%d\n",r);
	printWinner(r, choice);
	return 0;
}

/*
어느쪽이 이길까요? 카드의 숫자 합 중 일의 자리 수가 9에 가까운 사람이 이깁니다.
카드는 두장입니다. ace는 1, JQK은 10으로 합니다.
컴퓨터나 플레이어 둘 중 한명의 카드 수 합이 8이나 9면 '내추럴'이라 하여 추가 카드는 없습니다.
내추럴이 아닐 경우에는 다음과 같습니다.:
플레이어가 가진 카드의 합이 6 또는 7이라면 카드를 받을지 말지 선택할 수 있고, 컴퓨터 역시 플레이어와 동일하게 카드를 받거나 받지않습니다.
0-5라면 추가로 한 장 받습니다.
*/
