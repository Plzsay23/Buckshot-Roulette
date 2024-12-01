#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <stdbool.h>
#include <time.h>
#include <mmsystem.h> //브금
#pragma comment(lib,"winmm.lib") //브금

#define RANKING 10 //랭킹 배열 크기
#define ITEM 10 //아이템 배열 크기, 아이템 사용 함수에서 잘못된 키를 입력했을때 아이템을 사용하면 안되기에 인덱스 9라는 더미데이터를 포함한 수
#define MAX_ITEM 8 //아이템 최대 갯수
#define MAX_BULLET 8 //최대 총알 갯수 8개
#define LEFT 75
#define RIGHT 77

//함수 순서때문에 따로 선언
void Gamecheck(); void Rankingscreen(); void Archive(); void Game(); void Gamewin(); void Draw_handcuff(); void Item_use(int index);
void Draw_hp(); void Com_adrenaline(int item); void Who_useitem(char c); void Archive_select(int n);

//o전역 변수 선언 구간o//

int Score = 0, /*유저의 점수를 저장할 변수*/ Score_item = 0, /*아이템 사용시 획득하는 점수*/ Score_fire = 0; /*총알 발사시 획득하는 점수*/
int Score_turn = 0, /*매턴마다 획득하는 점수*/ Score_reload = 0; /*장전시마다 획득하는 점수*/
char Name[30]; //유저의 이름을 저장할 변수

bool Score_double = false; //묻더시 점수 두배 변수

int Real_bullet, Fake_bullet; //실탄, 공포탄의 갯수

int Com_hp, User_hp, Max_hp; //컴퓨터 체력, 유저 체력, 매판 랜덤 체력
int Now_bullet = 0, check_bullet; //현재 총알 인덱스, 인덱스 위치 체크 변수

//총알 순서 배열, 최소 2발, 최대 8발인데 배열 크기를 동적으로 할 수 없어서 실탄, 공포탄이 아니면 2로 지정함
int Bullets[MAX_BULLET] = { 2,2,2,2,2,2,2,2 }; 

bool Turn; //true일때 유저, false일때 컴퓨터 턴

bool Handcuff_on = false, /*수갑 사용 여부*/ Saw_on = false, /*톱 사용 여부*/ Inverter_on = false; /*인버터 사용 여부*/
bool Adrenaline_on = false, /*아드레날린 사용 여부*/ Adrenaline_used = false; /*아이템을 지울때 아드레날린이 사용 전인지 판단*/

//맥주0 수갑1 담배2 톱3 아드레날린4 상한약5 돋보기6 대포폰7 인버터8 더미데이터9
//더미 값이 필요한 이유는 잘못된 키 입력시 생길 오류를 방지하기 위함
int User_item[ITEM] = { 0 }, Com_item[ITEM] = { 0 }; //소지 아이템 갯수 배열
int Have_item[ITEM] = { 0 }; //아이템을 획득했는지 확인하는 배열, 각 인덱스별로 확인, 도감에서 사용

//현재 테이블 위치에 아이템이 있는지 없는지 저장하는 배열
bool User_item_table[MAX_ITEM] = { 0 }, Com_item_table[MAX_ITEM] = { 0 }; 

//아이템 테이블 주솟값, 어떤 아이템이 있는지에 따라 주솟값이 바뀜
char User_Table[MAX_ITEM] = { '1','1','1','1','1','1','1','1' }, Com_Table[MAX_ITEM] = { '1','1','1','1','1','1','1','1' }; 

int Com_phone_index = 9; bool Com_phone_bullet = 0; //PC가 대포폰 사용시 받을 정보

bool Bgm_mainscreen = 0; //메인에서 브금이 재시작되는 것을 막는 변수

char END[] = "계속하려면\r아무\r키나\r누르십시오\r.\r.\r."; //여러번 쓰인 문구

int x = 0, y = 0; //아이템을 그릴때 쓸 좌표

typedef struct User { //랭킹 배열을 만들 배경을 생성
    int Userscore; //점수, 게임 종료시 Score가 저장될 공간
    char Username[30]; //이름, 게임 종료시 Name이 저장될 공간
}User; User Ranking[RANKING]; //랭킹 배열 선언

//이미지 출력 관련 설정 코드, GPT 사용
HWND hwnd; //현재 포그라운드 윈도우의 핸들을 가져옴
HDC hdc; //윈도우의 클라이언트 영역에 대한 디바이스 컨텍스트를 가져옴

//위의 이미지 변수들을 전역 변수로 활용하기 위한 셋팅을 하는 함수
void Global_make()
{
    hwnd = GetForegroundWindow();
    hdc = GetWindowDC(hwnd);
}

//색깔 구조체, ex) Textcolor(black, white) 이러면 배경은 검정, 글자는 흰색으로 지정됨
//https://makerejoicegames.tistory.com/115
enum {
    black, blue, green, skyblue, red, pink, orange, white, gray, lightblue, brightgreen, sky, brightred, brightpink, brightyellow, brightwhite
};
//글자의 배경과 글자 색을 지정하는 함수
void Textcolor(int bgcolor, int txcolor)
{
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(handle, (bgcolor << 4) + txcolor);
}

//o브금 함수 구간o// 학교 선배에게 부탁하여 받은 코드

const char* Main_bgm = "Sounds\\Main_bgm.wav"; //메인화면 브금
const char* Gamecheck_bgm = "Sounds\\Gamecheck_bgm.wav"; //게임시작 전 화면 브금
const char* Gameover_bgm = "Sounds\\Gameover_bgm.wav"; //게임오버화면 브금
const char* Gamewin_bgm = "Sounds\\Gamewin_bgm.wav"; //게임승리화면 브금

const char* Reload_sound = "Sounds\\Reload_sound.wav"; //장전소리
const char* Reload_bullet = "Sounds\\Reload_bullet.wav"; //총알 넣는 소리
const char* Fire_real = "Sounds\\Fire_real.wav"; //실탄
const char* Fire_fake = "Sounds\\Fire_fake.wav"; //공포탄

const char* Beer_sound = "Sounds\\Beer_sound.wav"; //아이템 효과음
const char* Handcuff_sound = "Sounds\\Handcuff_sound.wav";
const char* Cigarette_sound = "Sounds\\Cigarette_sound.wav"; 
const char* Saw_sound = "Sounds\\Saw_sound.wav";
const char* Adrenaline_sound = "Sounds\\Adrenaline_sound.wav";
const char* Medicine_sound = "Sounds\\Medicine_sound.wav"; 
const char* Magnifier_sound = "Sounds\\Magnifier_sound.wav";
const char* Phone_sound = "Sounds\\Phone_sound.wav";
const char* Inverter_sound = "Sounds\\Inverter_sound.wav";

//브금을 재생하는 함수
void Play_bgm(const char* filename, bool loop)
{
    int filenameLength = strlen(filename) + 1;
    int bufferSize = MultiByteToWideChar(CP_UTF8, 0, filename, filenameLength, NULL, 0);
    TCHAR* wideFilename = (TCHAR*)malloc(bufferSize * sizeof(TCHAR));
    MultiByteToWideChar(CP_UTF8, 0, filename, filenameLength, wideFilename, bufferSize);
    PlaySound(NULL, NULL, 0);
    if(loop == 1) //반복재생하는 경우
        PlaySound(wideFilename, NULL, SND_FILENAME | SND_LOOP | SND_ASYNC);
    else //한 번만 재생하는 경우
        PlaySound(wideFilename, NULL, SND_FILENAME | SND_ASYNC);
    free(wideFilename);
}

//반복재생된 브금을 끝내는 함수
void Stop_bgm()
{
    PlaySound(NULL, NULL, 0);
}

//o이미지 함수 구간o// 전부 GPT 사용

const char* Beer = "Images\\Beer.raw"; //이미지 선언, 맥주 등 아이템
const char* Handcuff = "Images\\Handcuff.raw"; 
const char* Cigarette = "Images\\Cigarette.raw";
const char* Saw = "Images\\Saw.raw";
const char* Adrenaline = "Images\\Adrenaline.raw";
const char* Medicine = "Images\\Medicine.raw";
const char* Magnifier = "Images\\Magnifier.raw";
const char* Phone = "Images\\Phone.raw";
const char* Inverter = "Images\\Inverter.raw";
const char* Buckshot_left = "Images\\Buckshot_left.raw"; //총, 좌측을 겨눔
const char* Buckshot_right = "Images\\Buckshot_right.raw"; //우측을 겨눔
const char* None = "Images\\None.raw"; //이미지 지움
const char* Warning = "Images\\Warning.raw"; //경고
const char* Game_win = "Images\\Gamewin.raw"; //게임 승리
const char* Real_bullet_image = "Images\\Real_bullet_image.raw"; //실탄
const char* Fake_bullet_image = "Images\\Fake_bullet_image.raw"; //공포탄
const char* HP = "Images\\HP.raw"; //체력
const char* User_image = "Images\\User.raw"; //유저
const char* Com_image = "Images\\Com.raw"; //컴, 1라운드
const char* Com2_image = "Images\\Com2.raw"; //2라운드
const char* Com3_image = "Images\\Com3.raw"; //3라운드

//이미지를 읽어 불러오는 함수
void ReadImage(const char* filePath, unsigned char** image)
{
    FILE* fp = fopen(filePath, "rb");
    fread(*image, 1, 256 * 256, fp);
    fclose(fp);
}

//256x256 크기의 이미지를 그리는 함수
void Draw_image_256(const char* filePath, HDC hdc, int offsetX, int offsetY)
{
    //힙 메모리에 이미지 배열 할당
    unsigned char* image = (unsigned char*)malloc(256 * 256 * sizeof(unsigned char));
    if (image == NULL) return;
    ReadImage(filePath, &image); //이미지 파일을 읽어옴
    //이미지를 그대로 출력
    for (int r = 0; r < 256; r++)
    {
        for (int c = 0; c < 256; c++)
        {
            int gray = image[r * 256 + c];
            SetPixel(hdc, c + offsetX, r + offsetY, RGB(gray, gray, gray));
        }
    }
    free(image); //힙 메모리 해제
}

//256x256 크기의 이미지를 128x128로 축소하여 그리는 함수
void Draw_image_128(const char* filePath, HDC hdc, int offsetX, int offsetY)
{
    //힙 메모리에 이미지 배열 할당
    unsigned char* image = (unsigned char*)malloc(256 * 256 * sizeof(unsigned char));
    if (image == NULL) return;
    ReadImage(filePath, &image); //이미지 파일을 읽어옴
    //128x128 크기의 축소된 이미지 배열을 힙 메모리에 할당
    unsigned char* smallImage = (unsigned char*)malloc(128 * 128 * sizeof(unsigned char));
    if (smallImage == NULL) return;
    //이미지 크기 축소
    for (int r = 0; r < 128; r++)
    {
        for (int c = 0; c < 128; c++)
        {
            int sum = image[2 * r * 256 + 2 * c] +
                image[2 * r * 256 + 2 * c + 1] +
                image[(2 * r + 1) * 256 + 2 * c] +
                image[(2 * r + 1) * 256 + 2 * c + 1];
            smallImage[r * 128 + c] = sum / 4;
        }
    }
    //축소된 이미지를 출력
    for (int r = 0; r < 128; r++)
    {
        for (int c = 0; c < 128; c++)
        {
            int gray = smallImage[r * 128 + c];
            SetPixel(hdc, c + offsetX, r + offsetY, RGB(gray, gray, gray));
        }
    }
    free(image); //힙 메모리 해제
    free(smallImage);
}

//256x256 크기의 이미지를 64x64로 축소하여 그리는 함수
void Draw_image_64(const char* filePath, HDC hdc, int offsetX, int offsetY)
{
    //힙 메모리에 이미지 배열 할당
    unsigned char* image = (unsigned char*)malloc(256 * 256 * sizeof(unsigned char));
    if (image == NULL) return;
    ReadImage(filePath, &image); //이미지 파일을 읽어옴
    //64x64 크기의 축소된 이미지 배열을 힙 메모리에 할당
    unsigned char* smallImage = (unsigned char*)malloc(64 * 64 * sizeof(unsigned char));
    if (smallImage == NULL) return;

    //이미지 크기 축소
    for (int r = 0; r < 64; r++)
    {
        for (int c = 0; c < 64; c++)
        {
            int sum = image[4 * r * 256 + 4 * c] +
                image[4 * r * 256 + 4 * c + 1] +
                image[4 * r * 256 + 4 * c + 2] +
                image[4 * r * 256 + 4 * c + 3] +
                image[(4 * r + 1) * 256 + 4 * c] +
                image[(4 * r + 1) * 256 + 4 * c + 1] +
                image[(4 * r + 1) * 256 + 4 * c + 2] +
                image[(4 * r + 1) * 256 + 4 * c + 3] +
                image[(4 * r + 2) * 256 + 4 * c] +
                image[(4 * r + 2) * 256 + 4 * c + 1] +
                image[(4 * r + 2) * 256 + 4 * c + 2] +
                image[(4 * r + 2) * 256 + 4 * c + 3] +
                image[(4 * r + 3) * 256 + 4 * c] +
                image[(4 * r + 3) * 256 + 4 * c + 1] +
                image[(4 * r + 3) * 256 + 4 * c + 2] +
                image[(4 * r + 3) * 256 + 4 * c + 3];
            smallImage[r * 64 + c] = sum / 16;
        }
    }
    //축소된 이미지를 출력
    for (int r = 0; r < 64; r++)
    {
        for (int c = 0; c < 64; c++)
        {
            int gray = smallImage[r * 64 + c];
            SetPixel(hdc, c + offsetX, r + offsetY, RGB(gray, gray, gray));
        }
    }
    free(image); //힙 메모리 해제
    free(smallImage);
}

