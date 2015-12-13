#ifndef VIEW_FACTORY_H
#define VIEW_FACTORY_H

#include <memory>
#include <string>

class View;
class World_map;

struct View_factory_return {
    std::shared_ptr<View> view_ptr;
    std::weak_ptr<World_map> world_map_ptr;
};

View_factory_return create_view(const std::string& name);

#endif // VIEW_FACTORY_H
