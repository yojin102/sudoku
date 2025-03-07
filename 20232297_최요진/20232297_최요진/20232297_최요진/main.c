#include<stdio.h>
#include<windows.h>
#include<conio.h>
#include<stdlib.h>
#include<time.h>
#define _CRT_SECURE_NO_WARNINGS
#define EXAMPLE_MACRO_NAME

// 방향키 및 특수키 정의
#define LEFT 75
#define RIGHT 77
#define UP 72
#define DOWN 80
#define Q 113
#define ESC 27
#define ENTER 13
#define R 114

// 32번==> SP: Space
// 13번 ==> CR: 엔터
// 27번 ==> ESC

// 전역 변수 선언
int x[1000], y[1000];	// x,y 좌표값을 저장 (최대 1000개) 
// 먹이의 좌표와 지렁이 크기, 점수 등을 기록하기 위한 변수들
int food_x, food_y;		// 먹이 좌표값 저장
int length;	// 지렁이 몸 길이
int speed;	// 지렁이 속도
int score;	// 현재 점수 저장 (reset함수에 의해 초기화)
int best_score = 0; // 최고 점수 저장 (reset함수에 의해 초기화X)
int last_score = 0; // 마지막 점수 저장 (reset함수에 의해 초기화X)
int direction;		// 이동방향 저장(LEFT, RIGHT, UP, DOWN)
int key;	// 입력받은 키 값 저장 

// 지정된 좌표로 커서를 이동시키고, 문자열 출력
void gotoxy(int x, int y, char* s) {  
	COORD pos = { 2 * x,y };	// 화면에 x좌표는 2배 크기로 이동(문자나 기호의 공간 차지 고려)
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos); // x,y 값을 저장, 표준콘솔출력의 핸들을 반환
	printf("%s", s);	// 지정된 좌표에 문자열 출력
}

// 함수
void game_title(void);	// 게임 시작 화면 
void reset(void);		// 게임 초기화 
void draw_map(void);	// 게임 테두리 
void move_snake(int direction); // 지렁이 머리 이동
void game_pause(void);	// 일시 정지 
void game_over(void);	// 게임 종료
void food(void);		// 먹이 생성 
void status(void);		// 게임 상태 표시(현재 위치, 길이, 속도 등)

int main() {
	game_title();	// 게임 시작 화면 호출

	while (1) {
		// _kbhit()함수 => 키가 입력된 상태인가 체크(입력된 키는 1, 입력 없으면 0 반환)
		// _getch()함수 => 키보드의 입력을 받는 함수
		// _getch()함수가 반환한 값을 변수 key에 저장함.(이때 반환되는 값은 사용자가 누른 키의 ASCII 코드)
		
		// while (key == 224) => key가 224일 때 do-while 반복문 계속 실행
		// 방향키는 특수키여서 _getch() 함수가 첫 번째 값으로 224를 반환하므로
		// 224일 경우 반복문을 통해 두 번째 키 값을 읽도록 함(예: 왼쪽 키를 누르면 두 번째 값으로 75 반환)
		if (_kbhit()) do { key = _getch(); } while (key == 224); // 키 입력 처리(224는 특수 키 처리)
		Sleep(speed);	// 게임 속도에 맞춰 일정 시간 대기

		switch (key) { // 입력받은 키 실행
		case LEFT: case RIGHT: case UP: case DOWN:
			// 방향이 반대로 바뀌지 않도록 처리 (180도 회전 방지함)
			if ((direction == LEFT && key != RIGHT) || (direction == RIGHT && key != LEFT) || (direction == UP && key != DOWN) ||
				(direction == DOWN && key != UP))
				direction = key;	// 유효한 방향만 설정
			key = 0; // 입력된 키 값 초기화
			break;
		case Q: // Q 누르면 일시정지 
			game_pause();
			break;
		case ESC: // ESC 누르면 프로그램 종료 
			exit(0);
		}
		move_snake(direction);	// 지렁이 이동

		status(); // 게임 상태 표시
	}
}

