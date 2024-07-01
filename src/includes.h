#pragma once

#include <z0/z0.h>

#ifndef USE_PCH
#include <z0/nodes/node.h>
#include <z0/resources/image.h>
#include <z0/resources/texture.h>
#include <z0/resources/material.h>
#include <z0/resources/mesh.h>
#include <z0/resources/cubemap.h>
#include <z0/resources/font.h>
#include <z0/resources/shape.h>
#include <z0/nodes/mesh_instance.h>
#include <z0/resources/convex_hull_shape.h>
#include <z0/resources/sub_shape.h>
#include <z0/resources/mesh_shape.h>
#include <z0/resources/static_compound_shape.h>

#include <z0/application.h>
#include <z0/input.h>
#include <z0/loader.h>

#include <z0/nodes/camera.h>
#include <z0/nodes/collision_object.h>
#include <z0/nodes/collision_area.h>
#include <z0/nodes/physics_body.h>
#include <z0/nodes/rigid_body.h>
#include <z0/nodes/kinematic_body.h>
#include <z0/nodes/static_body.h>
#include <z0/nodes/character.h>
#include <z0/nodes/skybox.h>
#include <z0/nodes/raycast.h>
#include <z0/nodes/light.h>
#include <z0/nodes/directional_light.h>
#include <z0/nodes/omni_light.h>
#include <z0/nodes/spot_light.h>
#include <z0/nodes/environment.h>
#include <z0/nodes/viewport.h>

#include <z0/framebuffers/shadow_map_frame_buffer.h>
#include <z0/renderers/renderpass.h>
#include <z0/renderers/skybox_renderer.h>
#include <z0/renderers/shadowmap_renderer.h>
#include <z0/renderers/postprocessing_renderer.h>
#include <z0/renderers/simple_postprocessing_renderer.h>
#include <z0/renderers/models_renderer.h>
#include <z0/renderers/scene_renderer.h>
#include <z0/renderers/vector_renderer.h>

#include <z0/gui/gresource.h>
#include <z0/gui/gstyle.h>
#include <z0/gui/gevent.h>
#include <z0/gui/gwindow.h>
#include <z0/gui/gmanager.h>
#include <z0/gui/gwidget.h>
#include <z0/gui/gbox.h>
#include <z0/gui/gpanel.h>
#include <z0/gui/gtext.h>
#include <z0/gui/gbutton.h>
#include <z0/gui/gcheck_widget.h>
#include <z0/gui/gtoggle_button.h>
#include <z0/gui/gline.h>
#include <z0/gui/gframe.h>
#include <z0/gui/gvalue_select.h>
#include <z0/gui/gscroll_bar.h>
#endif

using namespace z0;