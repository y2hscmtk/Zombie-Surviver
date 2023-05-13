#include <GL/glut.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <vector>
using namespace std;

//캐릭터는 30초안에 좀비를 처치해야한다.
//좀비는 100의 체력을 갖고 있으며 총알을 한대 맞을때마다 10씩 체력이 감소한다.
//캐릭터는 스페이스바를 눌러서 총알을 날릴수 있다.
//캐릭터는 w,a,s,d키를 눌러서 캐릭터의 위치를 바꿀수 있다.

//캐릭터는 몸,몸통,어깨,위팔,팔꿈치,아래팔,손(구),다리로 구성되어 있고,
//팔은 각 부위별로 다르게 움직인다.

//좀비는 몸,몸통,팔,손,다리 로 구성되어 있고
//좀비는 손을 앞으로 쭉 뻗고 흔들흔들 거리면서 좌우로 이동한다.


int MX = 300, MY = 300;
float t = 0;
float t2 = 0;

float x = 0, y = 0; // 캐릭터의 위치 전역변수

int HEALTH = 100; //좀비체력
int playTime = 0; //게임 플레이 타임

//게임 클리어 여부 확인
bool isClear = false;  // => 게임 클리어시 true로 변경=>시간 기록x
bool lose = false; //게임을 클리어하지못했을경우 => 키 기능 잠금

class vec3 {
public:
    float m[3];
    vec3() {} //기본 생성자
    vec3(float x, float y, float z) {
        m[0] = x; m[1] = y; m[2] = z;
    }

    vec3 operator-(vec3 x) {
        vec3 ret(m[0] - x.m[0],
            m[1] - x.m[1],
            m[2] - x.m[2]);
        return ret;
    }

    float Length() {
        return sqrtf(m[0] * m[0] + m[1] * m[1] + m[2] * m[2]);
    }
};

//총알 객체 
class Bullet {
public:
    //속도와 위치
    vec3 v;
    vec3 pos;
};

vector<Bullet> bullets; //총알의 위치 좌표를 기록할 벡터

bool HitCheck() { // 총알과 Zombie가 충돌하면 true리턴
    vec3 zombie(sin(t * 0.0001) * 10, 1.0, -7); //좀비의 목 위치
    int size = bullets.size();
    for (int i = 0; i < size; i++) {
        vec3 diff = zombie - bullets[i].pos;
        //각각의 거리 차이가 1.5이하라면 명중이라고 판단
        if (diff.Length() < 1.5)
            return true;
    }
    return false;
}


//총알 위치 업데이트 => 벡터 안의 총알 좌표 변경
void UpdateBullets() {
    int size = bullets.size();
    for (int i = 0; i < size; i++) {
        //현재 위치 += 속도*시간
        for (int j = 0; j < 3; j++)
            bullets[i].pos.m[j] += bullets[i].v.m[j] * 0.5;
    }
}

//날아가는 총알을 그리기 위한 함수
void DrawBullets() {
    UpdateBullets(); // 총알 좌표 update
    int size = bullets.size(); //벡터 안의 총알 개수만큼
    for (int i = 0; i < size; i++) {
        glPushMatrix();
        glColor3f(0, 0, 0);//총알 색 검은색
        glTranslatef(bullets[i].pos.m[0], bullets[i].pos.m[1], bullets[i].pos.m[2]);
        glutSolidSphere(0.1, 20, 20);
        glPopMatrix();
    }
}


//스페이스바가 눌리면 총알을 하나 날림
//w,a,s,d가 눌리면 캐릭터의 위치를 조절함
void MyKeyboard(unsigned char key, int X, int Y) {
    if (lose == false) { //패배하지 않았을 경우에만 키 조작 가능
        switch (key) {
        case 'w': // w 키를 눌렀을 경우
            y -= 0.5f;
            break;
        case 's': // s 키를 눌렀을 경우
            y += 0.5f;
            break;
        case 'a': // a 키를 눌렀을 경우
            x -= 0.5f;
            break;
        case 'd': // d 키를 눌렀을 경우
            x += 0.5f;
            break;
        case ' ':
            Bullet bullet;
            //x좌표와 z좌표는 캐릭터의 위치를 따르고 y좌표는 캐릭터가 생성될 위치
            bullet.pos.m[0] = x + 0.5; // 현재 캐릭터의 좌표
            bullet.pos.m[1] = 2.1;
            bullet.pos.m[2] = y + 7;
            bullet.v.m[0] = 0;
            bullet.v.m[1] = 0;
            bullet.v.m[2] = -1.5;
            bullets.push_back(bullet); //생성한 총알 객체의 좌표 => 벡터에 삽입
            break;
        }
        glutPostRedisplay();
    } 
}



