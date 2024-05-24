#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <linux/input.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

//
//2=tie, 1=com win, 0=user win

//buzzer
#define IOCTL_START_BUZZER      _IOW('b', 0x07, int) 
#define IOCTL_END_BUZZER        _IOW('b', 0x09, int) 
#define IOCTL_SET_TONE          _IOW('b', 0x0b, int) 
#define IOCTL_SET_VOLUME        _IOW('b', 0x0c, int) 
#define IOCTL_GET_TONE          _IOW('b', 0x0d, int)
#define IOCTL_GET_VOLUME        _IOW('b', 0x0e, int)

bool play = true;

long freqToTone(double freq);
void playTone(long tone, int volume, int time);

long freqToTone(double freq) {
	double tone;
	tone = (1.0f / freq) * 1000000000;
	return (long)tone;
}

void playTone(long tone, int volume, int time) {
	int buzzer_fd = open("/dev/buzzer", O_RDONLY);

	ioctl(buzzer_fd, IOCTL_SET_VOLUME, volume);
	ioctl(buzzer_fd, IOCTL_SET_TONE, tone);
	//printf("Tone: %lu, Volume: %d\n", tone, volume);

	ioctl(buzzer_fd, IOCTL_START_BUZZER, 0);

	usleep(time);

	ioctl(buzzer_fd, IOCTL_END_BUZZER, 0);

	close(buzzer_fd);
}

//523, 587, 659, 698, 783, 880, 987, 1046
void playStartBuzzer() {
	double TONEHZ[8] = { 523, 587, 659, 698, 783, 880, 987, 1046 };

	for (int i = 0; i < 8; i++) {
		long tone = freqToTone((TONEHZ[i]));
		playTone(tone, 25000, 100 * 1000);
	}

}
void playWinBuzzer() {
	double TONEHZ[8] = { 523, 587, 659, 783 };

	for (int i = 0; i < 8; i++) {
		long tone = freqToTone((TONEHZ[i]));
		playTone(tone, 25000, 100 * 1000);
	}

}

void playLoseBuzzer() {
	double TONEHZ[8] = { 1046, 987, 698};

	for (int i = 0; i < 8; i++) {
		long tone = freqToTone((TONEHZ[i]));
		playTone(tone, 25000, 100 * 1000);
	}

}

//led
#define GPIO(BANK, IO) (BANK-1)*32+IO
#define SYSFS_GPIO_DIR "/sys/class/gpio"
#define MAX_BUF 64

#define LED_BACK GPIO(1, 21)
#define LED_HOME GPIO(1, 16)
#define LED_MENU GPIO(1, 20)

#define DELAY 50000

int gpio_export(unsigned int gpio);
int gpio_unexport(unsigned int gpio);
int gpio_set_dir(unsigned int gpio, int value);
int gpio_set_value(unsigned int gpio, int value);


int gpio_export(unsigned int gpio) {
	int fd, len;
	char buf[MAX_BUF];

	fd = open(SYSFS_GPIO_DIR "/export", O_WRONLY);
	if (fd < 0) {
		printf("gpio/export\n");
		return 1;
	}

	len = snprintf(buf, sizeof(buf), "%d", gpio);
	write(fd, buf, len);
	close(fd);

	return 0;
}

int gpio_unexport(unsigned int gpio) {
	int fd, len;
	char buf[MAX_BUF];

	fd = open(SYSFS_GPIO_DIR "/unexport", O_WRONLY);
	if (fd < 0) {
		printf("gpio/unexport\n");
		return 1;
	}

	len = snprintf(buf, sizeof(buf), "%d", gpio);
	write(fd, buf, len);
	close(fd);

	return 0;
}

int gpio_set_dir(unsigned int gpio, int dir) {
	int fd;
	char buf[MAX_BUF];

	snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/direction", gpio);
	fd = open(buf, O_WRONLY);
	if (fd < 0) {
		printf("gpio/direction\n");
		return 1;
	}

	if (dir)
		write(fd, "out\n", 4);
	else
		write(fd, "in\n", 3);

	close(fd);
	return 0;
}

int gpio_set_value(unsigned int gpio, int value) {
	int fd;
	char buf[MAX_BUF];

	snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/value", gpio);
	fd = open(buf, O_WRONLY);
	if (fd < 0) {
		printf("gpio/set-value\n");
		return 1;
	}

	if (value == 1) {
		write(fd, "1\n", 2);
	}
	else {
		write(fd, "0\n", 2);
	}

	close(fd);
	return 0;
}

void playLed()
{
	printf("Application Started!\n");

	int led[3] = { LED_BACK, LED_HOME, LED_MENU };

	for (int i = 0; i < 3; i++) {
		gpio_export(led[i]);
		gpio_set_dir(led[i], 1);
	}

	while (1)
	{
		for (int i = 0; i < 3; i++) {
			usleep(DELAY);
			gpio_set_value(led[i], 1);
		}

		for (int i = 0; i < 3; i++) {
			usleep(DELAY);
			gpio_set_value(led[i], 0);
		}
	}
}


//game
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

/*
void set3rdCard(struct Player* p) {
	p->card3 = rand() % 11;
}*/

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
	u->cardSum = (u->card1 + u->card2)%10;
	c->cardSum = (c->card1 + c->card2)%10;
	printf("User Sum:%d\nComputer Sum:%d\n",u->cardSum, c-> cardSum);
	int uSum = u->cardSum; int cSum = c->cardSum;
	int result = defineWinner(uSum,cSum);
	printf("uSum,cSum: %d,%d", uSum, cSum); //정상
	//shallow copy?
	return result;
	//print cardsum = normal
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
	printf("Which will win?\n 1.There are total two card, one will reveal.\
		\n2.The person whose sum of two cards' last digit is closest to 9 wins.\
\n 3.Choose whether the sum of your two cards or the computer's two cards is closer to 9. If you guess correctly, you win; if you guess wrong, you lose.\
\nSoon the Cards reveal ...\n");
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
	playWinBuzzer();
	playLed();
	
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
	playLoseBuzzer();
	playLed();
	
}

void introduction() {
	logo();
	printf("\n");
	explain();
}

void ask_play() {
	char answer=' ';
	printf("Do you want to play again? Type y or n\n");
	scanf("%c", &answer);
	answer == 'y' ? (play = true) : (play = false);
}


int main() {
	introduction();
	playStartBuzzer(); 

	while(play){
	struct Player User;
	struct Player Computer;
	struct Player* pUser = &User;
	struct Player* pCom = &Computer;
	setCards(pUser);
	setCards(pCom);
	printf("User Cards number: %d,%d\n", User.card1, User.card2);
	printf("Computer Cards number: %d,%d\n", Computer.card1, Computer.card2);
	printf("main cardsum: %d,%d\n", User.cardSum, Computer.cardSum); //wrong

	int choice = userChoice();
	int r = compareCards(pUser, pCom); 
	printf("result:%d\n",r);
	printWinner(r, choice);

	ask_play();//check this works after buzzer&led.
	}
	printf("End Of Program\n");
	return 0;
}
