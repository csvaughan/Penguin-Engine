#pragma once
#include "Penguin.h"

class GameMap;

class AreaSystem : public pgn::BaseSystem
{
public:
    AreaSystem(pgn::CameraController& c);
    ~AreaSystem();

    void LoadLevelMap(const std::string& tmxPath);
    void LoadLevel(const std::string& filepath) {}
    
    virtual void OnUpdate(pgn::Timestep ts) override;
    virtual void OnRender(float alpha, pgn::Renderer& renderer) override;

private:
    pgn::Scope<GameMap> m_currentMap;
    pgn::CameraController& m_camera;
};