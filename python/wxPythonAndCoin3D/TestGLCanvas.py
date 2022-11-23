import wx
from wx.glcanvas import GLCanvas
from wx.glcanvas import GLContext
from pivy.coin import *
from OpenGL.GL import *
from OpenGL.GLUT import *


# This routine is called for every mouse button event.
def myMousePressCB(gate, eventCB):
    event = eventCB.getEvent()

    print("myMousePressCB")
    # Check for mouse button being pressed
    if SoMouseButtonEvent.isButtonPressEvent(event, SoMouseButtonEvent.ANY):

        # Toggle the gate that controls the duck motion
        if gate.enable.getValue():
            gate.enable = FALSE
        else:
            gate.enable = TRUE

        eventCB.setHandled()


class TestGLCanvas(GLCanvas):
    TIMER_ID = 1

    def __init__(self, parent, attribs):
        GLCanvas.__init__(self, parent, attribList=attribs)

        self.init = False
        self.context = GLContext(self)

        # initial mouse position
        self.lastx = self.x = 30
        self.lasty = self.y = 30
        self.width = None
        self.height = None
        self.size = None
        self.timer = wx.Timer(self, TestGLCanvas.TIMER_ID)
        self.timer.Start(1000 / 12)

        self.root = None

        self.Bind(wx.EVT_ERASE_BACKGROUND, self.OnEraseBackground)
        self.Bind(wx.EVT_SIZE, self.OnSize)
        self.Bind(wx.EVT_PAINT, self.OnPaint)
        self.Bind(wx.EVT_LEFT_DOWN, self.OnMouseDown)
        self.Bind(wx.EVT_LEFT_UP, self.OnMouseUp)
        self.Bind(wx.EVT_TIMER, self.OnTimer)

        self.init_coin3d()

    def init_coin3d(self):
        SoDB.init()
        if not SoDB.isInitialized():
            SoDB.init

        self.root = SoSeparator()
        root = self.root

        # Add a camera and light
        myCamera = SoPerspectiveCamera()
        myCamera.position = (0., -4., 8.0)
        myCamera.heightAngle = M_PI / 2.5
        myCamera.nearDistance = 1.0
        myCamera.farDistance = 15.0
        root.addChild(myCamera)
        root.addChild(SoDirectionalLight())

        # Rotate scene slightly to get better view
        globalRotXYZ = SoRotationXYZ()
        globalRotXYZ.axis = SoRotationXYZ.X
        globalRotXYZ.angle = M_PI / 9
        root.addChild(globalRotXYZ)

        # Pond group
        pond = SoSeparator()
        root.addChild(pond)
        pondTranslation = SoTranslation()
        pondTranslation.translation = (0., -6.725, 0.)
        pond.addChild(pondTranslation)
        # water
        waterMaterial = SoMaterial()
        waterMaterial.diffuseColor = (0., 0.3, 0.8)
        pond.addChild(waterMaterial)
        waterCylinder = SoCylinder()
        waterCylinder.radius = 4.0
        waterCylinder.height = 0.5
        pond.addChild(waterCylinder)
        # rock
        rockMaterial = SoMaterial()
        rockMaterial.diffuseColor = (0.8, 0.23, 0.03)
        pond.addChild(rockMaterial)
        rockSphere = SoSphere()
        rockSphere.radius = 0.9
        pond.addChild(rockSphere)

        # Read the duck object from a file and add to the group
        myInput = SoInput()
        if not myInput.openFile("duck.iv"):
            sys.exit(1)
        duckObject = SoDB.readAll(myInput)
        if duckObject == None:
            sys.exit(1)

        #############################################################
        # CODE FOR The Inventor Mentor STARTS HERE

        # Bigger duck group
        bigDuck = SoSeparator()
        root.addChild(bigDuck)
        bigDuckRotXYZ = SoRotationXYZ()
        bigDuck.addChild(bigDuckRotXYZ)
        bigInitialTransform = SoTransform()
        bigInitialTransform.translation = (0., 0., 3.5)
        bigInitialTransform.scaleFactor = (6., 6., 6.)
        bigDuck.addChild(bigInitialTransform)
        bigDuck.addChild(duckObject)

        # Smaller duck group
        smallDuck = SoSeparator()
        root.addChild(smallDuck)
        smallDuckRotXYZ = SoRotationXYZ()
        smallDuck.addChild(smallDuckRotXYZ)
        smallInitialTransform = SoTransform()
        smallInitialTransform.translation = (0., -2.24, 1.5)
        smallInitialTransform.scaleFactor = (4., 4., 4.)
        smallDuck.addChild(smallInitialTransform)
        smallDuck.addChild(duckObject)

        # Use a gate engine to start/stop the rotation of
        # the bigger duck.
        bigDuckGate = SoGate(SoMFFloat.getClassTypeId())
        bigDuckTime = SoElapsedTime()
        bigDuckGate.input.connectFrom(bigDuckTime.timeOut)
        bigDuckRotXYZ.axis = SoRotationXYZ.Y  # Y axis
        bigDuckRotXYZ.angle.connectFrom(bigDuckGate.output)

        # Each mouse button press will enable/disable the gate
        # controlling the bigger duck.
        myEventCB = SoEventCallback()
        myEventCB.addEventCallback(SoMouseButtonEvent.getClassTypeId(),
                                   myMousePressCB,
                                   bigDuckGate)
        root.addChild(myEventCB)

        # Use a Boolean engine to make the rotation of the smaller
        # duck depend on the bigger duck.  The smaller duck moves
        # only when the bigger duck is still.
        myBoolean = SoBoolOperation()
        myBoolean.a.connectFrom(bigDuckGate.enable)
        myBoolean.operation = SoBoolOperation.NOT_A

        smallDuckGate = SoGate(SoMFFloat.getClassTypeId())
        smallDuckTime = SoElapsedTime()
        smallDuckGate.input.connectFrom(smallDuckTime.timeOut)
        smallDuckGate.enable.connectFrom(myBoolean.output)
        smallDuckRotXYZ.axis = SoRotationXYZ.Y  # Y axis
        smallDuckRotXYZ.angle.connectFrom(smallDuckGate.output)

    def OnEraseBackground(self, event):
        pass  # Do nothing, to avoid flashing on MSW.

    def OnSize(self, event):
        wx.CallAfter(self.DoSetViewport)
        event.Skip()

    def DoSetViewport(self):
        size = self.size = self.GetClientSize()
        self.SetCurrent(self.context)
        glViewport(0, 0, size.width, size.height)
        self.width = size.width
        self.height = size.height

    def OnPaint(self, event):
        self.SetCurrent(self.context)
        self.InitGL()
        self.OnDraw()

    def OnMouseDown(self, evt):
        self.x, self.y = self.lastx, self.lasty = evt.GetPosition()
        myViewport = SbViewportRegion(self.width, self.height)
        handleEventAction = SoHandleEventAction(myViewport)
        mouseButtonEvent = SoMouseButtonEvent()
        mouseButtonEvent.setButton(button=SoMouseButtonEvent.BUTTON1)
        mouseButtonEvent.setState(state=SoButtonEvent.DOWN)
        handleEventAction.setEvent(mouseButtonEvent)
        handleEventAction.apply(self.root)
        self.Refresh(False)
        SoDB.getSensorManager().processTimerQueue()
        evt.Skip()

    def OnMouseUp(self, evt):
        print("OnMouseUp")
        evt.Skip()

    def OnMouseMotion(self, evt):
        if evt.Dragging() and evt.LeftIsDown():
            self.lastx, self.lasty = self.x, self.y
            self.x, self.y = evt.GetPosition()
            self.Refresh(False)

    # inizializziamo il canvas
    def InitGL(self):
        if self.init:
            return
        glEnable(GL_DEPTH_TEST)
        self.init = True

    def OnDraw(self, *args, **kwargs):
        self.SetCurrent(self.context)
        glClearColor(0.3, 0.4, 0.6, 1)
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)

        myViewport = SbViewportRegion(self.width, self.height)
        myRenderAction = SoGLRenderAction(myViewport)
        myRenderAction.apply(self.root)

        glFlush()
        self.SwapBuffers()
        self.Refresh(False)

    def OnTimer(self, event):
        SoDB.getSensorManager().processTimerQueue()
        self.Refresh(False)