// 게임 시작 화면 그리기
void game_title(void) {
	int i, j;

	while (_kbhit()) _getch(); // 입력된 키값을 버림(게임 시작 전에 입력된 키 처리하지 않음)

	draw_map();    // 게임 테두리 그리기
	// 맵 테두리 안쪽을 빈칸으로 채움
	// i는 새로 방향(행)을 순차적으로 나타냄(화면의 세로 크기 설정)
	// 2 + 1(맵의 첫 번째 행을 넘어가는 위치)부터 시작해서 2 + 20 - 1(화면의 화단에 근접하는 위치)까지 반복함
	// j는 가로 방향(열)을 순차적으로 나타냄
	// 3 + 1(맵의 첫 번째 열을 넘어가는 위치)부터 시작해서 3 + 30 - 1(화면의 오른쪽 끝에 근접하는 위치)까지 반복
	for (i = 2 + 1; i < 2 + 20 - 1; i++) {	
		for (j = 3 + 1; j < 3 + 30 - 1; j++) gotoxy(j, i, "  ");	// 지정된 좌표로 커서 이동하고 그 위치에 빈칸 출력
	}
	// 지렁이 데코
	gotoxy(13, 4, "◎");	// 지렁이 머리(13,4 위치로 커서 이동 후 출력, 머리 시작 위치 지정하고 출력함)
	/*
	* 몸통(위로)
	* i가 4부터 시작해 20까지 2식 증가(몸통이 일정 간격으로 배치하기 위함)
	* 11 + i는 몸통의 가로 위치를 지정. i가 증가할수록 몸통이 오른쪽으로 조금씩 이동
	* 4번 행에 몸통을 수평으로 오른쪽에 배치
	* 
	* 몸통(아래로)
	* i가 5부터 시작해서 20까지 2씩 증가
	* 5번 행에 고정. 계속해서 배치되며 오른쪽으로 나열함
	* 
	* 두 몸통 차이점
	* 첫 번째 부분 => 머리(4번 행)에 이어지는 몸통
	* 두 번째 부분 => 5번 행에 새로운 몸통을 그리는 부분(위에서 그린 몸통과 이어지지만
	*				 행이 달라져 수직 방향으로 아래로 이동한 위치에 그려지게 됨)
	*/
	// 몸통(위로)
	for (i = 4; i < 20; i += 2)
	{
		gotoxy(11 + i, 4, "●");	// 몸통 위치 지정하고 출력
	}
	// 몸통(아래로)
	for (i = 5; i < 20; i += 2)
	{
		gotoxy(9 + i, 5, "●");		// 몸통 위치 지정하고 출력
	}

	// 게임 시작 화면 표시
	gotoxy(12, 7.4, "┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓");
	gotoxy(12, 8.5, "┃            지렁이게임          ┃");
	gotoxy(12, 9.5, "┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛");

	gotoxy(11, 11, " ┃ 플레이하려면 아무 키나 눌러주세요 ┃ ");

	gotoxy(13, 13, "┏━━━━━━━━━ 게임방법 ━━━━━━━━━┓");
	gotoxy(13, 14, "┃                            ┃");
	gotoxy(13, 15, "┃   ▶ ←,→,↑,↓ : 이동    ┃");
	gotoxy(13, 16, "┃   ▶ Q : 일시정지          ┃");
	gotoxy(13, 17, "┃   ▶ ESC : 종료            ┃");
	gotoxy(13, 18, "┃                            ┃");
	gotoxy(13, 19, "┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛");

	while (1) {
		if (_kbhit()) { // 키 입력 받음 
			key = _getch();
			if (key == ESC) exit(0); // ESC키면 종료 
			else break; // 아니면 계속 진행 
		}
		// 출력 메시지 깜빡임 주는 부분
		gotoxy(11, 11, " ┃ 플레이하려면 아무 키나 눌러주세요 ┃ ");
		Sleep(800);	// 메시지가 화면에 보이게하고 지우기 위해 대기
		gotoxy(11, 11, "                                       ");	// 출력한 메시지 지우기 위해 공백 입력
		Sleep(400);	// 메시지 지우고 화면을 깔끔하게 정리하기 위해 대기
	}
	reset(); // 게임을 초기화  
}

