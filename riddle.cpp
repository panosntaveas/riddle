#include <conio.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#define width 20       //global parametres
#define height 20
#define MAX_LEN 100    //string length
#define RED 4
#define BLACK 0

void SetColorAndBackground(int ForgC, int BackC) {
	WORD wColor = ((BackC & 0x0F) << 4) + (ForgC & 0x0F);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), wColor);
	return;
}

//set to invisible
void hidecursor(bool flag) {             
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO info;
	info.dwSize = 1;
	info.bVisible = flag;
	SetConsoleCursorInfo(consoleHandle, &info);
}

void Gotoxy(int x, int y) {
	COORD coord;
	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

struct index {
	char *q;
	char a[MAX_LEN];
	char h[MAX_LEN];
	int size;
	int count;
};

typedef struct index index;
index *library;

//removes the \n char. from the strings containing the answer and the hint
void remove_newline(char str[]) {  
	int i = 0;
	while (str[i] != '\0') {
		if (str[i] == '\n') {
			str[i] = '\0';
		}
		i++;
	}
}

//reads the library.txt file with the levels 
//in the global variable library dynamically
//gets each line in buffer->checks if is question->
//stores it in char array temp->puts it in a single string stream->
//finally copies it to library.q
void open_file() {
	FILE *fp;
	char file[20] = "library.txt";
	fp = fopen(file, "r");
	if (fp == NULL) {
		exit(1);
	}
	int i = 0;
	int size = 1;
	int sizet = 1;
	char buffer[MAX_LEN];
	while (!feof(fp)) {
		char *stream;
		int j = 0;
		library = (index *)realloc(library, sizeof(index) * size);
		char **temp = (char **)malloc(sizeof(char *));
		fgets(buffer, MAX_LEN, fp);
		if (buffer[0] == '\n') {
			fgets(buffer, MAX_LEN, fp);
			while (buffer[0] != '\n') {
				temp = (char **)realloc(temp, sizeof(char *) * sizet);
				temp[j] = (char *)malloc(MAX_LEN * sizeof(char));
				strcpy(temp[j], buffer);
				fgets(buffer, MAX_LEN, fp);
				sizet++;
				j++;
			}
			int k = 0;
			stream = (char *)malloc(sizeof(char) * (j * MAX_LEN));
			stream[0] = '\0';
			for (k = 0; k < j; k++) {
				strcat(stream, temp[k]);
			}
			library[i].count = j;
		}
		library[i].q = (char *)malloc(sizeof(char) * j * MAX_LEN);
		strcpy(library[i].q, stream);
		fgets(library[i].a, MAX_LEN, fp);
		fgets(library[i].h, MAX_LEN, fp);
		remove_newline(library[i].a);
		remove_newline(library[i].h);
		library->size = size;
		i++;
		size++;
		free(stream);
		free(temp);
	}
	fclose(fp);
}

//checks for border coords
int is_border(int x, int y) {
	return (x == 0 || x == width - 1 || y == 0 || y == height - 1);
}

//checks for answer coords
int is_answer(int x, int y, int asize) {
	if (x == (width / 2 - asize / 2 - asize % 2) && y == 12) {
		return 1;
	}
	return 0;
}

//rerads multiline quests and print them 
//the way they are inserted in library.txt file
void print_quest(int r) {
	int i = 0;
	int j = 0;
	int count = 0;
	char split[library[r].count][MAX_LEN];
	for (i = 0; i <= (strlen(library[r].q)); i++) {
		if (library[r].q[i] == '\n' || library[r].q[i] == '\0') {
			split[count][j] = '\0';
			count++;
			j = 0;
		} else {
			split[count][j] = library[r].q[i];
			j++;
		}
	}
	int max = 0;
	int k = 0;
	for (k = 0; k < count; k++) {
		if (strlen(split[k]) > max) {
			max = strlen(split[k]);
		}
	}

	int y = height / 3 - count / 2;
	int x = width / 2 - max / 2 - max % 2;
	for (int s = 0; s < count; s++) {
		Gotoxy(x, y);
		printf("%s", split[s]);
		y++;
	}
}

//prints the play environment
void print(int asize, char *quest, int level) {
	char buffer[width + 1];
	int y = 0;
	for (y = 0; y < height; y++) {
		int x = 0;
		for (x = 0; x < width; x++) {
			if (is_border(x, y)) {
				if (y == 0 && x == 0) {
					buffer[x] = 201;
				} else if (y == height - 1 && x == 0) {
					buffer[x] = 200;
				} else if (y == height - 1 && x == width - 1) {
					buffer[x] = 188;
				} else if (y == 0 && x == width - 1) {
					buffer[x] = 187;
				} else if (y == height - 1 || y == 0) {
					buffer[x] = 205;
				} else {
					buffer[x] = 186;
				}
			} else if (is_answer(x, y, asize)) {
				int i = 0;
				for (i = 0; i < asize; i++) {
					buffer[x] = '*';
					x++;
				}
			} else {
				buffer[x] = ' ';
			}
			if (x == 1 && y == height - 1) {
				buffer[x] = 'l';
				x++;
				buffer[x] = 'v';
				x++;
				buffer[x] = '.';
				x++;
				level++;
				if (level < 10) {
					buffer[x] = '0' + level;
				} else if (level >= 10) {
					buffer[x] = '0' + level / 10;
					x++;
					buffer[x] = '0' + level % 10;
				}
			}
		}
		buffer[width] = '\0';
		printf("%s\n", buffer);
	}
	print_quest(level - 1);
	Gotoxy(0, height);
}

struct current {
	struct index index;
	int len_q;
	int len_a;
};
typedef struct current current;

current currentlevel(current cur, int level) {
	cur.index = library[level];
	cur.len_a = strlen(library[level].a);
	cur.len_q = strlen(library[level].q);
	return cur;
}

void print_hint(char *hint, int i) {
	hint = library[i].h;
	Gotoxy(width / 2 - 5, height + 4);
	printf(":%s", hint);
}

//resets the level to:0
//by default is called 
//whenever the game is finished
void reset_level() {
	FILE *fp;
	char file[15] = "player_log.txt";
	int level = 0;
	fp = fopen(file, "w");
	putw(level, fp);
	fclose(fp);
}


void win_message() {
	char c[MAX_LEN] = "You Win!";
	system("cls");
	Gotoxy(width / 2 - strlen(c) / 2, height / 2);
	printf("%s", c);
	Gotoxy(0, height);
	reset_level();
	system("pause");
}

//input function that 
//translates upper case to lower case
//for user ease
char caps_lock_trick(char c) {
	if (c >= 'A' && c <= 'Z') {
		c += 'a' - 'A';
	}
	return c;
}

//creates player_log file if not exists
//reads the current lvl, writes if lvl up
//prevents stray keys from input
//play method
void play() {
	FILE *fp;
	char file[15] = "player_log.txt";
	fopen(file, "r");
	char c;
	int lvl = 0;
	fgetc(fp);
	fclose(fp);
	if (c == '\n') {
		lvl = 0;
		fopen(file, "w");
		fprintf(fp, "%d", lvl);
		fclose(fp);
	}
	fopen(file, "r");
	lvl = 0;
	fscanf(fp, "%d", &lvl);
	int level = lvl;
	fclose(fp);
	int result;
	do {
		fopen(file, "w");
		fprintf(fp, "%d", level);
		fclose(fp);
		current cur = currentlevel(cur, level);
		do {
			print(cur.len_a, cur.index.q, level);
			printf("\n    Enter to solve");
			printf("\n     ESC to exit");
			SetColorAndBackground(4, 0);
			printf("\n     ! for HINT");
			SetColorAndBackground(7, 0);
			result = 1;
			int i = 0;
			char input[cur.len_a];
			int x = width / 2 - cur.len_a / 2 - cur.len_a % 2;
			int y = 12;
			Gotoxy(x, y);
			int pos = x;
			char c;
			while (i < cur.len_a + 1) {
				c = getch();
				c = caps_lock_trick(c);
				if (c == '\r' || pos == x - 1) {
					break;
				} else if (c == '\b' && pos == x) {
					break;
				} else if (c != '\b' && c != '\r' && c != '\n') {
					input[i] = c;
					printf("%c", input[i]);
					pos++;
					i++;
				} else if (c == '\b') {
					i--;
					input[i] = '*';
					pos--;
					Gotoxy(pos, y);
					printf("%c", input[i]);
					Gotoxy(pos, y);
					input[i] = '\0';
				}
				if (c == 27) {
					system("cls");
					return;
				}
				if (c == 33) {
					print_hint(library[i].a, level);
					break;
				}
			}
			input[i] = '\0';
			result = strcmp(input, cur.index.a);
			if (result == 0) {
				level++;
				break;
			}
			Gotoxy(0, 0);
		} while (result != 0);
		system("cls");
		Gotoxy(0, 0);
	} while (level < library->size - 1);
	if (level == library->size - 1) {
		win_message();
	}
}

void welcome() {
	char wel[14] = "...Welcome...";
	int size = strlen(wel);
	int x = width / 2 - size / 2;
	int y = height / 3;
	int i;
	Sleep(400);
	for (i = 0; i < size; i++) {
		Sleep(200);
		Gotoxy(x, y);
		printf("%c", wel[i]);
		Sleep(200);
		Gotoxy(x - 3, y);
		printf(" ");
		x++;
	}
}

int main() {
	hidecursor(FALSE);
	system("cls");
	welcome();
//	reset_level();
	system("cls");
	open_file();
	play();
	free(library);
	return 0;
}
