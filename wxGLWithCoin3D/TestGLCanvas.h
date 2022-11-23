//
// Created by coflight on 19/11/22.
//

#ifndef WXGL_TESTGLCANVAS_H
#define WXGL_TESTGLCANVAS_H

#include <wx/glcanvas.h>
#include <wx/wx.h>
#include <wx/timer.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>


// OpenGL view data
struct GLData
{
    float beginx, beginy;       // position of mouse
    float quat[4];              // orientation of object
    float zoom;                 // field of view in degrees
};

class TestGLCanvas : public wxGLCanvas
{
public:
    TestGLCanvas(wxWindow *parent,
                 wxGLAttributes&,
                 wxWindowID id = wxID_ANY,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize, long style = 0,
                 const wxString& name = "TestGLCanvas"
    );

    virtual ~TestGLCanvas();

    void LoadIV(const wxString& filename);

protected:
    void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnEraseBackground(wxEraseEvent& event);
    void OnMouse(wxMouseEvent& event);
    void OnTimer(wxTimerEvent& event);

private:
    enum {
        TIMER_ID = 3000
    };
    void InitGL();
    void ResetProjectionMode();
    int initSceneGraph(const std::string& fileName);

    bool m_isGLInitialized;
    wxTimer m_timer;
    wxGLContext* m_glRC;
    GLData m_gldata;
    SoSeparator *root;
    SoPerspectiveCamera *myCamera;
    int W;
    int H;
    wxDECLARE_NO_COPY_CLASS(TestGLCanvas);
    wxDECLARE_EVENT_TABLE();
};


#endif //WXGL_TESTGLCANVAS_H
