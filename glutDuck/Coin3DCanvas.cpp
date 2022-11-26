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
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/engines/SoBoolOperation.h>
#include <Inventor/engines/SoCompose.h>
#include <Inventor/engines/SoElapsedTime.h>
#include <Inventor/engines/SoGate.h>
#include <Inventor/events/SoMouseButtonEvent.h>
#include <Inventor/nodes/SoCylinder.h>
#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoRotationXYZ.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoText3.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/SbViewportRegion.h>
#include <Inventor/SoDB.h>
#include <Inventor/SoInput.h>
#include <Inventor/events/SoMotion3Event.h>
#include <Inventor/nodekits/SoNodeKit.h>
#include <Inventor/SoInteraction.h>

wxBEGIN_EVENT_TABLE(Coin3DCanvas, wxGLCanvas)
                EVT_SIZE(Coin3DCanvas::OnSize)
                EVT_PAINT(Coin3DCanvas::OnPaint)
                EVT_ERASE_BACKGROUND(Coin3DCanvas::OnEraseBackground)
                EVT_MOUSE_EVENTS(Coin3DCanvas::OnMouse)
                EVT_TIMER(TIMER_ID, Coin3DCanvas::OnTimer)
wxEND_EVENT_TABLE()


// This routine is called for every mouse button event.
void
myMousePressCB(void *userData, SoEventCallback *eventCB)
{
    SoGate *gate = (SoGate *) userData;
    const SoEvent *event = eventCB->getEvent();

    // Check for mouse button being pressed
    if (SO_MOUSE_PRESS_EVENT(event, ANY)) {
        // Toggle the gate that controls the duck motion
        if (gate->enable.getValue()) {
            gate->enable.setValue(FALSE);
        }
        else {
            gate->enable.setValue(TRUE);
        }
        eventCB->setHandled();
    }
}

