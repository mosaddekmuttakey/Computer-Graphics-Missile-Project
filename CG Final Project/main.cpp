/*
    Animated Missile Interception Project
    ------------------------------------
    This version contains added comments for readability.
    No variable names, function names, scenario logic, or animation logic were changed.

    Project Flow:
    1. Scenario 1: Missile transport vehicle moves toward the launch area.
    2. Scenario 2: Missile is launched from the launch base.
    3. Scenario 3: Missile flies through the sky using a curved path.
    4. Scenario 4: Defense system detects and destroys the missile before it reaches the city.

    Controls:
    S = Start animation
    P = Pause / Resume animation
    N = Go to next scenario
    R = Reset current scenario
    ESC = Exit program
*/

#include <windows.h>

#include <GL/gl.h>

#include <GL/glu.h>

#include <GL/glut.h>

#include <cmath>

#include <cstring>

#include <cstdlib>

#define PI 3.14159265358979323846

// ---------------- Global Variables ----------------
// These variables control the active scene and the animation state.

int currentScenario = 1;        // Stores which scenario is currently displayed: 1, 2, 3, or 4.

bool animationStart = false;    // Becomes true when the user presses 'S'.

bool pauseAnimation = false;    // Toggles animation pause/resume when the user presses 'P'.

float cloudMove = 0.0f;         // Controls horizontal cloud movement in the background.

float radarAngle = 0.0f;        // Controls radar rotation angle in Scenario 4.

float truckX = -520.0f;         // Controls missile transport vehicle position in Scenario 1.

float launchY = 0.0f;           // Controls vertical missile launch movement in Scenario 2.

float flightT = 0.0f;           // Controls missile progress along the sky path in Scenario 3.

float interceptT = 0.0f;        // Controls missile and interceptor progress in Scenario 4.

float explosionRadius = 5.0f;   // Controls the growing explosion effect after interception.

// ---------------- Utility Functions ----------------

// Draws text at a given 2D screen position using GLUT bitmap font.
void drawText(const char* text, float x, float y)

{

    glRasterPos2f(x, y);

    for (int i = 0; i < strlen(text); i++)

    {

        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);

    }

}

// Draws a filled circle using GL_TRIANGLE_FAN.
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

// Draws only the outline of a circle using GL_LINE_LOOP.
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

// DDA Line Drawing Algorithm

// Draws a line using the DDA algorithm. This is used for path/guide line drawing.
void drawLineDDA(float x1, float y1, float x2, float y2)

