/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. %YEAR%
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    OsgWidget.h
 * @brief   OsgWidget to handle rendering event
 *******************************************************************************
 */

#pragma once

#include <QOpenGLWidget>
#include <QPoint>

#include <osg/ref_ptr>

#include <osgViewer/CompositeViewer>
#include <osgViewer/GraphicsWindow>

namespace Render
{
//! The subclass of osgViewer::CompositeViewer we use
/*!
* This subclassing allows us to remove the annoying automatic
* setting of the CPU affinity to core 0 by osgViewer::ViewerBase,
* osgViewer::CompositeViewer's base class.
*/
class Viewer : public osgViewer::CompositeViewer
{
public:
    virtual void setupThreading();
};

class OsgWidget : public QOpenGLWidget
{
    Q_OBJECT

public:
    OsgWidget(QWidget* aParent = 0,
              Qt::WindowFlags aFlag = 0);

    virtual ~OsgWidget();

protected:

    virtual void paintEvent(QPaintEvent* aPaintEvent);
    virtual void paintGL();
    virtual void resizeGL(int aWidth, int aHeight);

    virtual void keyPressEvent(QKeyEvent* aEvent);
    virtual void keyReleaseEvent(QKeyEvent* aEvent);

    virtual void mouseMoveEvent(QMouseEvent* aEvent);
    virtual void mousePressEvent(QMouseEvent* aEvent);
    virtual void mouseReleaseEvent(QMouseEvent* aEvent);
    virtual void wheelEvent(QWheelEvent* aEvent);

    virtual bool event(QEvent* aEvent);

private:

    virtual void onHome();
    virtual void onResize(int aWidth, int aHeight);

    osgGA::EventQueue* getEventQueue() const;

    osg::ref_ptr<osgViewer::GraphicsWindowEmbedded> mGraphicsWindow;
    osg::ref_ptr<Viewer> mViewer;

    QPoint mSelectionStart;
    QPoint mSelectionEnd;

    bool mSelectionActive;
    bool mSelectionFinished;

    void processSelection();
};
}
