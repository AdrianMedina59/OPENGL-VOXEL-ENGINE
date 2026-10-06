#include "Chunk.hpp"

#include<noise/noise.h>
#include <algorithm>
#include <random>
#include "World.hpp"
using namespace noise;

Chunk::Chunk(int sizeX, int sizeY, int sizeZ)
	: CHUNK_SIZE_X(sizeX), CHUNK_SIZE_Y(sizeY), CHUNK_SIZE_Z(sizeZ), dirty(true)
{
	blocks.resize(CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z);
	for (int x = 0; x < CHUNK_SIZE_X; ++x) {
		for (int y = 0; y < CHUNK_SIZE_Y; ++y) {
			for (int z = 0; z < CHUNK_SIZE_Z; ++z) {
				int index = x + CHUNK_SIZE_X * (y + CHUNK_SIZE_Y * z);
				blocks[index] = Block(glm::vec3(x, y, z), glm::vec3(1.0f), CUSTOM);
			}
		}
	}
}

Chunk::~Chunk() {
	if (VBO != 0) glDeleteBuffers(1, &VBO);
	if (VAO != 0) glDeleteVertexArrays(1, &VAO);
}

void Chunk::Draw(Shader& shader)
{
	if (dirty) {
		BuildMesh();
		UploadMeshToGPU();
		dirty = false;
	}

	shader.setMat4("model", glm::translate(glm::mat4(1.0f), origin));

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, vertexCount); // vertexCount is number of vertices
	glBindVertexArray(0);
}

Block* Chunk::GetBlock(int x, int y, int z)
{
	if (x < 0 || x >= CHUNK_SIZE_X ||
		y < 0 || y >= CHUNK_SIZE_Y ||
		z < 0 || z >= CHUNK_SIZE_Z)
		return nullptr; // Out-of-bounds safety

	int index = x + CHUNK_SIZE_X * (y + CHUNK_SIZE_Y * z);
	return &blocks[index];
}

void Chunk::SetBlockType(int x, int y, int z, BlockType type)
{
	if (x < 0 || x >= CHUNK_SIZE_X ||
		y < 0 || y >= CHUNK_SIZE_Y ||
		z < 0 || z >= CHUNK_SIZE_Z)
		return;

	int index = x + CHUNK_SIZE_X * (y + CHUNK_SIZE_Y * z);
	Block& block = blocks[index];
	if (block.type != type)
	{
		block.type = type;
		block.setDefaultUVs();
		dirty = true;
	}
}
void Chunk::GenerateTerrain(int seed)
{
	module::Perlin perlin;
	perlin.SetFrequency(0.05);
	perlin.SetPersistence(0.4);
	perlin.SetLacunarity(2.0);
	perlin.SetOctaveCount(4);
	perlin.SetSeed(seed); // Keep same seed for all chunks

	// Add billowy "hills"
	module::Billow billow;
	billow.SetFrequency(2.0);
	billow.SetPersistence(0.5);
	billow.SetOctaveCount(4);
	billow.SetSeed(seed + 1);

	// Add sharp mountains using ridged noise
	module::RidgedMulti ridged;
	ridged.SetFrequency(1.5);
	ridged.SetLacunarity(2.2);
	ridged.SetSeed(seed + 2);

	// Combine perlin and billow using Add
	module::Add hillsAndBase;
	hillsAndBase.SetSourceModule(0, perlin);
	hillsAndBase.SetSourceModule(1, billow);

	// Multiply hills with ridged noise for jagged terrain
	module::Multiply ruggedTerrain;
	ruggedTerrain.SetSourceModule(0, hillsAndBase);
	ruggedTerrain.SetSourceModule(1, ridged);
	int snowLevel = CHUNK_SIZE_Y * 0.8;
	for (int x = 0; x < CHUNK_SIZE_X; ++x) {
		for (int z = 0; z < CHUNK_SIZE_Z; ++z) {
			// Convert to world coordinates for seamless terrain
			int worldX = static_cast<int>(origin.x) + x;
			int worldZ = static_cast<int>(origin.z) + z;

			double noiseVal = ruggedTerrain.GetValue(worldX * 0.01, 0.0, worldZ * 0.01);
			noiseVal = (noiseVal + 1.0) / 2.0; // Normalize to [0,1]
			int height = std::min(static_cast<int>(noiseVal * CHUNK_SIZE_Y), CHUNK_SIZE_Y - 1);// Adjustable max height

			bool isSnowy = height > snowLevel;


			for (int y = 0; y <= height; ++y) {
				if (y == height) {
					SetBlockType(x, y, z, isSnowy ? STONEV2 : GRASS);
				}
				else if (!isSnowy && y > height - 4) {
					SetBlockType(x, y, z, DIRT);
				}
				else {
					SetBlockType(x, y, z, STONE);
				}
			}
		}
	}
}
void Chunk::BuildMesh()
{
    vertexData.clear();
    vertexCount = 0;

    for (int x = 0; x < CHUNK_SIZE_X; ++x)
    {
        for (int y = 0; y < CHUNK_SIZE_Y; ++y)
        {
            for (int z = 0; z < CHUNK_SIZE_Z; ++z)
            {
                int index = x + CHUNK_SIZE_X * (y + CHUNK_SIZE_Y * z);
                Block& block = blocks[index];
                if (block.type == CUSTOM) continue; // skip air

                for (int face = 0; face < 6; ++face)
                {
                    int nx = x + faceOffsets[face][0];
                    int ny = y + faceOffsets[face][1];
                    int nz = z + faceOffsets[face][2];

                    bool faceVisible = false;

                    if (nx < 0 || nx >= CHUNK_SIZE_X ||
                        ny < 0 || ny >= CHUNK_SIZE_Y ||
                        nz < 0 || nz >= CHUNK_SIZE_Z)
                    {
                        // Neighbor is in another chunk
                        if (world)
                        {
                            int worldX = static_cast<int>(origin.x) + x + faceOffsets[face][0];
                            int worldY = y + faceOffsets[face][1];
                            int worldZ = static_cast<int>(origin.z) + z + faceOffsets[face][2];
                            faceVisible = !world->IsBlockSolidAtWorld(worldX, worldY, worldZ);
                        }
                        else
                        {
                            faceVisible = true; // fallback if no world pointer
                        }
                    }
                    else
                    {
                        int neighborIndex = nx + CHUNK_SIZE_X * (ny + CHUNK_SIZE_Y * nz);
                        if (blocks[neighborIndex].type == CUSTOM)
                        {
                            faceVisible = true;
                        }
                    }

                    if (faceVisible)
                    {
                        AppendFaceVertices(x, y, z, face, block.atlasCoords[face]);
                    }
                }
            }
        }
    }
	vertexCount = static_cast<int>(vertexData.size() / 9); // 9 floats per vertex: pos(3) + norm(3) + uv(2) + ao(1)
}

