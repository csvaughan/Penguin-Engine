#include "States/Game/GameState.h"

int main(int argc, char const *argv[])
{
  auto specs = pgn::ApplicationSpecification("Sandbox", ASSET_PATH);

  pgn::Application app(specs);
  
  app.PushState<GameState>();
  
  app.Run(); 
}