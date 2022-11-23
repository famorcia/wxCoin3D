import wx.glcanvas
from TestGLCanvas import TestGLCanvas


class MyFrame(wx.Frame):
    def __init__(self, parent, title):
        try:
            wx.Frame.__init__(self,
                              parent,
                              id=wx.ID_ANY,
                              title=title,  # wx.EmptyString,
                              pos=wx.DefaultPosition,
                              size=wx.Size(500, 300),
                              style=wx.DEFAULT_FRAME_STYLE | wx.TAB_TRAVERSAL)
            self.Show(True)
            attribs = [wx.glcanvas.WX_GL_DEPTH_SIZE, 32]
            TestGLCanvas(self, attribs)

        except Exception as e:
            print(e)

    def __del__(self):
        pass