//마우스 값 반영
void MyMouseMove(GLint X, GLint Y) {
    //마우스의 위치로 MX와 MY를 받아와서
    MX = X;
    MY = Y;

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    //관찰하는 시점 변경
    gluLookAt(MX / 300.0, MY / 300.0, 1.0,
        0.0, 0.0, 0.0,
        0.0, 1.0, 0.0);  //시점변환
    glutPostRedisplay(); //다시화면에 그리기
}


GLUquadricObj* cyl;
//캐릭터 그리기
void DrawCharacter() {
    //원기둥을 그리기 위해
    cyl = gluNewQuadric();  
    gluQuadricDrawStyle(cyl, GLU_LINE);
    
    glColor3f(0, 0, 1); //파란색

    //몸통 그리기
    glPushMatrix();
        //머리
        glPushMatrix();
            glTranslatef(0, 9, 0); //바닥에서 6만큼 위로 이동
            glutSolidSphere(1.8, 20, 20); //구 생성(머리)
        glPopMatrix();
        
        //몸통
        glTranslatef(0, 2, 0); // 몸통의 위치 => 바닥보다 높아야함
        glRotatef(-90, 1, 0, 0); // -90
        gluCylinder(cyl, 1.5, 2, 5, 80, 50); // 원기둥 생성
        //glutSolidCube(5);

        //오른팔 그리기
        glPushMatrix();
            glTranslatef(2, 0.9, 4.5); 
            glutWireSphere(0.6, 20, 20); //관절부 생성
            glRotatef(-90, 1, 0, 0); // -90 => 앞으로 피고 있음

            //glRotatef(sin(t2 * 0.0005) * 60 + 180, 1, 0, 0); //좌우로 흔들리게
            gluCylinder(cyl, 0.6, 0.6, 2.2, 20, 10); //팔뚝 길이 2.2
            glPushMatrix();
                glTranslatef(0.0, 0.0, 2.2); //팔뚝끝에 관절 생성
                glutWireSphere(0.6, 20, 20); //관절
                //glRotatef(sin(t2 * 0.0005) * 20 + 10, 1, 0, 0);
                gluCylinder(cyl, 0.6, 0.6, 1.5, 20, 10); //팔 길이2

                //둥그런 손
                glPushMatrix();
                    glTranslatef(0.0, 0.0, 1.5); //팔끝에 손 생성
                    glutWireSphere(0.7, 20, 20); //손(둥그런)

                    //총 손잡이
                    glPushMatrix();
                        glRotatef(-30, 1, 0, 0);
                        glTranslatef(0.0, -0.5, 0.0); //손위로 이동 
                        glScalef(0.1, 0.5, 0.1); //큐브 얇게 변경 
                        glutSolidCube(5);
              

                    glPopMatrix();
                    //총 구
                    glPushMatrix();
                        glRotatef(-90, 1, 0, 0);
                        glTranslatef(0.0, -2, -1.5); //손위로 이동 

                        glScalef(0.1, 0.5, 0.1); //큐브 얇게 변경 
                        glutSolidCube(5);
                    glPopMatrix();
                glPopMatrix();

            glPopMatrix();

        glPopMatrix();

        //왼팔 그리기
        glPushMatrix();
            glTranslatef(-2, 0.9, 4.5);
            glutWireSphere(0.6, 20, 20); //관절부 생성
            //glRotatef(-90, 1, 0, 0); // -90 => 앞으로 피고 있음

            glRotatef(-sin(t2 * 0.0005) * 60 + 180, 1, 0, 0); //좌우로 흔들리게
            gluCylinder(cyl, 0.6, 0.6, 2.2, 20, 10); //팔뚝 길이 2.2
            glPushMatrix();
                glTranslatef(0.0, 0.0, 2.2); //팔뚝끝에 관절 생성
                glutWireSphere(0.6, 20, 20); //관절
                glRotatef(-sin(t2 * 0.0005) * 20 + 10, 1, 0, 0);
                gluCylinder(cyl, 0.6, 0.6, 2, 20, 10); //팔 길이2

                    //둥그런 손
                    glPushMatrix();
                        glTranslatef(0.0, 0.0, 2); //팔끝에 손 생성
                        glutWireSphere(0.7, 20, 20); //손(둥그런)

                    glPopMatrix();

            glPopMatrix();

        glPopMatrix();

        //오른쪽 다리 그리기
        glPushMatrix();
            glTranslatef(0.8, 0, -0.1);
            glutWireSphere(0.6, 20, 20); //관절부 생성
            glRotatef(-sin(t2 * 0.0005) * 60 + 180, 1, 0, 0); //좌우로 흔들리게
            gluCylinder(cyl, 0.8, 0.8, 4, 20, 10); //다리 길이 4

        glPopMatrix();

        //왼쪽 다리 그리기
        glPushMatrix();
            glTranslatef(-0.8, 0, -0.1);
            glutWireSphere(0.6, 20, 20); //관절부 생성
            glRotatef(sin(t2 * 0.0005) * 60 + 180, 1, 0, 0); //좌우로 흔들리게
            gluCylinder(cyl, 0.8, 0.8, 4, 20, 10);

        glPopMatrix();


    glPopMatrix();
}