{

    float dx = x2 - x1;

    float dy = y2 - y1;

    int steps = fabs(dx) > fabs(dy) ? fabs(dx) : fabs(dy);

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

// ---------------- Common Visual Functions ----------------

// Draws a cloud by combining several filled circles.
void drawCloud(float x, float y)

{

    glColor3ub(225, 225, 230);

    drawCircle(x, y, 22);

    drawCircle(x + 28, y + 10, 28);

    drawCircle(x + 56, y, 22);

    drawCircle(x + 30, y - 8, 18);

}

// Draws a simple tree with trunk and circular leaves. Scale controls tree size.
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

// ---------------- People ----------------

// Draws a simple human figure. Shirt color is passed through RGB values.
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

// Draws a group of three people at the given position.
void drawPeopleGroup(float x, float y)

{

    drawPerson(x, y, 0.8f, 40, 120, 220);

    drawPerson(x + 35, y, 0.75f, 220, 80, 70);

    drawPerson(x + 70, y, 0.78f, 50, 160, 90);

}

// ---------------- Houses, Buildings, Road ----------------

// Draws a 3D-style house with front wall, side wall, roof, door, and windows.
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

// Draws a 3D-style building. Width, height, and depth define its size.
void drawBuilding3D(float x, float y, float w, float h, float depth)

{

    // front

    glColor3ub(105, 115, 135);

    glBegin(GL_QUADS);

    glVertex2f(x, y);

    glVertex2f(x + w, y);

    glVertex2f(x + w, y + h);

    glVertex2f(x, y + h);

    glEnd();

    // side

    glColor3ub(75, 85, 105);

    glBegin(GL_QUADS);

    glVertex2f(x + w, y);

    glVertex2f(x + w + depth, y + 20);

    glVertex2f(x + w + depth, y + h + 20);

    glVertex2f(x + w, y + h);

    glEnd();

    // top

    glColor3ub(60, 70, 90);

    glBegin(GL_QUADS);

    glVertex2f(x, y + h);

    glVertex2f(x + depth, y + h + 20);

    glVertex2f(x + w + depth, y + h + 20);

    glVertex2f(x + w, y + h);

    glEnd();

    // windows

    glColor3ub(255, 230, 120);

    for (float wx = x + 15; wx < x + w - 10; wx += 30)

    {

        for (float wy = y + 25; wy < y + h - 20; wy += 45)

        {

            glBegin(GL_QUADS);

            glVertex2f(wx, wy);

            glVertex2f(wx + 15, wy);

            glVertex2f(wx + 15, wy + 22);

            glVertex2f(wx, wy + 22);

            glEnd();

        }

    }

}

// Draws the road, footpath, and road divider marks.
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

// ---------------- Missile, Vehicle, Launch, Smoke ----------------

// Draws the missile. Angle rotates it, scale resizes it, and flame controls exhaust visibility.
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

// Draws the missile transport truck with trailer, cabin, wheels, and mounted missile.
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

// Draws the fixed launch platform and support tower used in Scenario 2.
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

// Draws smoke clouds during missile launch.
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

// Draws flame blast below the launching missile.
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

// ---------------- Improved 3D Control Room ----------------

// Draws the 3D-style control room with antenna and signal light.
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

    // side window

    glColor3ub(100, 190, 230);

    glBegin(GL_QUADS);

    glVertex2f(164, 45);

    glVertex2f(182, 56);

    glVertex2f(182, 79);

    glVertex2f(164, 68);

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

// ---------------- Defense System ----------------

// Draws the defense vehicle and its launcher rail in Scenario 4.
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

// Draws a rotating radar screen with circular ranges and scanning line.
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

// ---------------- Improved Interception Effects ----------------

// Draws smoke trail behind a moving missile or interceptor.
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

// Draws debris pieces after the explosion expands.
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

// Draws the interception explosion with shockwave, smoke, sparks, and debris.
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

// ---------------- Different Backgrounds ----------------

// Creates the full background for Scenario 1: road, village/city elements, mountains, sky, and clouds.
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

// Creates the launch-area background for Scenario 2.
void drawScenario2Background()

{

    // launch area sky

    glBegin(GL_QUADS);

    glColor3ub(240, 170, 130);

    glVertex2f(-400, -350);

    glVertex2f(600, -350);

    glColor3ub(70, 95, 150);

    glVertex2f(600, 350);

    glVertex2f(-400, 350);

    glEnd();

    // sun glow

    glColor3ub(255, 210, 120);

    drawCircle(430, 230, 28);

    // hills

    glColor3ub(105, 95, 85);

    glBegin(GL_TRIANGLES);

    glVertex2f(-400, -130);

    glVertex2f(-220, 60);

    glVertex2f(-40, -130);

    glEnd();

    glColor3ub(120, 100, 85);

    glBegin(GL_TRIANGLES);

    glVertex2f(20, -130);

    glVertex2f(200, 55);

    glVertex2f(380, -130);

    glEnd();

    glColor3ub(145, 120, 95);

    glBegin(GL_TRIANGLES);

    glVertex2f(260, -130);

    glVertex2f(430, 70);

    glVertex2f(600, -130);

    glEnd();

    // ground

    glColor3ub(145, 125, 85);

    glBegin(GL_QUADS);

    glVertex2f(-400, -350);

    glVertex2f(600, -350);

    glVertex2f(600, -130);

    glVertex2f(-400, -130);

    glEnd();

    // clouds

    drawCloud(-340 + cloudMove, 190);

    drawCloud(-90 + cloudMove * 0.8f, 240);

    drawCloud(180 + cloudMove * 0.6f, 200);

    // concrete launch strip

    glColor3ub(90, 90, 95);

    glBegin(GL_QUADS);

    glVertex2f(-400, -350);

    glVertex2f(600, -350);

    glVertex2f(600, -230);

    glVertex2f(-400, -230);

    glEnd();

}

// Creates the bright sky and distant ground background for Scenario 3.
void drawScenario3Background()

{

    // bright sky

    glBegin(GL_QUADS);

    glColor3ub(160, 215, 255);

    glVertex2f(-400, -350);

    glVertex2f(600, -350);

    glColor3ub(15, 80, 180);

    glVertex2f(600, 350);

    glVertex2f(-400, 350);

    glEnd();

    // sun

    glColor3ub(255, 245, 180);

    drawCircle(470, 260, 35);

    // clouds

    drawCloud(-350 + cloudMove, 240);

    drawCloud(-170 + cloudMove * 0.9f, 285);

    drawCloud(20 + cloudMove * 0.7f, 220);

    drawCloud(210 + cloudMove * 1.1f, 295);

    drawCloud(420 + cloudMove * 0.8f, 235);

    // lower horizon

    glColor3ub(110, 150, 110);

    glBegin(GL_QUADS);

    glVertex2f(-400, -350);

    glVertex2f(600, -350);

    glVertex2f(600, -210);

    glVertex2f(-400, -210);

    glEnd();

    glColor3ub(75, 110, 80);

    glBegin(GL_TRIANGLES);

    glVertex2f(-400, -210);

    glVertex2f(-240, -120);

    glVertex2f(-80, -210);

    glEnd();

    glBegin(GL_TRIANGLES);

    glVertex2f(50, -210);

    glVertex2f(220, -130);

    glVertex2f(390, -210);

    glEnd();

}

// Creates the night city background for Scenario 4.
void drawScenario4Background()

{

    // night sky

    glBegin(GL_QUADS);

    glColor3ub(5, 10, 30);

    glVertex2f(-400, -350);

    glVertex2f(600, -350);

    glColor3ub(20, 30, 80);

    glVertex2f(600, 350);

    glVertex2f(-400, 350);

    glEnd();

    // moon

    glColor3ub(240, 240, 210);

    drawCircle(470, 270, 30);

    // stars

    glColor3ub(255, 255, 255);

    drawCircle(-330, 285, 2);

    drawCircle(-260, 245, 2);

    drawCircle(-150, 300, 2);

    drawCircle(-40, 260, 2);

    drawCircle(80, 310, 2);

    drawCircle(200, 250, 2);

    drawCircle(350, 300, 2);

    drawCircle(520, 230, 2);

    // clouds

    drawCloud(-330 + cloudMove, 160);

    drawCloud(-70 + cloudMove * 0.8f, 220);

    drawCloud(200 + cloudMove * 0.6f, 180);

    // city ground

    glColor3ub(50, 120, 60);

    glBegin(GL_QUADS);

    glVertex2f(-400, -350);

    glVertex2f(600, -350);

    glVertex2f(600, -150);

    glVertex2f(-400, -150);

    glEnd();

    drawRoad();

    drawHouse3D(-370, -245, 0.75f, 235, 190, 120);

    drawHouse3D(-260, -245, 0.70f, 190, 220, 240);

    drawHouse3D(-150, -245, 0.80f, 230, 170, 170);

    drawHouse3D(10, -245, 0.75f, 210, 210, 150);

    drawBuilding3D(180, -245, 100, 360, 25);

    drawBuilding3D(320, -245, 130, 410, 30);

    drawTree(490, -245, 0.9f);

    drawTree(550, -245, 0.75f);

    // lamp posts

    glColor3ub(240, 220, 120);

    glLineWidth(4);

    glBegin(GL_LINES);

    glVertex2f(-330, -245);

    glVertex2f(-330, -160);

    glVertex2f(80, -245);

    glVertex2f(80, -160);

    glVertex2f(510, -245);

    glVertex2f(510, -160);

    glEnd();

    glLineWidth(1);

    glColor3ub(255, 255, 180);

    drawCircle(-330, -155, 10);

    drawCircle(80, -155, 10);

    drawCircle(510, -155, 10);

}

// ---------------- Scenario 1 ----------------

// Scenario 1: Shows missile transport vehicle moving toward the launch area.
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

    drawText("Press S = Start | N = Next | P = Pause | R = Reset | ESC = Exit", -360, -325);

}

