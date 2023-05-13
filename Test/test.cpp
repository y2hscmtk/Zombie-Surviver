#include <GL/glut.h>
#include <math.h>

float h = 0.25;
float a = 0.001f;
float t = 0;
GLUquadricObj* cyl;

void MyIdle()
{
    t = t + 1.0f;
    if (h <= 0.1) a = +0.0001f;
    if (h >= 0.6) a = -0.0001f;
    h = h + a;
    glutPostRedisplay();
}

void MyDisplay() {
    cyl = gluNewQuadric();   
    gluQuadricDrawStyle(cyl, GLU_LINE);

    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glColor3f(1.0, 0.0, 0.0);
    glScalef(5, 0.3, 5);
    glutWireCube(0.3);
    glPopMatrix();

    //glPushMatrix();
    glColor3f(0.0, 0.0, 1.0);
    //glTranslatef(0, h, 0);
    //glutWireSphere(0.1, 20, 20);
    //glPopMatrix();

    //머리 만들기
    glPushMatrix();
    glTranslatef(0, 0.6, 0.0); //몸통에서 0.15만큼 떨어진 위치에
    glutWireSphere(0.1, 20, 20); //구 생성 => 머리
    glPopMatrix();



    // 1. 몸통
    glPushMatrix();
        glRotatef(-90, 1, 0, 0); // -90
        // Cynliner => 원기둥은 처음에 가로상태 => 90도 회전
        gluCylinder(cyl, 0.1, 0.1, 0.5, 50, 5); 

        // 2. 팔
        glPushMatrix();
            glTranslatef(0.15, 0.0, 0.5); //몸통에서 0.15만큼 떨어진 위치에
            glutWireSphere(0.05, 20, 20); //구 생성 => 팔 관절
            glRotatef(sin(t * 0.003) * 60 + 180, 1, 0, 0); //좌우로 흔들리게
            //
            gluCylinder(cyl, 0.05, 0.05, 0.2, 20, 1);

            glPushMatrix();
                glTranslatef(0.0, 0.0, 0.2);
                glutWireSphere(0.05, 20, 20);

                glRotatef(sin(t * 0.003) * 20 + 20, 1, 0, 0);   

                gluCylinder(cyl, 0.05, 0.05, 0.2, 20, 1);
            glPopMatrix();
        glPopMatrix();
        // 팔2
        glPushMatrix();
        glTranslatef(-0.15, 0.0, 0.5);
        glutWireSphere(0.05, 20, 20);
        glRotatef(-sin(t * 0.003) * 60 + 180, 1, 0, 0);     
        gluCylinder(cyl, 0.05, 0.05, 0.2, 20, 1);
        glPushMatrix();
        glTranslatef(0.0, 0.0, 0.2);
        glutWireSphere(0.05, 20, 20);
        glRotatef(-sin(t * 0.003) * 20 + 20, 1, 0, 0); 

        gluCylinder(cyl, 0.05, 0.05, 0.2, 20, 1);
        glPopMatrix();
        glPopMatrix();
        // 다리
        glPushMatrix();
        glTranslatef(0.05, 0.0, 0.0);
        glutWireSphere(0.05, 20, 20); //다리 관절
        glRotatef(-sin(t * 0.003) * 60 + 180, 1, 0, 0);    
        gluCylinder(cyl, 0.05, 0.05, 0.3, 20, 1);

        //다리 만들기
        glPushMatrix();
        glTranslatef(0.0, 0.0, 0.3);
        glutWireSphere(0.05, 20, 20); //무릎
        glRotatef(sin(t * 0.003) * 20 + 20, 1, 0, 0);     
        gluCylinder(cyl, 0.05, 0.05, 0.2, 20, 1);
        glPopMatrix();
        glPopMatrix();
        // 다리2
        glPushMatrix();
        glTranslatef(-0.05, 0.0, 0.0); //왼쪽 다리
        glutWireSphere(0.05, 20, 20);
        glRotatef(sin(t * 0.003) * 60 + 180, 1, 0, 0);
        gluCylinder(cyl, 0.05, 0.05, 0.3, 20, 1);
        glPopMatrix();

    glPopMatrix(); //        


    glFlush();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA);
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("OpenGL Sample Drawing");
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(20, 1, 0.1, 1000);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(2, 0.6, 4, 0, 0, 0, 0, 1, 0);

    glutDisplayFunc(MyDisplay);
    glutIdleFunc(MyIdle);
    glutMainLoop();
    return 0;
}