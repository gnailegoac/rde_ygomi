/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. %YEAR%
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    OsgWidget.cpp
 * @brief   OsgWidget to handle rendering event
 *******************************************************************************
 */

#include "OsgWidget.h"

#include <cassert>
#include <stdexcept>
#include <string>
#include <vector>

#include <QDebug>
#include <QKeyEvent>
#include <QPainter>
#include <QWheelEvent>

#include <osg/Camera>
#include <osg/DisplaySettings>
#include <osg/Geode>
#include <osg/Material>
#include <osg/Shape>
#include <osg/ShapeDrawable>
#include <osg/StateSet>

#include <osgGA/EventQueue>
#include <osgDB/ReadFile>
#include <osgGA/TerrainManipulator>
#include <osgDB/WriteFile>

#include <osgUtil/IntersectionVisitor>
#include <osgUtil/PolytopeIntersector>

#include <osgViewer/View>
#include <osgViewer/ViewerEventHandlers>

#include "PickHandler.h"

#include "facade/ApplicationFacade.h"
#include "model/GeoJsonConverter.h"
#include "model/SceneModel.h"
#include "proxy/MainProxy.h"
#include "PickHandler.h"

void View::Viewer::setupThreading()
{
    if(_threadingModel == SingleThreaded)
    {
        if(_threadsRunning)
        {
            stopThreading();
        }
    }
    else
    {
        if(!_threadsRunning)
        {
            startThreading();
        }
    }
}

View::OsgWidget::OsgWidget(QWidget* aParent, Qt::WindowFlags aFlag) :
    QOpenGLWidget(aParent, aFlag),
    mGraphicsWindow(new osgViewer::GraphicsWindowEmbedded(this->x(),
                                                          this->y(),
                                                          this->width(),
                                                          this->height())),
    mView(new osgViewer::View),
    mViewer(new View::Viewer),
    mPickHandler(new Controller::PickHandler(this->devicePixelRatio())),
    mSyncMap(false)
{
    float aAspectRatio = static_cast<float>(this->width()) / static_cast<float>(this->height());
    auto aPixelRatio = this->devicePixelRatio();

    osg::Camera* aCamera = new osg::Camera;
    aCamera->setGraphicsContext(mGraphicsWindow);
    aCamera->setClearColor(osg::Vec4( 0.f, 0.f, 0.f, 1.f ));
    aCamera->setViewport(0, 0, this->width() * aPixelRatio, this->height() * aPixelRatio);
    aCamera->setProjectionMatrixAsPerspective(30.f, aAspectRatio, 1.f, 1000.f);

    mView->setCamera(aCamera);
    mView->addEventHandler(new osgViewer::StatsHandler);
    mView->addEventHandler(mPickHandler.get());
    osgGA::TerrainManipulator* aManipulator = new osgGA::TerrainManipulator;
    aManipulator->setAllowThrow(false);

    mView->setCameraManipulator(aManipulator);

    mViewer->addView(mView);
    mViewer->setThreadingModel(osgViewer::CompositeViewer::SingleThreaded);
    mViewer->realize();

    // This ensures that the widget will receive keyboard events. This focus
    // policy is not set by default. The default, Qt::NoFocus, will result in
    // keyboard events that are ignored.
    this->setFocusPolicy(Qt::StrongFocus);
    this->setMinimumSize(600, 400);

    // Ensures that the widget receives mouse move events even though no
    // mouse button has been pressed. We require this in order to let the
    // graphics window switch viewports properly.
    this->setMouseTracking(true);
}

View::OsgWidget::~OsgWidget()
{
}

osg::Polytope View::OsgWidget::GetPolytope()
{
    osg::Matrixd viewMatrix = mView->getCamera()->getViewMatrix();
    osg::Matrixd projectionMatrix = mView->getCamera()->getProjectionMatrix();
    osg::Matrixd viewProjectMatrix = viewMatrix * projectionMatrix;
    osg::Polytope frustum;
    frustum.setToUnitFrustum();
    frustum.transformProvidingInverse(viewProjectMatrix);
    return frustum;
}

