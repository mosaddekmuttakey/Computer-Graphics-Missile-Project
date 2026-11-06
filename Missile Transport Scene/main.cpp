#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <cmath>
#include <cstring>
#include <cstdlib>

#define PI 3.14159265358979323846

// ---------------- Global Variables ----------------

bool animationStart = false;
bool pauseAnimation = false;

float cloudMove = 0.0f;
float radarAngle = 0.0f;

float truckX = -520.0f;
float launchY = 0.0f;
float flightT = 0.0f;
float interceptT = 0.0f;
float explosionRadius = 5.0f;


// ---------------- Utility and Drawing Functions ----------------

void drawText(const char* text, float x, float y)
{
    glRasterPos2f(x, y);

    for (int i = 0; i < strlen(text); i++)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
    }
}


void drawCircle(float cx, float cy, float r)
{
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);

    for (int i = 0; i <= 360; i++)
    {
        float angle = i * PI / 180.0f;
        glVertex2f(cx + cos(angle) * r, cy + sin(angle) * r);
    }

    glEnd();
}


void drawCircleLine(float cx, float cy, float r)
{
    glBegin(GL_LINE_LOOP);

    for (int i = 0; i <= 360; i++)
    {
        float angle = i * PI / 180.0f;
        glVertex2f(cx + cos(angle) * r, cy + sin(angle) * r);
    }

    glEnd();
}


void drawLineDDA(float x1, float y1, float x2, float y2)
{
    float dx = x2 - x1;
    float dy = y2 - y1;

    int steps = (int)ceil(fmax(fabs(dx), fabs(dy)));

    if (steps == 0)
    {
        return;
    }

    float xInc = dx / steps;
    float yInc = dy / steps;

    float x = x1;
    float y = y1;

    glBegin(GL_POINTS);

    for (int i = 0; i <= steps; i++)
    {
        glVertex2f(x, y);
        x += xInc;
        y += yInc;
    }

    glEnd();
}


void drawCloud(float x, float y)
{
    glColor3ub(225, 225, 230);

    drawCircle(x, y, 22);
    drawCircle(x + 28, y + 10, 28);
    drawCircle(x + 56, y, 22);
    drawCircle(x + 30, y - 8, 18);
}


void drawTree(float x, float y, float scale)
{
    glPushMatrix();

    glTranslatef(x, y, 0);
    glScalef(scale, scale, 1);

    glColor3ub(100, 60, 20);
    glBegin(GL_QUADS);
    glVertex2f(-5, 0);
    glVertex2f(5, 0);
    glVertex2f(5, 45);
    glVertex2f(-5, 45);
    glEnd();

    glColor3ub(25, 140, 50);
    drawCircle(0, 60, 22);
    drawCircle(-18, 45, 18);
    drawCircle(18, 45, 18);
    drawCircle(0, 82, 16);

    glPopMatrix();
}


void drawPerson(float x, float y, float scale, int shirtR, int shirtG, int shirtB)
{
    glPushMatrix();

    glTranslatef(x, y, 0);
    glScalef(scale, scale, 1);

    // head
    glColor3ub(240, 190, 140);
    drawCircle(0, 45, 7);

    // hair
    glColor3ub(30, 25, 20);
    drawCircle(0, 50, 5);

    // body
    glColor3ub(shirtR, shirtG, shirtB);
    glBegin(GL_QUADS);
    glVertex2f(-8, 15);
    glVertex2f(8, 15);
    glVertex2f(8, 38);
    glVertex2f(-8, 38);
    glEnd();

    // arms
    glColor3ub(240, 190, 140);
    glLineWidth(3);
    glBegin(GL_LINES);
    glVertex2f(-8, 33);
    glVertex2f(-18, 22);

    glVertex2f(8, 33);
    glVertex2f(18, 22);
    glEnd();

    // legs
    glColor3ub(25, 25, 45);
    glLineWidth(4);
    glBegin(GL_LINES);
    glVertex2f(-4, 15);
    glVertex2f(-10, 0);

    glVertex2f(4, 15);
    glVertex2f(10, 0);
    glEnd();

    glLineWidth(1);

    glPopMatrix();
}


