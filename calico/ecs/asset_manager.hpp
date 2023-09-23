#ifndef __CALICO_ASSET_MANAGER_HPP__
#define __CALICO_ASSET_MANAGER_HPP__

namespace Calico {

// Interface for the `AssetMap`s instantiated by the `AssetManager`
struct IAssetMap {
    virtual ~IAssetMap() = default;
};

// Manages storing and retrieving `Asset`s which are usable throughout the game engine.
// An `Asset` is any class that represents data usable by multiple entities, such as
// meshes or textures, or usable by systems, such as shader code or config.
//
// `Asset`s are registered to the manager, which instantiates an `AssetMap` to act as
// a storage interface of a map connecting string identifiers to instances of the `Asset`.
class AssetManager {

    // Map of typeids of `Asset` classes to the storage classes implementing the string to
    // asset map for them
    std::unordered_map<std::type_index, std::unique_ptr<IAssetMap>> asset_arrays = {};

    template <typename Asset>
    struct AssetMap final : public IAssetMap {
        std::unordered_map<std::string, Asset> assets = {};

        // Add an asset to the map by passing a string identifier and any arguments needed
        // to construct the asset in-place
        template <typename... Args>
        Asset &add_asset(const std::string &name, Args&&... args) noexcept {
            assets.emplace(std::make_pair(name, Asset(args...)));
            return assets.at(name);
        }

        std::optional<std::reference_wrapper<Asset>> get_asset(const std::string &name) {
            if (assets.contains(name)) {
                // need to dereference the iterator over using `at` or `[]` to avoid a copy
                return assets.find(name)->second;
            }

            return std::nullopt;
        }
    };

    // Access the storage class holding `Asset` from the asset manager's map of asset maps
    template <typename Asset>
    AssetMap<Asset> *get_array() noexcept {
        return reinterpret_cast<AssetMap<Asset>*>(asset_arrays[std::type_index(typeid(Asset))].get());
    }

public:
    // Called to instantiate an `AssetMap` in the manager to hold instances of the asset
    // in memory
    template <typename Asset>
    void register_asset() {
        if (asset_arrays.contains(std::type_index(typeid(Asset)))) {
            throw std::runtime_error("Asset already registered to manager");
        } else {
            asset_arrays.insert({
                std::type_index(typeid(Asset)),
                std::make_unique<AssetMap<Asset>>()
            });
        }
    }

    // Instantiates an asset 
    template <typename Asset, typename... Args>
    Asset &add_asset(const std::string &name, Args&&... args) {
        if (!asset_arrays.contains(std::type_index(typeid(Asset)))) {
            throw std::runtime_error("Asset type not registered to manager");
        } else {
            return get_array<Asset>()->add_asset(name, std::forward<Args>(args)...);
        }
    }

    // Optionally return a reference to the asset identified by `name` if the asset exists
    template <typename Asset>
    std::optional<std::reference_wrapper<Asset>> get_asset(const std::string &name) {
        if (!asset_arrays.contains(std::type_index(typeid(Asset)))) {
            throw std::runtime_error("Asset type not registered to manager");
        } else {
            return get_array<Asset>()->get_asset(name);
        }
    }

    // Return the map holding all assets of type `Asset`
    template <typename Asset>
    std::unordered_map<std::string, Asset> &get_all_assets() {
        if (!asset_arrays.contains(std::type_index(typeid(Asset)))) {
            throw std::runtime_error("Asset type not registered to manager");
        } else {
            return get_array<Asset>()->assets;
        }
    }
};

}

#endif // __CALICO_ASSET_MANAGER_HPP__
