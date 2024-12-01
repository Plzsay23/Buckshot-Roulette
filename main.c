#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <stdbool.h>
#include <time.h>
#include <mmsystem.h> //���
#pragma comment(lib,"winmm.lib") //���

#define RANKING 10 //��ŷ �迭 ũ��
#define ITEM 10 //������ �迭 ũ��, ������ ��� �Լ����� �߸��� Ű�� �Է������� �������� ����ϸ� �ȵǱ⿡ �ε��� 9��� ���̵����͸� ������ ��
#define MAX_ITEM 8 //������ �ִ� ����
#define MAX_BULLET 8 //�ִ� �Ѿ� ���� 8��
#define LEFT 75
#define RIGHT 77

//�Լ� ���������� ���� ����
void Gamecheck(); void Rankingscreen(); void Archive(); void Game(); void Gamewin(); void Draw_handcuff(); void Item_use(int index);
void Draw_hp(); void Com_adrenaline(int item); void Who_useitem(char c); void Archive_select(int n);

//o���� ���� ���� ����o//

int Score = 0, /*������ ������ ������ ����*/ Score_item = 0, /*������ ���� ȹ���ϴ� ����*/ Score_fire = 0; /*�Ѿ� �߻�� ȹ���ϴ� ����*/
int Score_turn = 0, /*���ϸ��� ȹ���ϴ� ����*/ Score_reload = 0; /*�����ø��� ȹ���ϴ� ����*/
char Name[30]; //������ �̸��� ������ ����

bool Score_double = false; //������ ���� �ι� ����

int Real_bullet, Fake_bullet; //��ź, ����ź�� ����

int Com_hp, User_hp, Max_hp; //��ǻ�� ü��, ���� ü��, ���� ���� ü��
int Now_bullet = 0, check_bullet; //���� �Ѿ� �ε���, �ε��� ��ġ üũ ����

//�Ѿ� ���� �迭, �ּ� 2��, �ִ� 8���ε� �迭 ũ�⸦ �������� �� �� ��� ��ź, ����ź�� �ƴϸ� 2�� ������
int Bullets[MAX_BULLET] = { 2,2,2,2,2,2,2,2 }; 

bool Turn; //true�϶� ����, false�϶� ��ǻ�� ��

bool Handcuff_on = false, /*���� ��� ����*/ Saw_on = false, /*�� ��� ����*/ Inverter_on = false; /*�ι��� ��� ����*/
bool Adrenaline_on = false, /*�Ƶ巹���� ��� ����*/ Adrenaline_used = false; /*�������� ���ﶧ �Ƶ巹������ ��� ������ �Ǵ�*/

//����0 ����1 ���2 ��3 �Ƶ巹����4 ���Ѿ�5 ������6 ������7 �ι���8 ���̵�����9
//���� ���� �ʿ��� ������ �߸��� Ű �Է½� ���� ������ �����ϱ� ����
int User_item[ITEM] = { 0 }, Com_item[ITEM] = { 0 }; //���� ������ ���� �迭
int Have_item[ITEM] = { 0 }; //�������� ȹ���ߴ��� Ȯ���ϴ� �迭, �� �ε������� Ȯ��, �������� ���

//���� ���̺� ��ġ�� �������� �ִ��� ������ �����ϴ� �迭
bool User_item_table[MAX_ITEM] = { 0 }, Com_item_table[MAX_ITEM] = { 0 }; 

//������ ���̺� �ּڰ�, � �������� �ִ����� ���� �ּڰ��� �ٲ�
char User_Table[MAX_ITEM] = { '1','1','1','1','1','1','1','1' }, Com_Table[MAX_ITEM] = { '1','1','1','1','1','1','1','1' }; 

int Com_phone_index = 9; bool Com_phone_bullet = 0; //PC�� ������ ���� ���� ����

bool Bgm_mainscreen = 0; //���ο��� ����� ����۵Ǵ� ���� ���� ����

char END[] = "����Ϸ���\r�ƹ�\rŰ��\r�����ʽÿ�\r.\r.\r."; //������ ���� ����

int x = 0, y = 0; //�������� �׸��� �� ��ǥ

typedef struct User { //��ŷ �迭�� ���� ����� ����
    int Userscore; //����, ���� ����� Score�� ����� ����
    char Username[30]; //�̸�, ���� ����� Name�� ����� ����
}User; User Ranking[RANKING]; //��ŷ �迭 ����

//�̹��� ��� ���� ���� �ڵ�, GPT ���
HWND hwnd; //���� ���׶��� �������� �ڵ��� ������
HDC hdc; //�������� Ŭ���̾�Ʈ ������ ���� ����̽� ���ؽ�Ʈ�� ������

//���� �̹��� �������� ���� ������ Ȱ���ϱ� ���� ������ �ϴ� �Լ�
void Global_make()
{
    hwnd = GetForegroundWindow();
    hdc = GetWindowDC(hwnd);
}

//���� ����ü, ex) Textcolor(black, white) �̷��� ����� ����, ���ڴ� ������� ������
//https://makerejoicegames.tistory.com/115
enum {
    black, blue, green, skyblue, red, pink, orange, white, gray, lightblue, brightgreen, sky, brightred, brightpink, brightyellow, brightwhite
};
//������ ���� ���� ���� �����ϴ� �Լ�
void Textcolor(int bgcolor, int txcolor)
{
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(handle, (bgcolor << 4) + txcolor);
}

//o��� �Լ� ����o// �б� ���迡�� ��Ź�Ͽ� ���� �ڵ�

const char* Main_bgm = "Sounds\\Main_bgm.wav"; //����ȭ�� ���
const char* Gamecheck_bgm = "Sounds\\Gamecheck_bgm.wav"; //���ӽ��� �� ȭ�� ���
const char* Gameover_bgm = "Sounds\\Gameover_bgm.wav"; //���ӿ���ȭ�� ���
const char* Gamewin_bgm = "Sounds\\Gamewin_bgm.wav"; //���ӽ¸�ȭ�� ���

const char* Reload_sound = "Sounds\\Reload_sound.wav"; //�����Ҹ�
const char* Reload_bullet = "Sounds\\Reload_bullet.wav"; //�Ѿ� �ִ� �Ҹ�
const char* Fire_real = "Sounds\\Fire_real.wav"; //��ź
const char* Fire_fake = "Sounds\\Fire_fake.wav"; //����ź

const char* Beer_sound = "Sounds\\Beer_sound.wav"; //������ ȿ����
const char* Handcuff_sound = "Sounds\\Handcuff_sound.wav";
const char* Cigarette_sound = "Sounds\\Cigarette_sound.wav"; 
const char* Saw_sound = "Sounds\\Saw_sound.wav";
const char* Adrenaline_sound = "Sounds\\Adrenaline_sound.wav";
const char* Medicine_sound = "Sounds\\Medicine_sound.wav"; 
const char* Magnifier_sound = "Sounds\\Magnifier_sound.wav";
const char* Phone_sound = "Sounds\\Phone_sound.wav";
const char* Inverter_sound = "Sounds\\Inverter_sound.wav";

//����� ����ϴ� �Լ�
void Play_bgm(const char* filename, bool loop)
{
    int filenameLength = strlen(filename) + 1;
    int bufferSize = MultiByteToWideChar(CP_UTF8, 0, filename, filenameLength, NULL, 0);
    TCHAR* wideFilename = (TCHAR*)malloc(bufferSize * sizeof(TCHAR));
    MultiByteToWideChar(CP_UTF8, 0, filename, filenameLength, wideFilename, bufferSize);
    PlaySound(NULL, NULL, 0);
    if(loop == 1) //�ݺ�����ϴ� ���
        PlaySound(wideFilename, NULL, SND_FILENAME | SND_LOOP | SND_ASYNC);
    else //�� ���� ����ϴ� ���
        PlaySound(wideFilename, NULL, SND_FILENAME | SND_ASYNC);
    free(wideFilename);
}

//�ݺ������ ����� ������ �Լ�
void Stop_bgm()
{
    PlaySound(NULL, NULL, 0);
}

//o�̹��� �Լ� ����o// ���� GPT ���

const char* Beer = "Images\\Beer.raw"; //�̹��� ����, ���� �� ������
const char* Handcuff = "Images\\Handcuff.raw"; 
const char* Cigarette = "Images\\Cigarette.raw";
const char* Saw = "Images\\Saw.raw";
const char* Adrenaline = "Images\\Adrenaline.raw";
const char* Medicine = "Images\\Medicine.raw";
const char* Magnifier = "Images\\Magnifier.raw";
const char* Phone = "Images\\Phone.raw";
const char* Inverter = "Images\\Inverter.raw";
const char* Buckshot_left = "Images\\Buckshot_left.raw"; //��, ������ �ܴ�
const char* Buckshot_right = "Images\\Buckshot_right.raw"; //������ �ܴ�
const char* None = "Images\\None.raw"; //�̹��� ����
const char* Warning = "Images\\Warning.raw"; //���
const char* Game_win = "Images\\Gamewin.raw"; //���� �¸�
const char* Real_bullet_image = "Images\\Real_bullet_image.raw"; //��ź
const char* Fake_bullet_image = "Images\\Fake_bullet_image.raw"; //����ź
const char* HP = "Images\\HP.raw"; //ü��
const char* User_image = "Images\\User.raw"; //����
const char* Com_image = "Images\\Com.raw"; //��, 1����
const char* Com2_image = "Images\\Com2.raw"; //2����
const char* Com3_image = "Images\\Com3.raw"; //3����

//�̹����� �о� �ҷ����� �Լ�
void ReadImage(const char* filePath, unsigned char** image)
{
    FILE* fp = fopen(filePath, "rb");
    fread(*image, 1, 256 * 256, fp);
    fclose(fp);
}

//256x256 ũ���� �̹����� �׸��� �Լ�
void Draw_image_256(const char* filePath, HDC hdc, int offsetX, int offsetY)
{
    //�� �޸𸮿� �̹��� �迭 �Ҵ�
    unsigned char* image = (unsigned char*)malloc(256 * 256 * sizeof(unsigned char));
    if (image == NULL) return;
    ReadImage(filePath, &image); //�̹��� ������ �о��
    //�̹����� �״�� ���
    for (int r = 0; r < 256; r++)
    {
        for (int c = 0; c < 256; c++)
        {
            int gray = image[r * 256 + c];
            SetPixel(hdc, c + offsetX, r + offsetY, RGB(gray, gray, gray));
        }
    }
    free(image); //�� �޸� ����
}

//256x256 ũ���� �̹����� 128x128�� ����Ͽ� �׸��� �Լ�
void Draw_image_128(const char* filePath, HDC hdc, int offsetX, int offsetY)
{
    //�� �޸𸮿� �̹��� �迭 �Ҵ�
    unsigned char* image = (unsigned char*)malloc(256 * 256 * sizeof(unsigned char));
    if (image == NULL) return;
    ReadImage(filePath, &image); //�̹��� ������ �о��
    //128x128 ũ���� ��ҵ� �̹��� �迭�� �� �޸𸮿� �Ҵ�
    unsigned char* smallImage = (unsigned char*)malloc(128 * 128 * sizeof(unsigned char));
    if (smallImage == NULL) return;
    //�̹��� ũ�� ���
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
    //��ҵ� �̹����� ���
    for (int r = 0; r < 128; r++)
    {
        for (int c = 0; c < 128; c++)
        {
            int gray = smallImage[r * 128 + c];
            SetPixel(hdc, c + offsetX, r + offsetY, RGB(gray, gray, gray));
        }
    }
    free(image); //�� �޸� ����
    free(smallImage);
}

