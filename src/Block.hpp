#ifndef BLOCK_H
#define BLOCK_H

#include <glm/glm.hpp>
#include "Shader.hpp"
#include <glm/gtc/matrix_transform.hpp>

enum Face {
    FACE_BACK = 0,
    FACE_FRONT,
    FACE_LEFT,
    FACE_RIGHT,
    FACE_BOTTOM,
    FACE_TOP
};

enum BlockType {
    GRASS,
    DIRT,
    STONE,
    LIGHT,
    STONEV2,
    CUSTOM
};

class Block {
public:
    glm::vec3 position;
    glm::ivec2 atlasCoords[6]; // Per-face atlas tile coords
    BlockType type;
    glm::vec3 scaleRatio;
    glm::mat4 modelMatrix;

    static const int atlasGridSize = 5;

    Block(glm::vec3 pos, glm::vec3 scale, BlockType t = CUSTOM)
        : position(pos), type(t), scaleRatio(scale), modelMatrix(glm::mat4(1.0f))
    {
        setDefaultUVs();
    }

    //default constctor
    Block()
        : position(0.0f), type(CUSTOM), scaleRatio(1.0f), modelMatrix(1.0f)
    {
        setDefaultUVs();
    }
    void Draw(Shader& shader, unsigned int VAO)
    {
        shader.setMat4("model", modelMatrix);

        glBindVertexArray(VAO);

        for (int face = 0; face < 6; ++face)
        {
            shader.setVec2("atlasOffset", glm::vec2(atlasCoords[face]) / float(atlasGridSize));
            shader.setFloat("tileSize", 1.0f / atlasGridSize);
            glDrawArrays(GL_TRIANGLES, face * 6, 6);
        }
    }

    void SetModelMatrix(const glm::mat4& matrix) {
        modelMatrix = matrix;
    }

    bool isActive() const
    {
        return active;
    }

    void setActive(bool isActive)
    {
        active = isActive;
    }
    
    glm::vec2 GetAtlasUV(int face) const {
        return glm::vec2(atlasCoords[face]) / float(atlasGridSize);
    }
    void setDefaultUVs()
    {
        switch (type) {
        case GRASS:
            atlasCoords[FACE_TOP] = { 1, 0 };     // grass top
            atlasCoords[FACE_BOTTOM] = { 2, 0 };  // dirt
            for (int i = 0; i < 6; ++i) {
                if (i != FACE_TOP && i != FACE_BOTTOM)
                    atlasCoords[i] = { 0, 0 }; // grass side
            }
            break;

        case DIRT:
            for (int i = 0; i < 6; ++i)
                atlasCoords[i] = { 2, 0 };
            break;

        case STONE:
            for (int i = 0; i < 6; ++i)
                atlasCoords[i] = { 0, 2 };
            break;
        case LIGHT:
            for (int i = 0; i < 6; ++i)
                atlasCoords[i] = { 0,1 };
            break;
        case STONEV2:
            for (int i = 0; i < 6; ++i)
                atlasCoords[i] = { 0,3 };
            break;

        case CUSTOM:
        default:
            for (int i = 0; i < 6; ++i)
                atlasCoords[i] = { 4, 0 }; // fallback tile
            break;
        }
    }
private:
    bool active = true;

   
};

#endif