void drawPeopleGroup(float x, float y)
{
    drawPerson(x, y, 0.8f, 40, 120, 220);
    drawPerson(x + 35, y, 0.75f, 220, 80, 70);
    drawPerson(x + 70, y, 0.78f, 50, 160, 90);
}


void drawHouse3D(float x, float y, float scale, int r, int g, int b)
{
    glPushMatrix();

    glTranslatef(x, y, 0);
    glScalef(scale, scale, 1);

    float depth = 25.0f;

    int sr = r * 0.65;
    int sg = g * 0.65;
    int sb = b * 0.65;

    // side wall
    glColor3ub(sr, sg, sb);
    glBegin(GL_QUADS);
    glVertex2f(90, 0);
    glVertex2f(90 + depth, 18);
    glVertex2f(90 + depth, 88);
    glVertex2f(90, 70);
    glEnd();

    // front wall
    glColor3ub(r, g, b);
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(90, 0);
    glVertex2f(90, 70);
    glVertex2f(0, 70);
    glEnd();

    // roof side
    glColor3ub(100, 40, 30);
    glBegin(GL_QUADS);
    glVertex2f(100, 70);
    glVertex2f(100 + depth, 88);
    glVertex2f(45 + depth, 138);
    glVertex2f(45, 120);
    glEnd();

    // roof front
    glColor3ub(160, 60, 40);
    glBegin(GL_TRIANGLES);
    glVertex2f(-10, 70);
    glVertex2f(100, 70);
    glVertex2f(45, 120);
    glEnd();

    // roof outline
    glColor3ub(70, 30, 20);
    glLineWidth(2);
    glBegin(GL_LINE_LOOP);
    glVertex2f(-10, 70);
    glVertex2f(100, 70);
    glVertex2f(45, 120);
    glEnd();
    glLineWidth(1);

    // door
    glColor3ub(80, 45, 25);
    glBegin(GL_QUADS);
    glVertex2f(35, 0);
    glVertex2f(58, 0);
    glVertex2f(58, 42);
    glVertex2f(35, 42);
    glEnd();

    glColor3ub(230, 210, 80);
    drawCircle(53, 21, 2.5);

    // windows
    glColor3ub(165, 225, 255);
    glBegin(GL_QUADS);
    glVertex2f(10, 35);
    glVertex2f(30, 35);
    glVertex2f(30, 55);
    glVertex2f(10, 55);
    glEnd();

    glBegin(GL_QUADS);
    glVertex2f(65, 35);
    glVertex2f(85, 35);
    glVertex2f(85, 55);
    glVertex2f(65, 55);
    glEnd();

    // window frames
    glColor3ub(60, 80, 100);
    glLineWidth(1.5);
    glBegin(GL_LINES);
    glVertex2f(20, 35);
    glVertex2f(20, 55);
    glVertex2f(10, 45);
    glVertex2f(30, 45);

    glVertex2f(75, 35);
    glVertex2f(75, 55);
    glVertex2f(65, 45);
    glVertex2f(85, 45);
    glEnd();
    glLineWidth(1);

    // side window
    glColor3ub(125, 195, 230);
    glBegin(GL_QUADS);
    glVertex2f(98, 40);
    glVertex2f(110, 48);
    glVertex2f(110, 64);
    glVertex2f(98, 56);
    glEnd();

    glPopMatrix();
}


