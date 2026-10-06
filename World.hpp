#ifndef WORLD_H
#define WORLD_H


#include <unordered_map>
#include "Chunk.hpp"
#include <memory> // for std::unique_ptr
#include <functional> 
//custom hash for unorderd map
struct Vec2Hash {
	std::size_t operator()(const glm::ivec2& v) const {
		return std::hash<int>()(v.x) ^ (std::hash<int>()(v.y) << 1);
	}
};

class World {
public:
	World(int seed);

	void Update(const glm::vec3& playerPos);
	void Render(Shader& shader, const glm::vec3& cameraPos);

	Block* GetBlock(int worldX, int worldY, int worldZ);
	void SetBlock(int worldX, int worldY, int worldZ, BlockType type);

	bool IsBlockSolidAtWorld(int x, int y, int z);
	int renderDistance = 8;
private:
	std::unordered_map<glm::ivec2, std::unique_ptr<Chunk>, Vec2Hash> chunks;
	int seed; //using the seed to set up the world seed

	glm::ivec2 getPlayerChunkCoord(const glm::vec3& pos);
	void GenerateChunk(int chunkX, int chunkZ); // method to generate chunks on the x and y axis of the world
};
#endif