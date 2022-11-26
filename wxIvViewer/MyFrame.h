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

#ifndef WXGL_FRAME_H
#define WXGL_FRAME_H

#include <wx/frame.h>
#include "wx/defs.h"

class Coin3DCanvas;

class MyFrame : public wxFrame
{
public:
    MyFrame(wxFrame *frame,
            const wxString& title,
            const wxPoint& pos,
            const wxSize& size,
            long style = wxDEFAULT_FRAME_STYLE);

    void OnMenuFileOpen(wxCommandEvent& event);
    void OnMenuFileExit(wxCommandEvent& event);
    void OnMenuHelpAbout(wxCommandEvent& event);

private:
    Coin3DCanvas *m_canvas;

    wxDECLARE_EVENT_TABLE();
};


#endif //WXGL_FRAME_H