//256x256 크기의 이미지를 512x512로 확대하여 그리는 함수
void Draw_image_512(const char* filePath, HDC hdc, int offsetX, int offsetY)
{
    //힙 메모리에 이미지 배열 할당
    unsigned char* image = (unsigned char*)malloc(256 * 256 * sizeof(unsigned char));
    if (image == NULL) return;
    ReadImage(filePath, &image); //이미지 파일을 읽어옴
    //512x512 크기의 확대된 이미지 배열을 힙 메모리에 할당
    unsigned char* largeImage = (unsigned char*)malloc(512 * 512 * sizeof(unsigned char));
    if (largeImage == NULL) return;
    //양선형 보간법을 사용하여 이미지 크기 확대
    for (int r = 0; r < 512; r++)
    {
        for (int c = 0; c < 512; c++)
        {
            float x = c / 2.0f;
            float y = r / 2.0f;
            int x1 = (int)x;
            int y1 = (int)y;
            int x2 = x1 + 1;
            int y2 = y1 + 1;
            if (x2 >= 256) x2 = 255;
            if (y2 >= 256) y2 = 255;
            float a = x - x1;
            float b = y - y1;
            //양선형 보간
            float top = (1 - a) * image[y1 * 256 + x1] + a * image[y1 * 256 + x2];
            float bottom = (1 - a) * image[y2 * 256 + x1] + a * image[y2 * 256 + x2];
            float gray = (1 - b) * top + b * bottom;
            largeImage[r * 512 + c] = (unsigned char)gray;
        }
    }
    //확대된 이미지를 출력
    for (int r = 0; r < 512; r++)
    {
        for (int c = 0; c < 512; c++)
        {
            int gray = largeImage[r * 512 + c];
            SetPixel(hdc, c + offsetX, r + offsetY, RGB(gray, gray, gray));
        }
    }
    free(image); //힙 메모리 해제
    free(largeImage);
}

//o세팅 함수 구간o//

//커서 깜빡거리는거 지우기
//https://blog.naver.com/ifthe1201/221056588399
void Invisible_Cursor(char show)
{
    HANDLE hConsole; CONSOLE_CURSOR_INFO ConsoleCursor;
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    ConsoleCursor.bVisible = show; ConsoleCursor.dwSize = 1;
    SetConsoleCursorInfo(hConsole, &ConsoleCursor);
}