//256x256 ũ���� �̹����� 64x64�� ����Ͽ� �׸��� �Լ�
void Draw_image_64(const char* filePath, HDC hdc, int offsetX, int offsetY)
{
    //�� �޸𸮿� �̹��� �迭 �Ҵ�
    unsigned char* image = (unsigned char*)malloc(256 * 256 * sizeof(unsigned char));
    if (image == NULL) return;
    ReadImage(filePath, &image); //�̹��� ������ �о��
    //64x64 ũ���� ��ҵ� �̹��� �迭�� �� �޸𸮿� �Ҵ�
    unsigned char* smallImage = (unsigned char*)malloc(64 * 64 * sizeof(unsigned char));
    if (smallImage == NULL) return;

    //�̹��� ũ�� ���
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
    //��ҵ� �̹����� ���
    for (int r = 0; r < 64; r++)
    {
        for (int c = 0; c < 64; c++)
        {
            int gray = smallImage[r * 64 + c];
            SetPixel(hdc, c + offsetX, r + offsetY, RGB(gray, gray, gray));
        }
    }
    free(image); //�� �޸� ����
    free(smallImage);
}

//256x256 ũ���� �̹����� 512x512�� Ȯ���Ͽ� �׸��� �Լ�
void Draw_image_512(const char* filePath, HDC hdc, int offsetX, int offsetY)
{
    //�� �޸𸮿� �̹��� �迭 �Ҵ�
    unsigned char* image = (unsigned char*)malloc(256 * 256 * sizeof(unsigned char));
    if (image == NULL) return;
    ReadImage(filePath, &image); //�̹��� ������ �о��
    //512x512 ũ���� Ȯ��� �̹��� �迭�� �� �޸𸮿� �Ҵ�
    unsigned char* largeImage = (unsigned char*)malloc(512 * 512 * sizeof(unsigned char));
    if (largeImage == NULL) return;
    //�缱�� �������� ����Ͽ� �̹��� ũ�� Ȯ��
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
            //�缱�� ����
            float top = (1 - a) * image[y1 * 256 + x1] + a * image[y1 * 256 + x2];
            float bottom = (1 - a) * image[y2 * 256 + x1] + a * image[y2 * 256 + x2];
            float gray = (1 - b) * top + b * bottom;
            largeImage[r * 512 + c] = (unsigned char)gray;
        }
    }
    //Ȯ��� �̹����� ���
    for (int r = 0; r < 512; r++)
    {
        for (int c = 0; c < 512; c++)
        {
            int gray = largeImage[r * 512 + c];
            SetPixel(hdc, c + offsetX, r + offsetY, RGB(gray, gray, gray));
        }
    }
    free(image); //�� �޸� ����
    free(largeImage);
}

//o���� �Լ� ����o//

//Ŀ�� �����Ÿ��°� �����
//https://blog.naver.com/ifthe1201/221056588399
void Invisible_Cursor(char show)
{
    HANDLE hConsole; CONSOLE_CURSOR_INFO ConsoleCursor;
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    ConsoleCursor.bVisible = show; ConsoleCursor.dwSize = 1;
    SetConsoleCursorInfo(hConsole, &ConsoleCursor);
}