void drawBuilding3D(float x, float y, float w, float h, float depth)
{
    // front wall
    glColor3ub(100, 112, 140);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + w, y);
    glVertex2f(x + w, y + h);
    glVertex2f(x, y + h);
    glEnd();

    // side wall
    glColor3ub(65, 78, 105);
    glBegin(GL_QUADS);
    glVertex2f(x + w, y);
    glVertex2f(x + w + depth, y + 20);
    glVertex2f(x + w + depth, y + h + 20);
    glVertex2f(x + w, y + h);
    glEnd();

    // top roof
    glColor3ub(45, 55, 80);
    glBegin(GL_QUADS);
    glVertex2f(x, y + h);
    glVertex2f(x + depth, y + h + 20);
    glVertex2f(x + w + depth, y + h + 20);
    glVertex2f(x + w, y + h);
    glEnd();

    // roof border
    glColor3ub(30, 35, 55);
    glLineWidth(3);
    glBegin(GL_LINES);
    glVertex2f(x, y + h);
    glVertex2f(x + w, y + h);

    glVertex2f(x + w, y);
    glVertex2f(x + w, y + h);
    glEnd();
    glLineWidth(1);

    // windows
    for (float wx = x + 15; wx < x + w - 12; wx += 30)
    {
        for (float wy = y + 40; wy < y + h - 30; wy += 45)
        {
            glColor3ub(255, 235, 125);
            glBegin(GL_QUADS);
            glVertex2f(wx, wy);
            glVertex2f(wx + 15, wy);
            glVertex2f(wx + 15, wy + 22);
            glVertex2f(wx, wy + 22);
            glEnd();

            // window frame
            glColor3ub(40, 50, 70);
            glLineWidth(1);
            glBegin(GL_LINE_LOOP);
            glVertex2f(wx, wy);
            glVertex2f(wx + 15, wy);
            glVertex2f(wx + 15, wy + 22);
            glVertex2f(wx, wy + 22);
            glEnd();
        }
    }

    // main entrance
    glColor3ub(35, 45, 65);
    glBegin(GL_QUADS);
    glVertex2f(x + w * 0.38f, y);
    glVertex2f(x + w * 0.62f, y);
    glVertex2f(x + w * 0.62f, y + 38);
    glVertex2f(x + w * 0.38f, y + 38);
    glEnd();

    // glass entrance highlight
    glColor3ub(120, 190, 230);
    glBegin(GL_QUADS);
    glVertex2f(x + w * 0.43f, y + 6);
    glVertex2f(x + w * 0.57f, y + 6);
    glVertex2f(x + w * 0.57f, y + 34);
    glVertex2f(x + w * 0.43f, y + 34);
    glEnd();

    // rooftop antenna
    glColor3ub(220, 220, 230);
    glLineWidth(3);
    glBegin(GL_LINES);
    glVertex2f(x + w * 0.5f, y + h + 20);
    glVertex2f(x + w * 0.5f, y + h + 65);

    glVertex2f(x + w * 0.5f, y + h + 50);
    glVertex2f(x + w * 0.35f, y + h + 70);

    glVertex2f(x + w * 0.5f, y + h + 50);
    glVertex2f(x + w * 0.65f, y + h + 70);
    glEnd();
    glLineWidth(1);

    glColor3ub(255, 70, 70);
    drawCircle(x + w * 0.5f, y + h + 70, 4);
}


void drawRoad()
{
    glColor3ub(40, 40, 40);
    glBegin(GL_QUADS);
    glVertex2f(-400, -350);
    glVertex2f(600, -350);
    glVertex2f(600, -265);
    glVertex2f(-400, -265);
    glEnd();

    // footpath
    glColor3ub(180, 180, 180);
    glBegin(GL_QUADS);
    glVertex2f(-400, -265);
    glVertex2f(600, -265);
    glVertex2f(600, -245);
    glVertex2f(-400, -245);
    glEnd();

    // road divider
    glColor3ub(255, 255, 255);
    glLineWidth(4);

    for (int x = -380; x < 600; x += 120)
    {
        glBegin(GL_LINES);
        glVertex2f(x, -308);
        glVertex2f(x + 60, -308);
        glEnd();
    }

    glLineWidth(1);
}


