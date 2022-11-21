#include "MyFrame.h"
#include "TestGLCanvas.h"

#include "wx/menu.h"
#include "wx/glcanvas.h"
#include "wx/wx.h"

#include "sample.xpm"

// ---------------------------------------------------------------------------
// MyFrame
// ---------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
                EVT_MENU(wxID_OPEN, MyFrame::OnMenuFileOpen)
                EVT_MENU(wxID_EXIT, MyFrame::OnMenuFileExit)
                EVT_MENU(wxID_HELP, MyFrame::OnMenuHelpAbout)
wxEND_EVENT_TABLE()

// MyFrame constructor
MyFrame::MyFrame(wxFrame *frame, const wxString& title, const wxPoint& pos,
                 const wxSize& size, long style)
        : wxFrame(frame, wxID_ANY, title, pos, size, style)
{
    SetIcon(wxICON(sample));

// Make the "File" menu
    wxMenu *fileMenu = new wxMenu;
    fileMenu->Append(wxID_OPEN, "&Open...");
    fileMenu->AppendSeparator();
    fileMenu->Append(wxID_EXIT, "E&xit\tALT-X");
// Make the "Help" menu
    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(wxID_HELP, "&About");

    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(fileMenu, "&File");
    menuBar->Append(helpMenu, "&Help");
    SetMenuBar(menuBar);

    Show(true);

    wxGLAttributes dispAttrs;
    dispAttrs.PlatformDefaults().MinRGBA(8, 8, 8, 8).DoubleBuffer().Depth(32).EndList();
    m_canvas = new TestGLCanvas(this,
                                dispAttrs,
                                wxID_ANY,
                                wxDefaultPosition,
                                GetClientSize(),
                                wxSUNKEN_BORDER);
}

// File|Open... command
void MyFrame::OnMenuFileOpen( wxCommandEvent& WXUNUSED(event) )
{
    wxString filename = wxFileSelector("Choose IV Model", "", "", "",
                                       "Open Inventor (*.iv)|*.iv|All files (*.*)|*.*",
                                       wxFD_OPEN);
    if (!filename.IsEmpty())
    {
        m_canvas->LoadIV(filename);
        m_canvas->Refresh(false);
    }
}

// File|Exit command
void MyFrame::OnMenuFileExit( wxCommandEvent& WXUNUSED(event) )
{
    // true is to force the frame to close
    Close(true);
}

// Help|About command
void MyFrame::OnMenuHelpAbout( wxCommandEvent& WXUNUSED(event) )
{
    wxMessageBox("Base on OpenGL Penguin Sample (c) Robert Roebling, Sandro Sigala et al");
}
