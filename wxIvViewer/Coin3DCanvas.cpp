/*
 * wxCoin3D Porting on wxWidgets of Coin3D (a.k.a Open Inventor) examples
 * Copyright (C) 2022  Fabrizio Morciano

 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.

 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 * USA
 */

#include "Coin3DCanvas.h"
#include "wx/wx.h"
#include "wx/file.h"
#include "wx/dcclient.h"

#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/engines/SoGate.h>
#include <Inventor/events/SoMouseButtonEvent.h>
#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/SbViewportRegion.h>
#include <Inventor/SoDB.h>
#include <Inventor/SoInput.h>
#include <Inventor/nodekits/SoNodeKit.h>
#include <Inventor/SoInteraction.h>
#include <Inventor/manips/SoTrackballManip.h>


wxBEGIN_EVENT_TABLE(Coin3DCanvas, wxGLCanvas)
                EVT_SIZE(Coin3DCanvas::OnSize)
                EVT_PAINT(Coin3DCanvas::OnPaint)
                EVT_ERASE_BACKGROUND(Coin3DCanvas::OnEraseBackground)
                EVT_MOUSE_EVENTS(Coin3DCanvas::OnMouse)
                EVT_TIMER(TIMER_ID, Coin3DCanvas::OnTimer)
wxEND_EVENT_TABLE()

Coin3DCanvas::Coin3DCanvas(wxWindow *parent,
                           wxGLAttributes& attributes,
                           wxWindowID id,
                           const wxPoint& pos,
                           const wxSize& size,
                           long style,
                           const wxString& name)
        : wxGLCanvas(parent,
                     attributes,
                     id,
                     pos,
                     size,
                     style | wxFULL_REPAINT_ON_RESIZE,
                     name)
        , timer(this, TIMER_ID)
{
    SoDB::init();
    SoNodeKit::init();
    SoInteraction::init();

    W = size.x;
    H = size.y;


    // Add a camera and light
    SoPerspectiveCamera * myCamera = new SoPerspectiveCamera;
    myCamera->ref();
    myCamera->position.setValue(0., -4., 8.0);
    myCamera->heightAngle = M_PI/2.5;
    myCamera->nearDistance = 1.0;
    myCamera->farDistance = 15.0;

    sceneManager = new SoSceneManager;
    sceneManager->setCamera(myCamera);

    SbViewportRegion viewportRegion;
    viewportRegion.setWindowSize(W, H);
    sceneManager->setViewportRegion(viewportRegion);

    // Explicitly create a new rendering context instance for this canvas.
    glRealContext = new wxGLContext(this);

    isGLInitialized = false;

    timer.Start(1000 / 12);    // 1/12 second interval
}

Coin3DCanvas::~Coin3DCanvas()
{
    delete glRealContext;
}

void Coin3DCanvas::OnPaint(wxPaintEvent& WXUNUSED(event) ) {
    wxPaintDC dc(this);

    InitGL();

    sceneManager->render();

    glFlush();
    SwapBuffers();
}

void Coin3DCanvas::OnSize(wxSizeEvent& event)
{
    W = event.GetSize().x;
    H = event.GetSize().y;
    SbViewportRegion viewportRegion;
    viewportRegion.setWindowSize(W, H);
    sceneManager->setViewportRegion(viewportRegion);
}

void Coin3DCanvas::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
    // Do nothing, to avoid flashing on MSW
}

void Coin3DCanvas::LoadIV(const wxString& fileName)
{
    SoSeparator *root = new SoSeparator;
    root->ref();

    SoTrackballManip * manip = new SoTrackballManip;
    root->addChild(manip);

    // Read the duck object from a file and add to the group
    SoInput myInput;
    SoInput::addDirectoryFirst("./data");

    if (!myInput.openFile(fileName.c_str()))
        exit (1);
    SoSeparator *ivObject = SoDB::readAll(&myInput);
    if (ivObject == NULL)
        exit (1);

    root->addChild(ivObject);

    sceneManager->setSceneGraph(root);
}

void Coin3DCanvas::OnMouse(wxMouseEvent& event)
{
    if (event.Dragging())
    {
        //wxSize sz(GetClientSize());
        SoMouseButtonEvent mouseButtonEvent;
        SO_MOUSE_PRESS_EVENT(&mouseButtonEvent, SoMouseButtonEvent::BUTTON1);
        sceneManager->processEvent(&mouseButtonEvent);
        SO_MOUSE_RELEASE_EVENT(&mouseButtonEvent, SoMouseButtonEvent::BUTTON1);

        /* orientation has changed, redraw mesh */
        Refresh(false);
    } else if(event.ButtonDown()) {
        SoMouseButtonEvent mouseButtonEvent;
        SO_MOUSE_PRESS_EVENT(&mouseButtonEvent, SoMouseButtonEvent::BUTTON1);
        sceneManager->processEvent(&mouseButtonEvent);
        SO_MOUSE_RELEASE_EVENT(&mouseButtonEvent, SoMouseButtonEvent::BUTTON1);

        /* something changed, redraw mesh */
        Refresh(false);
    }
}

void Coin3DCanvas::InitGL()
{
    SetCurrent(*glRealContext);
    if(!isGLInitialized) {
        glEnable(GL_DEPTH_TEST);
        isGLInitialized = true;
    }

    SbColor backColor;
    backColor.setValue(0.3f, 0.4f, 0.6f);
    sceneManager->setBackgroundColor( backColor);
}


void Coin3DCanvas::OnTimer(wxTimerEvent& event)
{
    SoDB::getSensorManager()->processTimerQueue();
    Refresh(false);
}