void drawMissile(float x, float y, float angle, float scale, bool flame)
{
    glPushMatrix();

    glTranslatef(x, y, 0);
    glRotatef(angle, 0, 0, 1);
    glScalef(scale, scale, 1);

    // side body
    glColor3ub(155, 155, 165);
    glBegin(GL_QUADS);
    glVertex2f(10, 0);
    glVertex2f(22, 8);
    glVertex2f(22, 78);
    glVertex2f(10, 70);
    glEnd();

    // main body
    glColor3ub(220, 220, 225);
    glBegin(GL_QUADS);
    glVertex2f(-12, 0);
    glVertex2f(10, 0);
    glVertex2f(10, 70);
    glVertex2f(-12, 70);
    glEnd();

    // highlight
    glColor3ub(245, 245, 250);
    glBegin(GL_QUADS);
    glVertex2f(-10, 4);
    glVertex2f(-3, 4);
    glVertex2f(-3, 66);
    glVertex2f(-10, 66);
    glEnd();

    // nose front
    glColor3ub(220, 45, 45);
    glBegin(GL_TRIANGLES);
    glVertex2f(-12, 70);
    glVertex2f(10, 70);
    glVertex2f(0, 105);
    glEnd();

    // nose side
    glColor3ub(150, 25, 25);
    glBegin(GL_TRIANGLES);
    glVertex2f(10, 70);
    glVertex2f(22, 78);
    glVertex2f(0, 105);
    glEnd();

    // fins
    glColor3ub(110, 110, 120);
    glBegin(GL_TRIANGLES);
    glVertex2f(-12, 10);
    glVertex2f(-35, -12);
    glVertex2f(-12, 32);
    glEnd();

    glColor3ub(75, 75, 85);
    glBegin(GL_TRIANGLES);
    glVertex2f(10, 10);
    glVertex2f(35, -8);
    glVertex2f(10, 32);
    glEnd();

    // window
    glColor3ub(55, 135, 185);
    drawCircle(-1, 52, 6);

    // body rings
    glColor3ub(90, 90, 100);
    glLineWidth(2);
    glBegin(GL_LINES);
    glVertex2f(-12, 20);
    glVertex2f(10, 20);

    glVertex2f(-12, 42);
    glVertex2f(10, 42);

    glVertex2f(-12, 62);
    glVertex2f(10, 62);
    glEnd();
    glLineWidth(1);

    // flame
    if (flame)
    {
        glColor3ub(255, 110, 0);
        glBegin(GL_TRIANGLES);
        glVertex2f(-8, -4);
        glVertex2f(8, -4);
        glVertex2f(0, -48);
        glEnd();

        glColor3ub(255, 230, 0);
        glBegin(GL_TRIANGLES);
        glVertex2f(-5, -4);
        glVertex2f(5, -4);
        glVertex2f(0, -30);
        glEnd();

        glColor3ub(255, 255, 220);
        glBegin(GL_TRIANGLES);
        glVertex2f(-2, -4);
        glVertex2f(2, -4);
        glVertex2f(0, -18);
        glEnd();
    }

    glPopMatrix();
}