// 게임 초기화
void reset(void) {
	int i;
	system("cls");				// 화면 초기화(화면 지우기)
	draw_map();					// 맵 테두리 다시 그리기
	while (_kbhit()) _getch();	// 버퍼에 있는 키값 지움(다시 실행할 때 영향 미치지 않도록 처리)

	// 게임 변수 초기화
	direction = LEFT; // 이동 방향 초기화
	speed = 100;	// 속도 초기화
	length = 4;		// 지렁이 길이 초기화
	score = 0;		// 점수 초기화
	for (i = 0; i < length; i++) { // 지렁이 몸통값 좌표 초기화
		x[i] = 10 + i;
		y[i] = 5;
		gotoxy(3 + x[i], 2 + y[i], "●");
	}
	gotoxy(3 + x[0], 2 + y[0], "◎"); // 지렁이 머리 그림 
	food(); // 먹이 생성 함수 호출
}

// 게임 맵 테두리
void draw_map(void) {
	int i;
	gotoxy(2, 0, "┏");			// 맵 상단 왼쪽
	for (i = 0; i < 37; i++) {
		gotoxy(3 + i, 0, "━");	// 상단 경계선
	}
	gotoxy(40, 0, "┓");		// 맵 상단 오른쪽
	for (i = 1; i < 28; i++) {
		gotoxy(2, i, "┃");		// 좌측 경계선
		gotoxy(40, i, "┃");	// 우측 경계선
	}
	gotoxy(2, 27, "┗");		// 맵 하단 왼쪽
	for (i = 0; i < 37; i++) {
		gotoxy(3 + i, 27, "━");// 하단 경계선
	}
	gotoxy(40, 27, "┛");		// 맵 하단 오른쪽
}

// 지렁이 머리 이동시키고, 벽이나 몸과 충돌이 있는지 확인
void move_snake(int direction) {
	int i;

	// 먹이 먹었을 때
	// 지렁이의 머리 위치가 먹이의 위치와 같으면 먹이를 먹은 것으로 판단
	if (x[0] == food_x && y[0] == food_y) {
		score += 100;	// 먹이 하나당 100점씩 점수 증가 
		food();			// 새로운 먹이 생성 
		length++;		// 먹이를 먹을 때마다 길이 1씩 증가
		x[length - 1] = x[length - 2]; // 새로만든 몸통에 값 입력 (새로운 몸통 위치 설정)
		y[length - 1] = y[length - 2];
	}

	// 벽에 닿았을 때 게임 종료
	// 차례로 왼쪽 벽, 오른쪽 벽, 상단 벽, 하단 벽임
	if (x[0] == -1 || x[0] == 37 || y[0] == -2 || y[0] == 25) {
		game_over();	// 게임 오버 처리
		return; // 게임 오버시 다시 시작하게 되면 여기서부터 반복되므로 
				// return을 사용하여 move_snake의 나머지 부분이 실행되지 않도록 함 
	}

	// 몸에 닿았을 때 게임 종료
	for (i = 1; i < length; i++) {	// 지렁이 머리를 제외한 모든 몸통과 비교함
		if (x[0] == x[i] && y[0] == y[i]) {	// 지렁이의 머리가 몸통 중 하나와 충돌났는지 확인
			game_over();
			return;
		}
	}

	// 몸통을 한 칸씩 이동시키고, 새로운 위치에 그리는 로직
	gotoxy(3 + x[length - 1], 2 + y[length - 1], "  "); // 몸통 마지막을 지움 
	for (i = length - 1; i > 0; i--) { // 몸통 좌표를 한 칸씩 옮김 
		x[i] = x[i - 1];
		y[i] = y[i - 1];
	}
	gotoxy(3 + x[0], 2 + y[0], "●"); // 머리가 있던 곳을 몸통으로 변경
	// 방향에 따라 새로운 머리좌표(x[0],y[0])값을 변경
	if (direction == LEFT) --x[0];	// 왼쪽으로 이동
	if (direction == RIGHT) ++x[0];	// 오른쪽으로 이동
	if (direction == UP) --y[0];	// 위로 이동
	if (direction == DOWN) ++y[0];	// 아래로 이동
	gotoxy(3 + x[i], 2 + y[i], "◎"); // 새로운 머리 좌표 값에 머리를 그림 
}

