#ifndef CHUNK_H
#define CHUNK_H
#include "Block.hpp"

#include <vector>

class World; // forward declaration

class Chunk {
public:
	Chunk(int sizeX, int sizeY, int sizeZ);
	~Chunk();
	void Update(float dt);
	void Draw(Shader& shader);
	
	//block accessors
	Block* GetBlock(int x, int y, int z) ;
	void SetBlockType(int x, int y, int z, BlockType type);

	//generation of chunk meshes
	void GenerateTerrain(int seed);
	void BuildMesh();
	void UploadMeshToGPU();
	void RebuildMeshIfDirty();

	bool IsDirty() const;
	void MarkDirty();
	void SetWorld(World* worldPtr);
	void SetOrgin(glm::vec3 orgin);
	int sizeX, sizeY, sizeZ;
	glm::vec3 origin; //orgin of the current chunk
private:
	int CHUNK_SIZE_X, CHUNK_SIZE_Y, CHUNK_SIZE_Z;
	int seed = 1337;
	int maxHeight = 32;
	std::vector<Block> blocks;
	World* world = nullptr;


	bool dirty;
	std::vector<float> vertexData; // Combined vertex data: position, normal, texcoord
	int vertexCount = 0;           // Total number of vertices after building the mesh
	unsigned int VAO = 0, VBO = 0;

	void AppendFaceVertices(int x, int y, int z, int face, glm::ivec2 atlasCoord);

	const int faceOffsets[6][3] = {
	{  0,  0, -1 }, // Back
	{  0,  0,  1 }, // Front
	{ -1,  0,  0 }, // Left
	{  1,  0,  0 }, // Right
	{  0, -1,  0 }, // Bottom
	{  0,  1,  0 }, // Top
	};
};

#endif