void drawTransportVehicle(float x, float y)
{
    glPushMatrix();

    glTranslatef(x, y, 0);

    // trailer
    glColor3ub(75, 95, 85);
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(230, 0);
    glVertex2f(230, 50);
    glVertex2f(0, 50);
    glEnd();

    // trailer side
    glColor3ub(50, 70, 65);
    glBegin(GL_QUADS);
    glVertex2f(230, 0);
    glVertex2f(260, 15);
    glVertex2f(260, 65);
    glVertex2f(230, 50);
    glEnd();

    // cabin
    glColor3ub(65, 110, 150);
    glBegin(GL_QUADS);
    glVertex2f(230, 0);
    glVertex2f(310, 0);
    glVertex2f(310, 75);
    glVertex2f(230, 75);
    glEnd();

    // cabin side
    glColor3ub(45, 80, 120);
    glBegin(GL_QUADS);
    glVertex2f(310, 0);
    glVertex2f(340, 20);
    glVertex2f(340, 90);
    glVertex2f(310, 75);
    glEnd();

    // window
    glColor3ub(140, 215, 255);
    glBegin(GL_QUADS);
    glVertex2f(245, 40);
    glVertex2f(295, 40);
    glVertex2f(295, 65);
    glVertex2f(245, 65);
    glEnd();

    // front window
    glColor3ub(120, 190, 230);
    glBegin(GL_QUADS);
    glVertex2f(315, 45);
    glVertex2f(335, 55);
    glVertex2f(335, 75);
    glVertex2f(315, 65);
    glEnd();

    // wheels
    glColor3ub(0, 0, 0);
    drawCircle(45, 0, 20);
    drawCircle(115, 0, 20);
    drawCircle(250, 0, 20);
    drawCircle(310, 0, 20);

    glColor3ub(180, 180, 180);
    drawCircle(45, 0, 9);
    drawCircle(115, 0, 9);
    drawCircle(250, 0, 9);
    drawCircle(310, 0, 9);

    // mounted missile
    drawMissile(95, 62, -90, 0.75f, false);

    // holders
    glColor3ub(220, 220, 220);
    glLineWidth(4);
    glBegin(GL_LINES);
    glVertex2f(55, 50);
    glVertex2f(75, 82);

    glVertex2f(125, 50);
    glVertex2f(145, 82);

    glVertex2f(175, 50);
    glVertex2f(195, 82);
    glEnd();
    glLineWidth(1);

    glPopMatrix();
}


void drawLaunchBase()
{
    // platform
    glColor3ub(85, 85, 95);
    glBegin(GL_QUADS);
    glVertex2f(-300, -155);
    glVertex2f(-170, -155);
    glVertex2f(-145, -120);
    glVertex2f(-325, -120);
    glEnd();

    // tower
    glColor3ub(120, 120, 130);
    glBegin(GL_QUADS);
    glVertex2f(-260, -120);
    glVertex2f(-240, -120);
    glVertex2f(-240, -40);
    glVertex2f(-260, -40);
    glEnd();

    // support structure
    glColor3ub(190, 190, 200);
    glLineWidth(3);
    glBegin(GL_LINES);
    glVertex2f(-300, -120);
    glVertex2f(-240, -40);

    glVertex2f(-190, -120);
    glVertex2f(-260, -40);

    glVertex2f(-250, -120);
    glVertex2f(-250, 20);
    glEnd();
    glLineWidth(1);
}


void drawSmoke(float x, float y)
{
    glColor3ub(130, 130, 130);
    drawCircle(x - 25, y, 20);
    drawCircle(x + 5, y - 5, 24);
    drawCircle(x + 35, y, 18);

    glColor3ub(165, 165, 165);
    drawCircle(x - 5, y + 22, 16);
    drawCircle(x + 25, y + 20, 14);
}


void drawFireBlast(float x, float y)
{
    glColor3ub(255, 110, 0);
    glBegin(GL_TRIANGLES);
    glVertex2f(x - 35, y);
    glVertex2f(x + 35, y);
    glVertex2f(x, y - 95);
    glEnd();

    glColor3ub(255, 230, 0);
    glBegin(GL_TRIANGLES);
    glVertex2f(x - 22, y);
    glVertex2f(x + 22, y);
    glVertex2f(x, y - 60);
    glEnd();
}


