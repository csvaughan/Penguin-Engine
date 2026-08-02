#pragma once
#include "Application.h"
#include "Timestep.h"
#include "Events/EventToken.h"
#include "Memory/Ref.h"
#include "Asset/Audio.h"
#include "Asset/Font.h"
#include "Asset/Texture.h"

namespace pgn
{
    class AppContext
    {
    public:
        AppContext() : app(Application::Get()), assets(app.GetAssetManager()), window(*app.Get().GetWindow()) {}
        ~AppContext() {}

        //Application getters
        Vector2 GetFramebufferSize() const          { return app.GetFramebufferSize(); }
        Window& GetWindow() const                   { return window; }
        double GetTime()                            { return app.GetTime(); }
        float GetFPS()                              { return app.GetFPS(); }

        //Asset Methods
        Ref<Texture> GetTexture(const std::string& id) const                            { return assets.getTexture(id); }
        void LoadTexture(const std::string& id, const std::string& filename)            { assets.loadTexture(id, filename); }
        void RemoveTexture(const std::string& id)                                       { assets.removeTexture(id); }
        
        Ref<Font> GetFont(const std::string& id) const                                  { return assets.getFont(id); }
        void LoadFont(const std::string& id, const std::string& filename, float size)   { assets.loadFont(id, filename, size); }
        void RemoveFont(const std::string& id)                                          { assets.removeFont(id); }

        Ref<Audio> GetAudio(const std::string& id) const                                { return assets.getAudio(id); }
        void LoadMusic(const std::string& id, const std::string& filename)              { assets.loadMusic(id, filename); }
        void LoadSound(const std::string& id, const std::string& filename)              { assets.loadSound(id, filename); }
        void RemoveAudio(const std::string& id)                                         { assets.removeAudio(id); }
        
        void ClearTextures()    { assets.clearTextures(); }
        void ClearFonts()       { assets.clearFonts(); }
        void ClearAudio()       { assets.clearAudio();}
        void ClearAllAssets()   { assets.clearAllAssets(); }

        //Events
        void RaiseEvent(Event& event) { app.RaiseEvent(event); }

    private:
        Application& app;
        AssetManager& assets;
        Window& window;
    };
} // namespace pgn