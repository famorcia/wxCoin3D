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

#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/engines/SoElapsedTime.h>
#include <Inventor/nodes/SoCylinder.h>
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/SbViewportRegion.h>
#include <Inventor/SoDB.h>

#include <Inventor/nodes/SoComplexity.h>
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoTexture2.h>
#include <Inventor/nodes/SoTranslation.h>

#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/nodes/SoRotationXYZ.h>


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
    root = nullptr;
    globeSpin = nullptr;
    angle = 0;

    // Explicitly create a new rendering context instance for this canvas.
    glRealContext = new wxGLContext(this);
    isGLInitialized = false;

    SoDB::init();

    W = static_cast<short>(size.x);
    H = static_cast<short>(size.y);
    constexpr int timer_fire_interval = 1000/12;  // 1/12 second interval
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

    initGL();

    glClearColor( 0.3f, 0.4f, 0.6f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    SbViewportRegion myViewport(W, H);
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
    angle += 0.01;
    globeSpin->angle = angle;
    Refresh(false);
}

void Coin3DCanvas::initGL()
{
    SetCurrent(*glRealContext);
    if(!isGLInitialized) {
        glEnable(GL_DEPTH_TEST);
        isGLInitialized = true;
        globeScene();
    }
}

void  Coin3DCanvas::globeScene(){
    root = new SoSeparator;
    root->ref();

    // Add a camera and light
    SoPerspectiveCamera *myCamera = new SoPerspectiveCamera;
    myCamera->position.setValue(0., 0., 2.2);
    myCamera->heightAngle = M_PI/2.5;
    myCamera->nearDistance = 0.5;
    myCamera->farDistance = 10.0;
    root->addChild(myCamera);
    root->addChild(new SoDirectionalLight);

    SoRotationXYZ *globalRotXYZ = new SoRotationXYZ;
    globalRotXYZ->axis = SoRotationXYZ::X;
    globalRotXYZ->angle = M_PI/9;
    root->addChild(globalRotXYZ);

    // Set up the globe transformations
    globeSpin = new SoRotationXYZ;
    root->addChild(globeSpin);
    globeSpin->angle = angle;
    globeSpin->axis = SoRotationXYZ::Y;  // rotate about Y axis

    // Add the globe, a sphere with a texture map.
    // Put it within a separator.
    SoSeparator *sphereSep = new SoSeparator;
    SoTexture2  *myTexture2 = new SoTexture2;
    SoComplexity *sphereComplexity = new SoComplexity;
    sphereComplexity->value = 0.55;
    root->addChild(sphereSep);
    sphereSep->addChild(myTexture2);
    sphereSep->addChild(sphereComplexity);
    sphereSep->addChild(new SoSphere);
    myTexture2->filename = "globe.rgb";
}

