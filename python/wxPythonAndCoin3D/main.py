# App contains the main of wx
import wx
from MyFrame import MyFrame

AppBaseClass = wx.App


class MyApp(AppBaseClass):
    def OnInit(self):
        frame = MyFrame(None, "Simple wxPython App")
        self.SetTopWindow(frame)
        frame.Show(True)
        return True


if __name__ == "__main__":
    app = MyApp(redirect=False)
    app.MainLoop()