// Q키를 눌렀을 때 게임 일시정지
void game_pause(void) {
	while (1) {
		if (key == Q) {	// Q가 눌렸을 때
			// 일시정지 메시지 깜빡임 효과
			gotoxy(3, 1, "일시정지");	// 일시정지 메시지 출력
			Sleep(400);					// 일시정지 메시지 보이기 위해 대기
			gotoxy(3, 1, "        ");	// 일시 정지 메시지 지움
			Sleep(400);					// 빈 화면 보이게 함
		}
		else {	// Q키 외 다른 키 눌렸을 때
			draw_map();	// 맵 다시 그리기(게임 진행)
			return;		// 일시정지 해제, 함수 종료
		}
		// 키 입력 처리
		if (_kbhit()) {
			do {
				key = _getch();		// 키 입력 받기
			} while (key == 224);	// 특수 키 눌린 경우 무시
		}

	}
}

// 게임 종료(종료 상태 표시, 점수 기록)
void game_over(void) { 

	gotoxy(13, 10, "┏━━━━━━━━━━━━━━━━━━━━━━━━━━┓");
	gotoxy(13, 11, "┃        죽었습니다.       ┃");
	gotoxy(13, 12, "┗━━━━━━━━━━━━━━━━━━━━━━━━━━┛");

	gotoxy(13, 16.5, " ※ 다시 시작 : ENTER ");

	// 점수 및 최고 점수 기록
	if (score > best_score) {	// 현재 점수가 최고 점수보다 큰 경우
		best_score = score;		// 최고 점수 갱신
		gotoxy(13, 14, "★ 최고 점수 ★ 점수 :");
		printf("%d", last_score = score);	// 마지막 점수 갱신
	}
	else if (score < best_score) {	// 현재 점수가 최고 점수보다 작은 경우
		gotoxy(13, 14, " 점수 : ");
		printf("%d", last_score = score);	// 마지막 점수 출력
	}
	else if (score == 0) {	// 현재 점수가 0인 경우
		gotoxy(13, 14, " 점수 : 0");
	}
	else if (score == best_score) {	// 현재 점수가 최고 점수와 같은 경우
		gotoxy(13, 14, " 점수 : ");
		printf("%d", last_score = score);	// 마지막 점수 출력
	}
	else {
		gotoxy(13, 11, "********error********");	// 오류 처리
	}
	Sleep(500);
	while (_kbhit()) _getch();	// 입력된 키 비움
	key = _getch();	// 키 입력 받음
	game_title();	// 게임 시작 화면으로 돌아감
}