void drawControlRoom3D(float x, float y, float scale)
{
    glPushMatrix();

    glTranslatef(x, y, 0);
    glScalef(scale, scale, 1);

    float depth = 28.0f;

    // base platform
    glColor3ub(95, 95, 105);
    glBegin(GL_QUADS);
    glVertex2f(-10, -8);
    glVertex2f(165, -8);
    glVertex2f(185, 8);
    glVertex2f(10, 8);
    glEnd();

    // front wall
    glColor3ub(180, 188, 205);
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(155, 0);
    glVertex2f(155, 95);
    glVertex2f(0, 95);
    glEnd();

    // side wall
    glColor3ub(130, 145, 165);
    glBegin(GL_QUADS);
    glVertex2f(155, 0);
    glVertex2f(155 + depth, 18);
    glVertex2f(155 + depth, 113);
    glVertex2f(155, 95);
    glEnd();

    // roof front
    glColor3ub(80, 90, 110);
    glBegin(GL_QUADS);
    glVertex2f(-10, 95);
    glVertex2f(165, 95);
    glVertex2f(145, 118);
    glVertex2f(10, 118);
    glEnd();

    // roof side
    glColor3ub(60, 70, 90);
    glBegin(GL_QUADS);
    glVertex2f(165, 95);
    glVertex2f(165 + depth, 113);
    glVertex2f(145 + depth, 136);
    glVertex2f(145, 118);
    glEnd();

    // windows
    glColor3ub(120, 210, 245);
    glBegin(GL_QUADS);
    glVertex2f(20, 42);
    glVertex2f(55, 42);
    glVertex2f(55, 72);
    glVertex2f(20, 72);
    glEnd();

    glBegin(GL_QUADS);
    glVertex2f(65, 42);
    glVertex2f(100, 42);
    glVertex2f(100, 72);
    glVertex2f(65, 72);
    glEnd();

    // door
    glColor3ub(75, 55, 40);
    glBegin(GL_QUADS);
    glVertex2f(112, 0);
    glVertex2f(140, 0);
    glVertex2f(140, 52);
    glVertex2f(112, 52);
    glEnd();

    glColor3ub(230, 210, 90);
    drawCircle(134, 25, 3);

    // antenna
    glColor3ub(230, 230, 230);
    glLineWidth(3);
    glBegin(GL_LINES);
    glVertex2f(85, 118);
    glVertex2f(85, 165);

    glVertex2f(85, 152);
    glVertex2f(65, 172);

    glVertex2f(85, 152);
    glVertex2f(105, 172);
    glEnd();
    glLineWidth(1);

    // signal light
    glColor3ub(255, 60, 60);
    drawCircle(85, 170, 4);

    glColor3ub(255, 255, 255);
    drawText("CONTROL ROOM", 20, 15);

    glPopMatrix();
}


void drawDefenseVehicle(float x, float y)
{
    glPushMatrix();

    glTranslatef(x, y, 0);

    glColor3ub(70, 100, 80);
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(130, 0);
    glVertex2f(130, 45);
    glVertex2f(0, 45);
    glEnd();

    glColor3ub(50, 75, 60);
    glBegin(GL_QUADS);
    glVertex2f(130, 0);
    glVertex2f(160, 15);
    glVertex2f(160, 60);
    glVertex2f(130, 45);
    glEnd();

    glColor3ub(0, 0, 0);
    drawCircle(25, 0, 16);
    drawCircle(85, 0, 16);
    drawCircle(135, 0, 16);

    glColor3ub(180, 180, 180);
    drawCircle(25, 0, 7);
    drawCircle(85, 0, 7);
    drawCircle(135, 0, 7);

    // launcher rail
    glColor3ub(210, 210, 220);
    glLineWidth(5);
    glBegin(GL_LINES);
    glVertex2f(45, 45);
    glVertex2f(130, 95);

    glVertex2f(52, 38);
    glVertex2f(137, 88);
    glEnd();
    glLineWidth(1);

    glColor3ub(255, 255, 255);
    drawText("DEFENSE", 28, -35);

    glPopMatrix();
}


void drawRadar(float x, float y)
{
    glColor3ub(0, 255, 80);
    glLineWidth(2);

    drawCircleLine(x, y, 45);
    drawCircleLine(x, y, 30);
    drawCircleLine(x, y, 15);

    float rad = radarAngle * PI / 180.0f;
    float x2 = x + cos(rad) * 45;
    float y2 = y + sin(rad) * 45;

    glBegin(GL_LINES);
    glVertex2f(x, y);
    glVertex2f(x2, y2);
    glEnd();

    // stand
    glColor3ub(100, 100, 110);
    glBegin(GL_QUADS);
    glVertex2f(x - 10, y - 95);
    glVertex2f(x + 10, y - 95);
    glVertex2f(x + 10, y);
    glVertex2f(x - 10, y);
    glEnd();

    // base
    glColor3ub(80, 80, 90);
    glBegin(GL_QUADS);
    glVertex2f(x - 40, y - 110);
    glVertex2f(x + 40, y - 110);
    glVertex2f(x + 40, y - 95);
    glVertex2f(x - 40, y - 95);
    glEnd();

    glLineWidth(1);
}