// ---------------- Scenario 2 ----------------

// Scenario 2: Shows missile launch with launch base, smoke, fire, control room, and people.
void scenario2()

{

    drawScenario2Background();

    drawLaunchBase();

    // parked vehicle

    drawTransportVehicle(-120, -315);

    // launching missile

    drawMissile(-250, -90 + launchY, 0, 1.0f, animationStart);

    if (animationStart)

    {

        drawSmoke(-250, -140);

        drawFireBlast(-250, -130);

    }

    // improved control room

    drawControlRoom3D(100, -170, 1.0f);

    // people near control room

    drawPeopleGroup(320, -245);

    drawPeopleGroup(420, -245);

    glColor3ub(255, 255, 255);

    drawText("Scenario 2: Missile Ready and Launching From Launch Area", -360, 315);

    drawText("Launch smoke, fire, launch base, control room, and people are shown.", -360, 285);

    drawText("Press S = Start | N = Next | P = Pause | R = Reset | ESC = Exit", -360, -325);

}

// ---------------- Scenario 3 ----------------

// Scenario 3: Shows the missile flying across the middle of the sky along a curved path.
void scenario3()

{

    drawScenario3Background();

    glColor3ub(255, 255, 255);

    drawText("Scenario 3: Missile Flying Through the Middle of the Sky", -360, 315);

    // people watching from safe distant ground area

    drawPeopleGroup(-300, -235);

    drawPeopleGroup(-200, -235);

    drawPeopleGroup(-100, -235);

    drawPeopleGroup(150, -235);

    drawPeopleGroup(250, -235);

    drawPeopleGroup(350, -235);

    // sky-middle flight path

    float startX = -550.0f; // Started much further left (off-screen)

    float startY = 115.0f;

    float endX = 1100.0f; // Ended much further right (off-screen) to let trailing smoke clear the screen

    float endY = 160.0f;

    float curveHeight = 95.0f;

    // curved flight path through middle sky

    glColor3ub(255, 255, 0);

    glPointSize(2);

    for (float t = 0; t < 1.0f; t += 0.04f)

    {

        float x1 = startX + (endX - startX) * t;

        float y1 = startY + (endY - startY) * t + curveHeight * sin(t * PI);

        float t2 = t + 0.03f;

        float x2 = startX + (endX - startX) * t2;

        float y2 = startY + (endY - startY) * t2 + curveHeight * sin(t2 * PI);

        drawLineDDA(x1, y1, x2, y2);

    }

    glPointSize(1);

    float x = startX + (endX - startX) * flightT;

    float y = startY + (endY - startY) * flightT + curveHeight * sin(flightT * PI);

    float dx = endX - startX;

    float dy = (endY - startY) + curveHeight * PI * cos(flightT * PI);

    float angle = atan2(dy, dx) * 180.0f / PI - 90.0f;

    drawMissile(x, y, angle, 0.85f, true);

    // smoke trail

    glColor3ub(150, 150, 150);

    for (int i = 1; i <= 7; i++)

    {

        float st = flightT - i * 0.035f;

        if (st > 0)

        {

            float sx = startX + (endX - startX) * st;

            float sy = startY + (endY - startY) * st + curveHeight * sin(st * PI);

            drawCircle(sx, sy, 8 + i * 2);

        }

    }

    glColor3ub(255, 255, 255);

    drawText("The missile flies through the middle area of the sky.", -360, -295);

    drawText("Press S = Start | N = Next | P = Pause | R = Reset | ESC = Exit", -360, -325);

}