// 랜덤하게 먹이를 생성하고, 먹으면 점수와 길이를 증가시킴
void food(void) {
	int i;
	int food_crush_on = 0;	// 먹이가 지렁이 몸통과 겹칠 경우 다시 생성
	int r = 0;				// 난수 생성에 사용되는 변수 

	// 점수판 표시
	gotoxy(41, 0, "┏━━━━━━━━━━   점수판  ━━━━━━━━━━┓");
	gotoxy(41, 1, "┃ ▶ 현재 점수 : "); // 현재 점수 표시 
	printf("%3d \t\t  ┃", score);
	gotoxy(41, 2, "┃                               ┃");
	gotoxy(41, 3, "┃ ▶ 최근 게임 점수 : "); // 최근 점수 표시 
	printf("%3d \t  ┃", last_score);
	gotoxy(41, 4, "┃                               ┃");
	gotoxy(41, 5, "┃ ▶ 최고 점수 : "); // 최고 점수 표시 
	printf("%3d\t\t  ┃", best_score);
	// 점수판 밑에 공간 채우기 용 하트
	gotoxy(41, 6, "┃"); gotoxy(57, 6, "┃");
	gotoxy(41, 7, "┃"); gotoxy(57, 7, "┃");
	gotoxy(44, 7, "  ♡♡          ♡♡");
	gotoxy(41, 8, "┃"); gotoxy(57, 8, "┃");
	gotoxy(44, 8, "♡♡♡♡      ♡♡♡♡");
	gotoxy(41, 9, "┃"); gotoxy(57, 9, "┃");
	gotoxy(43, 9, "♡♡♡♡♡♡  ♡♡♡♡♡♡");
	gotoxy(41, 10, "┃"); gotoxy(57, 10, "┃");
	gotoxy(42, 10, " ♡♡♡♡♡♡♡♡♡♡♡♡♡♡");
	gotoxy(41, 11, "┃"); gotoxy(57, 11, "┃");
	gotoxy(42, 11, " ♡♡♡♡♡♡♡♡♡♡♡♡♡♡");
	gotoxy(41, 12, "┃"); gotoxy(57, 12, "┃");
	gotoxy(42, 12, "  ♡♡♡♡♡♡♡♡♡♡♡♡♡");
	gotoxy(41, 13, "┃"); gotoxy(57, 13, "┃");
	gotoxy(42, 13, "   ♡♡♡♡♡♡♡♡♡♡♡♡");
	gotoxy(41, 14, "┃"); gotoxy(57, 14, "┃");
	gotoxy(42, 14, "     ♡♡♡♡♡♡♡♡♡♡");
	gotoxy(41, 15, "┃"); gotoxy(57, 15, "┃");
	gotoxy(42, 15, "       ♡♡♡♡♡♡♡♡");
	gotoxy(41, 16, "┃"); gotoxy(57, 16, "┃");
	gotoxy(42, 16, "         ♡♡♡♡♡♡");
	gotoxy(41, 17, "┃"); gotoxy(57, 17, "┃");
	gotoxy(42, 17, "           ♡♡♡♡");
	gotoxy(41, 18, "┃"); gotoxy(57, 18, "┃");
	gotoxy(42, 18, "             ♡♡");
	gotoxy(41, 19, "┃"); gotoxy(57, 19, "┃");
	gotoxy(41, 20, "┃"); gotoxy(57, 20, "┃");
	gotoxy(41, 21, "┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛");

	// 랜덤으로 먹이 좌표 설정
	while (1) {
		food_crush_on = 0;					// 먹이가 몸통과 겹치지 않도록 초기화
		srand((unsigned)time(NULL) + r);	// 난수표 생성 
		food_x = (rand() % 19);				// x 좌표에 난수 생성(0~18)
		food_y = (rand() % 25);				// y 좌표에 난수 생성(0~24)

		for (i = 0; i < length; i++) { // 먹이가 몸통과 겹치는지 확인  
			if (food_x == x[i] && food_y == y[i]) {
				food_crush_on = 1;	// 겹치면 다시 생성
				r++;				// 난수 생성을 위한 변수 증가
				break;
			}
		}

		if (food_crush_on == 1) continue;		// 겹쳤을 경우 while문을 다시 시작 

		gotoxy(3 + food_x, 2 + food_y, "★");	// 안겹쳤을 경우 좌표값에 먹이 그리기
		speed -= 5; // 속도 증가 
		break;		// 먹이 하나 생성 후 탈출
	}
}

// 게임 상태 표시(좌표, 먹이 좌표, 길이, 속도)
void status(void) {
	gotoxy(41, 22, "┏━━━━━━━━━━  현재스텟 ━━━━━━━━━━┓");
	gotoxy(41, 23, "┃ 좌      표 : ");
	printf("%2d,%2d            ┃", x[0], y[0]);		// 현재 지렁이 머리 좌표 출력
	gotoxy(41, 24, "┃ 별  좌  표 : ");
	printf("%2d,%2d            ┃", food_x, food_y);	// 먹이 좌표 출력
	gotoxy(41, 25, "┃ 몸  길  이 : ");
	printf("%2d               ┃", length);				// 지렁이 길이 출력
	gotoxy(41, 26, "┃ 지렁이속도 : ");
	printf("%3d              ┃", speed);				// 지렁이 속도 출력
	gotoxy(41, 27, "┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛");
}