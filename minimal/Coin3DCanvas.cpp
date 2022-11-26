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
#include "wx/wfstream.h"

wxBEGIN_EVENT_TABLE(Coin3DCanvas, wxGLCanvas)
                EVT_SIZE(Coin3DCanvas::OnSize)
                EVT_PAINT(Coin3DCanvas::OnPaint)
                EVT_ERASE_BACKGROUND(Coin3DCanvas::OnEraseBackground)
                EVT_TIMER(TIMER_ID, Coin3DCanvas::OnTimer)
wxEND_EVENT_TABLE()

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/engines/SoElapsedTime.h>
#include <Inventor/events/SoMouseButtonEvent.h>
#include <Inventor/nodes/SoCylinder.h>
#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoRotationXYZ.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/SbViewportRegion.h>
#include <Inventor/SoDB.h>
#include <Inventor/SoInput.h>
#include <Inventor/nodekits/SoNodeKit.h>
#include <Inventor/SoInteraction.h>


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
    root = 0;
    myCamera = 0;

    // Explicitly create a new rendering context instance for this canvas.
    glRealContext = new wxGLContext(this);
    isGLInitialized = false;

    SoDB::init();
    SoNodeKit::init();
    SoInteraction::init();

    if(InitDuckGraph()) {
        std::cerr<<"Couldn't read IV file"<<std::endl;
        exit(1);
    }

    W = size.x;
    H = size.y;
    const int timer_fire_interval = 1000/12;  // 1/12 second interval
    timer.Start(timer_fire_interval);
}

Coin3DCanvas::~Coin3DCanvas()
{
    delete glRealContext;
    root->unref();
}

void Coin3DCanvas::OnPaint(wxPaintEvent& WXUNUSED(event) )
{
    // must always be here
    wxPaintDC dc(this);

    InitGL();

    // Set the viewport
    SbViewportRegion myViewport(W, H);

    // Apply the render action for viewing the Coin3D on GL Window
    SoGLRenderAction myRenderAction(myViewport);
    myRenderAction.apply(root);

    glFlush();
    SwapBuffers();
}

void Coin3DCanvas::OnSize(wxSizeEvent& event)
{
    // on size Coin need to know the new view port
    W = event.GetSize().x;
    H = event.GetSize().y;
}

void Coin3DCanvas::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
    // Do nothing, to avoid flashing on MSW
}

void Coin3DCanvas::OnTimer(wxTimerEvent& event)
{
    // Very important, Coin need to process internal timer, this need to be performed in the canvas periodically
    SoDB::getSensorManager()->processTimerQueue();
    Refresh(false);
}

void Coin3DCanvas::InitGL()
{
    SetCurrent(*glRealContext);
    if(!isGLInitialized) {
        glEnable(GL_DEPTH_TEST);
        isGLInitialized = true;
    }
    glClearColor( 0.3f, 0.4f, 0.6f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

int Coin3DCanvas::InitDuckGraph()
{
    root = new SoSeparator;
    root->ref();

    // Add a camera and light
    myCamera = new SoPerspectiveCamera;
    myCamera->position.setValue(0., -4., 8.0);
    myCamera->heightAngle = M_PI/2.5;
    myCamera->nearDistance = 1.0;
    myCamera->farDistance = 15.0;
    root->addChild(myCamera);
    root->addChild(new SoDirectionalLight);

    // Rotate scene slightly to get better view
    SoRotationXYZ * globalRotXYZ = new SoRotationXYZ;
    globalRotXYZ->axis = SoRotationXYZ::X;
    globalRotXYZ->angle = M_PI/9;
    root->addChild(globalRotXYZ);

    // Pond group
    SoSeparator *pond = new SoSeparator;
    root->addChild(pond);
    SoTranslation *pondTranslation = new SoTranslation;
    pondTranslation->translation.setValue(0., -6.725, 0.);
    pond->addChild(pondTranslation);
    // water
    SoMaterial *waterMaterial = new SoMaterial;
    waterMaterial->diffuseColor.setValue(0., 0.3, 0.8);
    pond->addChild(waterMaterial);
    SoCylinder *waterCylinder = new SoCylinder;
    waterCylinder->radius.setValue(4.0);
    waterCylinder->height.setValue(0.5);
    pond->addChild(waterCylinder);

    // Read the duck object from a file and add to the group
    SoInput myInput;
    SoInput::addDirectoryFirst("./data");

    if (!myInput.openFile("duck.iv")) {
        return (1);
    }
    SoSeparator *duckObject = SoDB::readAll(&myInput);
    if (duckObject == NULL) {
        return (1);
    }

    // Duck group
    SoSeparator *duck = new SoSeparator;
    root->addChild(duck);
    SoRotationXYZ *duckRotXYZ = new SoRotationXYZ;
    duck->addChild(duckRotXYZ);
    SoTransform *initialTransform = new SoTransform;
    initialTransform->translation.setValue(0., 0., 3.5);
    initialTransform->scaleFactor.setValue(6., 6., 6.);
    duck->addChild(initialTransform);
    duck->addChild(duckObject);

    // Attach the duck rotation to the elapsed time
    SoElapsedTime * bigDuckTime = new SoElapsedTime;
    bigDuckTime->ref();
    duckRotXYZ->axis = SoRotationXYZ::Y;  // Y axis
    duckRotXYZ->angle.connectFrom(&bigDuckTime->timeOut);

    // Set the viewport for having a better view
    SbViewportRegion myViewport(W, H);
    myCamera->viewAll(root, myViewport);

    return (0);
}

