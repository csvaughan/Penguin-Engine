#pragma once
#include <box2d/box2d.h>

namespace pgn
{
    //Body
    inline b2BodyId ToB2Body(uint64_t id) 
    {
        b2BodyId b; std::memcpy(&b, &id, sizeof(b2BodyId)); return b;
    }
    inline uint64_t FromB2(b2BodyId id) 
    {
        uint64_t b = 0; std::memcpy(&b, &id, sizeof(b2BodyId)); return b;
    }

    //Shape
    inline b2ShapeId ToB2Shape(uint64_t id) 
    {
        b2ShapeId s; std::memcpy(&s, &id, sizeof(b2ShapeId)); return s;
    }
    inline uint64_t FromB2(b2ShapeId id) 
    {
        uint64_t s = 0; std::memcpy(&s, &id, sizeof(b2ShapeId)); return s;
    }

    //World
    inline b2WorldId ToB2World(uint64_t id) 
    {
        b2WorldId w; std::memcpy(&w, &id, sizeof(b2WorldId)); return w;
    }
    inline uint64_t FromB2(b2WorldId id) 
    {
        uint64_t w = 0; std::memcpy(&w, &id, sizeof(b2WorldId)); return w;
    }

    //Joint
    inline b2JointId ToB2Joint(uint64_t id) 
    {
        b2JointId j; std::memcpy(&j, &id, sizeof(b2JointId)); return j;
    }
    inline uint64_t FromB2(b2JointId id) 
    {
        uint64_t j = 0; std::memcpy(&j, &id, sizeof(b2JointId)); return j;
    }
}