// ---------------- Scenario 4 ----------------

// Scenario 4: Shows radar detection, interceptor launch, collision, and city protection result.
void scenario4()

{

    drawScenario4Background();

    glColor3ub(255, 255, 255);

    drawText("Scenario 4: Interceptor Rocket Destroys Missile Before City", -360, 315);

    drawRadar(360, -130);

    drawDefenseVehicle(340, -260);

    // people in safe city area

    drawPeopleGroup(-80, -245);

    drawPeopleGroup(470, -245);

    // interception point

    float cx = 80;

    float cy = 125;

    if (interceptT < 1.0f)

    {

        // incoming missile path

        float imx = -350 + (cx + 60 - (-350)) * interceptT;

        float imy = 235 + (cy - 235) * interceptT;

        // interceptor rocket path

        float ix = 410 + (cx - 410) * interceptT;

        float iy = -205 + (cy - (-205)) * interceptT;

        // trails first

        drawMissileTrail(imx, imy, -75, 0.50f);

        drawMissileTrail(ix, iy, 43, 0.45f);

        // missiles

        drawMissile(imx, imy, -75, 0.65f, true);

        drawMissile(ix, iy, 43, 0.55f, true);

        // guide paths

        glColor3ub(255, 255, 0);

        glLineWidth(2);

        glBegin(GL_LINES);

        glVertex2f(-350, 235);

        glVertex2f(cx + 60, cy);

        glVertex2f(410, -205);

        glVertex2f(cx, cy);

        glEnd();

        glLineWidth(1);

        // warning and target lock circle

        if (interceptT > 0.25f)

        {

            glColor3ub(255, 0, 0);

            drawText("WARNING: Incoming object detected", -150, 255);

            drawCircle(cx, cy, 18);

            drawCircle(cx, cy, 28);

        }

        // final impact flash

        if (interceptT > 0.88f)

        {

            glColor3ub(255, 255, 255);

            drawCircle(cx, cy, 18);

            glColor3ub(255, 200, 0);

            drawCircle(cx, cy, 30);

            glColor3ub(255, 80, 0);

            drawCircle(cx, cy, 42);

        }

    }

    else

    {

        if (explosionRadius < 145)

        {

            drawImprovedExplosion(cx, cy);

        }

        else

        {

            // fading smoke after explosion

            glColor3ub(120, 120, 120);

            drawCircle(cx - 40, cy, 35);

            drawCircle(cx + 35, cy + 10, 30);

            drawCircle(cx, cy + 35, 28);

            drawCircle(cx + 10, cy - 30, 25);

            glColor3ub(0, 255, 100);

            drawText("CITY PROTECTED", -40, 140);

            glColor3ub(255, 255, 255);

            drawText("Interceptor destroyed the missile before it reached the city.", -250, 100);

        }

    }

    glColor3ub(255, 255, 255);

    drawText("Improved interception: flash, shockwave, smoke, sparks, and debris.", -360, -325);

}

