/*
* Copyright (c) 2006-2007 Erin Catto http://www.box2d.org
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*
* This source code is altered from the original version.
*
*/

#include "debug_draw.h"
#include "settings.h"
#include <GL/glui.h>
#include <signal.h>

#ifdef __APPLE__
	#include <GLUT/glut.h>
#else
	#include <GL/freeglut.h>
#endif

Settings    settings;
int32       width;
int32       height;
int32       drawPeriod = 16;
int32       updatePeriod = 1;
int32       mainWindow;
GLUI*       glui;
float32     viewZoom = 12.0f;
int         tx,
            ty,
            tw,
            th;
void (*exitHandler)(int);
void (*gameUpdateHandler)(Settings*);
void (*gameDrawHandler)(Settings*);

void resize(int32 w, int32 h)
{
	width = w;
	height = h;

	GLUI_Master.get_viewport_area(&tx, &ty, &tw, &th);
	glViewport(tx, ty, tw, th);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	float32 ratio = float32(tw) / float32(th);

	b2Vec2 extents(ratio * 25.0f, 25.0f);
	extents *= viewZoom;

	b2Vec2 lower = settings.viewCenter - extents;
	b2Vec2 upper = settings.viewCenter + extents;

	// L/R/B/T
	gluOrtho2D(lower.x, upper.x, lower.y, upper.y);
}

b2Vec2 convertScreenToWorld(int32 x, int32 y)
{
	float32 u = x / float32(tw);
	float32 v = (th - y) / float32(th);

	float32 ratio = float32(tw) / float32(th);
	b2Vec2 extents(ratio * 25.0f, 25.0f);
	extents *= viewZoom;

	b2Vec2 lower = settings.viewCenter - extents;
	b2Vec2 upper = settings.viewCenter + extents;

	b2Vec2 p;
	p.x = (1.0f - u) * lower.x + u * upper.x;
	p.y = (1.0f - v) * lower.y + v * upper.y;
	return p;
}

void drawTimer(int)
{
	glutSetWindow(mainWindow);
	glutPostRedisplay();
	glutTimerFunc(drawPeriod, drawTimer, 0);
}

void updateTimer(int)
{
	gameUpdateHandler(&settings);
	glutTimerFunc(updatePeriod, updateTimer, 0);
}

void drawLoop()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

    gameDrawHandler(&settings);

	glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y)
{
	B2_NOT_USED(x);
	B2_NOT_USED(y);

	switch (key)
	{
        case 27:
#ifndef __APPLE__
            // freeglut specific function
            glutLeaveMainLoop();
#endif
            exit(0);
            break;
		// Press 'z' to zoom out.
        case 'z':
            viewZoom = b2Min(1.1f * viewZoom, 20.0f);
            resize(width, height);
            break;
		// Press 'x' to zoom in.
        case 'x':
            viewZoom = b2Max(0.9f * viewZoom, 0.02f);
            resize(width, height);
            break;
	}
}

void keyboardSpecial(int key, int x, int y)
{
	B2_NOT_USED(x);
	B2_NOT_USED(y);

	switch (key)
	{
        case GLUT_ACTIVE_SHIFT:
        // Press left to pan left.
        case GLUT_KEY_LEFT:
            settings.viewCenter.x -= 0.5f;
            resize(width, height);
            break;
        // Press right to pan right.
        case GLUT_KEY_RIGHT:
            settings.viewCenter.x += 0.5f;
            resize(width, height);
            break;
        // Press down to pan down.
        case GLUT_KEY_DOWN:
            settings.viewCenter.y -= 0.5f;
            resize(width, height);
            break;
        // Press up to pan up.
        case GLUT_KEY_UP:
            settings.viewCenter.y += 0.5f;
            resize(width, height);
            break;
        // Press home to reset the view.
        case GLUT_KEY_HOME:
            viewZoom = 1.0f;
            settings.viewCenter.Set(0.0f, 20.0f);
            resize(width, height);
            break;
	}
}

void keyboardUp(unsigned char key, int x, int y)
{
	B2_NOT_USED(x);
	B2_NOT_USED(y);
}

