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

#include <Inventor/SoDB.h>
#include <Inventor/SoInput.h>
#include <Inventor/SbViewportRegion.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/nodes/SoCylinder.h>
#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoRotationXYZ.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoTranslation.h>

int W = 300, H = 300;
int spinning = 0;
//GLubyte *image = NULL;
SoSeparator *root = 0;
SoRotationXYZ *duckRotXYZ;
float angle = 0.0;
int moving  = 0;
int begin;


int
inventorScene(const std::string& fileName)
{
    if(root)
        root->unref();

    root = new SoSeparator;
    root->ref();

    // Add a camera and light
    SoPerspectiveCamera *myCamera = new SoPerspectiveCamera;
    myCamera->position.setValue(0., -4., 8.0);
    myCamera->heightAngle = M_PI/2.5;
    myCamera->nearDistance = 1.0;
    myCamera->farDistance = 15.0;
    root->addChild(myCamera);
    root->addChild(new SoDirectionalLight);

    // Rotate scene slightly to get better view
    SoRotationXYZ *globalRotXYZ = new SoRotationXYZ;
    globalRotXYZ->axis = SoRotationXYZ::X;
    globalRotXYZ->angle = M_PI/9;
    root->addChild(globalRotXYZ);

    // Pond group
    SoSeparator *pond = new SoSeparator;
    root->addChild(pond);
    SoMaterial *cylMaterial = new SoMaterial;
    cylMaterial->diffuseColor.setValue(0., 0.3, 0.8);
    pond->addChild(cylMaterial);
    SoTranslation *cylTranslation = new SoTranslation;
    cylTranslation->translation.setValue(0., -6.725, 0.);
    pond->addChild(cylTranslation);
    SoCylinder *myCylinder = new SoCylinder;
    myCylinder->radius.setValue(4.0);
    myCylinder->height.setValue(0.5);
    pond->addChild(myCylinder);

    // Duck group
    SoSeparator *duck = new SoSeparator;
    root->addChild(duck);

    // Read the duck object from a file and add to the group
    SoInput myInput;
    if(fileName.empty()) {
        if (!myInput.openFile("/usr/share/src/Inventor/examples/data/duck.iv")) {
            return 1;
        }
    } else if (!myInput.openFile(fileName.c_str()))  {
        return 1;
    }
    SoSeparator *duckObject = SoDB::readAll(&myInput);
    if (duckObject == NULL) return 1;

    // Set up the duck transformations
    duckRotXYZ = new SoRotationXYZ;
    duck->addChild(duckRotXYZ);
    duckRotXYZ->angle = angle;
    duckRotXYZ->axis = SoRotationXYZ::Y;  // rotate about Y axis
    SoTransform *initialTransform = new SoTransform;
    initialTransform->translation.setValue(0., 0., 3.);
    initialTransform->scaleFactor.setValue(6., 6., 6.);
    duck->addChild(initialTransform);
    duck->addChild(duckObject);

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
    // Explicitly create a new rendering context instance for this canvas.
    m_glRC = new wxGLContext(this);

    m_gldata.initialized = false;

    // initialize view matrix
    m_gldata.beginx = 0.0f;
    m_gldata.beginy = 0.0f;
    m_gldata.zoom   = 45.0f;


    if(inventorScene("duck.iv")) {
        fprintf(stderr, "couldn't read IV file\n");
        exit(1);
    }

    W = size.x;
    H = size.y;
    m_timer.Start(50);    // 50 milli second interval

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

    // Initialize OpenGL
    if (!m_gldata.initialized)
    {
        InitGL();
        ResetProjectionMode();
        m_gldata.initialized = true;
    }

    // Clear
    glClearColor( 0.3f, 0.4f, 0.6f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    duckRotXYZ->angle = angle;

    SbViewportRegion myViewport(W, H);
    SoGLRenderAction myRenderAction(myViewport);
    myRenderAction.apply(root);

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
    inventorScene(std::string(filename.c_str()));
}

void TestGLCanvas::OnMouse(wxMouseEvent& event)
{
    if (event.Dragging())
    {
        wxSize sz(GetClientSize());

        /* orientation has changed, redraw mesh */
        Refresh(false);
    }

    m_gldata.beginx = event.GetX();
    m_gldata.beginy = event.GetY();
}

void TestGLCanvas::InitGL()
{
    glEnable(GL_DEPTH_TEST);
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
    angle += 0.1;
    Refresh(false);
}