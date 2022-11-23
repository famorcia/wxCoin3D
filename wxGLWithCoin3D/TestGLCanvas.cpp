//
// Created by coflight on 19/11/22.
//

#include "TestGLCanvas.h"
#include "wx/wx.h"
#include "wx/file.h"
#include "wx/dcclient.h"
#include "wx/wfstream.h"
#if wxUSE_ZLIB
#include "wx/zstream.h"
#endif


// ---------------------------------------------------------------------------
// TestGLCanvas
// ---------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(TestGLCanvas, wxGLCanvas)
                EVT_SIZE(TestGLCanvas::OnSize)
                EVT_PAINT(TestGLCanvas::OnPaint)
                EVT_ERASE_BACKGROUND(TestGLCanvas::OnEraseBackground)
                EVT_MOUSE_EVENTS(TestGLCanvas::OnMouse)
                EVT_TIMER(TIMER_ID, TestGLCanvas::OnTimer)
wxEND_EVENT_TABLE()


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

// This routine is called for every mouse button event.
void
myMousePressCB(void *userData, SoEventCallback *eventCB)
{
    SoGate *gate = (SoGate *) userData;
    const SoEvent *event = eventCB->getEvent();

    std::cout<<"myMousePressCB\n";

    // Check for mouse button being pressed
    if (SO_MOUSE_PRESS_EVENT(event, ANY)) {

        std::cout<<"myMousePressCB 2\n";

        // Toggle the gate that controls the duck motion
        if (gate->enable.getValue()) {
            gate->enable.setValue(FALSE);
            std::cout<<"myMousePressCB 2.1\n";
        }
        else {
            std::cout<<"myMousePressCB 2.2\n";
            gate->enable.setValue(TRUE);
        }
        eventCB->setHandled();
    }
}


int
TestGLCanvas::initSceneGraph(const std::string& fileName)
{
    if(root)
        root->unref();

    root = new SoSeparator;
    root->ref();
#if 0
    // Add a camera, light, and material
    myCamera = new SoPerspectiveCamera;
    root->addChild(myCamera);
    root->addChild(new SoDirectionalLight);
    SoMaterial *myMaterial = new SoMaterial;
    myMaterial->diffuseColor.setValue(1.0, 0.0, 0.0);
    root->addChild(myMaterial);

    // Create a Text3 object, and connect to the realTime field
    SoText3 *myText = new SoText3;
    myText->set("Hello World");
    root->addChild(myText);
    myText->string.connectFrom(SoDB::getGlobalField("realTime"));

#else
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
#endif

    SbViewportRegion myViewport(W, H);
    myCamera->viewAll(root, myViewport);

    return 0;
}


TestGLCanvas::TestGLCanvas(wxWindow *parent,
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
        , m_timer(this, TIMER_ID)
{
    root = 0;
    myCamera = 0;

    // Explicitly create a new rendering context instance for this canvas.
    m_glRC = new wxGLContext(this);

    m_isGLInitialized = false;

    // initialize view matrix
    m_gldata.beginx = 0.0f;
    m_gldata.beginy = 0.0f;
    m_gldata.zoom   = 45.0f;

    SoDB::init();
    SoNodeKit::init();
    SoInteraction::init();


    if(initSceneGraph("duck.iv")) {
        fprintf(stderr, "couldn't read IV file\n");
        exit(1);
    }

    W = size.x;
    H = size.y;
    m_timer.Start(1000/12);    // 1/12 second interval
}

TestGLCanvas::~TestGLCanvas()
{
    delete m_glRC;
}

void TestGLCanvas::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
    // must always be here
    wxPaintDC dc(this);

    SetCurrent(*m_glRC);

    InitGL();

    // Clear
    glClearColor( 0.3f, 0.4f, 0.6f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    SbViewportRegion myViewport(W, H);

    SoGLRenderAction myRenderAction(myViewport);
    myRenderAction.apply(root);

    //*
    static int counter = 0;
    std::clog<<"Ci sono_"<<++counter<<std::endl;
    //*/
    // Flush
    glFlush();

    // Swap
    SwapBuffers();
}

void TestGLCanvas::OnSize(wxSizeEvent& event)
{
    // Reset the OpenGL view aspect.
    // This is OK only because there is only one canvas that uses the context.
    // See the cube sample for that case that multiple canvases are made current with one context.
    W = event.GetSize().x;
    H = event.GetSize().y;
    ResetProjectionMode();
}

void TestGLCanvas::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
    // Do nothing, to avoid flashing on MSW
}

void TestGLCanvas::LoadIV(const wxString& filename)
{
    initSceneGraph(std::string(filename.c_str()));
}

void TestGLCanvas::OnMouse(wxMouseEvent& event)
{
    m_gldata.beginx = event.GetX();
    m_gldata.beginy = event.GetY();

    if (event.Dragging())
    {
        wxSize sz(GetClientSize());

        /* orientation has changed, redraw mesh */
        Refresh(false);
    } else if(event.ButtonDown()) {
        std::cout<<"ButtonDown\n";

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

void TestGLCanvas::InitGL()
{
    if(m_isGLInitialized)
        return;
    ResetProjectionMode();
    glEnable(GL_DEPTH_TEST);
    m_isGLInitialized = true;
}

void TestGLCanvas::ResetProjectionMode()
{
    if ( !IsShownOnScreen() )
        return;

    // This is normally only necessary if there is more than one wxGLCanvas
    // or more than one wxGLContext in the application.
    SetCurrent(*m_glRC);

    const wxSize ClientSize = GetClientSize() * GetContentScaleFactor();

    // It's up to the application code to update the OpenGL viewport settings.
    // In order to avoid extensive context switching, consider doing this in
    // OnPaint() rather than here, though.
    glViewport(0, 0, ClientSize.x, ClientSize.y);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, double(ClientSize.x) / ClientSize.y, 1, 100);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void TestGLCanvas::OnTimer(wxTimerEvent& event)
{
    // do whatever you want to do every second here
    // angle += 0.1;
    SoDB::getSensorManager()->processTimerQueue();
    Refresh(false);
}