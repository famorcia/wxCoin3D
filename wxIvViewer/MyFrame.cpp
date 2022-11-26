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

#include "MyFrame.h"
#include "Coin3DCanvas.h"

#include "wx/menu.h"
#include "wx/glcanvas.h"
#include "wx/wx.h"

#include "../resources/wxc3d.xpm"

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
    SetIcon(wxICON(wxc3d));

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
    m_canvas = new Coin3DCanvas(this,
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