void drawMissileTrail(float x, float y, float angle, float scale)
{
    glPushMatrix();

    glTranslatef(x, y, 0);
    glRotatef(angle, 0, 0, 1);
    glScalef(scale, scale, 1);

    glColor3ub(150, 150, 150);
    drawCircle(0, -35, 10);
    drawCircle(-8, -55, 14);
    drawCircle(7, -72, 18);

    glColor3ub(180, 180, 180);
    drawCircle(-4, -90, 12);
    drawCircle(8, -105, 9);

    glPopMatrix();
}


void drawDebrisPieces(float cx, float cy, float radius)
{
    glColor3ub(90, 90, 95);
    glLineWidth(3);

    for (int i = 0; i < 12; i++)
    {
        float angle = i * 30.0f * PI / 180.0f;

        float x1 = cx + cos(angle) * radius * 0.45f;
        float y1 = cy + sin(angle) * radius * 0.45f;

        float x2 = cx + cos(angle) * radius * 0.85f;
        float y2 = cy + sin(angle) * radius * 0.85f;

        glBegin(GL_LINES);
        glVertex2f(x1, y1);
        glVertex2f(x2, y2);
        glEnd();
    }

    glLineWidth(1);

    glColor3ub(120, 120, 130);

    for (int i = 0; i < 8; i++)
    {
        float angle = i * 45.0f * PI / 180.0f;

        float x = cx + cos(angle) * radius * 0.65f;
        float y = cy + sin(angle) * radius * 0.65f;

        glBegin(GL_TRIANGLES);
        glVertex2f(x, y);
        glVertex2f(x + 8, y + 3);
        glVertex2f(x + 3, y + 10);
        glEnd();
    }
}


void drawImprovedExplosion(float cx, float cy)
{
    float r = explosionRadius;

    // outer shockwave
    glColor3ub(255, 210, 80);
    drawCircle(cx, cy, r);

    glColor3ub(255, 130, 20);
    drawCircle(cx, cy, r * 0.75f);

    glColor3ub(255, 245, 180);
    drawCircle(cx, cy, r * 0.40f);

    // smoke clouds
    glColor3ub(120, 120, 120);
    drawCircle(cx - r * 0.45f, cy - r * 0.10f, r * 0.20f);
    drawCircle(cx + r * 0.40f, cy + r * 0.12f, r * 0.18f);
    drawCircle(cx - r * 0.10f, cy + r * 0.45f, r * 0.16f);
    drawCircle(cx + r * 0.12f, cy - r * 0.42f, r * 0.15f);

    glColor3ub(170, 170, 170);
    drawCircle(cx - r * 0.22f, cy + r * 0.25f, r * 0.13f);
    drawCircle(cx + r * 0.22f, cy - r * 0.22f, r * 0.12f);

    // sparks
    glColor3ub(255, 240, 90);
    glLineWidth(2);

    for (int i = 0; i < 16; i++)
    {
        float angle = i * 22.5f * PI / 180.0f;

        float x1 = cx + cos(angle) * r * 0.35f;
        float y1 = cy + sin(angle) * r * 0.35f;

        float x2 = cx + cos(angle) * r * 1.10f;
        float y2 = cy + sin(angle) * r * 1.10f;

        glBegin(GL_LINES);
        glVertex2f(x1, y1);
        glVertex2f(x2, y2);
        glEnd();
    }

    glLineWidth(1);

    if (r > 35)
    {
        drawDebrisPieces(cx, cy, r);
    }
}


// ---------------- Background and Scenario ----------------

