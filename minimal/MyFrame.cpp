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

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
wxEND_EVENT_TABLE()

MyFrame::MyFrame(wxFrame *frame,
                 const wxString& title,
                 const wxPoint& pos,
                 const wxSize& size,
                 long style)
        : wxFrame(frame, wxID_ANY, title, pos, size, style)
{
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
