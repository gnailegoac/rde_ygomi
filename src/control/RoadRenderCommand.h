#ifndef ROADRENDERCOMMAND_H
#define ROADRENDERCOMMAND_H

#include "BasicCommand.h"
#include <osg/Polytope>

namespace Controller
{
class RoadRenderCommand : public BasicCommand
{
public:
    RoadRenderCommand();
    ~RoadRenderCommand();
    void OpenRoadRendering();
    void CloseRoadRendering();
};
}

#endif // ROADRENDERCOMMAND_H
