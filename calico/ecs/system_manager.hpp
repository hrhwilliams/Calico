#ifndef _CALICO_SYSTEM_MANAGER_HPP_
#define _CALICO_SYSTEM_MANAGER_HPP_

#include <bitset>
#include <memory>
#include <set>
#include <typeinfo>
#include <typeindex>
#include <unordered_map>

namespace Calico {

class ECSManager;

class System {
protected:
    std::set<Entity> entities = {};
    ECSManager *ecs;
public:
    System(ECSManager *ecs) : ecs(ecs) {}

    virtual void init_events(EventManager &manager) = 0;

    void add_entity(Entity entity) {
        entities.insert(entity);
    }
};

template <
    std::size_t Max_Components>
class SystemManager {
private:
    std::unordered_map<std::type_index, std::shared_ptr<System>> systems = {};
    std::unordered_map<std::shared_ptr<System>, std::bitset<Max_Components>> signatures = {};
public:
    template <typename T>
    std::shared_ptr<T> register_system(ECSManager *ecs) {
        auto system = std::make_shared<T>(ecs);
        systems.insert({ std::type_index(typeid(T)), system });
        signatures.insert({ system, {}});
        return system;
    }

    template <typename T>
    void add_signature(ComponentID id) {
        signatures[systems[std::type_index(typeid(T))]].set(id);
    }

    void on_add_component(Entity entity, std::bitset<Max_Components> new_signature) {
        for (auto &[index, system] : systems) {
            if ((signatures.at(system) & new_signature) == signatures.at(system)) {
                system->add_entity(entity);
            }
        }
    }
};

}

#endif // _CALICO_SYSTEM_MANAGER_HPP_
