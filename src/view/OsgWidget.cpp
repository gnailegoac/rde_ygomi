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
#include <osgGA/TrackballManipulator>
#include <osgDB/WriteFile>

#include <osgUtil/IntersectionVisitor>
#include <osgUtil/PolytopeIntersector>

#include <osgViewer/View>
#include <osgViewer/ViewerEventHandlers>

#include "PickHandler.h"

namespace
{

QRect MakeRectangle(const QPoint& first, const QPoint& second)
{
    // Relative to the first point, the second point may be in either one of the
    // four quadrants of an Euclidean coordinate system.
    //
    // We enumerate them in counter-clockwise order, starting from the lower-right
    // quadrant that corresponds to the default case:
    //
    //            |
    //       (3)  |  (4)
    //            |
    //     -------|-------
    //            |
    //       (2)  |  (1)
    //            |

    if(second.x() >= first.x() && second.y() >= first.y())
    {
        return QRect(first, second);
    }
    else if(second.x() < first.x() && second.y() >= first.y())
    {
        return QRect(QPoint(second.x(), first.y()), QPoint(first.x(), second.y()));
    }
    else if(second.x() < first.x() && second.y() < first.y())
    {
        return QRect(second, first);
    }
    else if(second.x() >= first.x() && second.y() < first.y())
    {
        return QRect(QPoint(first.x(), second.y()), QPoint(second.x(), first.y()));
    }

    // Should never reach that point...
    return QRect();
}

}

void Render::Viewer::setupThreading()
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

Render::OsgWidget::OsgWidget(QWidget* aParent, Qt::WindowFlags aFlag)
  : QOpenGLWidget(aParent, aFlag)
  , mGraphicsWindow(new osgViewer::GraphicsWindowEmbedded(this->x(),
                                                          this->y(),
                                                          this->width(),
                                                          this->height()))
  , mViewer(new Render::Viewer)
  , mSelectionActive(false)
  , mSelectionFinished(true)
{
    float aAspectRatio = static_cast<float>(this->width()) / static_cast<float>(this->height());
    auto aPixelRatio = this->devicePixelRatio();

    osg::Camera* aCamera = new osg::Camera;
    aCamera->setViewport(0, 0, this->width() * aPixelRatio, this->height() * aPixelRatio);
    aCamera->setClearColor(osg::Vec4( 0.f, 0.f, 0.f, 1.f ));
    aCamera->setProjectionMatrixAsPerspective(30.f, aAspectRatio, 1.f, 1000.f);
    aCamera->setGraphicsContext(mGraphicsWindow);

    osgViewer::View* aView = new osgViewer::View;
    aView->setCamera(aCamera);
    aView->addEventHandler(new osgViewer::StatsHandler);

    osgGA::TrackballManipulator* aManipulator = new osgGA::TrackballManipulator;
    aManipulator->setAllowThrow(false);

    aView->setCameraManipulator(aManipulator);

    mViewer->addView(aView);
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

Render::OsgWidget::~OsgWidget()
{
}

void Render::OsgWidget::paintEvent(QPaintEvent* aPaintEvent)
{
    this->makeCurrent();
    QPainter aPainter(this);
    aPainter.setRenderHint(QPainter::Antialiasing);
    this->paintGL();
    if(mSelectionActive && !mSelectionFinished)
    {
        aPainter.setPen(Qt::red);
        aPainter.setBrush(Qt::transparent);
        aPainter.drawRect(MakeRectangle(mSelectionStart, mSelectionEnd));
    }
    aPainter.end();
    this->doneCurrent();
}

void Render::OsgWidget::paintGL()
{
    mViewer->frame();
}

void Render::OsgWidget::resizeGL(int aWidth, int aHeight)
{
    this->getEventQueue()->windowResize(this->x(), this->y(), aWidth, aHeight);
    mGraphicsWindow->resized(this->x(), this->y(), aWidth, aHeight);
    this->onResize(aWidth, aHeight);
}

void Render::OsgWidget::keyPressEvent(QKeyEvent* aEvent)
{
    QString aKeyString = aEvent->text();
    const char* aKeyData = aKeyString.toLocal8Bit().data();

    if(aEvent->key() == Qt::Key_S)
    {
        mSelectionActive = !mSelectionActive;
        // Further processing is required for the statistics handler here, so we do
        // not return right away.
    }
    else if(aEvent->key() == Qt::Key_D)
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

void Render::OsgWidget::keyReleaseEvent(QKeyEvent* aEvent)
{
    QString aKeyString = aEvent->text();
    const char* aKeyData = aKeyString.toLocal8Bit().data();

    this->getEventQueue()->keyRelease(osgGA::GUIEventAdapter::KeySymbol(*aKeyData));
}

void Render::OsgWidget::mouseMoveEvent(QMouseEvent* aEvent)
{
    // Note that we have to check the buttons mask in order to see whether the
    // left button has been pressed. A call to `button()` will only result in
    // `Qt::NoButton` for mouse move events.
    if(mSelectionActive && (aEvent->buttons() & Qt::LeftButton))
    {
        mSelectionEnd = aEvent->pos();

        // Ensures that new paint events are created while the user moves the
        // mouse.
        this->update();
    }
    else
    {
        auto aPixelRatio = this->devicePixelRatio();
        this->getEventQueue()->mouseMotion(static_cast<float>(aEvent->x() * aPixelRatio),
                                           static_cast<float>(aEvent->y() * aPixelRatio));
    }
}

void Render::OsgWidget::mousePressEvent(QMouseEvent* aEvent)
{
    // Selection processing
    if(mSelectionActive && aEvent->button() == Qt::LeftButton)
    {
        mSelectionStart = aEvent->pos();
        mSelectionEnd = mSelectionStart;   // Deletes the old selection
        mSelectionFinished = false;        // As long as this is set, the rectangle will be drawn
    }
    // Normal processing
    else
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
        auto aPixelRatio = this->devicePixelRatio();
        this->getEventQueue()->mouseButtonPress(static_cast<float>(aEvent->x() * aPixelRatio),
                                                static_cast<float>(aEvent->y() * aPixelRatio),
                                                aButton);
    }
}

void Render::OsgWidget::mouseReleaseEvent(QMouseEvent* aEvent)
{
    // Selection processing: Store end position and obtain selected objects
    // through polytope intersection.
    if(mSelectionActive && aEvent->button() == Qt::LeftButton)
    {
        mSelectionEnd = aEvent->pos();
        mSelectionFinished = true; // Will force the painter to stop drawing the selection rectangle
        this->processSelection();
    }
    // Normal processing
    else
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
        auto aPixelRatio = this->devicePixelRatio();
        this->getEventQueue()->mouseButtonRelease(static_cast<float>(aPixelRatio * aEvent->x()),
                                                  static_cast<float>(aPixelRatio * aEvent->y()),
                                                  aButton);
    }
}

