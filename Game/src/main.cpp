#include "States/Game/GameState.h"

int main(int argc, char const *argv[])
{
  auto specs = pgn::ApplicationSpecification("Game", RESOURCES_PATH);

  pgn::Application game(specs);
  
  game.PushState<GameState>();
  game.Run();
}