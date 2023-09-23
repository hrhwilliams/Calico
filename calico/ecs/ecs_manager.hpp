#ifndef _CALICO_ECS_MANAGER_HPP_
#define _CALICO_ECS_MANAGER_HPP_

namespace Calico {

constexpr std::size_t Max_Components = 64;
constexpr std::size_t Max_Objects = 65535;

class ECSManager {
private:
    std::unique_ptr<ComponentManager<Max_Components, Max_Objects>> component_manager =
        std::make_unique<ComponentManager<Max_Components, Max_Objects>>();
    std::unique_ptr<EntityManager<Max_Components, Max_Objects>> entity_manager =
        std::make_unique<EntityManager<Max_Components, Max_Objects>>();
    std::unique_ptr<SystemManager<Max_Components>> system_manager =
        std::make_unique<SystemManager<Max_Components>>();
    std::unique_ptr<EventManager> event_manager = std::make_unique<EventManager>();
    std::unique_ptr<AssetManager> asset_manager = std::make_unique<AssetManager>();
public:
    ECSManager() {}

    // System manipulation functions
    template <typename System>
    std::shared_ptr<System> register_system() {
        auto system = system_manager->template register_system<System>(this);
        system->init_events(*event_manager.get());

        return system;
    }

    template <
        typename System,
        typename Component>
    void add_system_signature() {
        auto component_id = component_manager->template get_component_id<Component>();
        system_manager->template add_signature<System>(component_id);
    }

    // Component manipulation functions
    template <typename Component>
    void register_component() {
        component_manager->template register_component<Component>();
    }

    // Entity manipulation functions
    Entity new_entity() {
        return entity_manager->new_entity();
    }

    void delete_entity(Entity entity) {
        return entity_manager->delete_entity(entity);
    }

    template <typename Component>
    void add_component_to(Entity entity, Component component) {
        auto component_id = component_manager->template get_component_id<Component>();
        entity_manager->add_component_to(entity, component_id);
        component_manager->template add_component_to<Component>(entity, component);
        system_manager->on_add_component(entity, entity_manager->get_signature(entity));
    }

    template <typename Component, typename... Args>
    void add_component_to(Entity entity, Args&&... args) {
        if constexpr(sizeof...(Args) > 0) {
            add_component_to(entity, Component(std::forward<Component>(args...)));
        } else {
            add_component_to(entity, Component());
        }
    }

    template <typename Component>
    Component &get_component(Entity entity) {
        return component_manager->template get_component<Component>(entity);
    }

    // Events
    void broadcast(const Event &event) {
        event_manager->broadcast(event);
    }

    // Assets
    template <typename Asset>
    void register_asset() {
        asset_manager->template register_asset<Asset>();
    }

    template <typename Asset, typename... Args>
    Asset &add_asset(const std::string &name, Args&&... args) {
        return asset_manager->template add_asset<Asset>(name, std::forward<Args>(args)...);
    }

    template <typename Asset>
    Asset &get_asset(const std::string &name) {
        auto asset = asset_manager->template get_asset<Asset>(name);
        if (asset) {
            return asset.value();
        } else {
            throw std::runtime_error("No such asset");
        }
    }

    template <typename Asset>
    std::unordered_map<std::string, Asset> &get_all_assets() {
        return asset_manager->template get_all_assets<Asset>();
    }
};

}

#endif // _CALICO_ECS_MANAGER_HPP_