void drawScenario1Background()
{
    // evening sky
    glBegin(GL_QUADS);
    glColor3ub(255, 160, 90);
    glVertex2f(-400, -350);
    glVertex2f(600, -350);

    glColor3ub(90, 120, 200);
    glVertex2f(600, 350);
    glVertex2f(-400, 350);
    glEnd();

    // sun
    glColor3ub(255, 230, 140);
    drawCircle(450, 250, 35);

    // mountains
    glColor3ub(90, 90, 110);
    glBegin(GL_TRIANGLES);
    glVertex2f(-420, -120);
    glVertex2f(-250, 110);
    glVertex2f(-40, -120);
    glEnd();

    glColor3ub(70, 70, 95);
    glBegin(GL_TRIANGLES);
    glVertex2f(-80, -120);
    glVertex2f(120, 95);
    glVertex2f(310, -120);
    glEnd();

    glColor3ub(85, 85, 100);
    glBegin(GL_TRIANGLES);
    glVertex2f(210, -120);
    glVertex2f(390, 105);
    glVertex2f(590, -120);
    glEnd();

    // land
    glColor3ub(75, 140, 75);
    glBegin(GL_QUADS);
    glVertex2f(-400, -350);
    glVertex2f(600, -350);
    glVertex2f(600, -130);
    glVertex2f(-400, -130);
    glEnd();

    // clouds
    drawCloud(-320 + cloudMove, 140);
    drawCloud(-80 + cloudMove * 0.8f, 210);
    drawCloud(180 + cloudMove * 0.6f, 165);

    drawRoad();

    drawHouse3D(-360, -245, 0.72f, 235, 190, 140);
    drawHouse3D(-250, -245, 0.68f, 200, 220, 245);
    drawHouse3D(90, -245, 0.78f, 240, 185, 170);
    drawBuilding3D(320, -245, 120, 280, 24);

    drawTree(-120, -245, 0.9f);
    drawTree(-60, -245, 0.8f);
}

void scenario1()
{
    drawScenario1Background();

    drawTransportVehicle(truckX, -315);

    // people beside road
    drawPeopleGroup(-40, -245);
    drawPeopleGroup(210, -245);

    glColor3ub(255, 255, 255);
    drawText("Scenario 1: Missile Transport Vehicle Moving Toward Launch Area", -360, 315);
    drawText("Road, houses, buildings, people, and transport vehicle are shown.", -360, 285);
    drawText("Press S = Start | P = Pause | R = Reset | ESC = Exit", -360, -325);
}

// ---------------- Program Control ----------------

void resetScenario()
{
    truckX = -520.0f;
    launchY = 0.0f;
    flightT = 0.0f;
    interceptT = 0.0f;
    explosionRadius = 5.0f;

    cloudMove = 0.0f;
    radarAngle = 0.0f;

    animationStart = false;
    pauseAnimation = false;
}


void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    scenario1();

    glutSwapBuffers();
}


void update(int value)
{
    if (animationStart && !pauseAnimation)
    {
        cloudMove += 0.8f;

        if (cloudMove > 750)
        {
            cloudMove = -750;
        }

        truckX += 2.0f;

        if (truckX > -70)
        {
            truckX = -70;
        }
    }

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}


void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 's':
    case 'S':
        animationStart = true;
        break;

    case 'p':
    case 'P':
        pauseAnimation = !pauseAnimation;
        break;

    case 'r':
    case 'R':
        resetScenario();
        break;

    case 27:
        exit(0);
        break;
    }

    glutPostRedisplay();
}


void init()
{
    glClearColor(0, 0, 0, 1);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluOrtho2D(-400, 600, -350, 350);

    glMatrixMode(GL_MODELVIEW);
}

void reshape(int w, int h)
{
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluOrtho2D(-400, 600, -350, 350);

    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(900, 600);
    glutInitWindowPosition(100, 50);

    glutCreateWindow("Scenario 1 - Missile Transport Vehicle");

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(16, update, 0);

    glutMainLoop();

    return 0;
}