//Ŀ�� ��ġ �̵�
//https://blog.naver.com/ifthe1201/221056588399
void gotoxy(int x, int y)
{
    COORD pos = { x,y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

//�ܼ� â ũ��� ���� ����
//https://blockdmask.tistory.com/344
void SetConsoleView(void)
{
    system("mode con:cols=200 lines=50");
    system("title Buckshot Roulette");
}

//o���� ȭ�� ����o//

//����ȭ�� �׵θ� �׸��� �Լ�, ���� ������ ���ٺ��� �Լ��� ���� ���°� ȿ����, ���� �ϴ��� ��ǥ (199, 49)
void Draw_square()
{
    for (int i = 0; i < 199; i++) { gotoxy(i, 0); printf("��"); gotoxy(i, 49); printf("��"); }
    for (int i = 1; i < 49; i++) { gotoxy(0, i); printf("��"); gotoxy(198, i); printf("��"); }
}

//������ ���̺��� �׸��� �Լ�
void Draw_table()
{
    int x1; //������ PC�� ���̺� �ΰ��� �׸��� ����
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

//���� ù ���۽� �����ϴ� ���
void Warning_screen()
{
    gotoxy(52, 30); printf("    CAPS LOCK�� �����ִ��� Ȯ���Ͻÿ�! ���� �����ִٸ� ����� �۵����� ���� �� �ֽ��ϴ�!");
    gotoxy(52, 32); printf("                   ȭ�� Ŭ�� �Ǵ� �巡�� �� �̹����� ������ ���� �ֽ��ϴ�!");
    gotoxy(52, 34); printf("               ��� ������ �Ҹ��� Ŭ ���� �ֽ��ϴ�! ���� ���� �� �������ּ���!");
    gotoxy(52, 36); printf("                  Ű�� ��Ÿ���� ������! �ǵ�ġ �ʴ� �ൿ�� �� ���� �ֽ��ϴ�!");

    for (int i = 0; END[i] != '\0'; i++) //����Ϸ��� �ƹ� Ű�� �����ʽÿ� . . .
    {
        gotoxy(79 + i, 38); printf("%c", END[i]);
        Sleep(50);
    } Sleep(500);
    
    Draw_image_256(Warning, hdc, 670, 200); //��� �̹���

    _getch(); //Ű�Է� ���

    for (int i = 52; i < 150; i++) //�۾� �����
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

//����ȭ���� �۾� ������ �ٲٱ� ���� ���� �� �Լ�
void Draw_main()
{
    gotoxy(116, 20); printf("��������   ��            ��   ������� ��         ��   ");
    gotoxy(116, 21); printf("��            �� ��            ��  ��            ��      ��     ");
    gotoxy(116, 22); printf("��            �� ��            �� ��             ��    ��      ");
    gotoxy(116, 23); printf("��������   ��            �� ��             �� ��          ");
    gotoxy(116, 24); printf("��            �� ��            �� ��             ��    ��       ");
    gotoxy(116, 25); printf("��            �� ��            ��  ��            ��      ��     ");
    gotoxy(116, 26); printf("��������    ��������    ������� ��         ��  ");
    gotoxy(96, 30); printf("   �������  ��            ��   �������    ��������");
    gotoxy(96, 31); printf("  ��             ��            �� ��            ��        ��");
    gotoxy(96, 32); printf("  ��             ��            �� ��            ��        ��");
    gotoxy(96, 33); printf("    ������� ��������� ��            ��        ��");
    gotoxy(96, 34); printf("              �� ��            �� ��            ��        ��");
    gotoxy(96, 35); printf("              �� ��            �� ��            ��        ��");
    gotoxy(96, 36); printf("  �������   ��            ��   �������          ��");
}

//����ȭ�� //�ܼ� â ũ��� 120x30 �� ( �ȼ� ���� => ����:���� = 2:1 )
void Mainscreen(void)
{
    system("cls");
    if (Bgm_mainscreen == 0) //��� ����, ����,��ŷ,���� ȭ�� �̵��ÿ��� ����� �ٽ� ������� ����
    {
        Play_bgm(Main_bgm, 1);
        Bgm_mainscreen = true; //�� ������ 1�̸� ����,��ŷ,���� ȭ���� �̵��ص� ����� ������ ����
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

    gotoxy(41, 10); printf("���� ���� : 1");
    gotoxy(93, 10); printf("��ŷ ���� : 2");
    gotoxy(145, 10); printf("�������� : 3");
    gotoxy(186, 48); printf("�������� : Q");

    //���� https://jesus12.tistory.com/entry/C%EC%96%B8%EC%96%B4-%ED%81%B4%EB%9F%AD-%ED%95%A8%EC%88%98-clock-3%EC%B4%88-%EB%8F%99%EC%95%88-%EB%B0%98%EB%B3%B5%ED%95%98%EA%B8%B0
    clock_t time = clock(); //���� ������ ����ð��� ���

    while (1) //�ൿ ����
    {
        char input;
        if (_kbhit()) //Ű���尡 ��������
        {
            input = _getch();
            if (input == '1') // 1�� ������ ���ӽ���
            {
                Bgm_mainscreen = false; //�� ������ 0�� ��� ���߿� �ٽ� �������� ������ ����� �����
                Stop_bgm(); //��� ����
                Gamecheck(); break; //���� ����
            }
            else if (input == '2') // 2�� ������ ��ŷȭ��
            {
                Rankingscreen(); break;
            }
            else if (input == '3') // 3�� ������ ����ȭ��
            {
                Archive(); break;
            }
            else if (input == 'q') //��������
                exit(0);
        }

        if (time + 200 == clock()) //���� �������� ����ð� "clock()" ���� �ɸ� �ð��� 200ms�̸� �ߵ�
        {
            Textcolor(black, gray); //������ �۾��� ��¦�̰� ��
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
            time = clock(); //���� ������ �ʱ�ȭ�Ͽ� if������ ���� �ݺ��ǵ��� ����
            Textcolor(black, white);
            Draw_main();
            Textcolor(black, white);
        }
    }
}

//ù ���ӽ��۽� �����ϴ� �Լ�, �����ÿ� �������� ����
void Gamecheck(void)
{
    system("cls"); Sleep(3000);
    Play_bgm(Gamecheck_bgm, 1); Sleep(1500); //���ο� ���

    //���� �귿�� �����ϴ� ���� �����ϴ� ���༭, ���� ������ �°� ������
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
    gotoxy(67, 45); printf("�� �̸��� ������ ���� ��ŷ ��¿� ������ ���� �� �ֽ��ϴ�. ��");

    gotoxy(68, 42); printf("�̸��� �Է����ּ��� : "); fgets(Name, 20, stdin); //�̸� �Է¹���, ������ �����ϱ� ����
    Score = 0; Score_item = 0; Score_fire = 0;//���� �ʱ�ȭ
    
    Stop_bgm(); //��� ����
    Game(); //���ӽ���
}

//���� ����� ���� ���� �Լ�
void Check_double()
{
    Sleep(2000);
    system("cls");
    Sleep(2000);

    gotoxy(59, 28); printf("����    ����  ��     �� �����  ��       �����        ����  �����");
    gotoxy(59, 29); printf("��   ��  ��    �� ��     �� ��     �� ��       ��             ��    �� ��     ��");
    gotoxy(59, 30); printf("��    �� ��    �� ��     �� �����  ��       �����       ��    �� �����");
    gotoxy(59, 31); printf("��    �� ��    �� ��     �� ��     �� ��       ��             ��    �� ��     ��");
    gotoxy(59, 32); printf("��   ��  ��    �� ��     �� ��     �� ��       ��             ��    �� ��     ��");
    gotoxy(59, 33); printf("����    ����   ����   �����  ����� �����        ����  ��     ��");

    gotoxy(59, 35); printf("��      ��  ����  ������ ��      �� ���� ��     ��  �����    �����");
    gotoxy(59, 36); printf("���    �� ��    ��     ��     ��      ��   ��   ���   �� ��          ��      ��");
    gotoxy(59, 37); printf("�� ��   �� ��    ��     ��     ������   ��   �� ��  �� �� ����         ��");
    gotoxy(59, 38); printf("��  ��  �� ��    ��     ��     ��      ��   ��   ��  �� �� ��      ��      ��");
    gotoxy(59, 39); printf("��    ��� ��    ��     ��     ��      ��   ��   ��   ��� ��      ��");
    gotoxy(59, 40); printf("��      ��  ����      ��     ��      �� ���� ��     ��  �����       ��");

    Score = Score_item + Score_fire + Score_turn + Score_reload; //���� ����
    if (Score_double)
    {
        Score *= 2; //������ ���ͼ� �̰����� ���� �ι�
        Score_double = false;
    }
    gotoxy(94, 19); printf("Score : %d", Score);

    Sleep(2000);

    gotoxy(67, 4); printf("|----------|");
    gotoxy(67, 5); printf("| ��    �� |");
    gotoxy(67, 6); printf("|  ��  ��  |");
    gotoxy(67, 7); printf("|   ���   |");
    gotoxy(67, 8); printf("|    ��    |");
    gotoxy(67, 9); printf("|    ��    |");
    gotoxy(67, 10); printf("|----------|");

    gotoxy(121, 4); printf("|----------|");
    gotoxy(121, 5); printf("| ��    �� |");
    gotoxy(121, 6); printf("| ���  �� |");
    gotoxy(121, 7); printf("| �� �� �� |");
    gotoxy(121, 8); printf("| ��  ��� |");
    gotoxy(121, 9); printf("| ��    �� |");
    gotoxy(121, 10); printf("|----------|");

    while (1) //�ൿ ����
    {
        char input = _getch();
        if (input == 'y') //������ ����Ǿ� �ٷ� ���� ���� ����
        {
            Score_double = true;
            Game();
        }
        else if (input == 'n') //��������, �¸�ȭ������ �̵�
            Gamewin();
    }
}

//��ŷ ����
void Ranking_bubble()
{
    User user; //���� ���� �� �̸��� ������ ������ ����ü ����
    user.Userscore = Score; //�� ����ü�� ���� ��ǲ
    strcpy(user.Username, Name); //�̸� ��ǲ

    //��ŷ ����, ȹ���� ���� �Է�, �� �κи� GPT ���
    for (int i = 0; i < RANKING; i++)
    {
        if (user.Userscore > Ranking[i].Userscore ||
            (user.Userscore == Ranking[i].Userscore && strcmp(user.Username, Ranking[i].Username) < 0))
            //������ ���ٸ� ���ĺ� ������ ����
        {
            for (int j = RANKING - 1; j > i; j--)
            {
                Ranking[j] = Ranking[j - 1];
            }
            Ranking[i] = user;
            break;
        }
    }

    //��ŷ �ܺ� ���Ͽ� ����
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

//���� ��Ȳ �ܺ� ���Ͽ� ����
void Write_archive()
{
    FILE* Archive = fopen("Archive.txt", "w");
    for (int i = 0; i < ITEM; i++)  
        fprintf(Archive, "%d\n", Have_item[i]);
    fclose(Archive);
}

//���� ���۽� ����� ���� ��Ȳ�� �ҷ����� �Լ�
void Read_archive()
{
    FILE* Archive = fopen("Archive.txt", "r");
    if (Archive == NULL) return; //ó�� �����̾ ����� ������ ���ٸ� �����ϰ� ����
    int index = 0;
    while (fscanf(Archive, "%d", &Have_item[index]) != EOF) 
    { index++; }
    fclose(Archive);
}

//���� ���۽� ����� ��ŷ�� �ҷ����� �Լ�
void Read_ranking()
{
    FILE* Rank = fopen("Ranking.txt", "r");
    if (Rank == NULL) return; //ó�� �����̾ ����� ������ ���ٸ� �����ϰ� ����
    int rank = 0;
    while (fscanf(Rank, "%s %d", Ranking[rank].Username, &Ranking[rank].Userscore) != EOF)
    { rank++; }
    fclose(Rank);
}

//�Ҳɸ� Ÿ�������� ǥ���� ���� ���� �� �Լ�
void Draw_fire(bool off)
{
    gotoxy(72, 13); printf("                   )                      ");
    gotoxy(72, 14); printf("                  /(l                  /)");
    gotoxy(72, 15); printf("                 (  ��                / (   ");
    gotoxy(72, 16); printf("                 ) * )               ( , )");
    gotoxy(72, 17); printf("                 ��"); gotoxy(92, 17); printf("/                ��"); gotoxy(112, 17); printf("'  ");
    gotoxy(72, 19); printf("            ("); gotoxy(72, 20); printf("            )��"); gotoxy(96, 20); printf(",        /("); 
    gotoxy(115, 20); printf("/("); gotoxy(72, 21); printf("   (       /  )"); gotoxy(94, 21); printf(" (��      (��"); 
    gotoxy(114, 21); printf(") )"); gotoxy(72, 22); printf("   )��     (  ("); gotoxy(94, 22); printf(" ) )     ) , )"); 
    gotoxy(114, 22); printf("/ ("); gotoxy(72, 23); printf("  /  )     ) . )"); gotoxy(94, 23); printf("/  (     ( "); 
    gotoxy(106, 23); printf(" (     ( , )"); gotoxy(72, 24); printf(" ( * (     ��"); gotoxy(86, 24); printf("/|     (`"); 
    gotoxy(96, 24); printf(" )      `");  gotoxy(106, 24); printf("/"); gotoxy(114, 24); printf("`"); 
    gotoxy(116, 24); printf("/"); gotoxy(72, 25); printf(" ��"); gotoxy(76, 25); printf("/");

    if (off == 0)
    {
        gotoxy(125, 21); printf("("); gotoxy(125, 22); printf(")("); gotoxy(124, 23); printf("/  )");
        gotoxy(123, 24); printf("( * ("); gotoxy(123, 25); printf("��"); gotoxy(126, 25); printf("/");
    }
    else //�Ҳ��� ������ ���� ǥ���ϴ� �κ�
    {
        gotoxy(125, 21); printf(" "); gotoxy(125, 22); printf("  "); gotoxy(124, 23); printf("    ");
        gotoxy(123, 24); printf("     "); gotoxy(123, 25); printf("   "); gotoxy(126, 25); printf(" ");
    }
}

//���ӿ���
void Gameover(int Now_score)
{
    Play_bgm(Gameover_bgm, 1); //���ӿ���ȭ�� ��� ���
    system("cls");
    Textcolor(black, gray);
    gotoxy(72, 13); printf("                   )                      ");
    gotoxy(72, 14); printf("                  /(l                  /)");
    gotoxy(72, 15); printf("                 (  ��                / (   ");
    gotoxy(72, 16); printf("                 ) * )               ( , )");
    gotoxy(72, 17); printf("                 ��#/                ��#'  ");
    gotoxy(72, 18); printf("               .-'#'-.             .-'#'=, ");
    gotoxy(72, 19); printf("            (  |'-.='|             |'-,-'| ");
    gotoxy(72, 20); printf("            )��|     |  ,        /(|     | /(");
    gotoxy(72, 21); printf("   (       /  )|     | (��      (��|     |) )        (");
    gotoxy(72, 22); printf("   )��     (  (|     | ) )     ) , )     |/ (        )(");
    gotoxy(72, 23); printf("  /  )     ) . )     |/  (     ( # (     ( , )      /  )");
    gotoxy(72, 24); printf(" ( * (     ��#/|     (`# )      `#/|     |`#/      ( * (");
    gotoxy(72, 25); printf(" ��#/     .-'#'-.   .-'#'-,   .-'#'-.   .-=#'-;    ��#/");
    gotoxy(72, 26); printf(".-'#'-.   |'=,-'|   |'-.-'|   |'-.-'|   |'-.-'|   ,-'#'-.");
    gotoxy(72, 27); printf("|'-.-'|   |     |   |     |   |     |   |     |   |'-.-'|");
    gotoxy(72, 28); printf("|     |   |     |   |     |   |     |   |     |   |     |");
    gotoxy(72, 29); printf("|     |   |     |   |     |   |     |   |     |   |     |");
    gotoxy(72, 30); printf("|     |   |     |   |     |   |     |   |     |   |     |");
    gotoxy(72, 31); printf("'-._,-'   '-._,-'   '-._,-'   '-._,-'   '-._,-'   '-._,-'");
    Textcolor(black, white);
    
    Score = Score_item + Score_fire + Score_turn + Score_reload; //���� ����
    if (Score_double)
        Score = Now_score / 2; //���� ���н� ���� �ݰ�
    Score_double = false;

    gotoxy(95, 5); printf("Game  Over"); gotoxy(94, 40); printf("Score : %d", Score);

    clock_t time = clock(), offtime = clock(), texttime = clock(); //������ �ð� ���
    bool off = 0; //�Ҳ��� ������ Ÿ�̹��� ����ִ� ����

    while (1)
    {
        if (_kbhit() && clock() > texttime + 4000) //4�ʰ� ������ Ż�Ⱑ��
            break;
        if (time + 200 == clock()) //0.2�� �������� �Ҳ��� ��¦��
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
            time = clock(); //�ð� �ʱ�ȭ, ���� �ݺ�
            Textcolor(black, brightred);
            Draw_fire(off);
            Textcolor(black, white);
        }
        if (clock() >= offtime + 2000) //2���� �Ҳ��� ����
        {
            off = 1;
        }
        if (clock() >= texttime + 4000) //4���� ���� ���
        {
            gotoxy(82, 42); printf("����Ϸ��� �ƹ� Ű�� �����ʽÿ� . . .");
        }
    }

    Ranking_bubble(); //��ŷ ����
    Write_archive(); //������Ȳ ����
    Stop_bgm(); //��� ����
    Mainscreen(); //�������� ���ư�
}

//WIN �۾��� ��¦�̰� �ϱ� ���� ���� �� �Լ�
void Draw_win()
{
    gotoxy(82, 1); printf("��   ��   ��  ��  ��    ��  ��");
    gotoxy(82, 2); printf("��  ���  ��  ��  ���  ��  ��");
    gotoxy(82, 3); printf("��  ���  ��  ��  �� �� ��  ��");
    gotoxy(82, 4); printf(" ���  ���   ��  ��  ���    ");
    gotoxy(82, 5); printf("  ��    ��    ��  ��    ��  ��");
}

//���� �¸�, ���� 'N' ���� ��
void Gamewin()
{
    system("cls");
    Play_bgm(Gamewin_bgm, 1); //���ӽ¸� ��� ���, ���� ������ �°� �¸��ص� �ᱹ ��븦 �����ϰ� ���� ���ư��� ���̹Ƿ� ������ ��� ���
    Draw_image_512(Game_win, hdc, 1000, 200); //�������� �̹��� ���

    //�۾��� ������ ���������� ���
    char P1[] = "������\r����\r:\r";
    char P2[] = "�Ѿ�\r����\r:\r";
    char P3[] = "��\r����\r:\r";
    char P4[] = "����\r����\r:\r";
    char P5[] = "��\rȹ����\r����\r:\r";
    
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

    clock_t time = clock(); //�ð� ���

    while (1)
    {
        if (_kbhit()) break; //�ƹ� Ű�� ������ Ż��
        if (time + 200 == clock()) //0.2�� �������� ��¦��
        {
            Textcolor(black, gray); //�۾��� ��¦�̰� �ϴ� �κ�
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
    Ranking_bubble(); //��ŷ ����
    Write_archive(); //���� ��Ȳ ����
    system("cls"); Sleep(2000);
    Stop_bgm(); //��� ����
    Mainscreen(); //�������� ���ư�
}

//��ŷȭ��
void Rankingscreen(void)
{
    system("cls");
    Draw_square();
    gotoxy(97, 6); printf("��  ŷ"); gotoxy(76, 12); printf("��  ��");
    gotoxy(97, 12); printf("��  ��"); gotoxy(118, 12); printf("��  ��");
    gotoxy(186, 48); printf("�ڷΰ��� : Q");

    for (int i = 0; i < RANKING; i++) //��ŷ ���
    {
        gotoxy(78, 15 + 3 * i); printf("%d", i + 1); //����
        if (Ranking[i].Userscore > 0) //������ �־�߸� ���
        {
            gotoxy(97, 15 + 3 * i); printf("%s", Ranking[i].Username);
            gotoxy(118, 15 + 3 * i); printf("%d", Ranking[i].Userscore);
        }
    }

    while (1) //�ൿ ����
    {
        char input = _getch();
        if (input == 'q' || input == 'Q') // Q�� ������ ����ȭ������ �ڷΰ���
        {
            Mainscreen(); break;
        }
    }
}

//���� ȭ��
void Archive()
{
    system("cls");
    int n = 0; //���� ������ �ε�����, �ʱ갪�� 0�� �������� ����
    gotoxy(186, 48); printf("�ڷΰ��� : Q");
    Draw_square();

    gotoxy(63, 7); for (int i = 0; i < 37; i++) { printf("��"); }
    for (int i = 0; i < 28; i++) { gotoxy(63, 8 + i); printf("��"); }
    for (int i = 0; i < 28; i++) { gotoxy(135, 8 + i); printf("��"); }
    gotoxy(63, 35); for (int i = 0; i < 37; i++) { printf("��"); }

    Archive_select(n); //�ε��� ���� ���� ������ ������ ���

    while (1) //�ൿ ����
    {
        char key = _getch();
        if (key == LEFT && n > 0) //���� ����Ű�� ������ ���� ������
            n--;
        if (key == RIGHT && n < 8) //������ ����Ű�� ������ ���� ������
            n++;
        if (key == 'q') //q�� ������ Ż���Ͽ� �������� ���ư�
            break;
        Archive_select(n); //� ȭ���� �������� �����̸� ���
    }
    Mainscreen(); //q�� ������ while������ ������ �������� ���ư�
}

//���� ȭ�鿡�� �������� ������ �����ִ� �Լ�
void Archive_select(int n)
{
    gotoxy(63, 5); if (n > 0) printf("�� P R E V"); else printf("          "); //���� ù �������̸� �ڷ� �� �� ����
    gotoxy(127, 5); if (n < 8) printf("N E X T ��"); else printf("          "); //������ �������̸� ������ �� �� ����
    gotoxy(80, 42); for (int i = 43; i < 90; i++) { printf(" "); } //����� �ʱ�ȭ
    gotoxy(76, 43); for (int i = 43; i < 90; i++) { printf(" "); }
    gotoxy(76, 44); for (int i = 43; i < 90; i++) { printf(" "); }
    for (int k = 0; k < 27; k++) { for (int i = 0; i < 70; i++) { gotoxy(65 + i, 8 + k); printf(" "); } }
    gotoxy(86, 39); printf("No. %d", n + 1); gotoxy(98, 39); printf("<       ?      >"); //�̸��� �ʱ�ȭ

    gotoxy(99, 21); printf("?"); //������ ������ ���� ��, �ر��� �ȵǾ������� ����ǥ�� ��µǴ� ����

    if (n == 0 && Have_item[0] == 1) //����, �ѹ��̶� ȹ���� ���� ������ ���
    {
        gotoxy(104, 39); printf("����");
        gotoxy(82, 42); printf("���� ��ǿ� ������ �Ѿ��� �����մϴ�");
        Draw_image_256(Beer, hdc, 680, 250); //�̹��� ���
    }
    else if (n == 1 && Have_item[1] == 1) //����
    {
        gotoxy(104, 39); printf("����");
        gotoxy(87, 42); printf("����� ���� 1�� �����մϴ�");
        Draw_image_256(Handcuff, hdc, 680, 250);
    }
    else if (n == 2 && Have_item[2] == 1) //���
    {
        gotoxy(104, 39); printf("���");
        gotoxy(89, 42); printf("�������� 1 ȸ���մϴ�");
        gotoxy(82, 43); printf("�ִ�ü���� �Ѿ� ȸ���� �� �����ϴ�");
        Draw_image_256(Cigarette, hdc, 680, 250);
    }
    else if (n == 3 && Have_item[3] == 1) //��
    {
        gotoxy(105, 39); printf("��");
        gotoxy(83, 42); printf("���� �Ѿ��� ���ظ� 2��ŭ ���մϴ�");
        Draw_image_256(Saw, hdc, 680, 250);
    }
    else if (n == 4 && Have_item[4] == 1) //�Ƶ巹����
    {
        gotoxy(101, 39); printf("�Ƶ巹����");
        gotoxy(84, 42); printf("����� �������� ���� ����մϴ�");
        Draw_image_256(Adrenaline, hdc, 680, 250);
    }
    else if (n == 5 && Have_item[5] == 1) //���� ��
    {
        gotoxy(102, 39); printf("����  ��");
        gotoxy(86, 42); printf("50%% Ȯ���� �������� 1 �Ұų�");
        gotoxy(84, 43); printf("50%% Ȯ���� �������� 2 ȸ���մϴ�");
        gotoxy(83, 44); printf("�ִ�ü���� �Ѿ� ȸ���� �� �ֽ��ϴ�");
        Draw_image_256(Medicine, hdc, 680, 250);
    }
    else if (n == 6 && Have_item[6] == 1) //������
    {
        gotoxy(103, 39); printf("������");
        gotoxy(84, 42); printf("��ǿ� ������ �Ѿ��� Ȯ���մϴ�");
        Draw_image_256(Magnifier, hdc, 680, 250);
    }
    else if (n == 7 && Have_item[7] == 1) //������
    {
        gotoxy(103, 39); printf("������");
        gotoxy(87, 42); printf("���� ���� ź���� ��������");
        gotoxy(76, 43); printf("1��° ź�� ������ ������ ź�� ������ �˷��ݴϴ�");
        Draw_image_256(Phone, hdc, 680, 250);
    }
    else if (n == 8 && Have_item[8] == 1) //�ι���
    {
        gotoxy(103, 39); printf("�ι���");
        gotoxy(80, 42); printf("���� ��ǿ� ������ ź�� �ؼ��� �ٲߴϴ�");
        Draw_image_256(Inverter, hdc, 680, 250);
    }
} 

//o������ ����o//

//������ ��� Ű �ȳ�, ������ PC�� �ʻ�ȭ ���
void Item_draw(int round)
{
    gotoxy(57, 0); printf("����(Q) ����(W) ���(E) ��(R) �Ƶ巹����(T) ���� ��(Y) ������(U) ������(I) �ι���(O)");
    Draw_image_128(User_image, hdc, 290, 540); //���� �ʻ�ȭ ���
    switch (round) //���忡 ���� PC�� �ʻ�ȭ�� �޶���
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

//������ ���̺��� �̹��� ��ǥ�� ã���ִ� �Լ�
void Find_table_position(int table)
{
    switch (table) //���̺� ��ȣ�� �޾ƿ�
    {
    case 0: x = 120; y = 400; break; //0�� ���̺�
    case 1: x = 290; y = 400; break;
    case 2: x = 470; y = 400; break;
    case 3: x = 120; y = 540; break;
    case 4: x = 470; y = 540; break;
    case 5: x = 120; y = 685; break;
    case 6: x = 290; y = 685; break;
    case 7: x = 470; y = 685; break; //7�� ���̺�
    }
    if (Turn == 0) { x += 900; } //PC���̸� ��ǥ �̵�

    //x, y�� ���������̹Ƿ� ���� ��ȯ������ ����
}

//���̺� ��ȣ�� ������ ��ȣ�� �޾Ƽ�, ���̺� �������� �׸��� �Լ�
void Draw_eachitem(int table, int item)
{
    switch (item)
    {
    case 0: //����
        Draw_image_128(Beer, hdc, x, y); //���� �̹����� ����ϰ� �� ���̺��� �ּڰ��� B (Beer) �� ����, �� ���̺� ���ְ� �ִٰ� �˸��� �ǹ�
        if (Turn) User_Table[table] = 'B'; else Com_Table[table] = 'B'; break;
    case 1: //����
        Draw_image_128(Handcuff, hdc, x, y); //���� ����
        if (Turn) User_Table[table] = 'H'; else Com_Table[table] = 'H'; break;
    case 2: //���
        Draw_image_128(Cigarette, hdc, x, y);
        if (Turn) User_Table[table] = 'C'; else Com_Table[table] = 'C'; break;
    case 3: //��
        Draw_image_128(Saw, hdc, x, y);
        if (Turn) User_Table[table] = 'S'; else Com_Table[table] = 'S'; break;
    case 4: //�Ƶ巹����
        Draw_image_128(Adrenaline, hdc, x, y);
        if (Turn) User_Table[table] = 'A'; else Com_Table[table] = 'A'; break;
    case 5: //��
        Draw_image_128(Medicine, hdc, x, y);
        if (Turn) User_Table[table] = 'M'; else Com_Table[table] = 'M'; break;
    case 6: //������
        Draw_image_128(Magnifier, hdc, x, y);
        if (Turn) User_Table[table] = 'm'; else Com_Table[table] = 'm'; break;
    case 7: //������
        Draw_image_128(Phone, hdc, x, y);
        if (Turn) User_Table[table] = 'P'; else Com_Table[table] = 'P'; break;
    case 8: //�ι���
        Draw_image_128(Inverter, hdc, x, y);
        if (Turn) User_Table[table] = 'I'; else Com_Table[table] = 'I'; break;
    }
}

//������ ��ȣ�� �޾Ƽ� �ּڰ��� ã�� �̹����� ����� �Լ�
void Find_erase_table(int item)
{
    int istable = 0; //ã�Ƴ� ���̺� ��ȣ�� ������ ����
    if (Adrenaline_on == true && Adrenaline_used == true) //�Ƶ巹�������� ���� �� �������� ���췯 �Դٸ�
    {
        Turn = !Turn; //����� �������� ����� ���� �� �Լ� �ȿ����� �� ����
    }

    if (Turn == true) //���� ���̸�
    {
        switch (item)
        {
        case 0: //����
            for (int table = 0; table < MAX_ITEM; table++) //8���� ���̺� Ž��
            {
                if (User_Table[table] == 'B') //�ּڰ��� B ��
                {
                    istable = table; //��ȣ ����, switch�� Ż��
                    break;
                }
            } break;
        case 1: //����, ������ ���� �����ԡ�
            for (int table = 0; table < MAX_ITEM; table++) {
                if (User_Table[table] == 'H') { istable = table; break; }
            } break;
        case 2: //���
            for (int table = 0; table < MAX_ITEM; table++) {
                if (User_Table[table] == 'C') { istable = table; break; }
            } break;
        case 3: //��
            for (int table = 0; table < MAX_ITEM; table++) {
                if (User_Table[table] == 'S') { istable = table; break; }
            } break;
        case 4: //�Ƶ巹����
            for (int table = 0; table < MAX_ITEM; table++) {
                if (User_Table[table] == 'A') { istable = table; break; }
            } break;
        case 5: //���� ��
            for (int table = 0; table < MAX_ITEM; table++) {
                if (User_Table[table] == 'M') { istable = table; break; }
            } break;
        case 6: //������
            for (int table = 0; table < MAX_ITEM; table++) {
                if (User_Table[table] == 'm') { istable = table; break; }
            } break;
        case 7: //������
            for (int table = 0; table < MAX_ITEM; table++) {
                if (User_Table[table] == 'P') { istable = table; break; }
            } break;
        case 8: //�ι���
            for (int table = 0; table < MAX_ITEM; table++) {
                if (User_Table[table] == 'I') { istable = table; break; }
            } break;
        }
    }
    else //PC ���̸�
    {
        switch (item)
        {
        case 0: //����
            for (int table = 0; table < MAX_ITEM; table++) //8���� ���̺� Ž��
            {
                if (Com_Table[table] == 'B') //�ּڰ��� B ��
                {
                    istable = table; //��ȣ ����, switch�� Ż��
                    break;
                }
            } break;
        case 1: //����, ������ ���� �����ԡ�
            for (int table = 0; table < MAX_ITEM; table++) {
                if (Com_Table[table] == 'H') { istable = table; break; }
            } break;
        case 2: //���
            for (int table = 0; table < MAX_ITEM; table++) {
                if (Com_Table[table] == 'C') { istable = table; break; }
            } break;
        case 3: //��
            for (int table = 0; table < MAX_ITEM; table++) {
                if (Com_Table[table] == 'S') { istable = table; break; }
            } break;
        case 4: //�Ƶ巹����
            for (int table = 0; table < MAX_ITEM; table++) {
                if (Com_Table[table] == 'A') { istable = table; break; }
            } break;
        case 5: //���� ��
            for (int table = 0; table < MAX_ITEM; table++) {
                if (Com_Table[table] == 'M') { istable = table; break; }
            } break;
        case 6: //������
            for (int table = 0; table < MAX_ITEM; table++) {
                if (Com_Table[table] == 'm') { istable = table; break; }
            } break;
        case 7: //������
            for (int table = 0; table < MAX_ITEM; table++) {
                if (Com_Table[table] == 'P') { istable = table; break; }
            } break;
        case 8: //�ι���
            for (int table = 0; table < MAX_ITEM; table++) {
                if (Com_Table[table] == 'I') { istable = table; break; }
            } break;
        }
    }

    Find_table_position(istable); //���̺� ��ǥ �ް�
    Draw_image_128(None, hdc, x, y); //������ �����
    if (Turn == true) //���� ���̸�
    {
        User_item_table[istable] = 0; //������ �� ���̺� ����
        User_Table[istable] = '1'; //�ּڰ� �ʱ�ȭ
    }
    else //PC���̸�
    {
        Com_item_table[istable] = 0; //PC�� �� ���̺� ����
        Com_Table[istable] = '1'; //�ּڰ� �ʱ�ȭ
    }

    if (Adrenaline_on == true && Adrenaline_used == false) //�Ƶ巹������ ����� ������� �Ƶ巹������ ����� �Դٸ�
    {
        Adrenaline_used = true; //�Ƶ巹�������� �������� ���� �� ����       
    }
    else if (Adrenaline_on == true && Adrenaline_used == true)  //�Ƶ巹�������� ��� �������� �����ٸ�
    {
        Turn = !Turn; Adrenaline_used = false; //�� ���� ����
    }
}

//�������� ���� ������ �����ϴ� �Լ�
//������ �Ƶ巹������ ��⼺�� �ùķ��̼��� ���� ����� �����Ǿ��� ������, �����۵��� �����ϴ� ������ Ȱ�뵵�� ���� ������
int Item_weight(int weight)
{
    int item = 9; //���̰�
    if (weight > 94) item = 4;      //���� 5,  �Ƶ巹����
    else if (weight > 89) item = 1; //���� 5,  ����
    else if (weight > 78) item = 3; //���� 11, ��
    else if (weight > 67) item = 6; //���� 11, ������
    else if (weight > 55) item = 2; //���� 12, ���
    else if (weight > 43) item = 5; //���� 12, ��
    else if (weight > 31) item = 8; //���� 12, �ι���
    else if (weight > 15) item = 0; //���� 16, ����
    else item = 7;                  //���� 16, ������, ��ü���� 100
    return item;
}

//���� ������ ����
void Item_give()
{
    bool turn = Turn; //���� ������ ������ ����, �������� �����ϴ��Ŀ� ���� ���� ������Ű�� ����
    srand((unsigned int)time(NULL));
    int Howmanyitem = rand() % 3 + 2; //�ִ� ������ ���� 2~4

    Turn = true; //�������� ����, ������ �̹����� �׸��� ����

    for (int i = 0; i < Howmanyitem; i++) //���� ������ ����, for�� �ȿ����� �õ尡 �����̹Ƿ� PC�� �и���
    {
        int weight = rand() % 100; //��ü ���� 100
        int item = Item_weight(weight); //���߿� ���� 9�� ������ �� �ϳ� ���� ����
        for (int table = 0; table < MAX_ITEM; table++) //0�� ���̺���� ����ִ� ���̺��� ã���� ��
        {
            if (User_item_table[table] == 0) //�� ���̺��� ����־�߸� ������
            {
                Find_table_position(table); //ã�Ƴ� ����ִ� ���̺��� ��ǥ�� �޾Ƽ�
                Draw_eachitem(table, item); //�����ϰ� ������ �������� �� ���̺� �ְ�
                User_item[item]++; User_item_table[table] = 1; //������ ���� ��, ���̺��� ä�����ٰ� �˸�
                Have_item[item] = 1; //������ ȹ��� ���� �ر�
                Score_item += 5; //������ ȹ��� 5��
                break;
            }
        }
    } //���������� �����ϰ� ������ 2~4���� �������� ���޵ǰ�, ���̺��� ��� ä�����ִٸ� �� �̻� ȹ�� �Ұ�
      //���̺��� �ڸ��� 8���̹Ƿ� ������ �ִ� ���� ������ 8���� ��

    Turn = false; //PC���� ������ ����

    for (int i = 0; i < Howmanyitem; i++) //���� ���� ����
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
    Turn = turn; //������ ������ �������Ƿ� ������ ���� �־� �� �ǵ�������
}

//�Է��� ���ĺ��� �׿� �´� �ε��� ��ȣ�� �ٲ��ִ� �Լ�, Who_use() �� Item_ardreanline() ���� ���
int Trans_index(char input)
{
    int index = 9; //�̻��� ���� ���� ���, ���̰�
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

//�������� ����Ʈ�� ����ϴ� �Լ�, �Ϻ� �������� �۵������ �޶� ������� ����
void Item_effect(int item, const char* itemName)
{
    Find_erase_table(item); //�Է¹��� �������� �ִ� ���̺��� �̹����� ����
    int xpos = 290; if (!Turn) xpos += 900; //PC�� ���� ��� ���̺��� �ּڰ��� �ٸ�
    Draw_image_128(itemName, hdc, xpos, 240); //������ ����Ʈ
    Sleep(3000); Draw_image_128(None, hdc, xpos, 240); //����Ʈ �����
}

//������-����0
void Item_beer()
{
    Play_bgm(Beer_sound, 0); //���� ȿ����
    Find_erase_table(0); //���ְ� �ִ� ���̺��� ã�� �� ���̺��� �̹����� ����
    int xpos = 290; if (!Turn) xpos += 900; //PC�� ���� ���̺� ��ǥ���� �ٸ��� ����
    Draw_image_128(Beer, hdc, xpos, 240); //���� ����Ʈ

    //��ź �Ǵ� ����ź�� ���� ������ Now_bullet�� ������Ŵ
    while (Now_bullet < MAX_BULLET && Bullets[Now_bullet] == 2) Now_bullet++; //2 �� 2 ��

    if (Now_bullet < MAX_BULLET) //� ź�� ã�Ҵ��� Ȯ��, �� ź ������
    {
        if (Inverter_on) //�ι��͸� �� ���¶�� �������Ѿ� ��
        {
            if (Bullets[Now_bullet] == 1) //��ź�̶�� ���� ź�� ����ź��
            { 
                Fake_bullet--;
                Now_bullet++;
                Draw_image_128(Real_bullet_image, hdc, 745, 500); //���� ź Ȯ��
                Sleep(3000); Draw_image_128(None, hdc, 745, 500);
            }
            else if (Bullets[Now_bullet] == 0) //����ź�̶�� ���� ź�� ��ź��
            { 
                Real_bullet--;
                Now_bullet++;
                Draw_image_128(Fake_bullet_image, hdc, 745, 500); //���� ź Ȯ��
                Sleep(3000); Draw_image_128(None, hdc, 745, 500);
            }
        }
        else //�ι��͸� �Ƚ�ٸ�
        {
            if (Bullets[Now_bullet] == 1) //��ź�̶��
            { 
                Real_bullet--;
                Now_bullet++;
                Draw_image_128(Real_bullet_image, hdc, 745, 500); //���� ź Ȯ��
                Sleep(3000); Draw_image_128(None, hdc, 745, 500);
            }
            else if (Bullets[Now_bullet] == 0) //����ź�̶��
            { 
                Fake_bullet--;
                Now_bullet++;
                Draw_image_128(Fake_bullet_image, hdc, 745, 500); //���� ź Ȯ��
                Sleep(3000); Draw_image_128(None, hdc, 745, 500);
            }
        }
    }
    if (Turn) Score_item += 10; //������ ��� ���� ���� 10�� �߰�

    Inverter_on = false; //ź�� ���������Ƿ� �ι��� ��� ����

    Draw_image_128(None, hdc, xpos, 240); //���� ź �̹��� ����
}

//������-����1
void Item_handcuff()
{
    Play_bgm(Handcuff_sound, 0); //���� ȿ����, ���� ���� ����
    if (Turn)  Score_item += 15; //������ ��� ���� ���� 15�� �߰�

    Handcuff_on = true; //������ ���Ǿ����� �˸�

    Item_effect(1, Handcuff); //���� ����Ʈ
    Draw_handcuff(); //���� �̹��� ä��
}

//������ ä���� �ִٸ� ���̺� ���̿� �������� ������ �ɷ��ִ��� ǥ��
void Draw_handcuff()
{
    if (Handcuff_on && !Turn) Draw_image_128(Handcuff, hdc, 640, 700);
    else if (Handcuff_on && Turn) Draw_image_128(Handcuff, hdc, 850, 700);
}

//������-���2
void Item_cigarette()
{
    Play_bgm(Cigarette_sound, 0);
    if (Turn == true)
    {
        User_hp++; //������ ���� ȸ��, �Ҹ�
        Score_item -= 5; //��� ���� ü���� ȸ�������� ������ ����
    }
    else Com_hp++; //PC�� ���� PC ȸ��

    Item_effect(2, Cigarette); //��� ����Ʈ
}

//������-��3
void Item_saw()
{
    Play_bgm(Saw_sound, 0);
    if (Turn) Score_item += 10;

    Saw_on = true; //���� ���Ǿ����� �˸�

    Item_effect(3, Saw); //�� ����Ʈ
}

//PC�� ���� ����� �� �ִ��� �Ǵ� �� ����ϴ� �Լ�
void Com_saw()
{
    if (Com_item[3] > 0) //���� �������̶��
    {
        Who_useitem('r'); //�� ���
        Sleep(2000);
    }
    else if (Com_item[4] > 0 && User_item[3] > 0) //�Ƶ巹������ �ְ� ������ ���� �������̶��
    {
        Com_adrenaline(3); //�� ����
        Sleep(2000);
    }
}

//������-�Ƶ巹����4, ������ ����Ҷ�
//PC�� ��� �������� �����ϴ� ������ �������ִ� ���·� �����⿡ ����� �޶� ������
void Item_adrenaline()
{
    Play_bgm(Adrenaline_sound, 0);
    int index = 9; //������ � �������� ������ �� �ε��� ��, �ʱⰪ�� ���̰��� 9
    Score_item += 15;

    Adrenaline_on = true; //�Ƶ巹������ ��������� �˸�

    //���� ������ ��� ��� �� ����
    gotoxy(81, 2); printf("���Ѱ� ���� PC�� �������� �����ϼ���");

    Find_erase_table(4);
    Draw_image_128(Adrenaline, hdc, 290, 240); //�Ƶ巹���� ����Ʈ

    while (1)
    {
        char item_choice = _getch();
        index = Trans_index(item_choice); //���ڸ� ������ �ε����� ��ȯ

        if (index >= 0 && index < ITEM && Com_item[index] > 0)
        {
            if (index == 1 && Handcuff_on == true) {} //�̹� ���� ������̶�� ������� ����
            else if (index == 2 && User_hp >= Max_hp) {} //Ǯ�Ǹ� ��� ������� ����
            else if (index == 3 && Saw_on == true) {} //�̹� �� ������̶�� ������� ����
            else if (index == 7 && Real_bullet + Fake_bullet == 1) {} //�Ѿ��� �ѹ߳��Ҵٸ� ������ ������� ����
            else if (index == 4) {} //�Ƶ巹�������� �Ƶ巹������ ���� �� ����
            else if (index != 9)//������ ����� ������ ���
            {
                Com_item[index]--;
                Item_use(index);
                break; //Ż��
            }
        }
    }

    gotoxy(81, 2); for (int i = 0; i < 36; i++) printf(" "); //������ ��� ���� �����
    Adrenaline_on = false; //�Ƶ巹���� ����� �������� �˸�
}
//PC�� �Ƶ巹���� ���
void Com_adrenaline(int item) 
{
    Play_bgm(Adrenaline_sound, 0);

    Find_erase_table(4); //PC ���̺��� �Ƶ巹������ ����
    Draw_image_128(Adrenaline, hdc, 1190, 240); //�Ƶ巹���� ����Ʈ
    Com_item[4]--; //�Ƶ巹���� ���� ����
    Adrenaline_on = true; //�Ƶ巹���� ������� �˸�
    Sleep(3500);

    //PC�� ��� ���þ��� � �������� ������� �����������Ƿ�
    //��ٷ� �Ƶ巹�������� �������� ���� ���ʶ�� �˷������μ� ���� ���̺��� �������� ���쵵�� �����ϴ� ����
    Adrenaline_used = true; 

    Item_use(item); User_item[item]--; //���� ������ ���� ���
    Adrenaline_on = false; //�Ƶ巹���� ��� ����
}

//������-���� ��5
void Item_medicine()
{
    Play_bgm(Medicine_sound, 0);
    srand((unsigned int)time(NULL));
    int x = rand() % 2; //�� ��뿡 ������ ����, 0�Ǵ� 1 ����
    if (x == 0) //����
    {
        if (Turn == true)
        {
            User_hp--; //ü�� 1 ����
            Score_item -= 10; //�� ���� ���н� ���� ����
        }
        else if (Turn == false) Com_hp--; //ü�� 1 ����
    }
    else if (x == 1) //����
    {
        if (Turn == true)
        {
            User_hp += 2; //ü�� 2ȸ��
            Score_item += 10; //�� ���� ������ ���� 10��
        }
        else if (Turn == false) Com_hp += 2; //ü�� 2ȸ��
    }

    Item_effect(5, Medicine); //���� �� ����Ʈ
}

//������-������6, �����⵵ ������ PC�� ����� �и���
//PC�� ��� ���ٷ� ������ �����ϱ� ������ �� �Լ��� ���İ� �ʿ䰡 ������ ������
bool Item_magnifier()
{
    Play_bgm(Magnifier_sound, 0);
    //��ź �Ǵ� ����ź�� ���� ������ Now_bullet�� ������Ŵ
    while (Now_bullet < MAX_BULLET && Bullets[Now_bullet] == 2) Now_bullet++;

    if (Turn) //���� ���϶��� ǥ��
    {
        gotoxy(100, 1);
        if (Now_bullet < MAX_BULLET) //�ε��� ��� ����� ����
        {
            if (Bullets[Now_bullet] == 1) 
            { 
                Draw_image_128(Real_bullet_image, hdc, 745, 500); //��ź Ȯ��
            }
            else if (Bullets[Now_bullet] == 0) 
            { 
                Draw_image_128(Fake_bullet_image, hdc, 745, 500); //����ź Ȯ��
            }
        }
        Score += 5;
    }
    bool bullet = 0; //PC�� ����� ���� ź�� ������ ���� ����
    if (Now_bullet < MAX_BULLET) bullet = Bullets[Now_bullet]; //�ε��� ��� ����� ����

    Item_effect(6, Magnifier); //������ ����Ʈ
    Draw_image_128(None, hdc, 745, 500); //�Ѿ� �̹��� ����

    return bullet; //PC�� ��� ���� ź�� ������ �޾� �����
}

//������-������7
void Item_phone()
{
    gotoxy(81, 2); for (int i = 0; i < 30; i++) printf(" "); //������ ��� ���� �����
    Play_bgm(Phone_sound, 0);
    srand((unsigned int)time(NULL));
    if (Turn) Score_item += 5;

    //��ź �Ǵ� ����ź�� ���� ������ Now_bullet�� ������Ŵ
    while (Now_bullet < MAX_BULLET && Bullets[Now_bullet] == 2) Now_bullet++;

    //���������� �ҷ��� �ε���, ���� �Ѿ�, ���������� �ҷ��� �ε����� ������ ���Ŀ� ���� �Ѿ�
    int Phone_index, Rest_bullet = 0, Rest_phone_bullet = 0;

    //�� �� *�� 2 �� 2 /�� ��            ��4 ��2

    do { Phone_index = rand() % MAX_BULLET; } //������ �ε����� 0~7 ���� ����
    while (Phone_index <= Now_bullet || Bullets[Phone_index] == 2);
    //�̹� ������ �Ѿ˰� ������ �Ѿ��� �˷��ָ� �ȵǹǷ� ���� �Ѿ� �ε������� �۰ų� ������ ���� �ٽ� ����
    //�Ѿ��� 2���� ���� �ٽ� ����

    //���� ���� �Ѿ� ���� ���
    for (int i = Now_bullet; i < MAX_BULLET; i++) { if (Bullets[i] != 2) Rest_bullet++; }

    //������ ������ ������ �ε����� ������ ���Ŀ� ���� �Ѿ� ���� ���
    for (int i = Phone_index; i < MAX_BULLET; i++) { if (Bullets[i] != 2) Rest_phone_bullet++; }

    //�׷��� ������ �ε����� ���� �Ѿ� �� �� ��° �Ѿ����� ���
    int Item_phone_index = Rest_bullet - Rest_phone_bullet + 1;

    if (Turn) //�������϶��� ǥ��
    {
        gotoxy(91, 2);
        if (Bullets[Phone_index] == 1) printf("%d ��° ź : ��ź", Item_phone_index);
        else if (Bullets[Phone_index] == 0) printf("%d ��° ź : ����ź", Item_phone_index);
    }

    Com_phone_index = Phone_index; Com_phone_bullet = Bullets[Phone_index]; //PC�� �� ���� ����

    Item_effect(7, Phone);
    gotoxy(81, 2); for (int i = 0; i < 30; i++) printf(" "); //������ ��� ���� �����
}

//������-�ι���8
void Item_inverter()
{
    Play_bgm(Inverter_sound, 0);
    //��ź �Ǵ� ����ź�� ���� ������ Now_bullet�� ������Ŵ
    while (Now_bullet < MAX_BULLET && Bullets[Now_bullet] == 2) Now_bullet++;

    if (Now_bullet < MAX_BULLET) //�ε��� ���� ��� ���ֱ� ���� if���� ����
    {
        if (Bullets[Now_bullet] == 1) //��ź�̸�
        {
            Bullets[Now_bullet] = 0; //����ź���� �ٲ�
        }
        else if (Bullets[Now_bullet] == 0) //����ź�̸�
        {
            Bullets[Now_bullet] = 1; //��ź���� �ٲ�       
        }
    }

    if (Turn == true) Score_item += 15;

    Inverter_on = true; //�ι��� ����� �˸�

    Item_effect(8, Inverter); //�ι��� ����Ʈ
}

//PC�� �ι��͸� ����� �� �ִ��� �Ǵ� �� ����ϴ� �Լ�
void Com_inverter()
{
    if (Com_item[8] > 0) //�ι��͸� �������̶��
    {
        Who_useitem('o'); //�ι��� ���
        Sleep(2000);
    }
    else if (Com_item[4] > 0 && User_item[8] > 0) //�Ƶ巹������ �ְ� ������ �ι��͸� �������̶��
    {
        Com_adrenaline(3); //�ι��� ����
        Sleep(2000);
    }
}

//������ ��� �Լ�, ������ pc ��� �������� ����ϱ⿡ �ι� ���� �ڵ尡 ����� ���� ����
void Item_use(int index)
{
    int Com_itemsum = 0; for (int i = 0; i < ITEM; i++) { Com_itemsum += Com_item[i]; } //PC �������� �� ����, �Ƶ巹�������� ���

    //�� �������� ȿ���� ����
    switch (index)
    {
    case 0: //����
        Item_beer(); break;

    case 1: //����
        if (Handcuff_on == false) Item_handcuff(); //������� �ƴҶ���
        else 
        {
            if (Turn == true) User_item[1]++; //������϶��� ������ �پ����� ����
        } break;

    case 2: //���
        if (Turn == true) 
        { 
            if (User_hp >= Max_hp) User_item[2]++; //���� �ִ�ü���� �Ѿ�ٸ� ������� �����Ƿ� ���� ����
            else Item_cigarette(); 
        } 
        else if (Turn == false) Item_cigarette(); break;

    case 3: //��
        if (Saw_on == false) { Item_saw(); } //������� �ƴҶ���
        else User_item[3]++; break; //PC�� ���� �� �� ���� ��찡 ������ �����߱⿡ PC�� ���� ����� �ʿ䰡 ����

    case 4: //�Ƶ巹����
        if (Com_itemsum > 0) Item_adrenaline(); //��� �������� �־�߸� ���
        else User_item[4]++; break; //������ ������� �ʰ� ���� ��ȯ
        //PC�� ��� �ٸ� ������� �Ƶ巹������ ���⿡ ����� �ʿ䰡 ����

    case 5: //���� ��, ���� ���� �ִ�ü���� �Ѿ� ȸ���� �����ϱ⿡ ������ ����
        Item_medicine(); break;

    case 6: //������
        Item_magnifier(); break;

    case 7: //������
        if (Real_bullet + Fake_bullet == 1) //���� �Ѿ��� �� ���̶��
            User_item[7]++; //������� �ʰ� ���� ��ȯ, ���������� PC�� ���� ����� �ʿ䰡 ����
        else Item_phone(); break;

    case 8: //�ι���
        Item_inverter(); break;
    }

    Draw_hp(); //ü���� �����Ǿ����Ƿ� �ٽ� ���
}

//���� �������� ������ �Ǵ��ϰ� ������ ����� �ҷ����� �Լ�
void Who_useitem(char c) {
    int index = 9;

    //Ű �Է� ���� ���� �ε��� ��ȣ ����
    index = Trans_index(c);
    if (Turn == true) //���� ���̸� ������ ���
    {
        if (User_item[index] > 0) //����Ϸ��� �������� ������ 0���� ū�� Ȯ��
        {
            User_item[index]--; //������ ���� ����
            Item_use(index);
        }
    }
    else if (Turn == false) //��ǻ�� ���̸� ��ǻ�Ͱ� ���
    {
        if (Com_item[index] > 0)
        {
            Com_item[index]--;
            Item_use(index);
        }
    }
}

//o���� ���� ����o//

//���� ü���� �׸��� �Լ�
void Draw_hp()
{
    gotoxy(10, 20); for (int i = 0; i < User_hp; i++) { printf("��"); } printf("    ");
    int i = 0;
    for (i = 0; i < Com_hp; i++) { gotoxy(188 - 2 * i, 20); printf("��"); }
    for (i = i; i < 5; i++) { gotoxy(188 - 2 * i, 20); printf("  "); }
    gotoxy(11, 18); printf("User"); gotoxy(186, 18); printf("Com");
}

//�Ѿ��� ������ �������� �����ϰ� ��ź�� ����ź�� ������ �������� ����
void Random_reload()
{
    Sleep(2000);
    srand((unsigned int)time(NULL));
    Score_reload += 20; //�������� ���� ȹ��

    Real_bullet = rand() % 4 + 1; //��ź 1~4�� ����
    int Real_lower = Real_bullet - 1, Real_upper = Real_bullet + 1; //��ź ��1�� ������ ����ź ������ Ȱ��
    do { Fake_bullet = rand() % 4 + 1; } //����ź 1~4�� ����
    while (!((Fake_bullet >= Real_lower) && (Fake_bullet <= Real_upper) && Fake_bullet <= 4));
    //����ź�� ������ ��ź��1�� �ǵ���, �׷��鼭 �ִ� ������ 4���� �Ѿ�� �ȵ�
    //��ź�� ����ź�� ���� ���̰� ���� ���� ���̵��� �������Ƿ� ������

    //�����ϱ� �� ź�� ������ �˷���
    int real = Real_bullet; //��ź ������ŭ
    for (int i = 0; i < real; i++) 
        Draw_image_64(Real_bullet_image, hdc, 100 + i * 70, 100); //��ź �׸���
    for (real; real < Real_bullet + Fake_bullet; real++) 
        Draw_image_64(Fake_bullet_image, hdc, 100 + real * 70, 100); //������ ����ź �׸���
    Sleep(5000);

    for (int i = 0; i < Real_bullet + Fake_bullet; i++) //���� �����
        Draw_image_64(None, hdc, 100 + i * 70, 100);

    for (int i = 0; i < Real_bullet + Fake_bullet; i++) //ź ���� ��ŭ
    {
        Play_bgm(Reload_bullet, 0); Sleep(500); //�Ѿ� �ִ� �Ҹ�
    }
    Play_bgm(Reload_sound, 0); //���� �Ҹ�

    for (int i = 0; i < MAX_BULLET; i++) { Bullets[i] = 2; } //���� �迭 �ʱ�ȭ

    int count = 0;
    for (int i = 0; i < Real_bullet; i++) //��ź ������ŭ �迭�� 1 �߰�
    {
        Bullets[i] = 1; count++;
    }
    for (int i = 0; i < Fake_bullet; i++) //����ź ������ŭ �迭�� 0�߰�
    {
        Bullets[i + count] = 0;
    }

    for (int i = 7; i > 0; i--) //GPT�� ���� �迭 �������� ����
    {
        // 0���� i���� �߿��� �������� �ε��� ����
        int j = rand() % (i + 1);

        // arr[i]�� arr[j]�� ���� ���� �ٲ�
        int temp = Bullets[i];
        Bullets[i] = Bullets[j];
        Bullets[j] = temp;
    }
}

//�ι��� ���ÿ� �Ѿ� ���� ���Ҹ� �Ǵ��ϴ� �Լ�
void Check_inverter(int i)
{
    if (Bullets[i] == 1) //��ź�̸�
    {
        if (Inverter_on) Fake_bullet--; //�ι��͸� ��������� ���� ź�� ����ź��
        else Real_bullet--; //�ȹٲ���ٸ� �״�� ��ź ���� ����
    }
    else if (Bullets[i] == 0) //����ź�̸�
    {
        if (Inverter_on) Real_bullet--; //�ι��͸� ��������� ���� ź�� ��ź��
        else Fake_bullet--; //�ȹٲ���ٸ� �״�� ����ź ���� ����
    }
    if (Bullets[i] != 2) Inverter_on = false; //���̵������� 2�� �ƴϸ� �ι��� ��Ȱ��ȭ
}

//�Ѿ��� ��� � �Ѿ��� ������ Ȯ���ϴ� ����Ʈ
void Bullet_effect(const char* Sound, const char* Bullet)
{
    Play_bgm(Sound, 0); Draw_image_128(Bullet, hdc, 745, 500); //�� �Ѿ� �̹���
    Sleep(3000); Draw_image_128(None, hdc, 745, 500); //��� ��� �� �̹��� �����
    Draw_image_256(None, hdc, 675, 200); //���� �̹����� ����
}

//��뿡�� �Ѿ� �߻��ϴ� �Լ�
int Fire_toYou(int i)
{
    gotoxy(100, 1); printf("                    ");
    if (Bullets[i] == 1) //���� ���� �Ѿ��� ��ź�̸�
    {
        if (Turn == 1)
        {
            Com_hp--; //���� ���̸� ��ǻ�� ü�� ����
            Score_fire += 50; //��뿡�� ��ź�� �������Ƿ� ����ȹ��
            Draw_image_256(Buckshot_right, hdc, 675, 200); //��븦 ���� ���� �ܴ��� �̹���
            if (Saw_on == true)
            {
                Com_hp--; Score_fire += 50; //�� Ȱ��ȭ�� �� �߰� ü�°��ҿ� ���� ȹ��
            }
        }
        else if (Turn == 0)
        {
            User_hp--; //PC���̸� ���� ü�� ����
            Draw_image_256(Buckshot_left, hdc, 675, 200); //��븦 ���� ���� �ܴ��� �̹���
            if (Saw_on == true) User_hp--; //�� Ȱ��ȭ�� �� �߰� ü�°���
        }

        Bullet_effect(Fire_real, Real_bullet_image); //���� ź Ȯ�� ����Ʈ
        Draw_hp(); //ü���� �����Ǿ����Ƿ� �ٽ� ���
    }
    else if (Bullets[i] == 0) //����ź�̸�
    {
        if (Turn == 1) { Draw_image_256(Buckshot_right, hdc, 675, 200); } //��븦 ���� ���� �ܴ��� �̹���
        else { Draw_image_256(Buckshot_left, hdc, 675, 200); } //��븦 ���� ���� �ܴ��� �̹���

        Bullet_effect(Fire_fake, Fake_bullet_image); //���� ź Ȯ�� ����Ʈ
    }
    if (Bullets[i] != 2) Saw_on = false; //���� �߻��ߴٸ� �� ��� ���� ����

    Check_inverter(i); //�ι��� ������ ���� �Ѿ� ���� ���Ұ� �޶���

    if (Handcuff_on && Bullets[i] != 2) //������ ��ٸ� ���� �Ѿ�� ����
    {
        Handcuff_on = false;
        Draw_image_128(None, hdc, 640, 700); //���� �̹��� ����
        Draw_image_128(None, hdc, 850, 700);
    }
    else if (Handcuff_on == false && Bullets[i] != 2) Turn = !Turn; //�Ѿ��� �߻������Ƿ� ��뿡�� �� �Ѿ

    if (Bullets[i] == 2) return Fire_toYou(i + 1); //���� ��ź�� ����ź�� �ƴϸ� ź�� ���ö����� �� �Լ� �ݺ�
    else return i + 1; //���������� �߻�Ǿ����Ƿ� i+1�� ��ȯ
}

//������ �Ѿ� �߻��ϴ� �Լ�
int Fire_toMe(int i)
{
    gotoxy(100, 1); printf("                    ");
    if (Bullets[i] == 1) //���� ���� �Ѿ��� ��ź�̸�
    {
        if (Turn == 1)
        {
            Draw_image_256(Buckshot_left, hdc, 675, 200); //�ڽ��� ���� ���� �ܴ��� �̹���
            User_hp--; //���� ���̸� ���� ü�� ����
            Score_fire -= 30;
            if (Saw_on == true)
            {
                User_hp--; //�� Ȱ��ȭ�� �� �߰� ü�°���
                Score_fire -= 30; //�ڽſ��� ���� �����Ƿ� ���� ����
            }
        }
        else if (Turn == 0)
        {
            Draw_image_256(Buckshot_right, hdc, 675, 200); //�ڽ��� ���� ���� �ܴ��� �̹���
            Com_hp--; //��ǻ�� ���̸� ��ǻ�� ü�� ����
            if (Saw_on == true) Com_hp--; //�� Ȱ��ȭ�� �� �߰� ü�°���
        }
        Draw_hp(); //ü���� �����Ǿ����Ƿ� �ٽ� ���

        Bullet_effect(Fire_real, Real_bullet_image); //���� ź Ȯ�� ����Ʈ

        if (Handcuff_on) //������ ��ٸ� ���� �Ѿ�� ����
        {
            Handcuff_on = false; 
            Draw_image_128(None, hdc, 640, 700); //���� �̹��� ����
            Draw_image_128(None, hdc, 850, 700);
        }
        else if (Handcuff_on == false && Bullets[i] != 2) Turn = !Turn; //�Ѿ��� �߻������Ƿ� ��뿡�� �� �Ѿ
    }
    if (Bullets[i] == 0) //����ź�̸�
    {
        if (Turn == 1)
        {
            Score_fire += 30; //������ ����ź�� �����Ƿ� ����ȹ��
            Draw_image_256(Buckshot_left, hdc, 675, 200); //�ڽ��� ���� ���� �ܴ��� �̹���
        }
        else Draw_image_256(Buckshot_right, hdc, 675, 200); //�ڽ��� ���� ���� �ܴ��� �̹���

        Bullet_effect(Fire_fake, Fake_bullet_image); //���� ź Ȯ�� ����Ʈ
    }
    if (Bullets[i] != 2) Saw_on = false; //�Ѿ��� ����ߴٸ� �� ��� ���� ����

    Check_inverter(i); //�ι��� ������ ���� �Ѿ� ���� ���Ұ� �޶���

    if (Bullets[i] == 2) return Fire_toMe(i + 1); //���� ��ź�� ����ź�� �ƴϸ� ź�� ���ö����� �� �Լ� �ݺ�
    else return i + 1; //���������� �߻�Ǿ����Ƿ� i+1�� ��ȯ
}

//������ �����ϴ� �� �Լ�
void User_turn()
{
    gotoxy(26, 8); printf("��뿡�� �߻� : 1   ������ �߻� : 2"); //Ű �Է� �ȳ�
    Score_turn += 1; //���� 1�� ȹ��
    while (1)
    {
        check_bullet = Now_bullet; //���� �� ��° �Ѿ����� �˷��ִ� ����

        char input = _getch(); //Ű �Է¹ޱ�
        if (input == '1') //��뿡�� ���
        {
            Now_bullet = Fire_toYou(Now_bullet); //��뿡�� �Ѿ� �߻� �� ���� �Ѿ˷�
        }
        else if (input == '2') //������ ���
        {
            Now_bullet = Fire_toMe(Now_bullet); //������ �Ѿ� �߻� �� ���� �Ѿ˷�
        }
        else Who_useitem(input); //������ ��� �Լ� ȣ��

        if (User_hp <= 0 || Com_hp <= 0) break; //������ ü���� 0�̸� ���� Ȥ�� ���� �����Ϸ� Ż��

        if (check_bullet != Now_bullet) { break; } //�Ѿ��� ��� ���� �Ѿ˷� �̵��Ͽ� �ݺ��� Ż��
    }
    gotoxy(26, 8); printf("                                   "); //Ű �Է� �ȳ��� �����
}

//PC�� ��Ȳ�� ���� ���������� �ൿ�ϴ� �Լ�
//PC�� �Ƶ巹�������� ���� ������ ��� : ����, ���, ��, �ι���
//�̿��� �ٸ� �������� ���� ���� �뷱���� ���� ������
void Com_turn()
{
    bool What_bullet; //������ ���� �޾ƿ� ���� ź�� ����
    srand((unsigned int)time(NULL));
    gotoxy(153, 8); printf("PC ��"); Sleep(1000); //PC�� ������ �˸��� �ȳ���

    // 2 < PC ü�� < �ִ�ü��-1, ���Ѿ��� ����� �� �ִٸ� �켱 ��� �� �� ����
    //���� �뷱���� ���� ��� ȯ���� ������
    if (Com_hp > 2 && Com_hp < Max_hp - 1 && Com_item[5]>0)
        Who_useitem('y');

    //��踦 ����� �� �ִٸ� �켱 ��� �� �� ����
    //ü���� ��Ұ� ��谡 �ִٸ� or ü���� ��Ұ� �Ƶ巹�������� ��踦 ���� �� �ִٸ� ��� ��� ���
    while (Com_hp < Max_hp && ( Com_item[2] > 0 || (Com_item[4] > 0 && User_item[2] > 0) ) )
    {
        if (Com_item[2] > 0) //��踦 �������̶��, �Ƶ巹�������ٵ� PC ������ ��踦 �켱 ���
        {
            Who_useitem('e'); //��� ���
            Sleep(2000);
        }
        else if (Com_item[4] > 0 && User_item[2] > 0) //�Ƶ巹������ �ְ� ������ ��踦 �������̶��
            Com_adrenaline(2);
    }

    //������ ����� �� �ִٸ� �켱 ��� �� �� ����
    if (Handcuff_on == false)
    {
        if (Com_item[1] > 0) //������ �������̶��
            Who_useitem('w'); //���� ���
        else if (Com_item[4] > 0 && User_item[1] > 0) //�Ƶ巹������ �ְ� ������ ������ �������̶��
            Com_adrenaline(1);
    }

    //��븦 ������ �� �ִ� �켱������ ���� �ൿ 
    if (Real_bullet != 0 && Fake_bullet == 0) //��ź�� ���� ��Ȳ, ������ ��ź�̹Ƿ� �Ѿ� �߻�
    {
        Com_saw(); //���� ����� �� �ִ��� �Ǵ� �� ���
        Now_bullet = Fire_toYou(Now_bullet); //��뿡�� �߻�
        Draw_hp(); Sleep(3000);
    }
    else if (Real_bullet == 0 && Fake_bullet != 0) //����ź�� ���� ��Ȳ, ������ ����ź�̹Ƿ� �ڽſ��� �߻�
    {
        //� ������ε� �ι��͸� ����� �� �ִٸ�
        if (Com_item[8] > 0 || (Com_item[4] > 0 && User_item[8] > 0))
        {
            Com_inverter(); //�� ������� �ι��� ���
            Com_saw(); //���� ����� �� �ִ��� �Ǵ� �� ���
            Now_bullet = Fire_toYou(Now_bullet); //��뿡�� �߻�
            Draw_hp(); Sleep(3000);
        }
        else //�ι��Ͱ� ���ٸ�, ����ź�̹Ƿ�
        {
            Now_bullet = Fire_toMe(Now_bullet); //�ڽſ��� �߻�
            Sleep(3000);
        }
    }
    else if (Now_bullet == Com_phone_index) //������ ��� ��, ���� �ε����� ���������� �� �ε����� ���
    {
        if (Com_phone_bullet == 1) //��ź�̶��
        {
            Com_saw(); //���� ����� �� �ִ��� �Ǵ� �� ���
            Now_bullet = Fire_toYou(Now_bullet); //��뿡�� �߻�
            Sleep(3000);
        }
        //����ź�϶�, � ������ε� �ι��͸� ����� �� �ִٸ�
        else if (Com_item[8] > 0 || (Com_item[4] > 0 && User_item[8] > 0))
        {
            Com_inverter(); //�� ������� �ι��� ���
            Com_saw(); //���� ����� �� �ִ��� �Ǵ� �� ���
            Now_bullet = Fire_toYou(Now_bullet); //��뿡�� �߻�
            Draw_hp(); Sleep(3000);
        }
        else //�ι��Ͱ� ���ٸ�, ����ź�̹Ƿ�
        {
            Now_bullet = Fire_toMe(Now_bullet); //�ڽſ��� �߻�
            Sleep(3000);
        }
    }
    else if (Com_item[6] > 0) //�����⸦ �������̶��
    {   
        //��ź �Ǵ� ����ź�� ���� ������ Now_bullet�� ������Ų��
        while (Now_bullet < MAX_BULLET && Bullets[Now_bullet] == 2) Now_bullet++;

        //������ ����� ���� ź�� �������� �޾ƿ�
        What_bullet = Item_magnifier(); Com_item[6]--;
        Sleep(3500);

        if (What_bullet == 1) //��ź�� �ôٸ�
        {
            Com_saw(); //���� ����� �� �ִ��� �Ǵ� �� ���
            Now_bullet = Fire_toYou(Now_bullet); //��뿡�� �߻�
            Sleep(3000);
        }
        //����ź�� �ôٸ�, � ������ε� �ι��͸� ����� �� �ִٸ�
        else if (Com_item[8] > 0 || (Com_item[4] > 0 && User_item[8] > 0))
        {
            Com_inverter(); //�� ������� �ι��� ���
            Com_saw(); //���� ����� �� �ִ��� �Ǵ� �� ���
            Now_bullet = Fire_toYou(Now_bullet); //��뿡�� �߻�
            Draw_hp(); Sleep(3000);
        }
        else //�ι��Ͱ� ���ٸ�, ����ź�̹Ƿ�
        {
            Now_bullet = Fire_toMe(Now_bullet); //�ڽſ��� �߻�
            Sleep(3000);
        }
    }
    else if (Com_item[0] > 0) //���ָ� �������̶��
    {
        Who_useitem('q'); //���� ���
        Sleep(2000);
    }
    else if (Com_item[7] > 0 && Real_bullet + Fake_bullet > 1) //�������� �������̰�, ���� �Ѿ��� �� ���� �ƴ϶��
    {
        Who_useitem('i'); //������ ���
        Sleep(2000);
    }
    else //�������� �ƿ� ���� ��, Ȥ�� ����Ҹ��� �������� ���� ��
    {
        if (Real_bullet > Fake_bullet) //��ź�� �� ������ �������� �߻�
        {
            Now_bullet = Fire_toYou(Now_bullet); Sleep(3000);
        }
        else if (Fake_bullet > Real_bullet) //����ź�� �� ������ �ڽſ��� �߻�
        {
            Now_bullet = Fire_toMe(Now_bullet); Sleep(3000);
        }
        else //��ź�� ����ź�� ������ �����ϴٸ�
        {
            int x = rand() % 2; //0, 1 ���� �������� ����
            if (x == 0) //�������� �߻�
            {
                Now_bullet = Fire_toYou(Now_bullet); Sleep(3000);
            }
            else if (x == 1) //�ڽſ��� �߻�
            {
                Now_bullet = Fire_toMe(Now_bullet); Sleep(3000);
            }
        }
    }
    gotoxy(153, 8); printf("     ");
    Sleep(1000);
}

//���� ���� �Լ�
void Game_setting(int Round)
{
    system("cls"); gotoxy(190, 0); printf("%d ����", Round + 1); //���� ���, ���带 �˸�
    srand((unsigned int)time(NULL));
    Max_hp = rand() % 4 + 2; //ü���� 2~5 ����, �뷱���� ���� �ּҸ� 3���� ������ ���� ����

    //���� ������ �ʱ�ȭ
    //PC�� ������ ü���� �ʱ�ȭ, ���� �Ѿ� �ε����� �ʱ�ȭ, ������ �ʱ�ȭ
    Com_hp = Max_hp; User_hp = Max_hp; Now_bullet = 0; Handcuff_on = false;

    for (int i = 0; i < ITEM; i++) { User_item[i] = 0; Com_item[i] = 0; } //���� ���۽� ������ ���� �ʱ�ȭ
    for (int i = 0; i < MAX_ITEM; i++) //���� ���۽� ���̺� �ʱ�ȭ
    {
        User_Table[i] = '1', Com_Table[i] = '1', User_item_table[i] = 0, Com_item_table[i] = 0;
    } 

    Turn = true; //���� ������ ����
    Draw_table(); //���̺� ǥ��
}

/*���� ����-------------------------------------------------------------------------------���� ����*/
void Game()
{
    int Now_score = Score; //���� ������ ��� ������ ���̹Ƿ� ���� ������ ���� ����
    for (int Round = 0; Round < 3; Round++) //3���� ���� ����
    {
        Game_setting(Round); //���� ����
        Draw_image_128(User_image, hdc, 290, 540); //���� �ʻ�ȭ ���

        switch (Round) //���忡 ���� PC�� �ʻ�ȭ�� �޶���
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

        while (1) //���� ���� ����Ǵ� ����
        {
            Random_reload(); //�Ѿ� ���� & ����
            Com_phone_index = 9; Com_phone_bullet = 0; //�������� ���� �� ������ ���� �ʱ�ȭ

            //ȭ���� ����Ƿ� �ٽ� �׷��� �ϴ� �͵�
            Draw_handcuff(); //������ ä���� �ִٸ� ǥ��
            Draw_hp(); //ü�� ǥ��
            Item_give(); //�����ϸ� ������ ����
            Item_draw(Round); //������ �ȳ�, ������ �� �ʻ�ȭ

            Now_bullet = 0; //���� �Ѿ� �ε��� �ʱ�ȭ

            while (1) //źâ �ϳ� ���� ����Ǵ� ����
            {
                while (Turn == true) //Turn == ture => ���� ���̸� ���� �� �ߵ�
                {
                    Draw_handcuff(); //ȭ���� �������Ƿ� ������ ä���� �ִٸ� ǥ��
                    User_turn(); //���� �� �ߵ�
                    if (Real_bullet == 0 && Fake_bullet == 0) break; //��� �Ѿ��� �Ҹ��ϸ� �������Ϸ� Ż��
                    if (User_hp <= 0 || Com_hp <= 0) break; //������ ü���� 0�� �Ǹ� ������ ������ Ż��
                }

                if (Real_bullet == 0 && Fake_bullet == 0) break; //���� ���� ����
                if (User_hp <= 0 || Com_hp <= 0) break; 

                while (Turn == false)
                {
                    Draw_handcuff(); //ȭ���� �������Ƿ� ������ ä���� �ִٸ� ǥ��
                    Com_turn(); //�� �� �ߵ�
                    if (Real_bullet == 0 && Fake_bullet == 0) break;
                    if (User_hp <= 0 || Com_hp <= 0) break;
                }

                if (Real_bullet == 0 && Fake_bullet == 0) break;
                if (User_hp <= 0 || Com_hp <= 0) break;
            }
            if (User_hp <= 0 || Com_hp <= 0) break; //������ �׾��ٸ� ���� ����
        }

        if (User_hp <= 0) break; //�� �¸��� ���� ����
        else if (Com_hp <= 0) //���� �¸���
        {
            Score_turn += 100 * (Round + 1); //���庰 �¸��� 100, 200, 300��
            Sleep(2000); //��� ��� �� ���� ����� �Ѿ
        }
    }
    if (Com_hp <= 0) Check_double(); //���� �¸��� ���� ���
    else if (User_hp <= 0) Gameover(Now_score); //�� �¸��� �й�, ������ ���� ȹ���� �����Ƿ� ó�� ������ ������ ��ȯ
}

//o���� �Լ�o//

int main(void)
{
    Invisible_Cursor(0); //Ŀ�� �����Ÿ� ����
    SetConsoleView(); //�ܼ�â ũ��� ���� ����
    Global_make(); //�̹��� ����� ���� �������� ����
    Read_ranking(); //�ܺ� ������ ��ŷ �о��
    Read_archive(); //�ܺ� ������ ���� ��Ȳ �о��

    Warning_screen(); //���� ���� �� ��� ȭ��

    Mainscreen(); //���� ȭ��, ���⼭ ������ �Լ��� �̾����� ������ ������ ����

    ReleaseDC(hwnd, hdc); //�̹��� ����� ������ �Լ�, �� ������ ��������� �ణ�� ��� �߻�

    return 0;
}