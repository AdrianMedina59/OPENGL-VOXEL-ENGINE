#include "World.hpp"

constexpr int CHUNK_DIM = 16;

World::World(int seed) : seed(seed)
{
	//generating chunks around (0,0)
	for (int x = -1; x <= 1; ++x)
	{
		for (int z = -1; z <= 1; ++z)
		{
			GenerateChunk(x, z);
		}
	}
	// Now that all chunks are generated, mark them dirty to rebuild mesh correctly
	for (auto& [coord, chunk] : chunks) {
		chunk->MarkDirty();
	}
}

void World::Update(const glm::vec3& playerPos)
{
	glm::ivec2 currentChunk = glm::ivec2(
		static_cast<int>(std::floor(playerPos.x / 16.0f)),
		static_cast<int>(std::floor(playerPos.z / 16.0f))
	);

	

	// Generate chunks within render distance
	for (int dx = -renderDistance; dx <= renderDistance; ++dx) {
		for (int dz = -renderDistance; dz <= renderDistance; ++dz) {
			glm::ivec2 chunkCoord = currentChunk + glm::ivec2(dx, dz);
			if (chunks.find(chunkCoord) == chunks.end()) {
				GenerateChunk(chunkCoord.x, chunkCoord.y);
			}
		}
	}
}

void World::Render(Shader& shader, const glm::vec3& cameraPos) {
	for (auto& pair : chunks) {
		auto&  chunk = pair.second;
		glm::vec3 chunkCenter = chunk->origin + glm::vec3(CHUNK_DIM / 2, 0, CHUNK_DIM / 2);
		float distance = glm::length(cameraPos - chunkCenter);
		float dx = cameraPos.x - chunkCenter.x;
		float dz = cameraPos.z - chunkCenter.z;
		float distanceSquared = dx * dx + dz * dz;

		float maxDist = renderDistance * CHUNK_DIM;
		if (distance < renderDistance * maxDist) {
			chunk->Draw(shader);
		}
	}
}

bool World::IsBlockSolidAtWorld(int x, int y, int z)
{
	int chunkX = floor(x / 16.0f);
	int chunkZ = floor(z / 16.0f);
	glm::ivec2 chunkCoord(chunkX, chunkZ);

	auto it = chunks.find(chunkCoord);
	if (it == chunks.end()) return false;

	// Use smart pointer (no need to manually delete)
	const std::unique_ptr<Chunk>& chunk = it->second;
	int localX = x - chunkX * 16;
	int localZ = z - chunkZ * 16;

	Block* block = chunk->GetBlock(localX, y, localZ);
	return block && block->type != CUSTOM;
}

glm::ivec2 World::getPlayerChunkCoord(const glm::vec3& pos)
{
	 return glm::ivec2(
        floor(pos.x / CHUNK_DIM),
        floor(pos.z / CHUNK_DIM)
    );
}

// method to generate chunks on the x and y axis of the world
void World::GenerateChunk(int chunkX, int chunkZ)
{
	glm::vec3 chunkOrigin = glm::vec3(chunkX * CHUNK_DIM, 0, chunkZ * CHUNK_DIM);

	auto newChunk = std::make_unique<Chunk>(CHUNK_DIM, CHUNK_DIM, CHUNK_DIM);
	newChunk->SetOrgin(chunkOrigin);
	newChunk->GenerateTerrain(seed);
	newChunk->SetWorld(this);

	glm::ivec2 chunkCoord(chunkX, chunkZ);
	chunks[chunkCoord] = std::move(newChunk);
}