void Chunk::UploadMeshToGPU()
{
	if (VAO == 0)
		glGenVertexArrays(1, &VAO);
	if (VBO == 0)
		glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW);

	// Vertex layout: pos(3), normal(3), uv(2)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(8 * sizeof(float)));
	glEnableVertexAttribArray(3);

	glBindVertexArray(0);
}

void Chunk::MarkDirty()
{
	dirty = true;
}

void Chunk::SetWorld(World* worldPtr) {
	world = worldPtr;
}

void Chunk::SetOrgin(glm::vec3 orgin)
{
	this->origin = orgin;
}



void Chunk::AppendFaceVertices(int x, int y, int z, int face, glm::ivec2 atlasCoord)
{
	static const glm::vec3 faceNormals[6] = {
		{  0,  0, -1 }, {  0,  0,  1 },
		{ -1,  0,  0 }, {  1,  0,  0 },
		{  0, -1,  0 }, {  0,  1,  0 }
	};

	static const glm::vec3 faceVertices[6][6] = {
		{ {0,0,0}, {1,1,0}, {1,0,0}, {0,0,0}, {0,1,0}, {1,1,0} }, // back
		{ {0,0,1}, {1,0,1}, {1,1,1}, {0,0,1}, {1,1,1}, {0,1,1} }, // front
		{ {0,0,0}, {0,1,1}, {0,1,0}, {0,0,0}, {0,0,1}, {0,1,1} }, // left
		{ {1,0,0}, {1,1,0}, {1,1,1}, {1,0,0}, {1,1,1}, {1,0,1} }, // right
		{ {0,0,0}, {1,0,0}, {1,0,1}, {0,0,0}, {1,0,1}, {0,0,1} }, // bottom
		{ {0,1,0}, {1,1,1}, {1,1,0}, {0,1,0}, {0,1,1}, {1,1,1} }  // top
	};


	glm::vec2 faceUVs[6][6] = {
		// BACK
		{
		{1.0f, 1.0f}, {0.0f, 0.0f}, {0.0f, 1.0f},
		{1.0f, 1.0f}, {1.0f, 0.0f}, {0.0f, 0.0f}
		},

		// FRONT
		{ {0,1}, {1,1}, {1,0}, {0,1}, {1,0}, {0,0} }, 

		// LEFT
		  { {0,1}, {1,0}, {0,0}, {0,1}, {1,1}, {1,0} },

		// RIGHT
		{
			{1.0f, 1.0f}, // top-left
			{1.0f, 0.0f}, // bottom-left
			{0.0f, 0.0f}, // bottom-right

			{1.0f, 1.0f}, // top-left
			{0.0f, 0.0f}, // bottom-right
			{0.0f, 1.0f}  // top-right
		},

		// BOTTOM
		{ {0,0}, {1,0}, {1,1}, {0,0}, {1,1}, {0,1} },

		// TOP
		{ {0.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f} }
	};

	glm::vec3 basePos = glm::vec3(x, y, z);
	glm::vec3 normal = faceNormals[face];
	float tileSize = 1.0f / 5.0f;
	glm::vec2 tileOffset = atlasCoord;

	float ao = 10.0f;

	for (int i = 0; i < 6; ++i) {
		glm::vec3 pos = basePos + faceVertices[face][i];
		glm::vec2 uv = (tileOffset + faceUVs[face][i]) * tileSize;

		vertexData.push_back(pos.x);
		vertexData.push_back(pos.y);
		vertexData.push_back(pos.z);

		vertexData.push_back(normal.x);
		vertexData.push_back(normal.y);
		vertexData.push_back(normal.z);

		vertexData.push_back(uv.x);
		vertexData.push_back(uv.y);

		vertexData.push_back(ao);
	}
}