void View::OsgWidget::Refresh()
{
    MainProxy& mainProxy = dynamic_cast<MainProxy&>(ApplicationFacade::RetriveProxy(MainProxy::NAME));
    const std::shared_ptr<Model::SceneModel>& sceneModel = mainProxy.GetSceneModel();
    if (sceneModel)
    {
        mView->setSceneData(sceneModel->GetSceneModelRoot());
    }
    mView->getCameraManipulator()->getHomePosition(mEye, mCenter, mUp);
    mEye = mCenter + mUp * 30000.0;
    mView->getCameraManipulator()->setHomePosition(mEye, mCenter, mUp);
    mView->home();
    repaint();
    notifyCameraChange();
}

void View::OsgWidget::CameraMatrixChanged(const osg::Matrixd &aMatrix)
{
    osg::Vec3d eye(aMatrix(3, 0), aMatrix(3, 1), aMatrix(3, 2));
    osg::Vec3d direction(aMatrix(2, 0), aMatrix(2, 1), aMatrix(2, 2));
    osg::Vec3d center = eye - direction * 30000.0;
    osg::Vec3d up(aMatrix(1, 0), aMatrix(1, 1), aMatrix(1, 2));
    mView->getCameraManipulator()->setHomePosition(eye, center, up);
    mView->home();
    repaint();
}

void View::OsgWidget::paintEvent(QPaintEvent* aPaintEvent)
{
    Q_UNUSED(aPaintEvent)
    this->makeCurrent();
    QPainter aPainter(this);
    aPainter.setRenderHint(QPainter::Antialiasing);
    this->paintGL();
    aPainter.end();
    this->doneCurrent();
}

void View::OsgWidget::paintGL()
{
    mViewer->frame();
}

void View::OsgWidget::resizeGL(int aWidth, int aHeight)
{
    this->getEventQueue()->windowResize(this->x(), this->y(), aWidth, aHeight);
    mGraphicsWindow->resized(this->x(), this->y(), aWidth, aHeight);
    this->onResize(aWidth, aHeight);
}

void View::OsgWidget::keyPressEvent(QKeyEvent* aEvent)
{
    QString aKeyString = aEvent->text();
    const char* aKeyData = aKeyString.toLocal8Bit().data();

    if(aEvent->key() == Qt::Key_D)
    {
        osgDB::writeNodeFile( *mViewer->getView(0)->getSceneData(),
                              "./tmp/sceneGraph.osg" );

        return;
    }
    else if(aEvent->key() == Qt::Key_H)
    {
        this->onHome();
        return;
    }

    this->getEventQueue()->keyPress(osgGA::GUIEventAdapter::KeySymbol(*aKeyData));
}

void View::OsgWidget::keyReleaseEvent(QKeyEvent* aEvent)
{
    QString aKeyString = aEvent->text();
    const char* aKeyData = aKeyString.toLocal8Bit().data();

    this->getEventQueue()->keyRelease(osgGA::GUIEventAdapter::KeySymbol(*aKeyData));
}

void View::OsgWidget::mouseMoveEvent(QMouseEvent* aEvent)
{
    if (mSyncMap)
    {
        notifyCameraChange();
    }

    // change traffic sign geometry if someone rotate or drag the camera
    // tightly coupled implementation! maybe refactor this code next time
    if (isMouseButtonPressed(aEvent, 3))
    {
        updateTrafficSignView();
    }

    // Note that we have to check the buttons mask in order to see whether the
    // left button has been pressed. A call to `button()` will only result in
    // `Qt::NoButton` for mouse move events.
    auto aPixelRatio = this->devicePixelRatio();
    this->getEventQueue()->mouseMotion(static_cast<float>(aEvent->x() * aPixelRatio),
                                       static_cast<float>(aEvent->y() * aPixelRatio));
}

void View::OsgWidget::mousePressEvent(QMouseEvent* aEvent)
{
    // 1 = left mouse button
    // 2 = middle mouse button
    // 3 = right mouse button
    unsigned int aButton = 0;
    switch(aEvent->button())
    {
    case Qt::LeftButton:
        aButton = 1;
        break;
    case Qt::MiddleButton:
        aButton = 2;
        break;
    case Qt::RightButton:
        aButton = 3;
        break;
    default:
        break;
    }
    mSyncMap = true;
    auto aPixelRatio = this->devicePixelRatio();
    this->getEventQueue()->mouseButtonPress(static_cast<float>(aEvent->x() * aPixelRatio),
                                            static_cast<float>(aEvent->y() * aPixelRatio),
                                            aButton);
}