void Render::OsgWidget::wheelEvent(QWheelEvent* aEvent)
{
    // Ignore wheel events as long as the selection is active.
    if(mSelectionActive)
    {
        return;
    }
    aEvent->accept();
    int aDelta = aEvent->delta();
    osgGA::GUIEventAdapter::ScrollingMotion aMotion = aDelta > 0 ? osgGA::GUIEventAdapter::SCROLL_UP
                                                                 : osgGA::GUIEventAdapter::SCROLL_DOWN;
    this->getEventQueue()->mouseScroll(aMotion);
}

bool Render::OsgWidget::event(QEvent* aEvent)
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

void Render::OsgWidget::onHome()
{
    osgViewer::ViewerBase::Views aViews;
    mViewer->getViews(aViews);

    for(std::size_t i = 0; i < aViews.size(); ++i)
    {
        osgViewer::View* aView = aViews.at(i);
        aView->home();
    }
}

void Render::OsgWidget::onResize(int aWidth, int aHeight)
{
    std::vector<osg::Camera*> aCameras;
    mViewer->getCameras(aCameras);
    auto aPixelRatio = this->devicePixelRatio();
    aCameras[0]->setViewport(0, 0, aWidth * aPixelRatio, aHeight * aPixelRatio);
}

osgGA::EventQueue* Render::OsgWidget::getEventQueue() const
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

void Render::OsgWidget::processSelection()
{
    QRect selectionRectangle = MakeRectangle(mSelectionStart, mSelectionEnd);
    auto widgetHeight        = this->height();
    auto pixelRatio          = this->devicePixelRatio();

    double xMin = selectionRectangle.left();
    double xMax = selectionRectangle.right();
    double yMin = widgetHeight - selectionRectangle.bottom();
    double yMax = widgetHeight - selectionRectangle.top();

    xMin *= pixelRatio;
    yMin *= pixelRatio;
    xMax *= pixelRatio;
    yMax *= pixelRatio;

    osgUtil::PolytopeIntersector* polytopeIntersector
            = new osgUtil::PolytopeIntersector(osgUtil::PolytopeIntersector::WINDOW,
                                               xMin, yMin, xMax, yMax);

    // This limits the amount of intersections that are reported by the
    // polytope intersector. Using this setting, a single drawable will
    // appear at most once while calculating intersections. This is the
    // preferred and expected behaviour.
    polytopeIntersector->setIntersectionLimit(osgUtil::Intersector::LIMIT_ONE_PER_DRAWABLE);

    osgUtil::IntersectionVisitor aIntersectionVisitor(polytopeIntersector);

    for(unsigned int viewIndex = 0; viewIndex < mViewer->getNumViews(); viewIndex++)
    {
        qDebug() << "View index:" << viewIndex;

        osgViewer::View* view = mViewer->getView(viewIndex);

        if(!view)
        {
            throw std::runtime_error("Unable to obtain valid view for selection processing");
        }

        osg::Camera* camera = view->getCamera();

        if(!camera)
        {
            throw std::runtime_error("Unable to obtain valid camera for selection processing");
        }

        camera->accept(aIntersectionVisitor);

        if(!polytopeIntersector->containsIntersections())
        {
            continue;
        }

        auto intersections = polytopeIntersector->getIntersections();

        for(auto&& intersection : intersections)
        {
            qDebug() << "Selected a drawable:" << QString::fromStdString(intersection.drawable->getName());
        }
    }
}