void mouse(int32 button, int32 state, int32 x, int32 y)
{
	// Use the mouse to move things around.
	if (button == GLUT_LEFT_BUTTON)
	{
		int mod = glutGetModifiers();
		if (state == GLUT_DOWN)
		{
			if (mod == GLUT_ACTIVE_SHIFT)
            {
            }
			else
            {
            }
		}
		else if (state == GLUT_UP)
        {
        }
	}
	else if (button == GLUT_RIGHT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
		}
		else if (state == GLUT_UP)
        {
        }
	}
}

void mouseMotion(int32 x, int32 y)
{
}

void mouseWheel(int wheel, int direction, int x, int y)
{
	B2_NOT_USED(wheel);
	B2_NOT_USED(x);
	B2_NOT_USED(y);
	if (direction > 0)
        viewZoom /= 1.1f;
	else
        viewZoom *= 1.1f;
	resize(width, height);
}

void exit_(int code)
{
	// TODO: freeglut is not building on OSX.
#ifdef FREEGLUT
	glutLeaveMainLoop();
#endif
	raise(SIGINT);
}

void launchTestbed(void (*gameUpdateHandler_)(Settings*), void (*gameDrawHandler_)(Settings*),
                   int argc, char** argv, float width_, float height_)
{
    gameUpdateHandler = gameUpdateHandler_;
    gameDrawHandler = gameDrawHandler_;
    width = width_;
    height = height_;
    settings.viewCenter.x = width / 2;
    settings.viewCenter.y = height / 2;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(width, height);
	char title[64];
	sprintf(title, "Bomberboys Testbed, Box2D Version %d.%d.%d", b2_version.major, b2_version.minor, b2_version.revision);
	mainWindow = glutCreateWindow(title);
	//glutSetOption (GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

	glutDisplayFunc(drawLoop);
	GLUI_Master.set_glutReshapeFunc(resize);
	GLUI_Master.set_glutKeyboardFunc(keyboard);
	GLUI_Master.set_glutSpecialFunc(keyboardSpecial);
	GLUI_Master.set_glutMouseFunc(mouse);
#ifdef FREEGLUT
	glutMouseWheelFunc(mouseWheel);
#endif
	glutMotionFunc(mouseMotion);

	glutKeyboardUpFunc(keyboardUp);

	glui = GLUI_Master.create_glui_subwindow(mainWindow, GLUI_SUBWINDOW_RIGHT);

	glui->add_statictext("Game Params");

	glui->add_separator();

	GLUI_Spinner* velocityIterationSpinner =
		glui->add_spinner("Vel Iters", GLUI_SPINNER_INT, &settings.velocityIterations);
	velocityIterationSpinner->set_int_limits(1, 500);

	GLUI_Spinner* positionIterationSpinner =
		glui->add_spinner("Pos Iters", GLUI_SPINNER_INT, &settings.positionIterations);
	positionIterationSpinner->set_int_limits(0, 100);

	GLUI_Spinner* hertzSpinner =
		glui->add_spinner("Hertz", GLUI_SPINNER_FLOAT, &settings.hz);

	hertzSpinner->set_float_limits(5.0f, 200.0f);

	glui->add_checkbox("Warm Starting", &settings.enableWarmStarting);
	glui->add_checkbox("Time of Impact", &settings.enableContinuous);
	glui->add_checkbox("Sub-Stepping", &settings.enableSubStepping);

	GLUI_Panel* drawPanel =	glui->add_panel("Draw");
	glui->add_checkbox_to_panel(drawPanel, "Shapes", &settings.drawShapes);
	glui->add_checkbox_to_panel(drawPanel, "Joints", &settings.drawJoints);
	glui->add_checkbox_to_panel(drawPanel, "AABBs", &settings.drawAABBs);
	glui->add_checkbox_to_panel(drawPanel, "Pairs", &settings.drawPairs);
	glui->add_checkbox_to_panel(drawPanel, "Center of Masses", &settings.drawCOMs);
	glui->add_checkbox_to_panel(drawPanel, "Statistics", &settings.drawStats);
	glui->add_checkbox_to_panel(drawPanel, "Profile", &settings.drawProfile);

	glui->add_button("Quit", 0,(GLUI_Update_CB)exit_);
	glui->set_main_gfx_window(mainWindow);

	glutTimerFunc(drawPeriod, drawTimer, 0);
	glutTimerFunc(updatePeriod, updateTimer, 0);

	glutMainLoop();

	resize(width, height);
}