// ---------------- Reset Function ----------------

// Resets all animation variables to their initial state for the current scenario.
void resetScenario()

{

    truckX = -520.0f;

    launchY = 0.0f;

    flightT = 0.0f;

    interceptT = 0.0f;

    explosionRadius = 5.0f;

    animationStart = false;

    pauseAnimation = false;

}

// ---------------- Display Function ----------------

// Main display function. It clears the screen and draws the selected scenario.
void display()

{

    glClear(GL_COLOR_BUFFER_BIT);

    glLoadIdentity();

    if (currentScenario == 1)

    {

        scenario1();

    }

    else if (currentScenario == 2)

    {

        scenario2();

    }

    else if (currentScenario == 3)

    {

        scenario3();

    }

    else if (currentScenario == 4)

    {

        scenario4();

    }

    glutSwapBuffers();

}

// ---------------- Animation Update ----------------

// Timer-based animation function. It updates movement values every 16 milliseconds.
void update(int value)

{

    if (animationStart && !pauseAnimation)

    {

        cloudMove += 0.8f;

        if (cloudMove > 750)

        {

            cloudMove = -750;

        }

        radarAngle += 3.5f;

        if (radarAngle >= 360)

        {

            radarAngle = 0;

        }

        // Scenario 1 animation: move the truck from left to right until it reaches a stop point.
        if (currentScenario == 1)

        {

            truckX += 2.0f;

            if (truckX > -70)

            {

                truckX = -70;

            }

        }

        // Scenario 2 animation: move the missile vertically upward after launch.
        else if (currentScenario == 2)

        {

            launchY += 1.8f;

            if (launchY > 360)

            {

                launchY = 360;

            }

        }

        // Scenario 3 animation: increase flightT so the missile moves along the curved flight path.
        else if (currentScenario == 3)

        {

            flightT += 0.0045f;

            if (flightT > 1.0f)

            {

                flightT = 1.0f;

            }

        }

        // Scenario 4 animation: first move both missiles toward the intercept point, then expand explosion.
        else if (currentScenario == 4)

        {

            if (interceptT < 1.0f)

            {

                interceptT += 0.006f;

                if (interceptT > 1.0f)

                {

                    interceptT = 1.0f;

                }

            }

            else

            {

                explosionRadius += 1.5f;

                if (explosionRadius > 145)

                {

                    explosionRadius = 145;

                }

            }

        }

    }

    glutPostRedisplay();

    glutTimerFunc(16, update, 0);

}

// ---------------- Keyboard Control ----------------

// Handles keyboard input for start, pause, next scenario, reset, and exit controls.
void keyboard(unsigned char key, int x, int y)

{

    switch (key)

    {

    case 's':

    case 'S':

        // Start the animation.
        animationStart = true;

        break;

    case 'p':

    case 'P':

        // Toggle pause/resume.
        pauseAnimation = !pauseAnimation;

        break;

    case 'n':

    case 'N':

        // Move to the next scenario and reset animation variables.
        currentScenario++;

        if (currentScenario > 4)

        {

            currentScenario = 1;

        }

        resetScenario();

        break;

    case 'r':

    case 'R':

        // Reset current scenario.
        resetScenario();

        break;

    case 27:

        // ESC key closes the program.
        exit(0);

        break;

    }

    glutPostRedisplay();

}

// ---------------- Initialize OpenGL ----------------

// Initializes OpenGL background color and 2D orthographic projection.
void init()

{

    glClearColor(0, 0, 0, 1);

    glMatrixMode(GL_PROJECTION);

    glLoadIdentity();

    gluOrtho2D(-400, 600, -350, 350);

    glMatrixMode(GL_MODELVIEW);

}

// ---------------- Main Function ----------------

// Program entry point. Initializes GLUT, creates the window, and starts the main loop.
int main(int argc, char** argv)

{

    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

    glutInitWindowSize(900, 600);

    glutInitWindowPosition(100, 50);

    glutCreateWindow("Animated Missile Interception Project");

    init();

    glutDisplayFunc(display);

    glutKeyboardFunc(keyboard);

    glutTimerFunc(16, update, 0);

    glutMainLoop();

    return 0;

}