//좀비 그리기
void DrawZombie() {
    //원기둥을 그리기 위해
    cyl = gluNewQuadric();
    gluQuadricDrawStyle(cyl, GLU_LINE);

    glColor3f(0, 0.6, 0); //초록색

    //몸통 그리기
    glPushMatrix();
        //머리
        glPushMatrix();
            glTranslatef(0, 9, 0); //바닥에서 6만큼 위로 이동
            glutSolidSphere(1.8, 20, 20); //구 생성(머리)
        glPopMatrix();

        //몸통
        glTranslatef(0, 2, 0); // 몸통의 위치 => 바닥보다 높아야함
        glRotatef(-90, 1, 0, 0); // 세로로 세우기
        gluCylinder(cyl, 1.5, 2, 5, 80, 50); // 원기둥 생성
        //glutSolidCube(5);

        //왼팔 그리기(좀비 기준)
        glPushMatrix();
            glTranslatef(2, 0.9, 4.5);
            glutWireSphere(0.6, 20, 20); //관절부 생성
            glRotatef(90, 1, 0, 0); // 90 => 앞으로 피고 있음
            glRotatef(sin(t * 0.0005) * 20 + 10, 1, 0, 0); //흔들 흔들 거리게
            gluCylinder(cyl, 0.8, 0.8, 4.5, 20, 10); //팔뚝 길이 4.5
            glPushMatrix();
                glTranslatef(0.0, 0.0, 4.5); //팔뚝끝에 관절 생성
                glutWireSphere(0.8, 20, 20); //둥그런 손
            glPopMatrix();
        glPopMatrix();

        //오른팔 그리기(좀비 기준)
        glPushMatrix();
            glTranslatef(-2, 0.9, 4.5);
            glutWireSphere(0.6, 20, 20); //관절부 생성
            glRotatef(90, 1, 0, 0); // 90 => 앞으로 피고 있음
            glRotatef(-sin(t * 0.0005) * 20 + 10, 1, 0, 0); //흔들 흔들 거리게
            gluCylinder(cyl, 0.8, 0.8, 4.5, 20, 10); //팔뚝 길이 4.5
            glPushMatrix();
                glTranslatef(0.0, 0.0, 4.5); //팔뚝끝에 관절 생성
                glutWireSphere(0.8, 20, 20); //둥그런 손
            glPopMatrix();
        glPopMatrix();

       

        //오른쪽 다리 그리기
        glPushMatrix();
            glTranslatef(0.8, 0, -0.1);
            glutWireSphere(0.6, 20, 20); //관절부 생성
            glRotatef(-sin(t * 0.0005) * 60 + 180, 1, 0, 0); //좌우로 흔들리게
            gluCylinder(cyl, 0.8, 0.8, 4, 20, 10); //다리 길이 4

        glPopMatrix();

        //왼쪽 다리 그리기
        glPushMatrix();
            glTranslatef(-0.8, 0, -0.1);
            glutWireSphere(0.6, 20, 20); //관절부 생성
            glRotatef(sin(t * 0.0005) * 60 + 180, 1, 0, 0); //좌우로 흔들리게
            gluCylinder(cyl, 0.8, 0.8, 4, 20, 10);

        glPopMatrix();


    glPopMatrix();
}

//10ms후에 MyTimer를 호출함
void MyTimer(int Value) {
    //캐릭터가 아직 생존해있는 동안에만 움직임
    //not lose일때만
    if(lose==false)
        t2 += 60; //캐릭터는 총알과 관계없이 움직여야함
    if (lose == false&&isClear==false)
        t += 60;
    glutPostRedisplay(); //새로고침 => 전역변수가 변경되었으므로 다시 그림을 그려줌
    glutTimerFunc(10, MyTimer, 1); //MyTimer 호출
    
}

