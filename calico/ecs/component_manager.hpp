#ifndef _CALICO_COMPONENT_MANAGER_HPP_
#define _CALICO_COMPONENT_MANAGER_HPP_

namespace Calico {

// Interface for the `ComponentArray`s instantiated by the `ComponentManager`
struct IComponentArray {
    virtual ~IComponentArray() = default;
    virtual void on_entity_destroyed(Entity entity) = 0;
};

// Manages `Component`s which can be associated with entities allocated by the game engine.
// 
template <
    std::size_t Max_Components,
    std::size_t Max_Objects>
class ComponentManager {

    template <typename C>
    struct ComponentArray final : public IComponentArray {
    private:
        std::array<C, Max_Objects> component_array;
        std::unordered_map<Entity, Index> entity_to_component;
        std::unordered_map<Index, Entity> component_to_entity;
        const char *name = typeid(C).name(); // debug
        std::size_t current_index = 0;
    public:
        ComponentArray() = default;
        ~ComponentArray() = default;

        void insert_entity(Entity entity, C component) {
            component_array[current_index] = component;
            entity_to_component.insert({ entity, current_index });
            component_to_entity.insert({ current_index, entity });
            current_index++;
        }

        C &get_component(Entity entity) {
            return component_array[entity_to_component[entity]];
        }

        void on_entity_destroyed(Entity entity) override {
            // do nothing if entity is not registered to this component
            if (!entity_to_component.contains(entity)) {
                return;
            }

            // nothing to swap with if this is the last entity registered
            // to this component
            if (current_index == 1) {
                current_index--;
                return;
            }

            // swap last index and index of destroyed entity
            // set swapped entity's index to destroyed entity's index
            // delete destroyed entity
            Index component_to_swap = current_index - 1;
            Entity entity_to_swap = component_to_entity[component_to_swap];

            std::swap(get_component(entity), get_component(entity_to_swap));
            std::swap(entity_to_component[entity], entity_to_component[entity_to_swap]);
            // entity_to_component[entity] = component_to_swap;
            // entity_to_component[entity_to_swap] = component;
            std::printf("%s notified that %d was destroyed\n", this->name, entity);
            current_index--;
        }
    };

    std::unordered_map<const char*, ComponentID> component_ids = {};
    std::unordered_map<const char*, std::unique_ptr<IComponentArray>> components = {};
    ComponentID next_component_id = 0;

    template <typename C>
    ComponentArray<C> *get_array() {
        return reinterpret_cast<ComponentArray<C>*>(components[typeid(C).name()].get());
    }

public:
    template <typename C>
    void register_component() {
        component_ids.insert({ typeid(C).name(), next_component_id++ });
        components.insert({ typeid(C).name(), std::make_unique<ComponentArray<C>>() });
    }

    template <typename C>
    void add_component_to(Entity entity, C component) {
        get_array<C>()->insert_entity(entity, component);
    }

    template <typename Component>
    Component &get_component(Entity entity) {
        return get_array<Component>()->get_component(entity);
    }

    template <typename Component>
    ComponentID get_component_id() {
        return component_ids[typeid(Component).name()];
    }

    void entity_destroyed(Entity entity) {
        // inform all component arrays that `entity` was destroyed
        for (const auto &[name, component] : components) {
            component->on_entity_destroyed(entity);
        }
    }
};

}

#endif // _CALICO_COMPONENT_MANAGER_HPP_
