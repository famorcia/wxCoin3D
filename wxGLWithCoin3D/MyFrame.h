//
// Created by coflight on 19/11/22.
//

#ifndef WXGL_FRAME_H
#define WXGL_FRAME_H

#include <wx/frame.h>
#include "wx/defs.h"

class TestGLCanvas;

class MyFrame : public wxFrame
{
public:
    MyFrame(wxFrame *frame, const wxString& title, const wxPoint& pos,
            const wxSize& size, long style = wxDEFAULT_FRAME_STYLE);

    void OnMenuFileOpen(wxCommandEvent& event);
    void OnMenuFileExit(wxCommandEvent& event);
    void OnMenuHelpAbout(wxCommandEvent& event);

    void SetCanvas(TestGLCanvas *canvas) { m_canvas = canvas; }
    TestGLCanvas *GetCanvas() { return m_canvas; }

private:
    TestGLCanvas *m_canvas;

    wxDECLARE_EVENT_TABLE();
};


#endif //WXGL_FRAME_H
