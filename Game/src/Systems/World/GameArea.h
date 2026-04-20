#pragma once
#include "GameMap.h"

class AreaSystem : public pgn::BaseSystem
{
public:
    AreaSystem(pgn::CameraController& c) : BaseSystem("AreaSystem"), m_camera(c){}

    void LoadLevelMap(const std::string& tmxPath);
    void LoadLevel(const std::string& filepath) {}
    
    virtual void OnUpdate(pgn::Timestep ts) override;
    virtual void OnRender(float alpha) override;

private:
    pgn::Scope<GameMap> m_currentMap;
    pgn::CameraController& m_camera;
};