#pragma once


//----Core----//
#include "Core/Application.h"
#include "Core/LayerStack.h"
#include "Core/State.h"
#include "Core/Layer.h"
#include "Core/Input.h"
#include "Log/Log.h"
#include "Core/Timestep.h"

//----Renderer----//
#include "Renderer/Renderer.h"
#include "Renderer/Sprite.h"
#include "Renderer/Text.h"
#include "Renderer/Color.h"
#include "Renderer/RenderShape.h"
#include "Renderer/CameraController.h"

//----Events---//
#include "Events/Event.h"
#include "Events/InputEvents.h"
#include "Events/WindowEvents.h"
#include "Events/EngineEvents.h"
#include "Events/EventBus.h"

//----Debug----//
#include "Debug/DebugLayer.h"

//----Math----//
#include "Math/Random.h"
#include "Math/Rect.h"
#include "Math/MathUtils.h"
#include "Math/Vector.h"
#include "Math/Interpolated.h"

//----Memory----//
#include "Memory/Scope.h"
#include "Memory/Ref.h"

//----Audio----//
#include "Audio/AudioSystem.h"

//----Animation----//
#include "Animation/Animation.h"
#include "Animation/Animator.h"

//----ECS----//
#include "ECS/Registry.h"

//---UI---//
#include "GUI/UIManager.h"
#include "GUI/Button.h"
#include "GUI/UICanvas.h"
#include "GUI/Label.h"
#include "GUI/Panel.h"
#include "GUI/GUIElement.h"
#include "GUI/ImageBox.h"

