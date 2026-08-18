#pragma once
#include "pgpch.h"
#include "AssetImporter.h"
#include "Memory/Scope.h"
#include "Log/Log.h"
#include <typeindex>

struct SDL_Renderer;

namespace pgn 
{

    class AssetManager 
    {
    public:
        AssetManager(const AssetManager&) = delete;
        AssetManager& operator=(const AssetManager&) = delete;

        // --- Importer Registration ---
        template<typename AssetType, typename ImporterType, typename... Args>
        requires(std::is_base_of_v<IAsset, AssetType>, std::is_base_of_v<IAssetImporter, ImporterType>)
        void registerImporter(Args&&... args) 
        {
            m_importers[typeid(AssetType)] = CreateScope<ImporterType>(std::forward<Args>(args)...);
        }

        // --- Asset API ---
        template<typename T>
        requires(std::is_base_of_v<IAsset, T>)
        Ref<T> load(const std::string& id, const std::string& filename) 
        {
            auto it = m_assets.find(id);
            if (it != m_assets.end()) 
            {
                return StaticRefCast<T>(it->second);
            }

            auto impIt = m_importers.find(typeid(T));
            if (impIt == m_importers.end()) 
            {
                logNoImporterError(typeid(T).name());
                return nullptr;
            }

            std::filesystem::path path = resolvePath(filename); 
            if (!std::filesystem::exists(path)) 
            {
                logFileNotFoundError(id, path.string());
                return nullptr;
            }

            try 
            {
                Ref<IAsset> asset = impIt->second->load(path);
                if (!asset) 
                {
                    logNullAssetError(id, path.string());
                    return nullptr;
                }

                m_assets[id] = asset; 
                logAssetLoadedInfo(id, path.string());
                return StaticRefCast<T>(asset);
            }
            catch (const std::exception& e) 
            {
                logAssetExceptionError(id, e.what());
                return nullptr;
            }
        }

        template<typename T>
        requires(std::is_base_of_v<IAsset, T>)
        Ref<T> get(const std::string& id) 
        {
            auto it = m_assets.find(id);
            if (it == m_assets.end()) 
            {
                logAssetNotFoundError(id);
                return nullptr;
            }

        #if defined(PGN_DEBUG)
            auto resource = DynamicRefCast<T>(it->second);
            PGN_ASSERT(resource, "Asset type mismatch during get()!");
            return resource;
        #else
            return StaticRefCast<T>(it->second);
        #endif
        }

        bool has(const std::string& id) const;

        bool remove(const std::string& id);

        template<typename T>
        requires(std::is_base_of_v<IAsset, T>)
        void clearAllOfType() 
        {
            std::erase_if(m_assets, [](const auto& pair) {
                return DynamicRefCast<T>(pair.second) != nullptr;
            });
        }

        void clearAll();
        size_t getAssetCount() const { return m_assets.size(); } 

    private:
        AssetManager() = default;
        ~AssetManager() = default;

        void Init(SDL_Renderer* renderer, const std::string& resourcePath); 
        void Shutdown(); 
        std::filesystem::path resolvePath(const std::string& filename); 
        void loadDefaults(); 

        //Logging
        void logNoImporterError(const char* typeName);
        void logFileNotFoundError(const std::string& id, const std::string& path);
        void logNullAssetError(const std::string& id, const std::string& path);
        void logAssetLoadedInfo(const std::string& id, const std::string& path);
        void logAssetExceptionError(const std::string& id, const char* what);
        void logAssetNotFoundError(const std::string& id);

        SDL_Renderer* m_renderer = nullptr; 
        std::filesystem::path m_resourcePath; 

        std::unordered_map<std::type_index, Scope<IAssetImporter>> m_importers;
        std::unordered_map<std::string, Ref<IAsset>> m_assets;

        friend class Application; 
    };
}