void View::OsgWidget::mouseReleaseEvent(QMouseEvent* aEvent)
{
    // 1 = left mouse button
    // 2 = middle mouse button
    // 3 = right mouse button
    unsigned int aButton = 0;
    switch(aEvent->button())
    {
    case Qt::LeftButton:
        aButton = 1;
        break;
    case Qt::MiddleButton:
        aButton = 2;
        break;
    case Qt::RightButton:
        aButton = 3;
        break;
    default:
        break;
    }
    mSyncMap = false;
    auto aPixelRatio = this->devicePixelRatio();
    this->getEventQueue()->mouseButtonRelease(static_cast<float>(aPixelRatio * aEvent->x()),
                                              static_cast<float>(aPixelRatio * aEvent->y()),
                                              aButton);
}

void View::OsgWidget::wheelEvent(QWheelEvent* aEvent)
{
    // Ignore wheel events as long as the selection is active.
    aEvent->accept();
    int aDelta = aEvent->delta();
    osgGA::GUIEventAdapter::ScrollingMotion aMotion = aDelta > 0 ? osgGA::GUIEventAdapter::SCROLL_UP
                                                                 : osgGA::GUIEventAdapter::SCROLL_DOWN;
    this->getEventQueue()->mouseScroll(aMotion);
    notifyCameraChange();
}

bool View::OsgWidget::event(QEvent* aEvent)
{
    bool aHandled = QOpenGLWidget::event(aEvent);
    // This ensures that the OSG widget is always going to be repainted after the
    // user performed some interaction. Doing this in the event handler ensures
    // that we don't forget about some event and prevents duplicate code.
    switch(aEvent->type())
    {
    case QEvent::KeyPress:
    case QEvent::KeyRelease:
    case QEvent::MouseButtonDblClick:
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseMove:
    case QEvent::Wheel:
        this->update();
        break;
    default:
        break;
    }
    return aHandled;
}

bool View::OsgWidget::isMouseButtonPressed(QMouseEvent*& aEvent, std::int32_t aButtonMask) const
{
    return aEvent->buttons() & aButtonMask ;
}

void View::OsgWidget::updateTrafficSignView()
{
    MainProxy& mainProxy = dynamic_cast<MainProxy&>(ApplicationFacade::RetriveProxy(MainProxy::NAME));
    std::shared_ptr<Model::SceneModel>& sceneModel = mainProxy.GetMutableSceneModel();

    if (sceneModel)
    {
        auto manipulator = mView->getCameraManipulator();
        osg::Matrixd matrix =  manipulator->getMatrix();
        sceneModel->RotateTrafficSign(matrix);
        paintGL();
    }
}

void View::OsgWidget::onHome()
{
    mView->getCameraManipulator()->setHomePosition(mEye, mCenter, mUp);
    mView->home();
    notifyCameraChange();
}

void View::OsgWidget::onResize(int aWidth, int aHeight)
{
    auto aPixelRatio = this->devicePixelRatio();
    mView->getCamera()->setViewport(0, 0, aWidth * aPixelRatio, aHeight * aPixelRatio);
}

osgGA::EventQueue* View::OsgWidget::getEventQueue() const
{
    osgGA::EventQueue* aEventQueue = mGraphicsWindow->getEventQueue();
    if(aEventQueue)
    {
        return aEventQueue;
    }
    else
    {
        throw std::runtime_error("Unable to obtain valid event queue");
    }
}

void View::OsgWidget::notifyCameraChange()
{
    osg::Matrixd mat = dynamic_cast<osgGA::TerrainManipulator*>(mView->getCameraManipulator())->getMatrix();
    QJsonArray cameraMatrix = Model::GeoJsonConverter().Convert(mat);
    ApplicationFacade::SendNotification(ApplicationFacade::CHANGE_CAMERA, &cameraMatrix);
}
