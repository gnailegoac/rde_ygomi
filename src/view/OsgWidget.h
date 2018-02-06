/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2018
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
#include <memory>
#include <osg/Matrixd>
#include <osg/ref_ptr>
#include <osgViewer/CompositeViewer>
#include <osgViewer/GraphicsWindow>
#include "model/Common.h"

namespace Controller
{
class PickHandler;
}

namespace View
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

    osg::Polytope GetPolytope();
    void Refresh();
    void CameraMatrixChanged(const osg::Matrixd& aMatrix);
    void JumpToCenter(const osg::Vec3d& aCenter);
    void SetSelectType(const Model::SelectType& aSelectType);
    double GetDistance();
protected:
    void paintEvent(QPaintEvent* aPaintEvent) override;
    void paintGL() override;
    void resizeGL(int aWidth, int aHeight) override;

    void keyPressEvent(QKeyEvent* aEvent) override;
    void keyReleaseEvent(QKeyEvent* aEvent) override;

    void mouseMoveEvent(QMouseEvent* aEvent) override;
    void mousePressEvent(QMouseEvent* aEvent) override;
    void mouseReleaseEvent(QMouseEvent* aEvent) override;
    void wheelEvent(QWheelEvent* aEvent) override;

    bool event(QEvent* aEvent) override;

private:
    bool isMouseButtonPressed(QMouseEvent*& aEvent, std::int32_t aButtonMask) const;

    virtual void onHome();
    virtual void onResize(int aWidth, int aHeight);

    osgGA::EventQueue* getEventQueue() const;
    void notifyCameraChange();

private:
    osg::ref_ptr<osgViewer::GraphicsWindowEmbedded> mGraphicsWindow;
    osg::ref_ptr<osgViewer::View> mView;
    osg::ref_ptr<Viewer> mViewer;
    std::shared_ptr<Controller::PickHandler> mPickHandler;
    osg::Vec3d mEye;
    osg::Vec3d mCenter;
    osg::Vec3d mUp;
    bool mSyncMap;
};
}