//1초마다 playTime 1씩 추가
void plusPlayTime(int Value) {
    //게임이 클리어 되지 않은 경우에만 계속 시간 누적
    if (lose == false&&isClear==false) {
        if (isClear == false && playTime < 30) { //30초 이상은 기록 x
            playTime += 1;
            glutTimerFunc(1000, plusPlayTime, 1); //시간 누적함수 호출
            printf("playTime : %d\n", playTime);
        }
        if (playTime >= 30) {
            lose = true; //패배처리
            printf("You Lose!\n");
        }
    }
}




void MyDisplay() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    glColor3f(1, 1, 1); //하양
    glPushMatrix(); //캐릭터에게 반영되지 않도록
        glScalef(20, 0.5, 20); //x-z방향 넓은 판
        glutSolidCube(1.0);
    glPopMatrix();


    //캐릭터 1
    glPushMatrix();
        //캐릭터 색상 지정
        glColor3f(0, 0, 1); //파란색
        
        //캐릭터가 생성될 지점 지정
        glTranslated(0, 0, 8); //(바닥으로 부터) 8만큼 뒤에서
        glTranslatef(x, 0, y); //키보드로 조작하기 위해(x,y)
        //캐릭터 크기 작게 조절
        glScalef(0.2, 0.2, 0.2);
        glTranslatef(0, 3, 0); //캐릭터 바닥위치

        DrawCharacter(); //캐릭터 그리기
    glPopMatrix();


    //좀비 생성 (좀비는 손을 앞으로 뻗고 흔들거림)
    glPushMatrix();
        //몬스터가 생성될 기준 위치 설정
        glTranslated(0, 0, -8); // 8만큼 뒤에서
        glTranslatef(sin(t * 0.0001) * 10, 0, 0);  //좌우로 움직이도록 sin이용
        //좀비 크기 작게 조절
        glScalef(0.3, 0.3, 0.3);
        glTranslatef(0, 3, 0); //캐릭터 바닥위치

        //좀비 그리기
        DrawZombie();
    glPopMatrix();

    //캐릭터가 생성한 총알 날리기
    DrawBullets();
    

    //총알이 맞았는지 확인
    bool hit = HitCheck();
    if (hit&&isClear==false) { //중복 승리 메시지 방지
        printf("hit \n");
        HEALTH -= 10; //체력 10 감소
        //좀비의 체력이 0 이하가 되면, 그리고 아직 지지않았다면
        if (HEALTH <= 0 && lose == false) {
            isClear = true; //승리 처리
            printf("YOU WIN!!\n"); //승리
        } 
            
    }

    //체력바 그리기 => 좀비의 체력바(빨간색)
    if (HEALTH > 0) {
        glPushMatrix();
        glTranslatef(0, 8, 0);
        glScalef(0.3, 0.3, 0.3);
        glColor3f(1, 0, 0);
        glRotatef(90, 0, 1, 0);
        glTranslated(30, 0, -30);
        //좀비의 체력만큼 화면에 그려줌
        //좀비의 체력이 깎이면 그만큼 체력바도 줄어들음
        gluCylinder(cyl, 3, 3, HEALTH * 0.6, 40, 40);
        glPopMatrix();
    }
    

    glFlush();
}



void MyReshape(int w, int h) {
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-15.0, 15.0, -15.0, 15.0, -100.0, 100.0); // 15가 좋은 view
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    //관찰하는 시점
    //MX / 300.0, MY / 300.0, 1.0 에서 바라보고 있음
    gluLookAt(MX / 300.0, MY / 300.0, 1.0,
        0.0, 0.0, 0.0,
        0.0, 1.0, 0.0);  //시점변환
}



int main(int argc, char** argv) {
    srand(time(NULL)); //랜덤 정수를 위해
    glutInit(&argc, argv);
    glutInitWindowSize(1600, 1600); //윈도우 창 크기 설정
    glutCreateWindow("Zombie Surviver");
    glutDisplayFunc(MyDisplay); //그림을 그릴때
    glutReshapeFunc(MyReshape);
    glClearColor(0.4, 0.4, 0.4, 1); //화면을 지울때 쓰는 색상 지정

    //키보드 이벤트 설정
    glutKeyboardFunc(MyKeyboard);
    glutMotionFunc(MyMouseMove);
    //InitLight(); //조명 설정

    //10ms 후에 함수 호출 예약
    glutTimerFunc(10, MyTimer, 1);
    //10ms후에 플레이타임 누적 함수 작동 시작
    glutTimerFunc(10, plusPlayTime, 1);
    glutMainLoop();

    return 0;
}