int
Coin3DCanvas::initSceneGraph(const std::string& fileName)
{
    if(root)
        root->unref();

    root = new SoSeparator;
    root->ref();

    // Add a camera and light
    //SoPerspectiveCamera *
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
    // rock
    SoMaterial *rockMaterial = new SoMaterial;
    rockMaterial->diffuseColor.setValue(0.8, 0.23, 0.03);
    pond->addChild(rockMaterial);
    SoSphere *rockSphere = new SoSphere;
    rockSphere->radius.setValue(0.9);
    pond->addChild(rockSphere);

    // Read the duck object from a file and add to the group
    SoInput myInput;
    SoInput::addDirectoryFirst("./data");

    if (!myInput.openFile(fileName.c_str()))
        exit (1);
    SoSeparator *duckObject = SoDB::readAll(&myInput);
    if (duckObject == NULL)
        exit (1);

/////////////////////////////////////////////////////////////
// CODE FOR The Inventor Mentor STARTS HERE

    // Bigger duck group
    SoSeparator *bigDuck = new SoSeparator;
    root->addChild(bigDuck);
    SoRotationXYZ * bigDuckRotXYZ = new SoRotationXYZ;
    bigDuck->addChild(bigDuckRotXYZ);
    SoTransform *bigInitialTransform = new SoTransform;
    bigInitialTransform->translation.setValue(0., 0., 3.5);
    bigInitialTransform->scaleFactor.setValue(6., 6., 6.);
    bigDuck->addChild(bigInitialTransform);
    bigDuck->addChild(duckObject);

    // Smaller duck group
    SoSeparator *smallDuck = new SoSeparator;
    root->addChild(smallDuck);
    SoRotationXYZ *smallDuckRotXYZ = new SoRotationXYZ;
    smallDuck->addChild(smallDuckRotXYZ);
    SoTransform *smallInitialTransform = new SoTransform;
    smallInitialTransform->translation.setValue(0., -2.24, 1.5);
    smallInitialTransform->scaleFactor.setValue(4., 4., 4.);
    smallDuck->addChild(smallInitialTransform);
    smallDuck->addChild(duckObject);

    // Use a gate engine to start/stop the rotation of
    // the bigger duck.
    SoGate *bigDuckGate = new SoGate(SoMFFloat::getClassTypeId());
    bigDuckGate->ref();
    SoElapsedTime * bigDuckTime = new SoElapsedTime;
    bigDuckTime->ref();
    bigDuckGate->input->connectFrom(&bigDuckTime->timeOut);
    bigDuckRotXYZ->axis = SoRotationXYZ::Y;  // Y axis
    bigDuckRotXYZ->angle.connectFrom(bigDuckGate->output);

    // Each mouse button press will enable/disable the gate
    // controlling the bigger duck.
    SoEventCallback *myEventCB = new SoEventCallback;
    myEventCB->addEventCallback(
            SoMouseButtonEvent::getClassTypeId(),
            myMousePressCB, bigDuckGate);
    root->addChild(myEventCB);

    // Use a Boolean engine to make the rotation of the smaller
    // duck depend on the bigger duck.  The smaller duck moves
    // only when the bigger duck is still.
    SoBoolOperation *myBoolean = new SoBoolOperation;
    myBoolean->ref();
    myBoolean->a.connectFrom(&bigDuckGate->enable);
    myBoolean->operation = SoBoolOperation::NOT_A;

    SoGate * smallDuckGate = new SoGate(SoMFFloat::getClassTypeId());
    smallDuckGate->ref();
    SoElapsedTime * smallDuckTime = new SoElapsedTime;
    smallDuckTime->ref();
    smallDuckGate->input->connectFrom(&smallDuckTime->timeOut);
    smallDuckGate->enable.connectFrom(&myBoolean->output);
    smallDuckRotXYZ->axis = SoRotationXYZ::Y;  // Y axis
    smallDuckRotXYZ->angle.connectFrom(smallDuckGate->output);

// CODE FOR The Inventor Mentor ENDS HERE
/////////////////////////////////////////////////////////////

    SbViewportRegion myViewport(W, H);
    myCamera->viewAll(root, myViewport);

    return 0;
}


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

    if(initSceneGraph("duck.iv")) {
        fprintf(stderr, "couldn't read IV file\n");
        exit(1);
    }

    W = size.x;
    H = size.y;
    timer.Start(1000 / 12);    // 1/12 second interval
}

Coin3DCanvas::~Coin3DCanvas()
{
    delete glRealContext;
}

void Coin3DCanvas::OnPaint(wxPaintEvent& WXUNUSED(event) ) {
    wxPaintDC dc(this);

    InitGL();

    SbViewportRegion myViewport(W, H);

    SoGLRenderAction myRenderAction(myViewport);
    myRenderAction.apply(root);

    glFlush();
    SwapBuffers();
}

void Coin3DCanvas::OnSize(wxSizeEvent& event)
{
    W = event.GetSize().x;
    H = event.GetSize().y;
}

void Coin3DCanvas::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
    // Do nothing, to avoid flashing on MSW
}

void Coin3DCanvas::LoadIV(const wxString& filename)
{
    initSceneGraph(std::string(filename.c_str()));
}

void Coin3DCanvas::OnMouse(wxMouseEvent& event)
{
    if (event.Dragging())
    {
        wxSize sz(GetClientSize());

        /* orientation has changed, redraw mesh */
        Refresh(false);
    } else if(event.ButtonDown()) {
        SbViewportRegion myViewport(W, H);
        SoHandleEventAction handleEventAction(myViewport);
        SoMouseButtonEvent mouseButtonEvent;
        mouseButtonEvent.setButton(SoMouseButtonEvent::ANY);
        mouseButtonEvent.setState(SoButtonEvent::DOWN);
        handleEventAction.setEvent(&mouseButtonEvent);
        handleEventAction.apply(root);
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

    // Clear
    glClearColor( 0.3f, 0.4f, 0.6f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}


void Coin3DCanvas::OnTimer(wxTimerEvent& event)
{
    SoDB::getSensorManager()->processTimerQueue();
    Refresh(false);
}


void  Coin3DCanvas::InitEmptyScene() {

}