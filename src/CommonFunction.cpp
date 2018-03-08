/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. %YEAR%
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    CommonFunction.cpp
 * @brief   To handle pick event
 *******************************************************************************
 */

#include "CommonFunction.h"

#include <string>
#include <QImage>
#include <QGLWidget>
#include <osg/Image>

osgDB::ReaderWriter::ReadResult ReadImageFromResourceCallback::readImage(const std::string& aFileName,
                                                                         const osgDB::Options* aOptions)
{
    if (aFileName.find_first_of(":/") == 0 || aFileName.find_first_of("qrc:///") == 0)
    {
        osg::ref_ptr<osg::Image> osgImage = new osg::Image;
        QImage glImage = QGLWidget::convertToGLFormat(QImage(QString::fromStdString(aFileName)));
        unsigned char* bits = new unsigned char[glImage.byteCount()];
        memcpy(bits, glImage.bits(),glImage.byteCount());
        osgImage->setImage(glImage.width(), glImage.height(), 1, 4, GL_RGBA, GL_UNSIGNED_BYTE, bits, osg::Image::USE_NEW_DELETE);
        return osgImage;
    }
    else
    {
        return osgDB::Registry::instance()->readImageImplementation(aFileName, aOptions);
    }
}