//커서 위치 이동
//https://blog.naver.com/ifthe1201/221056588399
void gotoxy(int x, int y)
{
    COORD pos = { x,y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

//콘솔 창 크기와 제목 지정
//https://blockdmask.tistory.com/344
void SetConsoleView(void)
{
    system("mode con:cols=200 lines=50");
    system("title Buckshot Roulette");
}

//o게임 화면 구간o//

//메인화면 테두리 그리는 함수, 워낙 여러번 쓰다보니 함수로 따로 빼는게 효율적, 우측 하단의 좌표 (199, 49)
void Draw_square()
{
    for (int i = 0; i < 199; i++) { gotoxy(i, 0); printf("■"); gotoxy(i, 49); printf("■"); }
    for (int i = 1; i < 49; i++) { gotoxy(0, i); printf("■"); gotoxy(198, i); printf("■"); }
}

//아이템 테이블을 그리는 함수
void Draw_table()
{
    int x1; //유저와 PC의 테이블 두개를 그리기 위함
    for (int a = 0; a < 2; a++)
    {
        if (a == 0) x1 = 0; 
        else x1 = 113; 
        gotoxy(10 + x1, 22); printf("|"); for (int i = 0; i < 65; i++) printf("-"); printf("|");
        for (int k = 0; k < 8; k++) { gotoxy(10 + x1, 23 + k); for (int i = 0; i < 3; i++) { printf("|"); for (int j = 0; j < 21; j++) { printf(" "); } } printf("|"); }
        gotoxy(10 + x1, 31); printf("|"); for (int i = 0; i < 65; i++) printf("-"); printf("|");
        for (int k = 0; k < 8; k++) { gotoxy(10 + x1, 32 + k); for (int i = 0; i < 3; i++) { printf("|"); for (int j = 0; j < 21; j++) { printf(" "); } } printf("|"); }
        gotoxy(10 + x1, 40); printf("|"); for (int i = 0; i < 65; i++) printf("-"); printf("|");
        for (int k = 0; k < 8; k++) { gotoxy(10 + x1, 41 + k); for (int i = 0; i < 3; i++) { printf("|"); for (int j = 0; j < 21; j++) { printf(" "); } } printf("|"); }
        gotoxy(10 + x1, 49); printf("|"); for (int i = 0; i < 65; i++) printf("-"); printf("|");
    }
}

//게임 첫 시작시 등장하는 경고문
void Warning_screen()
{
    gotoxy(52, 30); printf("    CAPS LOCK이 켜져있는지 확인하시오! 만약 켜져있다면 제대로 작동하지 않을 수 있습니다!");
    gotoxy(52, 32); printf("                   화면 클릭 또는 드래그 시 이미지가 지워질 수도 있습니다!");
    gotoxy(52, 34); printf("               배경 음악의 소리가 클 수도 있습니다! 게임 진입 후 조정해주세요!");
    gotoxy(52, 36); printf("                  키를 연타하지 마세요! 의도치 않는 행동을 할 수도 있습니다!");

    for (int i = 0; END[i] != '\0'; i++) //계속하려면 아무 키나 누르십시오 . . .
    {
        gotoxy(79 + i, 38); printf("%c", END[i]);
        Sleep(50);
    } Sleep(500);
    
    Draw_image_256(Warning, hdc, 670, 200); //경고 이미지

    _getch(); //키입력 대기

    for (int i = 52; i < 150; i++) //글씨 지우기
    {
        gotoxy(i, 30); printf(" "); gotoxy(i, 32); printf(" ");
        gotoxy(i, 34); printf(" "); gotoxy(i, 36); printf(" ");
        gotoxy(i, 38); printf(" ");
        Sleep(10);
    }
    Sleep(1000);
    system("cls");
    Sleep(3000);
}

//메인화면의 글씨 색깔을 바꾸기 위해 따로 뺀 함수
void Draw_main()
{
    gotoxy(116, 20); printf("■■■■■■■   ■            ■   ■■■■■■ ■         ■   ");
    gotoxy(116, 21); printf("■            ■ ■            ■  ■            ■      ■     ");
    gotoxy(116, 22); printf("■            ■ ■            ■ ■             ■    ■      ");
    gotoxy(116, 23); printf("■■■■■■■   ■            ■ ■             ■ ■          ");
    gotoxy(116, 24); printf("■            ■ ■            ■ ■             ■    ■       ");
    gotoxy(116, 25); printf("■            ■ ■            ■  ■            ■      ■     ");
    gotoxy(116, 26); printf("■■■■■■■    ■■■■■■■    ■■■■■■ ■         ■  ");
    gotoxy(96, 30); printf("   ■■■■■■  ■            ■   ■■■■■■    ■■■■■■■");
    gotoxy(96, 31); printf("  ■             ■            ■ ■            ■        ■");
    gotoxy(96, 32); printf("  ■             ■            ■ ■            ■        ■");
    gotoxy(96, 33); printf("    ■■■■■■ ■■■■■■■■ ■            ■        ■");
    gotoxy(96, 34); printf("              ■ ■            ■ ■            ■        ■");
    gotoxy(96, 35); printf("              ■ ■            ■ ■            ■        ■");
    gotoxy(96, 36); printf("  ■■■■■■   ■            ■   ■■■■■■          ■");
}

//메인화면 //콘솔 창 크기는 120x30 임 ( 픽셀 비율 => 가로:세로 = 2:1 )
void Mainscreen(void)
{
    system("cls");
    if (Bgm_mainscreen == 0) //브금 시작, 메인,랭킹,도감 화면 이동시에는 브금이 다시 재생되지 않음
    {
        Play_bgm(Main_bgm, 1);
        Bgm_mainscreen = true; //이 변수가 1이면 메인,랭킹,도감 화면을 이동해도 브금이 끊기지 않음
    }
    Draw_square();
    gotoxy(20, 19); printf("         .... NO! ...                  ... MNO! ...  ");
    gotoxy(20, 19); printf("        .....MNO!! ...................... MNNOO! ... ");
    gotoxy(20, 20); printf("     ..... MMNO! ......................... MNNOO!! .   ");
    gotoxy(20, 21); printf("    .... MNOONNOO!   MMMMMMMMMMPPPOII!   MNNO!!!! . ");
    gotoxy(20, 22); printf("     ... !O! NNO! MMMMMMMMMMMMMPPPOOOII!! NO! ....  ");
    gotoxy(20, 23); printf("         ......! MMMMMMMMMMMMMPPPPOOOOIII! ! ...  ");
    gotoxy(20, 24); printf("       ........ MMMMMMMMMMMMPPPPPOOOOOOII!! .....  ");
    gotoxy(20, 25); printf("       ........ MMMMMOOOOOOPPPPPPPPOOOOMII! ...  ");
    gotoxy(20, 26); printf("        ....... MMMMM..    OPPMMP    .,OMI! ....");
    gotoxy(20, 27); printf("         ...... MMMM::   o.,OPMP,.o   ::I!! ...");
    gotoxy(20, 28); printf("             .... NNM:::.,,OOPM!P,.::::!! ....");
    gotoxy(20, 29); printf("              .. MMNNNNNOOOOPMO!!IIPPO!!O! .....");
    gotoxy(20, 30); printf("             ... MMMMMNNNNOO:!!:!!IPPPPOO! ....");
    gotoxy(20, 31); printf("               .. MMMMMNNOOMMNNIIIPPPOO!! ......");
    gotoxy(20, 32); printf("              ...... MMMONNMMNNNIIIOO!..........");
    gotoxy(20, 33); printf("           ....... MN MOMMMNNNIIIIIO! OO ..........");
    gotoxy(20, 34); printf("        ......... MNO! IiiiiiiiiiiiI OOOO ...........");
    gotoxy(20, 35); printf("         ....NNN.MNO! . O!!!!!!!!!O . OONO NO! ........");
    gotoxy(20, 36); printf("        ....MNNNNNO! ...OOOOOOOOOOO .  MMNNON!........");
    gotoxy(20, 37); printf("        ......MNNNNO! .. PPPPPPPPP .. MMNON!........");
    gotoxy(20, 38); printf("       ......    OO! ................. ON! .......");
    gotoxy(85, 19); printf("        |\\___________________,");
    gotoxy(85, 20); printf("       |    ===== _______)___) ");
    gotoxy(85, 21); printf("      |     ||----||");
    gotoxy(85, 22); printf("   __/___   ====/");
    gotoxy(85, 23); printf("  (O___)\\\\_(_/");
    gotoxy(85, 24); printf(" (O___)");
    gotoxy(85, 25); printf("(O___)");
    Draw_main();

    gotoxy(41, 10); printf("게임 시작 : 1");
    gotoxy(93, 10); printf("랭킹 보기 : 2");
    gotoxy(145, 10); printf("도감보기 : 3");
    gotoxy(186, 48); printf("게임종료 : Q");

    //참고 https://jesus12.tistory.com/entry/C%EC%96%B8%EC%96%B4-%ED%81%B4%EB%9F%AD-%ED%95%A8%EC%88%98-clock-3%EC%B4%88-%EB%8F%99%EC%95%88-%EB%B0%98%EB%B3%B5%ED%95%98%EA%B8%B0
    clock_t time = clock(); //시작 시점에 현재시간을 기록

    while (1) //행동 선택
    {
        char input;
        if (_kbhit()) //키보드가 눌렸을때
        {
            input = _getch();
            if (input == '1') // 1을 누르면 게임시작
            {
                Bgm_mainscreen = false; //이 변수를 0을 줘야 나중에 다시 메인으로 갔을때 브금이 재생됨
                Stop_bgm(); //브금 종료
                Gamecheck(); break; //게임 시작
            }
            else if (input == '2') // 2를 누르면 랭킹화면
            {
                Rankingscreen(); break;
            }
            else if (input == '3') // 3을 누르면 도감화면
            {
                Archive(); break;
            }
            else if (input == 'q') //게임종료
                exit(0);
        }

        if (time + 200 == clock()) //시작 시점에서 현재시간 "clock()" 까지 걸린 시간이 200ms이면 발동
        {
            Textcolor(black, gray); //메인의 글씨를 반짝이게 함
            Draw_main();
            Textcolor(black, white);
        }
        if (time + 400 == clock()) //400ms
        {
            Textcolor(black, brightwhite);
            Draw_main();
            Textcolor(black, white);
        }
        if (time + 600 == clock()) //600ms
        {
            time = clock(); //시작 시점을 초기화하여 if문들이 무한 반복되도록 설정
            Textcolor(black, white);
            Draw_main();
            Textcolor(black, white);
        }
    }
}

//첫 게임시작시 등장하는 함수, 묻더시엔 등장하지 않음
void Gamecheck(void)
{
    system("cls"); Sleep(3000);
    Play_bgm(Gamecheck_bgm, 1); Sleep(1500); //새로운 브금

    //벅샷 룰렛에 참여하는 것을 동의하는 서약서, 게임 컨셉에 맞게 디자인
    gotoxy(66, 5); for (int i = 0; i < 68; i++) printf("-");
    for (int i = 0; i < 38; i++) { gotoxy(66, 6 + i); printf("|"); gotoxy(66 + 67, 6 + i); printf("|"); }
    gotoxy(66, 44); for (int i = 0; i < 68; i++) printf("-");
    gotoxy(95, 6); printf("Memorandum");
    gotoxy(68, 8); printf("This pledge has been prepared for all those who voluntarily part");
    gotoxy(68, 9); printf("icipate in the Buckshot Roulette game. By signing this pledge, t");
    gotoxy(68, 10); printf("he signatory confirms that she understands and agrees to all of ");
    gotoxy(68, 11); printf("the following conditions.");
    gotoxy(68, 13); printf("1. Voluntary participation");
    gotoxy(68, 15); printf("    I pledge to participate voluntarily in the Buck Roulette gam");
    gotoxy(68, 16); printf("    e, and to participate at my own will without any coercion or");
    gotoxy(68, 17); printf("    pressure.");
    gotoxy(68, 19); printf("2. Recognition of risk");
    gotoxy(68, 21); printf("    I understand that buckshot roulette is an extremely dangerou");
    gotoxy(68, 22); printf("    s activity, and I am fully aware that there can be life-thre");
    gotoxy(68, 23); printf("    atening risks.");
    gotoxy(68, 25); printf("3. Exemption from liability");
    gotoxy(68, 27); printf("    I pledge that I will not hold the organizer, participant, or");
    gotoxy(68, 28); printf("    any third party legally liable for any injuries, damages, or");
    gotoxy(68, 29); printf("    deaths that may result from participating in this game.");
    gotoxy(68, 31); printf("4. Confidentiality");
    gotoxy(68, 33); printf("    I pledge that I will keep all information related to the pro");
    gotoxy(68, 34); printf("    gress of this game confidential and will not divulge it to t");
    gotoxy(68, 35); printf("    hird parties.");
    gotoxy(68, 37); printf("Date : --/--/--    Birthday : --/--/--    Phone-number : -------");
    gotoxy(68, 39); printf("Address : 28, Hangsin-ro 51-gil, Seongbuk-gu, Seoul (02876)");
    gotoxy(115, 42); printf("Sign : __________");
    gotoxy(67, 45); printf("※ 이름에 공백이 들어가면 랭킹 출력에 오류가 생길 수 있습니다. ※");

    gotoxy(68, 42); printf("이름을 입력해주세요 : "); fgets(Name, 20, stdin); //이름 입력받음, 공백을 포함하기 위함
    Score = 0; Score_item = 0; Score_fire = 0;//점수 초기화
    
    Stop_bgm(); //브금 종료
    Game(); //게임시작
}

//묻고 더블로 갈지 묻는 함수
void Check_double()
{
    Sleep(2000);
    system("cls");
    Sleep(2000);

    gotoxy(59, 28); printf("■■■    ■■■  ■     ■ ■■■■  ■       ■■■■        ■■■  ■■■■");
    gotoxy(59, 29); printf("■   ■  ■    ■ ■     ■ ■     ■ ■       ■             ■    ■ ■     ■");
    gotoxy(59, 30); printf("■    ■ ■    ■ ■     ■ ■■■■  ■       ■■■■       ■    ■ ■■■■");
    gotoxy(59, 31); printf("■    ■ ■    ■ ■     ■ ■     ■ ■       ■             ■    ■ ■     ■");
    gotoxy(59, 32); printf("■   ■  ■    ■ ■     ■ ■     ■ ■       ■             ■    ■ ■     ■");
    gotoxy(59, 33); printf("■■■    ■■■   ■■■   ■■■■  ■■■■ ■■■■        ■■■  ■     ■");

    gotoxy(59, 35); printf("■      ■  ■■■  ■■■■■ ■      ■ ■■■ ■     ■  ■■■■    ■■■■");
    gotoxy(59, 36); printf("■■    ■ ■    ■     ■     ■      ■   ■   ■■   ■ ■          ■      ■");
    gotoxy(59, 37); printf("■ ■   ■ ■    ■     ■     ■■■■■   ■   ■ ■  ■ ■ ■■■         ■");
    gotoxy(59, 38); printf("■  ■  ■ ■    ■     ■     ■      ■   ■   ■  ■ ■ ■      ■      ■");
    gotoxy(59, 39); printf("■    ■■ ■    ■     ■     ■      ■   ■   ■   ■■ ■      ■");
    gotoxy(59, 40); printf("■      ■  ■■■      ■     ■      ■ ■■■ ■     ■  ■■■■       ■");

    Score = Score_item + Score_fire + Score_turn + Score_reload; //점수 집계
    if (Score_double)
    {
        Score *= 2; //묻더로 들어와서 이겼으면 점수 두배
        Score_double = false;
    }
    gotoxy(94, 19); printf("Score : %d", Score);

    Sleep(2000);

    gotoxy(67, 4); printf("|----------|");
    gotoxy(67, 5); printf("| ■    ■ |");
    gotoxy(67, 6); printf("|  ■  ■  |");
    gotoxy(67, 7); printf("|   ■■   |");
    gotoxy(67, 8); printf("|    ■    |");
    gotoxy(67, 9); printf("|    ■    |");
    gotoxy(67, 10); printf("|----------|");

    gotoxy(121, 4); printf("|----------|");
    gotoxy(121, 5); printf("| ■    ■ |");
    gotoxy(121, 6); printf("| ■■  ■ |");
    gotoxy(121, 7); printf("| ■ ■ ■ |");
    gotoxy(121, 8); printf("| ■  ■■ |");
    gotoxy(121, 9); printf("| ■    ■ |");
    gotoxy(121, 10); printf("|----------|");

    while (1) //행동 선택
    {
        char input = _getch();
        if (input == 'y') //묻더가 실행되어 바로 다음 게임 시작
        {
            Score_double = true;
            Game();
        }
        else if (input == 'n') //게임종료, 승리화면으로 이동
            Gamewin();
    }
}

//랭킹 정렬
void Ranking_bubble()
{
    User user; //게임 종료 후 이름과 점수를 저장할 구조체 선언
    user.Userscore = Score; //그 구조체에 점수 인풋
    strcpy(user.Username, Name); //이름 인풋

    //랭킹 정렬, 획득한 점수 입력, 이 부분만 GPT 사용
    for (int i = 0; i < RANKING; i++)
    {
        if (user.Userscore > Ranking[i].Userscore ||
            (user.Userscore == Ranking[i].Userscore && strcmp(user.Username, Ranking[i].Username) < 0))
            //점수가 같다면 알파벳 순으로 정렬
        {
            for (int j = RANKING - 1; j > i; j--)
            {
                Ranking[j] = Ranking[j - 1];
            }
            Ranking[i] = user;
            break;
        }
    }

    //랭킹 외부 파일에 저장
    FILE* Rank = fopen("Ranking.txt", "w");

    for (int i = 0; i < RANKING; i++) 
    {
        if (Ranking[i].Userscore > 0)
        {
            fprintf(Rank, "%s %d\n", Ranking[i].Username, Ranking[i].Userscore);
        }
    }
    fclose(Rank);
}

//도감 현황 외부 파일에 저장
void Write_archive()
{
    FILE* Archive = fopen("Archive.txt", "w");
    for (int i = 0; i < ITEM; i++)  
        fprintf(Archive, "%d\n", Have_item[i]);
    fclose(Archive);
}

//게임 시작시 저장된 도감 현황을 불러오는 함수
void Read_archive()
{
    FILE* Archive = fopen("Archive.txt", "r");
    if (Archive == NULL) return; //처음 시작이어서 저장된 파일이 없다면 무시하고 진행
    int index = 0;
    while (fscanf(Archive, "%d", &Have_item[index]) != EOF) 
    { index++; }
    fclose(Archive);
}

//게임 시작시 저장된 랭킹을 불러오는 함수
void Read_ranking()
{
    FILE* Rank = fopen("Ranking.txt", "r");
    if (Rank == NULL) return; //처음 시작이어서 저장된 파일이 없다면 무시하고 진행
    int rank = 0;
    while (fscanf(Rank, "%s %d", Ranking[rank].Username, &Ranking[rank].Userscore) != EOF)
    { rank++; }
    fclose(Rank);
}

//불꽃만 타오르도록 표현을 위해 따로 뺀 함수
void Draw_fire(bool off)
{
    gotoxy(72, 13); printf("                   )                      ");
    gotoxy(72, 14); printf("                  /(l                  /)");
    gotoxy(72, 15); printf("                 (  ＼                / (   ");
    gotoxy(72, 16); printf("                 ) * )               ( , )");
    gotoxy(72, 17); printf("                 ＼"); gotoxy(92, 17); printf("/                ＼"); gotoxy(112, 17); printf("'  ");
    gotoxy(72, 19); printf("            ("); gotoxy(72, 20); printf("            )＼"); gotoxy(96, 20); printf(",        /("); 
    gotoxy(115, 20); printf("/("); gotoxy(72, 21); printf("   (       /  )"); gotoxy(94, 21); printf(" (＼      (＼"); 
    gotoxy(114, 21); printf(") )"); gotoxy(72, 22); printf("   )＼     (  ("); gotoxy(94, 22); printf(" ) )     ) , )"); 
    gotoxy(114, 22); printf("/ ("); gotoxy(72, 23); printf("  /  )     ) . )"); gotoxy(94, 23); printf("/  (     ( "); 
    gotoxy(106, 23); printf(" (     ( , )"); gotoxy(72, 24); printf(" ( * (     ＼"); gotoxy(86, 24); printf("/|     (`"); 
    gotoxy(96, 24); printf(" )      `");  gotoxy(106, 24); printf("/"); gotoxy(114, 24); printf("`"); 
    gotoxy(116, 24); printf("/"); gotoxy(72, 25); printf(" ＼"); gotoxy(76, 25); printf("/");

    if (off == 0)
    {
        gotoxy(125, 21); printf("("); gotoxy(125, 22); printf(")("); gotoxy(124, 23); printf("/  )");
        gotoxy(123, 24); printf("( * ("); gotoxy(123, 25); printf("＼"); gotoxy(126, 25); printf("/");
    }
    else //불꽃이 꺼지는 것을 표현하는 부분
    {
        gotoxy(125, 21); printf(" "); gotoxy(125, 22); printf("  "); gotoxy(124, 23); printf("    ");
        gotoxy(123, 24); printf("     "); gotoxy(123, 25); printf("   "); gotoxy(126, 25); printf(" ");
    }
}

//게임오버
void Gameover(int Now_score)
{
    Play_bgm(Gameover_bgm, 1); //게임오버화면 브금 재생
    system("cls");
    Textcolor(black, gray);
    gotoxy(72, 13); printf("                   )                      ");
    gotoxy(72, 14); printf("                  /(l                  /)");
    gotoxy(72, 15); printf("                 (  ＼                / (   ");
    gotoxy(72, 16); printf("                 ) * )               ( , )");
    gotoxy(72, 17); printf("                 ＼#/                ＼#'  ");
    gotoxy(72, 18); printf("               .-'#'-.             .-'#'=, ");
    gotoxy(72, 19); printf("            (  |'-.='|             |'-,-'| ");
    gotoxy(72, 20); printf("            )＼|     |  ,        /(|     | /(");
    gotoxy(72, 21); printf("   (       /  )|     | (＼      (＼|     |) )        (");
    gotoxy(72, 22); printf("   )＼     (  (|     | ) )     ) , )     |/ (        )(");
    gotoxy(72, 23); printf("  /  )     ) . )     |/  (     ( # (     ( , )      /  )");
    gotoxy(72, 24); printf(" ( * (     ＼#/|     (`# )      `#/|     |`#/      ( * (");
    gotoxy(72, 25); printf(" ＼#/     .-'#'-.   .-'#'-,   .-'#'-.   .-=#'-;    ＼#/");
    gotoxy(72, 26); printf(".-'#'-.   |'=,-'|   |'-.-'|   |'-.-'|   |'-.-'|   ,-'#'-.");
    gotoxy(72, 27); printf("|'-.-'|   |     |   |     |   |     |   |     |   |'-.-'|");
    gotoxy(72, 28); printf("|     |   |     |   |     |   |     |   |     |   |     |");
    gotoxy(72, 29); printf("|     |   |     |   |     |   |     |   |     |   |     |");
    gotoxy(72, 30); printf("|     |   |     |   |     |   |     |   |     |   |     |");
    gotoxy(72, 31); printf("'-._,-'   '-._,-'   '-._,-'   '-._,-'   '-._,-'   '-._,-'");
    Textcolor(black, white);
    
    Score = Score_item + Score_fire + Score_turn + Score_reload; //점수 집계
    if (Score_double)
        Score = Now_score / 2; //묻더 실패시 점수 반갈
    Score_double = false;

    gotoxy(95, 5); printf("Game  Over"); gotoxy(94, 40); printf("Score : %d", Score);

    clock_t time = clock(), offtime = clock(), texttime = clock(); //현재의 시간 기록
    bool off = 0; //불꽃이 꺼지는 타이밍을 잡아주는 변수

    while (1)
    {
        if (_kbhit() && clock() > texttime + 4000) //4초가 지나야 탈출가능
            break;
        if (time + 200 == clock()) //0.2초 간격으로 불꽃이 반짝임
        {
            Textcolor(black, red);
            Draw_fire(off);
            Textcolor(black, white);
        }
        if (time + 400 == clock())
        {
            Textcolor(black, pink);
            Draw_fire(off);
            Textcolor(black, white);
        }
        if (time + 600 == clock())
        {
            time = clock(); //시간 초기화, 무한 반복
            Textcolor(black, brightred);
            Draw_fire(off);
            Textcolor(black, white);
        }
        if (clock() >= offtime + 2000) //2초후 불꽃이 꺼짐
        {
            off = 1;
        }
        if (clock() >= texttime + 4000) //4초후 문자 출력
        {
            gotoxy(82, 42); printf("계속하려면 아무 키나 누르십시오 . . .");
        }
    }

    Ranking_bubble(); //랭킹 정령
    Write_archive(); //도감현황 저장
    Stop_bgm(); //브금 종료
    Mainscreen(); //메인으로 돌아감
}

//WIN 글씨를 반짝이게 하기 위해 따로 뺀 함수
void Draw_win()
{
    gotoxy(82, 1); printf("■   ■   ■  ■  ■    ■  ■");
    gotoxy(82, 2); printf("■  ■■  ■  ■  ■■  ■  ■");
    gotoxy(82, 3); printf("■  ■■  ■  ■  ■ ■ ■  ■");
    gotoxy(82, 4); printf(" ■■  ■■   ■  ■  ■■    ");
    gotoxy(82, 5); printf("  ■    ■    ■  ■    ■  ■");
}

//게임 승리, 묻더 'N' 선택 시
void Gamewin()
{
    system("cls");
    Play_bgm(Gamewin_bgm, 1); //게임승리 브금 재생, 게임 컨셉에 맞게 승리해도 결국 상대를 살해하고 집에 돌아가는 것이므로 음산한 브금 재생
    Draw_image_512(Game_win, hdc, 1000, 200); //보물상자 이미지 출력

    //글씨와 점수를 순차적으로 출력
    char P1[] = "아이템\r점수\r:\r";
    char P2[] = "총알\r점수\r:\r";
    char P3[] = "턴\r점수\r:\r";
    char P4[] = "장전\r점수\r:\r";
    char P5[] = "총\r획득한\r점수\r:\r";
    
    for (int i = 0; P1[i] != '\0'; i++) 
    {
        gotoxy(8 + i, 4); printf("%c", P1[i]);
        Sleep(50);
    } Sleep(500); 
    gotoxy(22, 4); printf("%d", Score_item); Sleep(500);
    for (int i = 0; P2[i] != '\0'; i++) 
    {
        gotoxy(8 + i, 8); printf("%c", P2[i]);
        Sleep(50);
    } Sleep(500);
    gotoxy(20, 8); printf("%d", Score_fire); Sleep(500);
    for (int i = 0; P3[i] != '\0'; i++) 
    {
        gotoxy(8 + i, 12); printf("%c", P3[i]);
        Sleep(50);
    } Sleep(500);
    gotoxy(18, 12); printf("%d", Score_turn); Sleep(500);
    for (int i = 0; P4[i] != '\0'; i++) 
    {
        gotoxy(8 + i, 16); printf("%c", P4[i]);
        Sleep(50);
    } Sleep(500);
    gotoxy(20, 16); printf("%d", Score_reload); Sleep(500);
    for (int i = 0; P5[i] != '\0'; i++) 
    {
        gotoxy(8 + i, 30); printf("%c", P5[i]);
        Sleep(50);
    } Sleep(500);
    gotoxy(25, 30); printf("%d", Score); Sleep(500);
    for (int i = 0; END[i] != '\0'; i++) 
    {
        gotoxy(8 + i, 34); printf("%c", END[i]);
        Sleep(50);
    } Sleep(500);

    clock_t time = clock(); //시간 기록

    while (1)
    {
        if (_kbhit()) break; //아무 키나 누르면 탈출
        if (time + 200 == clock()) //0.2초 간격으로 반짝임
        {
            Textcolor(black, gray); //글씨를 반짝이게 하는 부분
            Draw_win();
            Textcolor(black, white);
        }
        if (time + 400 == clock())
        {
            Textcolor(black, green);
            Draw_win();
            Textcolor(black, white);
        }
        if (time + 600 == clock())
        {
            time = clock();
            Textcolor(black, blue);
            Draw_win();
            Textcolor(black, white);
        }
    }
    Ranking_bubble(); //랭킹 정렬
    Write_archive(); //도감 현황 저장
    system("cls"); Sleep(2000);
    Stop_bgm(); //브금 종료
    Mainscreen(); //메인으로 돌아감
}

//랭킹화면
void Rankingscreen(void)
{
    system("cls");
    Draw_square();
    gotoxy(97, 6); printf("랭  킹"); gotoxy(76, 12); printf("순  위");
    gotoxy(97, 12); printf("이  름"); gotoxy(118, 12); printf("점  수");
    gotoxy(186, 48); printf("뒤로가기 : Q");

    for (int i = 0; i < RANKING; i++) //랭킹 출력
    {
        gotoxy(78, 15 + 3 * i); printf("%d", i + 1); //순위
        if (Ranking[i].Userscore > 0) //점수가 있어야만 출력
        {
            gotoxy(97, 15 + 3 * i); printf("%s", Ranking[i].Username);
            gotoxy(118, 15 + 3 * i); printf("%d", Ranking[i].Userscore);
        }
    }

    while (1) //행동 선택
    {
        char input = _getch();
        if (input == 'q' || input == 'Q') // Q를 누르면 메인화면으로 뒤로가기
        {
            Mainscreen(); break;
        }
    }
}

//도감 화면
void Archive()
{
    system("cls");
    int n = 0; //도감 페이지 인덱스값, 초깃값은 0번 아이템인 맥주
    gotoxy(186, 48); printf("뒤로가기 : Q");
    Draw_square();

    gotoxy(63, 7); for (int i = 0; i < 37; i++) { printf("■"); }
    for (int i = 0; i < 28; i++) { gotoxy(63, 8 + i); printf("■"); }
    for (int i = 0; i < 28; i++) { gotoxy(135, 8 + i); printf("■"); }
    gotoxy(63, 35); for (int i = 0; i < 37; i++) { printf("■"); }

    Archive_select(n); //인덱스 값에 따른 아이템 설명을 출력

    while (1) //행동 선택
    {
        char key = _getch();
        if (key == LEFT && n > 0) //왼쪽 방향키를 누르면 이전 아이템
            n--;
        if (key == RIGHT && n < 8) //오른쪽 방향키를 누르면 다음 아이템
            n++;
        if (key == 'q') //q를 누르면 탈출하여 메인으로 돌아감
            break;
        Archive_select(n); //어떤 화면을 보여줄지 움직이며 출력
    }
    Mainscreen(); //q를 누르면 while문에서 나오며 메인으로 돌아감
}

//도감 화면에서 아이템의 설명을 보여주는 함수
void Archive_select(int n)
{
    gotoxy(63, 5); if (n > 0) printf("← P R E V"); else printf("          "); //가장 첫 아이템이면 뒤로 갈 수 없음
    gotoxy(127, 5); if (n < 8) printf("N E X T →"); else printf("          "); //마지막 아이템이면 앞으로 갈 수 없음
    gotoxy(80, 42); for (int i = 43; i < 90; i++) { printf(" "); } //설명란 초기화
    gotoxy(76, 43); for (int i = 43; i < 90; i++) { printf(" "); }
    gotoxy(76, 44); for (int i = 43; i < 90; i++) { printf(" "); }
    for (int k = 0; k < 27; k++) { for (int i = 0; i < 70; i++) { gotoxy(65 + i, 8 + k); printf(" "); } }
    gotoxy(86, 39); printf("No. %d", n + 1); gotoxy(98, 39); printf("<       ?      >"); //이름란 초기화

    gotoxy(99, 21); printf("?"); //아이템 사진이 나올 곳, 해금이 안되어있으면 물음표만 출력되는 구조

    if (n == 0 && Have_item[0] == 1) //맥주, 한번이라도 획득한 적이 있으면 출력
    {
        gotoxy(104, 39); printf("맥주");
        gotoxy(82, 42); printf("현재 약실에 장전된 총알을 제거합니다");
        Draw_image_256(Beer, hdc, 680, 250); //이미지 출력
    }
    else if (n == 1 && Have_item[1] == 1) //수갑
    {
        gotoxy(104, 39); printf("수갑");
        gotoxy(87, 42); printf("상대의 턴을 1턴 봉인합니다");
        Draw_image_256(Handcuff, hdc, 680, 250);
    }
    else if (n == 2 && Have_item[2] == 1) //담배
    {
        gotoxy(104, 39); printf("담배");
        gotoxy(89, 42); printf("라이프를 1 회복합니다");
        gotoxy(82, 43); printf("최대체력을 넘어 회복할 수 없습니다");
        Draw_image_256(Cigarette, hdc, 680, 250);
    }
    else if (n == 3 && Have_item[3] == 1) //톱
    {
        gotoxy(105, 39); printf("톱");
        gotoxy(83, 42); printf("현재 총알이 피해를 2만큼 가합니다");
        Draw_image_256(Saw, hdc, 680, 250);
    }
    else if (n == 4 && Have_item[4] == 1) //아드레날린
    {
        gotoxy(101, 39); printf("아드레날린");
        gotoxy(84, 42); printf("상대의 아이템을 훔쳐 사용합니다");
        Draw_image_256(Adrenaline, hdc, 680, 250);
    }
    else if (n == 5 && Have_item[5] == 1) //상한 약
    {
        gotoxy(102, 39); printf("상한  약");
        gotoxy(86, 42); printf("50%% 확률로 라이프를 1 잃거나");
        gotoxy(84, 43); printf("50%% 확률로 라이프를 2 회복합니다");
        gotoxy(83, 44); printf("최대체력을 넘어 회복할 수 있습니다");
        Draw_image_256(Medicine, hdc, 680, 250);
    }
    else if (n == 6 && Have_item[6] == 1) //돋보기
    {
        gotoxy(103, 39); printf("돋보기");
        gotoxy(84, 42); printf("약실에 장전된 총알을 확인합니다");
        Draw_image_256(Magnifier, hdc, 680, 250);
    }
    else if (n == 7 && Have_item[7] == 1) //대포폰
    {
        gotoxy(103, 39); printf("대포폰");
        gotoxy(87, 42); printf("현재 남은 탄들을 기준으로");
        gotoxy(76, 43); printf("1번째 탄을 제외한 랜덤한 탄의 순서를 알려줍니다");
        Draw_image_256(Phone, hdc, 680, 250);
    }
    else if (n == 8 && Have_item[8] == 1) //인버터
    {
        gotoxy(103, 39); printf("인버터");
        gotoxy(80, 42); printf("현재 약실에 장전된 탄의 극성을 바꿉니다");
        Draw_image_256(Inverter, hdc, 680, 250);
    }
} 

//o아이템 구간o//

//아이템 사용 키 안내, 유저와 PC의 초상화 출력
void Item_draw(int round)
{
    gotoxy(57, 0); printf("맥주(Q) 수갑(W) 담배(E) 톱(R) 아드레날린(T) 상한 약(Y) 돋보기(U) 대포폰(I) 인버터(O)");
    Draw_image_128(User_image, hdc, 290, 540); //유저 초상화 출력
    switch (round) //라운드에 따라 PC의 초상화가 달라짐
    {
    case 0:
        Draw_image_128(Com_image, hdc, 1190, 540);
        break;
    case 1:
        Draw_image_128(Com2_image, hdc, 1190, 540);
        break;
    case 2:
        Draw_image_128(Com3_image, hdc, 1190, 540);
        break;
    }
}

//아이템 테이블의 이미지 좌표를 찾아주는 함수
void Find_table_position(int table)
{
    switch (table) //테이블 번호를 받아옴
    {
    case 0: x = 120; y = 400; break; //0번 테이블
    case 1: x = 290; y = 400; break;
    case 2: x = 470; y = 400; break;
    case 3: x = 120; y = 540; break;
    case 4: x = 470; y = 540; break;
    case 5: x = 120; y = 685; break;
    case 6: x = 290; y = 685; break;
    case 7: x = 470; y = 685; break; //7번 테이블
    }
    if (Turn == 0) { x += 900; } //PC턴이면 좌표 이동

    //x, y는 전역변수이므로 값을 반환하지는 않음
}

//테이블 번호와 아이템 번호를 받아서, 테이블에 아이템을 그리는 함수
void Draw_eachitem(int table, int item)
{
    switch (item)
    {
    case 0: //맥주
        Draw_image_128(Beer, hdc, x, y); //맥주 이미지를 출력하고 그 테이블의 주솟값을 B (Beer) 로 지정, 그 테이블에 맥주가 있다고 알리는 의미
        if (Turn) User_Table[table] = 'B'; else Com_Table[table] = 'B'; break;
    case 1: //수갑
        Draw_image_128(Handcuff, hdc, x, y); //이하 동일
        if (Turn) User_Table[table] = 'H'; else Com_Table[table] = 'H'; break;
    case 2: //담배
        Draw_image_128(Cigarette, hdc, x, y);
        if (Turn) User_Table[table] = 'C'; else Com_Table[table] = 'C'; break;
    case 3: //톱
        Draw_image_128(Saw, hdc, x, y);
        if (Turn) User_Table[table] = 'S'; else Com_Table[table] = 'S'; break;
    case 4: //아드레날린
        Draw_image_128(Adrenaline, hdc, x, y);
        if (Turn) User_Table[table] = 'A'; else Com_Table[table] = 'A'; break;
    case 5: //약
        Draw_image_128(Medicine, hdc, x, y);
        if (Turn) User_Table[table] = 'M'; else Com_Table[table] = 'M'; break;
    case 6: //돋보기
        Draw_image_128(Magnifier, hdc, x, y);
        if (Turn) User_Table[table] = 'm'; else Com_Table[table] = 'm'; break;
    case 7: //대포폰
        Draw_image_128(Phone, hdc, x, y);
        if (Turn) User_Table[table] = 'P'; else Com_Table[table] = 'P'; break;
    case 8: //인버터
        Draw_image_128(Inverter, hdc, x, y);
        if (Turn) User_Table[table] = 'I'; else Com_Table[table] = 'I'; break;
    }
}

//아이템 번호를 받아서 주솟값을 찾고 이미지를 지우는 함수
void Find_erase_table(int item)
{
    int istable = 0; //찾아낸 테이블 번호를 저장할 변수
    if (Adrenaline_on == true && Adrenaline_used == true) //아드레날린으로 뺏어 쓴 아이템을 지우러 왔다면
    {
        Turn = !Turn; //상대의 아이템을 지우기 위해 이 함수 안에서만 턴 반전
    }

    if (Turn == true) //유저 턴이면
    {
        switch (item)
        {
        case 0: //맥주
            for (int table = 0; table < MAX_ITEM; table++) //8개의 테이블 탐색
            {
                if (User_Table[table] == 'B') //주솟값이 B 면
                {
                    istable = table; //번호 저장, switch문 탈출
                    break;
                }
            } break;
        case 1: //수갑, ★이하 구조 동일함★
            for (int table = 0; table < MAX_ITEM; table++) {
                if (User_Table[table] == 'H') { istable = table; break; }
            } break;
        case 2: //담배
            for (int table = 0; table < MAX_ITEM; table++) {
                if (User_Table[table] == 'C') { istable = table; break; }
            } break;
        case 3: //톱
            for (int table = 0; table < MAX_ITEM; table++) {
                if (User_Table[table] == 'S') { istable = table; break; }
            } break;
        case 4: //아드레날린
            for (int table = 0; table < MAX_ITEM; table++) {
                if (User_Table[table] == 'A') { istable = table; break; }
            } break;
        case 5: //상한 약
            for (int table = 0; table < MAX_ITEM; table++) {
                if (User_Table[table] == 'M') { istable = table; break; }
            } break;
        case 6: //돋보기
            for (int table = 0; table < MAX_ITEM; table++) {
                if (User_Table[table] == 'm') { istable = table; break; }
            } break;
        case 7: //대포폰
            for (int table = 0; table < MAX_ITEM; table++) {
                if (User_Table[table] == 'P') { istable = table; break; }
            } break;
        case 8: //인버터
            for (int table = 0; table < MAX_ITEM; table++) {
                if (User_Table[table] == 'I') { istable = table; break; }
            } break;
        }
    }
    else //PC 턴이면
    {
        switch (item)
        {
        case 0: //맥주
            for (int table = 0; table < MAX_ITEM; table++) //8개의 테이블 탐색
            {
                if (Com_Table[table] == 'B') //주솟값이 B 면
                {
                    istable = table; //번호 저장, switch문 탈출
                    break;
                }
            } break;
        case 1: //수갑, ★이하 구조 동일함★
            for (int table = 0; table < MAX_ITEM; table++) {
                if (Com_Table[table] == 'H') { istable = table; break; }
            } break;
        case 2: //담배
            for (int table = 0; table < MAX_ITEM; table++) {
                if (Com_Table[table] == 'C') { istable = table; break; }
            } break;
        case 3: //톱
            for (int table = 0; table < MAX_ITEM; table++) {
                if (Com_Table[table] == 'S') { istable = table; break; }
            } break;
        case 4: //아드레날린
            for (int table = 0; table < MAX_ITEM; table++) {
                if (Com_Table[table] == 'A') { istable = table; break; }
            } break;
        case 5: //상한 약
            for (int table = 0; table < MAX_ITEM; table++) {
                if (Com_Table[table] == 'M') { istable = table; break; }
            } break;
        case 6: //돋보기
            for (int table = 0; table < MAX_ITEM; table++) {
                if (Com_Table[table] == 'm') { istable = table; break; }
            } break;
        case 7: //대포폰
            for (int table = 0; table < MAX_ITEM; table++) {
                if (Com_Table[table] == 'P') { istable = table; break; }
            } break;
        case 8: //인버터
            for (int table = 0; table < MAX_ITEM; table++) {
                if (Com_Table[table] == 'I') { istable = table; break; }
            } break;
        }
    }

    Find_table_position(istable); //테이블 좌표 받고
    Draw_image_128(None, hdc, x, y); //아이템 지우고
    if (Turn == true) //유저 턴이면
    {
        User_item_table[istable] = 0; //유저의 그 테이블 비우고
        User_Table[istable] = '1'; //주솟값 초기화
    }
    else //PC턴이면
    {
        Com_item_table[istable] = 0; //PC의 그 테이블 비우고
        Com_Table[istable] = '1'; //주솟값 초기화
    }

    if (Adrenaline_on == true && Adrenaline_used == false) //아드레날린을 사용후 사용자의 아드레날린을 지우고 왔다면
    {
        Adrenaline_used = true; //아드레날린으로 아이템을 뺏어 쓸 차례       
    }
    else if (Adrenaline_on == true && Adrenaline_used == true)  //아드레날린으로 뺏어쓴 아이템을 지웠다면
    {
        Turn = !Turn; Adrenaline_used = false; //턴 원상 복구
    }
}

//아이템의 등장 비중을 설정하는 함수
//수갑과 아드레날린의 사기성이 시뮬레이션을 통해 충분히 검증되었기 때문에, 아이템들의 등장하는 비중을 활용도에 따라 조정함
int Item_weight(int weight)
{
    int item = 9; //더미값
    if (weight > 94) item = 4;      //비중 5,  아드레날린
    else if (weight > 89) item = 1; //비중 5,  수갑
    else if (weight > 78) item = 3; //비중 11, 톱
    else if (weight > 67) item = 6; //비중 11, 돋보기
    else if (weight > 55) item = 2; //비중 12, 담배
    else if (weight > 43) item = 5; //비중 12, 약
    else if (weight > 31) item = 8; //비중 12, 인버터
    else if (weight > 15) item = 0; //비중 16, 맥주
    else item = 7;                  //비중 16, 대포폰, 전체비중 100
    return item;
}

//랜덤 아이템 지급
void Item_give()
{
    bool turn = Turn; //현재 누구의 턴인지 저장, 누구에게 지급하느냐에 따라 턴을 반전시키기 때문
    srand((unsigned int)time(NULL));
    int Howmanyitem = rand() % 3 + 2; //최대 아이템 갯수 2~4

    Turn = true; //유저에게 지급, 아이템 이미지를 그리기 위함

    for (int i = 0; i < Howmanyitem; i++) //유저 아이템 지급, for문 안에서는 시드가 고정이므로 PC와 분리함
    {
        int weight = rand() % 100; //전체 비중 100
        int item = Item_weight(weight); //비중에 따라 9개 아이템 중 하나 랜덤 선택
        for (int table = 0; table < MAX_ITEM; table++) //0번 테이블부터 비어있는 테이블을 찾으러 감
        {
            if (User_item_table[table] == 0) //그 테이블이 비어있어야만 지급함
            {
                Find_table_position(table); //찾아낸 비어있는 테이블의 좌표를 받아서
                Draw_eachitem(table, item); //랜덤하게 선택한 아이템을 그 테이블에 넣고
                User_item[item]++; User_item_table[table] = 1; //아이템 지급 후, 테이블이 채워졌다고 알림
                Have_item[item] = 1; //아이템 획득시 도감 해금
                Score_item += 5; //아이템 획득시 5점
                break;
            }
        }
    } //최종적으로 랜덤하게 선택한 2~4개의 아이템이 지급되고, 테이블이 모두 채워져있다면 그 이상 획득 불가
      //테이블의 자리가 8개이므로 아이템 최대 소지 갯수는 8개인 셈

    Turn = false; //PC에게 지급할 차례

    for (int i = 0; i < Howmanyitem; i++) //이하 구조 동일
    {
        int weight = rand() % 100;
        int item = Item_weight(weight);
        for (int table = 0; table < MAX_ITEM; table++)
        {
            if (Com_item_table[table] == 0)
            {
                Find_table_position(table);
                Draw_eachitem(table, item);
                Com_item[item]++; Com_item_table[table] = 1;
                break;
            }
        }
    }
    Turn = turn; //아이템 지급이 끝났으므로 저장한 턴을 넣어 턴 되돌려놓기
}

//입력한 알파벳을 그에 맞는 인덱스 번호로 바꿔주는 함수, Who_use() 와 Item_ardreanline() 에서 사용
int Trans_index(char input)
{
    int index = 9; //이상한 값이 들어올 경우, 더미값
    switch (input)
    {
    case 'q':index = 0; break;
    case 'w':index = 1; break;
    case 'e':index = 2; break;
    case 'r':index = 3; break;
    case 't':index = 4; break;
    case 'y':index = 5; break;
    case 'u':index = 6; break;
    case 'i':index = 7; break;
    case 'o':index = 8; break;
    }
    return index;
}

//아이템의 이펙트를 출력하는 함수, 일부 아이템은 작동방식이 달라 사용하지 못함
void Item_effect(int item, const char* itemName)
{
    Find_erase_table(item); //입력받은 아이템이 있는 테이블의 이미지를 지움
    int xpos = 290; if (!Turn) xpos += 900; //PC의 턴일 경우 테이블의 주솟값이 다름
    Draw_image_128(itemName, hdc, xpos, 240); //아이템 이펙트
    Sleep(3000); Draw_image_128(None, hdc, xpos, 240); //이펙트 지우기
}

//아이템-맥주0
void Item_beer()
{
    Play_bgm(Beer_sound, 0); //맥주 효과음
    Find_erase_table(0); //맥주가 있는 테이블을 찾아 그 테이블의 이미지를 지움
    int xpos = 290; if (!Turn) xpos += 900; //PC가 쓰면 테이블 좌표값을 다르게 받음
    Draw_image_128(Beer, hdc, xpos, 240); //맥주 이펙트

    //실탄 또는 공포탄이 나올 때까지 Now_bullet을 증가시킴
    while (Now_bullet < MAX_BULLET && Bullets[Now_bullet] == 2) Now_bullet++; //2 실 2 공

    if (Now_bullet < MAX_BULLET) //어떤 탄을 찾았는지 확인, 그 탄 버리기
    {
        if (Inverter_on) //인버터를 쓴 상태라면 반전시켜야 함
        {
            if (Bullets[Now_bullet] == 1) //실탄이라면 원래 탄은 공포탄임
            { 
                Fake_bullet--;
                Now_bullet++;
                Draw_image_128(Real_bullet_image, hdc, 745, 500); //버린 탄 확인
                Sleep(3000); Draw_image_128(None, hdc, 745, 500);
            }
            else if (Bullets[Now_bullet] == 0) //공포탄이라면 원래 탄은 실탄임
            { 
                Real_bullet--;
                Now_bullet++;
                Draw_image_128(Fake_bullet_image, hdc, 745, 500); //버린 탄 확인
                Sleep(3000); Draw_image_128(None, hdc, 745, 500);
            }
        }
        else //인버터를 안썼다면
        {
            if (Bullets[Now_bullet] == 1) //실탄이라면
            { 
                Real_bullet--;
                Now_bullet++;
                Draw_image_128(Real_bullet_image, hdc, 745, 500); //버린 탄 확인
                Sleep(3000); Draw_image_128(None, hdc, 745, 500);
            }
            else if (Bullets[Now_bullet] == 0) //공포탄이라면
            { 
                Fake_bullet--;
                Now_bullet++;
                Draw_image_128(Fake_bullet_image, hdc, 745, 500); //버린 탄 확인
                Sleep(3000); Draw_image_128(None, hdc, 745, 500);
            }
        }
    }
    if (Turn) Score_item += 10; //유저의 경우 맥주 사용시 10점 추가

    Inverter_on = false; //탄이 지나갔으므로 인버터 사용 종료

    Draw_image_128(None, hdc, xpos, 240); //버린 탄 이미지 지움
}

//아이템-수갑1
void Item_handcuff()
{
    Play_bgm(Handcuff_sound, 0); //수갑 효과음, 이하 동일 생략
    if (Turn)  Score_item += 15; //유저의 경우 수갑 사용시 15점 추가

    Handcuff_on = true; //수갑이 사용되었음을 알림

    Item_effect(1, Handcuff); //수갑 이펙트
    Draw_handcuff(); //수갑 이미지 채움
}

//수갑이 채워져 있다면 테이블 사이에 누구에게 수갑이 걸려있는지 표시
void Draw_handcuff()
{
    if (Handcuff_on && !Turn) Draw_image_128(Handcuff, hdc, 640, 700);
    else if (Handcuff_on && Turn) Draw_image_128(Handcuff, hdc, 850, 700);
}

//아이템-담배2
void Item_cigarette()
{
    Play_bgm(Cigarette_sound, 0);
    if (Turn == true)
    {
        User_hp++; //유저가 쓰면 회복, 소모
        Score_item -= 5; //담배 사용시 체력은 회복하지만 점수는 깎임
    }
    else Com_hp++; //PC가 쓰면 PC 회복

    Item_effect(2, Cigarette); //담배 이펙트
}

//아이템-톱3
void Item_saw()
{
    Play_bgm(Saw_sound, 0);
    if (Turn) Score_item += 10;

    Saw_on = true; //톱이 사용되었음을 알림

    Item_effect(3, Saw); //톱 이펙트
}

//PC가 톱을 사용할 수 있는지 판단 후 사용하는 함수
void Com_saw()
{
    if (Com_item[3] > 0) //톱을 소유중이라면
    {
        Who_useitem('r'); //톱 사용
        Sleep(2000);
    }
    else if (Com_item[4] > 0 && User_item[3] > 0) //아드레날린이 있고 유저가 톱을 소유중이라면
    {
        Com_adrenaline(3); //톱 뺏기
        Sleep(2000);
    }
}

//아이템-아드레날린4, 유저가 사용할때
//PC의 경우 아이템을 선택하는 과정이 정해져있는 상태로 들어오기에 기믹이 달라 분할함
void Item_adrenaline()
{
    Play_bgm(Adrenaline_sound, 0);
    int index = 9; //유저가 어떤 아이템을 뺏을지 고를 인덱스 값, 초기값은 더미값인 9
    Score_item += 15;

    Adrenaline_on = true; //아드레날린을 사용중임을 알림

    //상대방 아이템 목록 출력 및 선택
    gotoxy(81, 2); printf("빼앗고 싶은 PC의 아이템을 선택하세요");

    Find_erase_table(4);
    Draw_image_128(Adrenaline, hdc, 290, 240); //아드레날린 이펙트

    while (1)
    {
        char item_choice = _getch();
        index = Trans_index(item_choice); //문자를 숫자의 인덱스로 변환

        if (index >= 0 && index < ITEM && Com_item[index] > 0)
        {
            if (index == 1 && Handcuff_on == true) {} //이미 수갑 사용중이라면 사용하지 않음
            else if (index == 2 && User_hp >= Max_hp) {} //풀피면 담배 사용하지 않음
            else if (index == 3 && Saw_on == true) {} //이미 톱 사용중이라면 사용하지 않음
            else if (index == 7 && Real_bullet + Fake_bullet == 1) {} //총알이 한발남았다면 대포폰 사용하지 않음
            else if (index == 4) {} //아드레날린으로 아드레날린을 뺏을 수 없음
            else if (index != 9)//아이템 사용이 가능한 경우
            {
                Com_item[index]--;
                Item_use(index);
                break; //탈출
            }
        }
    }

    gotoxy(81, 2); for (int i = 0; i < 36; i++) printf(" "); //아이템 출력 문구 지우기
    Adrenaline_on = false; //아드레날린 사용이 끝났음을 알림
}
//PC의 아드레날린 사용
void Com_adrenaline(int item) 
{
    Play_bgm(Adrenaline_sound, 0);

    Find_erase_table(4); //PC 테이블의 아드레날린을 지움
    Draw_image_128(Adrenaline, hdc, 1190, 240); //아드레날린 이펙트
    Com_item[4]--; //아드레날린 갯수 감소
    Adrenaline_on = true; //아드레날린 사용중을 알림
    Sleep(3500);

    //PC의 경우 선택없이 어떤 아이템을 사용할지 정해져있으므로
    //곧바로 아드레날린으로 아이템을 뺏을 차례라고 알려줌으로서 유저 테이블의 아이템을 지우도록 유도하는 구문
    Adrenaline_used = true; 

    Item_use(item); User_item[item]--; //유저 아이템 뺏어 사용
    Adrenaline_on = false; //아드레날린 사용 종료
}

//아이템-상한 약5
void Item_medicine()
{
    Play_bgm(Medicine_sound, 0);
    srand((unsigned int)time(NULL));
    int x = rand() % 2; //약 사용에 성공할 변수, 0또는 1 랜덤
    if (x == 0) //실패
    {
        if (Turn == true)
        {
            User_hp--; //체력 1 깎임
            Score_item -= 10; //약 도박 실패시 점수 깎임
        }
        else if (Turn == false) Com_hp--; //체력 1 깎임
    }
    else if (x == 1) //성공
    {
        if (Turn == true)
        {
            User_hp += 2; //체력 2회복
            Score_item += 10; //약 도박 성공시 점수 10점
        }
        else if (Turn == false) Com_hp += 2; //체력 2회복
    }

    Item_effect(5, Medicine); //상한 약 이펙트
}

//아이템-돋보기6, 돋보기도 유저와 PC의 사용을 분리함
//PC의 경우 두줄로 정리가 가능하기 때문에 긴 함수를 거쳐갈 필요가 없도록 분할함
bool Item_magnifier()
{
    Play_bgm(Magnifier_sound, 0);
    //실탄 또는 공포탄이 나올 때까지 Now_bullet을 증가시킴
    while (Now_bullet < MAX_BULLET && Bullets[Now_bullet] == 2) Now_bullet++;

    if (Turn) //유저 턴일때만 표기
    {
        gotoxy(100, 1);
        if (Now_bullet < MAX_BULLET) //인덱스 경고를 지우기 위함
        {
            if (Bullets[Now_bullet] == 1) 
            { 
                Draw_image_128(Real_bullet_image, hdc, 745, 500); //실탄 확인
            }
            else if (Bullets[Now_bullet] == 0) 
            { 
                Draw_image_128(Fake_bullet_image, hdc, 745, 500); //공포탄 확인
            }
        }
        Score += 5;
    }
    bool bullet = 0; //PC가 사용할 현재 탄의 종류를 담은 변수
    if (Now_bullet < MAX_BULLET) bullet = Bullets[Now_bullet]; //인덱스 경고를 지우기 위함

    Item_effect(6, Magnifier); //돋보기 이펙트
    Draw_image_128(None, hdc, 745, 500); //총알 이미지 지움

    return bullet; //PC의 경우 현재 탄의 종류를 받아 사용함
}

//아이템-대포폰7
void Item_phone()
{
    gotoxy(81, 2); for (int i = 0; i < 30; i++) printf(" "); //아이템 출력 문구 지우기
    Play_bgm(Phone_sound, 0);
    srand((unsigned int)time(NULL));
    if (Turn) Score_item += 5;

    //실탄 또는 공포탄이 나올 때까지 Now_bullet을 증가시킴
    while (Now_bullet < MAX_BULLET && Bullets[Now_bullet] == 2) Now_bullet++;

    //대포폰으로 불러올 인덱스, 남은 총알, 대포폰으로 불러올 인덱스를 포함해 이후에 남은 총알
    int Phone_index, Rest_bullet = 0, Rest_phone_bullet = 0;

    //공 공 *실 2 실 2 /공 실            공4 실2

    do { Phone_index = rand() % MAX_BULLET; } //대포폰 인덱스에 0~7 랜덤 난수
    while (Phone_index <= Now_bullet || Bullets[Phone_index] == 2);
    //이미 지나온 총알과 지금의 총알은 알려주면 안되므로 현재 총알 인덱스보다 작거나 같으면 난수 다시 생성
    //총알이 2여도 난수 다시 생성

    //현재 남은 총알 갯수 계산
    for (int i = Now_bullet; i < MAX_BULLET; i++) { if (Bullets[i] != 2) Rest_bullet++; }

    //난수로 생성한 대포폰 인덱스를 포함해 이후에 남은 총알 갯수 계산
    for (int i = Phone_index; i < MAX_BULLET; i++) { if (Bullets[i] != 2) Rest_phone_bullet++; }

    //그렇게 생성된 인덱스가 남은 총알 중 몇 번째 총알인지 계산
    int Item_phone_index = Rest_bullet - Rest_phone_bullet + 1;

    if (Turn) //유저턴일때만 표기
    {
        gotoxy(91, 2);
        if (Bullets[Phone_index] == 1) printf("%d 번째 탄 : 실탄", Item_phone_index);
        else if (Bullets[Phone_index] == 0) printf("%d 번째 탄 : 공포탄", Item_phone_index);
    }

    Com_phone_index = Phone_index; Com_phone_bullet = Bullets[Phone_index]; //PC가 쓸 정보 저장

    Item_effect(7, Phone);
    gotoxy(81, 2); for (int i = 0; i < 30; i++) printf(" "); //아이템 출력 문구 지우기
}

//아이템-인버터8
void Item_inverter()
{
    Play_bgm(Inverter_sound, 0);
    //실탄 또는 공포탄이 나올 때까지 Now_bullet을 증가시킴
    while (Now_bullet < MAX_BULLET && Bullets[Now_bullet] == 2) Now_bullet++;

    if (Now_bullet < MAX_BULLET) //인덱스 범위 경고를 없애기 위해 if문을 엮음
    {
        if (Bullets[Now_bullet] == 1) //실탄이면
        {
            Bullets[Now_bullet] = 0; //공포탄으로 바꿈
        }
        else if (Bullets[Now_bullet] == 0) //공포탄이면
        {
            Bullets[Now_bullet] = 1; //실탄으로 바꿈       
        }
    }

    if (Turn == true) Score_item += 15;

    Inverter_on = true; //인버터 사용을 알림

    Item_effect(8, Inverter); //인버터 이펙트
}

//PC가 인버터를 사용할 수 있는지 판단 후 사용하는 함수
void Com_inverter()
{
    if (Com_item[8] > 0) //인버터를 소유중이라면
    {
        Who_useitem('o'); //인버터 사용
        Sleep(2000);
    }
    else if (Com_item[4] > 0 && User_item[8] > 0) //아드레날린이 있고 유저가 인버터를 소유중이라면
    {
        Com_adrenaline(3); //인버터 뺏기
        Sleep(2000);
    }
}

//아이템 사용 함수, 유저와 pc 모두 아이템을 사용하기에 두번 쓰면 코드가 길어져 따로 뺐음
void Item_use(int index)
{
    int Com_itemsum = 0; for (int i = 0; i < ITEM; i++) { Com_itemsum += Com_item[i]; } //PC 아이템의 총 갯수, 아드레날린에서 사용

    //각 아이템의 효과를 정의
    switch (index)
    {
    case 0: //맥주
        Item_beer(); break;

    case 1: //수갑
        if (Handcuff_on == false) Item_handcuff(); //사용중이 아닐때만
        else 
        {
            if (Turn == true) User_item[1]++; //사용중일때는 갯수만 줄었으니 복구
        } break;

    case 2: //담배
        if (Turn == true) 
        { 
            if (User_hp >= Max_hp) User_item[2]++; //만약 최대체력을 넘어간다면 사용하지 않으므로 갯수 복구
            else Item_cigarette(); 
        } 
        else if (Turn == false) Item_cigarette(); break;

    case 3: //톱
        if (Saw_on == false) { Item_saw(); } //사용중이 아닐때만
        else User_item[3]++; break; //PC는 톱을 두 번 쓰는 경우가 없도록 설계했기에 PC의 경우는 고려할 필요가 없음

    case 4: //아드레날린
        if (Com_itemsum > 0) Item_adrenaline(); //상대 아이템이 있어야만 사용
        else User_item[4]++; break; //없으면 사용하지 않고 갯수 반환
        //PC의 경우 다른 방식으로 아드레날린을 쓰기에 고려할 필요가 없음

    case 5: //상한 약, 상한 약은 최대체력을 넘어 회복이 가능하기에 제약이 없음
        Item_medicine(); break;

    case 6: //돋보기
        Item_magnifier(); break;

    case 7: //대포폰
        if (Real_bullet + Fake_bullet == 1) //남은 총알이 한 발이라면
            User_item[7]++; //사용하지 않고 갯수 반환, 마찬가지로 PC의 경우는 고려할 필요가 없음
        else Item_phone(); break;

    case 8: //인버터
        Item_inverter(); break;
    }

    Draw_hp(); //체력이 변동되었으므로 다시 출력
}

//누가 아이템을 쓰는지 판단하고 아이템 사용을 불러오는 함수
void Who_useitem(char c) {
    int index = 9;

    //키 입력 값에 따라 인덱스 번호 지정
    index = Trans_index(c);
    if (Turn == true) //유저 턴이면 유저가 사용
    {
        if (User_item[index] > 0) //사용하려는 아이템의 개수가 0보다 큰지 확인
        {
            User_item[index]--; //아이템 개수 감소
            Item_use(index);
        }
    }
    else if (Turn == false) //컴퓨터 턴이면 컴퓨터가 사용
    {
        if (Com_item[index] > 0)
        {
            Com_item[index]--;
            Item_use(index);
        }
    }
}

//o게임 진행 구간o//

//남은 체력을 그리는 함수
void Draw_hp()
{
    gotoxy(10, 20); for (int i = 0; i < User_hp; i++) { printf("♥"); } printf("    ");
    int i = 0;
    for (i = 0; i < Com_hp; i++) { gotoxy(188 - 2 * i, 20); printf("♥"); }
    for (i = i; i < 5; i++) { gotoxy(188 - 2 * i, 20); printf("  "); }
    gotoxy(11, 18); printf("User"); gotoxy(186, 18); printf("Com");
}

//총알의 갯수를 랜덤으로 지정하고 실탄과 공포탄의 순서를 랜덤으로 지정
void Random_reload()
{
    Sleep(2000);
    srand((unsigned int)time(NULL));
    Score_reload += 20; //재장전시 점수 획득

    Real_bullet = rand() % 4 + 1; //실탄 1~4발 랜덤
    int Real_lower = Real_bullet - 1, Real_upper = Real_bullet + 1; //실탄 ±1을 저장해 공포탄 생성에 활용
    do { Fake_bullet = rand() % 4 + 1; } //공포탄 1~4발 랜덤
    while (!((Fake_bullet >= Real_lower) && (Fake_bullet <= Real_upper) && Fake_bullet <= 4));
    //공포탄의 갯수가 실탄±1이 되도록, 그러면서 최대 갯수인 4개는 넘어가면 안됨
    //실탄과 공포탄의 갯수 차이가 많이 나면 난이도가 쉬워지므로 조정함

    //장전하기 전 탄의 갯수를 알려줌
    int real = Real_bullet; //실탄 갯수만큼
    for (int i = 0; i < real; i++) 
        Draw_image_64(Real_bullet_image, hdc, 100 + i * 70, 100); //실탄 그리기
    for (real; real < Real_bullet + Fake_bullet; real++) 
        Draw_image_64(Fake_bullet_image, hdc, 100 + real * 70, 100); //나머지 공포탄 그리기
    Sleep(5000);

    for (int i = 0; i < Real_bullet + Fake_bullet; i++) //이후 지우기
        Draw_image_64(None, hdc, 100 + i * 70, 100);

    for (int i = 0; i < Real_bullet + Fake_bullet; i++) //탄 갯수 만큼
    {
        Play_bgm(Reload_bullet, 0); Sleep(500); //총알 넣는 소리
    }
    Play_bgm(Reload_sound, 0); //장전 소리

    for (int i = 0; i < MAX_BULLET; i++) { Bullets[i] = 2; } //장전 배열 초기화

    int count = 0;
    for (int i = 0; i < Real_bullet; i++) //실탄 갯수만큼 배열에 1 추가
    {
        Bullets[i] = 1; count++;
    }
    for (int i = 0; i < Fake_bullet; i++) //공포탄 갯수만큼 배열에 0추가
    {
        Bullets[i + count] = 0;
    }

    for (int i = 7; i > 0; i--) //GPT로 만든 배열 무작위로 섞기
    {
        // 0부터 i까지 중에서 무작위로 인덱스 선택
        int j = rand() % (i + 1);

        // arr[i]와 arr[j]의 값을 서로 바꿈
        int temp = Bullets[i];
        Bullets[i] = Bullets[j];
        Bullets[j] = temp;
    }
}

//인버터 사용시에 총알 갯수 감소를 판단하는 함수
void Check_inverter(int i)
{
    if (Bullets[i] == 1) //실탄이면
    {
        if (Inverter_on) Fake_bullet--; //인버터를 사용했으면 실제 탄은 공포탄임
        else Real_bullet--; //안바뀌었다면 그대로 실탄 갯수 감소
    }
    else if (Bullets[i] == 0) //공포탄이면
    {
        if (Inverter_on) Real_bullet--; //인버터를 사용했으면 실제 탄은 실탄임
        else Fake_bullet--; //안바뀌었다면 그대로 공포탄 갯수 감소
    }
    if (Bullets[i] != 2) Inverter_on = false; //더미데이터인 2가 아니면 인버터 비활성화
}

//총알을 쏘고 어떤 총알을 쐈는지 확인하는 이펙트
void Bullet_effect(const char* Sound, const char* Bullet)
{
    Play_bgm(Sound, 0); Draw_image_128(Bullet, hdc, 745, 500); //쓴 총알 이미지
    Sleep(3000); Draw_image_128(None, hdc, 745, 500); //잠시 대기 후 이미지 지우기
    Draw_image_256(None, hdc, 675, 200); //총의 이미지도 지움
}

//상대에게 총알 발사하는 함수
int Fire_toYou(int i)
{
    gotoxy(100, 1); printf("                    ");
    if (Bullets[i] == 1) //현재 턴의 총알이 실탄이면
    {
        if (Turn == 1)
        {
            Com_hp--; //유저 턴이면 컴퓨터 체력 감소
            Score_fire += 50; //상대에게 실탄을 맞췄으므로 점수획득
            Draw_image_256(Buckshot_right, hdc, 675, 200); //상대를 향해 총을 겨누는 이미지
            if (Saw_on == true)
            {
                Com_hp--; Score_fire += 50; //톱 활성화일 떄 추가 체력감소와 점수 획득
            }
        }
        else if (Turn == 0)
        {
            User_hp--; //PC턴이면 유저 체력 감소
            Draw_image_256(Buckshot_left, hdc, 675, 200); //상대를 향해 총을 겨누는 이미지
            if (Saw_on == true) User_hp--; //톱 활성화일 떄 추가 체력감소
        }

        Bullet_effect(Fire_real, Real_bullet_image); //사용된 탄 확인 이펙트
        Draw_hp(); //체력이 변동되었으므로 다시 출력
    }
    else if (Bullets[i] == 0) //공포탄이면
    {
        if (Turn == 1) { Draw_image_256(Buckshot_right, hdc, 675, 200); } //상대를 향해 총을 겨누는 이미지
        else { Draw_image_256(Buckshot_left, hdc, 675, 200); } //상대를 향해 총을 겨누는 이미지

        Bullet_effect(Fire_fake, Fake_bullet_image); //사용된 탄 확인 이펙트
    }
    if (Bullets[i] != 2) Saw_on = false; //총을 발사했다면 톱 사용 종료 선언

    Check_inverter(i); //인버터 유무에 따라 총알 갯수 감소가 달라짐

    if (Handcuff_on && Bullets[i] != 2) //수갑을 썼다면 턴이 넘어가지 않음
    {
        Handcuff_on = false;
        Draw_image_128(None, hdc, 640, 700); //수갑 이미지 지움
        Draw_image_128(None, hdc, 850, 700);
    }
    else if (Handcuff_on == false && Bullets[i] != 2) Turn = !Turn; //총알을 발사했으므로 상대에게 턴 넘어감

    if (Bullets[i] == 2) return Fire_toYou(i + 1); //만약 실탄도 공포탄도 아니면 탄이 나올때까지 이 함수 반복
    else return i + 1; //성공적으로 발사되었으므로 i+1을 반환
}

//나에게 총알 발사하는 함수
int Fire_toMe(int i)
{
    gotoxy(100, 1); printf("                    ");
    if (Bullets[i] == 1) //현재 턴의 총알이 실탄이면
    {
        if (Turn == 1)
        {
            Draw_image_256(Buckshot_left, hdc, 675, 200); //자신을 향해 총을 겨누는 이미지
            User_hp--; //유저 턴이면 유저 체력 감소
            Score_fire -= 30;
            if (Saw_on == true)
            {
                User_hp--; //톱 활성화일 때 추가 체력감소
                Score_fire -= 30; //자신에게 총을 쐈으므로 점수 감점
            }
        }
        else if (Turn == 0)
        {
            Draw_image_256(Buckshot_right, hdc, 675, 200); //자신을 향해 총을 겨누는 이미지
            Com_hp--; //컴퓨터 턴이면 컴퓨터 체력 감소
            if (Saw_on == true) Com_hp--; //톱 활성화일 때 추가 체력감소
        }
        Draw_hp(); //체력이 변동되었으므로 다시 출력

        Bullet_effect(Fire_real, Real_bullet_image); //사용된 탄 확인 이펙트

        if (Handcuff_on) //수갑을 썼다면 턴이 넘어가지 않음
        {
            Handcuff_on = false; 
            Draw_image_128(None, hdc, 640, 700); //수갑 이미지 지움
            Draw_image_128(None, hdc, 850, 700);
        }
        else if (Handcuff_on == false && Bullets[i] != 2) Turn = !Turn; //총알을 발사했으므로 상대에게 턴 넘어감
    }
    if (Bullets[i] == 0) //공포탄이면
    {
        if (Turn == 1)
        {
            Score_fire += 30; //나에게 공포탄을 쐈으므로 점수획득
            Draw_image_256(Buckshot_left, hdc, 675, 200); //자신을 향해 총을 겨누는 이미지
        }
        else Draw_image_256(Buckshot_right, hdc, 675, 200); //자신을 향해 총을 겨누는 이미지

        Bullet_effect(Fire_fake, Fake_bullet_image); //사용된 탄 확인 이펙트
    }
    if (Bullets[i] != 2) Saw_on = false; //총알을 사용했다면 톱 사용 종료 선언

    Check_inverter(i); //인버터 유무에 따라 총알 갯수 감소가 달라짐

    if (Bullets[i] == 2) return Fire_toMe(i + 1); //만약 실탄도 공포탄도 아니면 탄이 나올때까지 이 함수 반복
    else return i + 1; //성공적으로 발사되었으므로 i+1을 반환
}

//유저가 선택하는 턴 함수
void User_turn()
{
    gotoxy(26, 8); printf("상대에게 발사 : 1   나에게 발사 : 2"); //키 입력 안내
    Score_turn += 1; //매턴 1점 획득
    while (1)
    {
        check_bullet = Now_bullet; //현재 몇 번째 총알인지 알려주는 변수

        char input = _getch(); //키 입력받기
        if (input == '1') //상대에게 쏘기
        {
            Now_bullet = Fire_toYou(Now_bullet); //상대에게 총알 발사 후 다음 총알로
        }
        else if (input == '2') //나에게 쏘기
        {
            Now_bullet = Fire_toMe(Now_bullet); //나에게 총알 발사 후 다음 총알로
        }
        else Who_useitem(input); //아이템 사용 함수 호출

        if (User_hp <= 0 || Com_hp <= 0) break; //누구든 체력이 0이면 라운드 혹은 게임 종료하러 탈출

        if (check_bullet != Now_bullet) { break; } //총알을 쏘면 다음 총알로 이동하여 반복문 탈출
    }
    gotoxy(26, 8); printf("                                   "); //키 입력 안내문 지우기
}

//PC가 상황에 따라 유동적으로 행동하는 함수
//PC가 아드레날린으로 뺏는 아이템 목록 : 수갑, 담배, 톱, 인버터
//이외의 다른 아이템을 뺏는 것은 밸런스를 위해 제한함
void Com_turn()
{
    bool What_bullet; //돋보기 사용시 받아올 현재 탄의 종류
    srand((unsigned int)time(NULL));
    gotoxy(153, 8); printf("PC 턴"); Sleep(1000); //PC의 턴임을 알리는 안내문

    // 2 < PC 체력 < 최대체력-1, 상한약을 사용할 수 있다면 우선 사용 후 턴 진행
    //게임 밸런스를 위해 사용 환경을 제한함
    if (Com_hp > 2 && Com_hp < Max_hp - 1 && Com_item[5]>0)
        Who_useitem('y');

    //담배를 사용할 수 있다면 우선 사용 후 턴 진행
    //체력이 닳았고 담배가 있다면 or 체력이 닳았고 아드레날린으로 담배를 뺏을 수 있다면 담배 계속 사용
    while (Com_hp < Max_hp && ( Com_item[2] > 0 || (Com_item[4] > 0 && User_item[2] > 0) ) )
    {
        if (Com_item[2] > 0) //담배를 소유중이라면, 아드레날린보다도 PC 본인의 담배를 우선 사용
        {
            Who_useitem('e'); //담배 사용
            Sleep(2000);
        }
        else if (Com_item[4] > 0 && User_item[2] > 0) //아드레날린이 있고 유저가 담배를 소유중이라면
            Com_adrenaline(2);
    }

    //수갑을 사용할 수 있다면 우선 사용 후 턴 진행
    if (Handcuff_on == false)
    {
        if (Com_item[1] > 0) //수갑을 소유중이라면
            Who_useitem('w'); //수갑 사용
        else if (Com_item[4] > 0 && User_item[1] > 0) //아드레날린이 있고 유저가 수갑을 소유중이라면
            Com_adrenaline(1);
    }

    //상대를 공격할 수 있는 우선순위에 따라서 행동 
    if (Real_bullet != 0 && Fake_bullet == 0) //실탄만 남은 상황, 무조건 실탄이므로 총알 발사
    {
        Com_saw(); //톱을 사용할 수 있는지 판단 후 사용
        Now_bullet = Fire_toYou(Now_bullet); //상대에게 발사
        Draw_hp(); Sleep(3000);
    }
    else if (Real_bullet == 0 && Fake_bullet != 0) //공포탄만 남은 상황, 무조건 공포탄이므로 자신에게 발사
    {
        //어떤 방법으로든 인버터를 사용할 수 있다면
        if (Com_item[8] > 0 || (Com_item[4] > 0 && User_item[8] > 0))
        {
            Com_inverter(); //그 방법으로 인버터 사용
            Com_saw(); //톱을 사용할 수 있는지 판단 후 사용
            Now_bullet = Fire_toYou(Now_bullet); //상대에게 발사
            Draw_hp(); Sleep(3000);
        }
        else //인버터가 없다면, 공포탄이므로
        {
            Now_bullet = Fire_toMe(Now_bullet); //자신에게 발사
            Sleep(3000);
        }
    }
    else if (Now_bullet == Com_phone_index) //대포폰 사용 후, 현재 인덱스가 대포폰으로 본 인덱스일 경우
    {
        if (Com_phone_bullet == 1) //실탄이라면
        {
            Com_saw(); //톱을 사용할 수 있는지 판단 후 사용
            Now_bullet = Fire_toYou(Now_bullet); //상대에게 발사
            Sleep(3000);
        }
        //공포탄일때, 어떤 방법으로든 인버터를 사용할 수 있다면
        else if (Com_item[8] > 0 || (Com_item[4] > 0 && User_item[8] > 0))
        {
            Com_inverter(); //그 방법으로 인버터 사용
            Com_saw(); //톱을 사용할 수 있는지 판단 후 사용
            Now_bullet = Fire_toYou(Now_bullet); //상대에게 발사
            Draw_hp(); Sleep(3000);
        }
        else //인버터가 없다면, 공포탄이므로
        {
            Now_bullet = Fire_toMe(Now_bullet); //자신에게 발사
            Sleep(3000);
        }
    }
    else if (Com_item[6] > 0) //돋보기를 소유중이라면
    {   
        //실탄 또는 공포탄이 나올 때까지 Now_bullet을 증가시킨뒤
        while (Now_bullet < MAX_BULLET && Bullets[Now_bullet] == 2) Now_bullet++;

        //돋보기 사용후 현재 탄이 무엇인지 받아옴
        What_bullet = Item_magnifier(); Com_item[6]--;
        Sleep(3500);

        if (What_bullet == 1) //실탄을 봤다면
        {
            Com_saw(); //톱을 사용할 수 있는지 판단 후 사용
            Now_bullet = Fire_toYou(Now_bullet); //상대에게 발사
            Sleep(3000);
        }
        //공포탄을 봤다면, 어떤 방법으로든 인버터를 사용할 수 있다면
        else if (Com_item[8] > 0 || (Com_item[4] > 0 && User_item[8] > 0))
        {
            Com_inverter(); //그 방법으로 인버터 사용
            Com_saw(); //톱을 사용할 수 있는지 판단 후 사용
            Now_bullet = Fire_toYou(Now_bullet); //상대에게 발사
            Draw_hp(); Sleep(3000);
        }
        else //인버터가 없다면, 공포탄이므로
        {
            Now_bullet = Fire_toMe(Now_bullet); //자신에게 발사
            Sleep(3000);
        }
    }
    else if (Com_item[0] > 0) //맥주를 소유중이라면
    {
        Who_useitem('q'); //맥주 사용
        Sleep(2000);
    }
    else if (Com_item[7] > 0 && Real_bullet + Fake_bullet > 1) //대포폰을 소유중이고, 남은 총알이 한 발이 아니라면
    {
        Who_useitem('i'); //대포폰 사용
        Sleep(2000);
    }
    else //아이템이 아예 없을 때, 혹은 사용할만한 아이템이 없을 때
    {
        if (Real_bullet > Fake_bullet) //실탄이 더 많으면 유저에게 발사
        {
            Now_bullet = Fire_toYou(Now_bullet); Sleep(3000);
        }
        else if (Fake_bullet > Real_bullet) //공포탄이 더 많으면 자신에게 발사
        {
            Now_bullet = Fire_toMe(Now_bullet); Sleep(3000);
        }
        else //실탄과 공포탄의 갯수가 동일하다면
        {
            int x = rand() % 2; //0, 1 난수 생성으로 도박
            if (x == 0) //유저에게 발사
            {
                Now_bullet = Fire_toYou(Now_bullet); Sleep(3000);
            }
            else if (x == 1) //자신에게 발사
            {
                Now_bullet = Fire_toMe(Now_bullet); Sleep(3000);
            }
        }
    }
    gotoxy(153, 8); printf("     ");
    Sleep(1000);
}

//게임 셋팅 함수
void Game_setting(int Round)
{
    system("cls"); gotoxy(190, 0); printf("%d 라운드", Round + 1); //우측 상단, 라운드를 알림
    srand((unsigned int)time(NULL));
    Max_hp = rand() % 4 + 2; //체력은 2~5 랜덤, 밸런스를 위해 최소를 3으로 조정할 수도 있음

    //게임 시작전 초기화
    //PC와 유저의 체력을 초기화, 현재 총알 인덱스를 초기화, 수갑도 초기화
    Com_hp = Max_hp; User_hp = Max_hp; Now_bullet = 0; Handcuff_on = false;

    for (int i = 0; i < ITEM; i++) { User_item[i] = 0; Com_item[i] = 0; } //게임 시작시 아이템 갯수 초기화
    for (int i = 0; i < MAX_ITEM; i++) //게임 시작시 테이블 초기화
    {
        User_Table[i] = '1', Com_Table[i] = '1', User_item_table[i] = 0, Com_item_table[i] = 0;
    } 

    Turn = true; //유저 턴으로 시작
    Draw_table(); //테이블 표시
}

/*게임 시작-------------------------------------------------------------------------------게임 시작*/
void Game()
{
    int Now_score = Score; //묻더 실패의 경우 점수가 깎이므로 진입 시점의 점수 저장
    for (int Round = 0; Round < 3; Round++) //3라운드 동안 진행
    {
        Game_setting(Round); //게임 셋팅
        Draw_image_128(User_image, hdc, 290, 540); //유저 초상화 출력

        switch (Round) //라운드에 따라 PC의 초상화가 달라짐
        {
        case 0:
            Draw_image_128(Com_image, hdc, 1190, 540);
            break;
        case 1:
            Draw_image_128(Com2_image, hdc, 1190, 540);
            break;
        case 2:
            Draw_image_128(Com3_image, hdc, 1190, 540);
            break;
        }

        while (1) //라운드 동안 진행되는 게임
        {
            Random_reload(); //총알 생성 & 장전
            Com_phone_index = 9; Com_phone_bullet = 0; //재장전시 컴이 쓴 대포폰 정보 초기화

            //화면을 지우므로 다시 그려야 하는 것들
            Draw_handcuff(); //수갑이 채워져 있다면 표시
            Draw_hp(); //체력 표시
            Item_give(); //장전하면 아이템 지급
            Item_draw(Round); //아이템 안내, 유저와 컴 초상화

            Now_bullet = 0; //현재 총알 인덱스 초기화

            while (1) //탄창 하나 동안 진행되는 게임
            {
                while (Turn == true) //Turn == ture => 유저 턴이면 유저 턴 발동
                {
                    Draw_handcuff(); //화면이 지워지므로 수갑이 채워져 있다면 표시
                    User_turn(); //유저 턴 발동
                    if (Real_bullet == 0 && Fake_bullet == 0) break; //모든 총알을 소모하면 재장전하러 탈출
                    if (User_hp <= 0 || Com_hp <= 0) break; //누구든 체력이 0이 되면 게임을 끝내러 탈출
                }

                if (Real_bullet == 0 && Fake_bullet == 0) break; //이하 동일 생략
                if (User_hp <= 0 || Com_hp <= 0) break; 

                while (Turn == false)
                {
                    Draw_handcuff(); //화면이 지워지므로 수갑이 채워져 있다면 표시
                    Com_turn(); //컴 턴 발동
                    if (Real_bullet == 0 && Fake_bullet == 0) break;
                    if (User_hp <= 0 || Com_hp <= 0) break;
                }

                if (Real_bullet == 0 && Fake_bullet == 0) break;
                if (User_hp <= 0 || Com_hp <= 0) break;
            }
            if (User_hp <= 0 || Com_hp <= 0) break; //누구든 죽었다면 라운드 종료
        }

        if (User_hp <= 0) break; //컴 승리시 게임 종료
        else if (Com_hp <= 0) //유저 승리시
        {
            Score_turn += 100 * (Round + 1); //라운드별 승리시 100, 200, 300점
            Sleep(2000); //잠시 대기 후 다음 라운드로 넘어감
        }
    }
    if (Com_hp <= 0) Check_double(); //유저 승리시 묻더 물어봄
    else if (User_hp <= 0) Gameover(Now_score); //컴 승리시 패배, 묻더시 점수 획득이 없으므로 처음 저장한 점수를 반환
}

//o메인 함수o//

int main(void)
{
    Invisible_Cursor(0); //커서 깜빡거림 지움
    SetConsoleView(); //콘솔창 크기와 제목 설정
    Global_make(); //이미지 출력을 위한 전역변수 선언
    Read_ranking(); //외부 파일의 랭킹 읽어옴
    Read_archive(); //외부 파일의 도감 현황 읽어옴

    Warning_screen(); //게임 시작 전 경고 화면

    Mainscreen(); //메인 화면, 여기서 끝없이 함수가 이어지며 게임이 끝나지 않음

    ReleaseDC(hwnd, hdc); //이미지 출력을 끝내는 함수, 이 때문에 강제종료시 약간의 경고가 발생

    return 0;
}