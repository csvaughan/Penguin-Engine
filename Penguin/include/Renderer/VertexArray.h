#pragma once
#include "Color.h"
#include "Math/Vector.h"
#include "Math/Rect.h" // Assuming this defines FloatRect
#include <vector>
#include <algorithm>

namespace pgn
{
    struct Vertex 
    {
        Vector2 position;
        Color color;
        Vector2 tex_coord;
    };

    class VertexArray 
    {
    public:
        VertexArray() { clear(); }

        /**
         * Adds a vertex and automatically expands the bounding box to encompass it.
         */
        void addVertex(const Vertex& v) 
        { 
            m_vertices.push_back(v); 
            expandBounds(v.position);
        }

        void addIndex(int index) { m_indices.push_back(index); }

        void reserve(size_t v, size_t i) 
        { 
            m_vertices.reserve(v); 
            m_indices.reserve(i); 
        }

        /**
         * Resets the array and the bounding box.
         */
        void clear() 
        { 
            m_vertices.clear(); 
            m_indices.clear(); 
            m_bounds = { 0.0f, 0.0f, 0.0f, 0.0f };
            m_isFirstVertex = true;
        }

        const std::vector<Vertex>& getVertices() const { return m_vertices; }
        std::vector<Vertex>& getVertices()             { return m_vertices; }

        const std::vector<int>& getIndices() const     { return m_indices; }
        std::vector<int>& getIndices()                 { return m_indices; }

        const FloatRect& getBounds() const             { return m_bounds; }

        /** * Appends another VertexArray and updates the bounds to include the other's bounds.
         */
        void append(const VertexArray& other) 
        {
            if (other.m_vertices.empty()) 
                return;

            int base = (int)m_vertices.size();
            m_vertices.insert(m_vertices.end(), other.m_vertices.begin(), other.m_vertices.end());
            
            for (int idx : other.m_indices) 
                m_indices.push_back(base + idx);

            // Expand bounds using the corners of the other array's bounds
            expandBounds({other.m_bounds.x, other.m_bounds.y});
            expandBounds({other.m_bounds.x + other.m_bounds.w, other.m_bounds.y + other.m_bounds.h});
        }

        /**
         * Helper to quickly add a quad. Handles indices automatically.
         */
        void addQuad(const Vertex& v0, const Vertex& v1, const Vertex& v2, const Vertex& v3) 
        {
            int base = (int)m_vertices.size();
            addVertex(v0);
            addVertex(v1);
            addVertex(v2);
            addVertex(v3);

            m_indices.insert(m_indices.end(), { base + 0, base + 1, base + 2, base + 2, base + 3, base + 0 });
        }

    private:
        void expandBounds(const Vector2& pos)
        {
            if (m_isFirstVertex)
            {
                m_bounds = { pos.x, pos.y, 0.0f, 0.0f };
                m_isFirstVertex = false;
                return;
            }

            float minX = std::min(m_bounds.x, pos.x);
            float minY = std::min(m_bounds.y, pos.y);
            float maxX = std::max(m_bounds.x + m_bounds.w, pos.x);
            float maxY = std::max(m_bounds.y + m_bounds.h, pos.y);

            m_bounds.x = minX;
            m_bounds.y = minY;
            m_bounds.w = maxX - minX;
            m_bounds.h = maxY - minY;
        }

    private:
        std::vector<Vertex> m_vertices;
        std::vector<int> m_indices;
        
        FloatRect m_bounds;
        bool m_isFirstVertex = true;
    };
}