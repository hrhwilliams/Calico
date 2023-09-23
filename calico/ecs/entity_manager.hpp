#ifndef _CALICO_ENTITY_MANAGER_HPP_
#define _CALICO_ENTITY_MANAGER_HPP_

namespace Calico {

template <
    std::size_t Max_Components,
    std::size_t Max_Objects>
class EntityManager {
private:
    std::array<std::bitset<Max_Components>, Max_Objects> signatures;
    std::deque<Entity> unallocated_entities;
public:
        EntityManager() {
            for (Entity e = 0; e < Max_Objects; e++) {
                unallocated_entities.push_back(e);
            }
        }

    Entity new_entity() {
        Entity entity = unallocated_entities.front();
        unallocated_entities.pop_front();
        return entity;
    }

    void delete_entity(Entity e) {
        signatures[e].reset();
        unallocated_entities.push_front(e);
    }

    void add_component_to(Entity e, ComponentID id) {
        signatures[e].set(id);
    }

    std::bitset<Max_Components> get_signature(Entity entity) {
        return signatures[entity];
    }
};

}

#endif // _CALICO_ENTITY_MANAGER_HPP_
