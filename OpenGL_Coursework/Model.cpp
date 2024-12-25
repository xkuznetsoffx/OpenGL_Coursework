#include "Model.h"

Model::Model(
	glm::vec3 pivotPoint,
	Material* material,
	std::vector<SPtrMesh> meshes
)
	:
	pivotPoint(pivotPoint), material(material)
{
	this->meshes.reserve(meshes.size());

	for (const auto& mesh : meshes) {
		this->meshes.emplace_back
		(
			std::make_shared<Mesh>(*mesh)
		);
		(*(this->meshes.end() - 1))->move(pivotPoint);
	}

}

Model::Model(
	glm::vec3 pivotPoint,
	Material* material,
	SPtrMesh mesh
)
	:
	pivotPoint(pivotPoint), material(material)
{
	this->meshes.push_back(mesh);
	(*(this->meshes.end() - 1))->move(pivotPoint);
}

Model::Model(std::string path, glm::vec3 pivotPoint) 
	:
	pivotPoint(pivotPoint)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_PreTransformVertices | aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices  );
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}

	directory = path.substr(0, path.find_last_of('/'));

	processNode(scene->mRootNode, scene);
}

Model::~Model()
{
}

void Model::update()
{

}

void Model::render(Shader* shader)
{
	updateUniforms();

	if(material)
		material->sendToShader(shader);

	for (auto& mesh : meshes) {
		mesh->render(shader);
	}

}

void Model::rotate(glm::vec3 rotation)
{
	for (auto& mesh : meshes) {
		mesh->rotate(rotation);
	}
}

void Model::move(const glm::vec3 position)
{
	for (auto& mesh : meshes) {
		mesh->move(position);
	}
}

void Model::scaleUp(const glm::vec3 scale)
{
	for (auto& mesh : meshes) {
		mesh->scaleUp(scale);
	}
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
	for (size_t i = 0; i < node->mNumMeshes; ++i) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		Mesh newMesh = processMesh(mesh, scene);
		meshes.emplace_back(std::make_shared<Mesh>(std::move(newMesh)));
		(*(this->meshes.end() - 1))->move(pivotPoint);
	}

	for (size_t i = 0; i < node->mNumChildren; ++i)
		processNode(node->mChildren[i], scene);
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture> textures;

	vertices.reserve(mesh->mNumVertices);
	indices.reserve(mesh->mNumFaces * 3);

	for (size_t i = 0; i < mesh->mNumVertices; ++i) {
		Vertex vertex;

		const aiVector3D& vertexData = mesh->mVertices[i];
		const aiVector3D& normalData = mesh->mNormals[i];

		vertex.position = glm::vec3(vertexData.x, vertexData.y, vertexData.z);
		vertex.normal = glm::vec3(normalData.x, normalData.y, normalData.z);

		if (mesh->mTextureCoords[0]) {
			const auto& texCoordData = mesh->mTextureCoords[0][i];
			vertex.texcoord = glm::vec2(texCoordData.x, texCoordData.y);
		}
		else {
			vertex.texcoord = glm::vec2(0.0f, 0.0f);
		}

		vertices.push_back(vertex);
	}

	for (size_t i = 0; i < mesh->mNumFaces; ++i) {
		aiFace face = mesh->mFaces[i];
		indices.insert(indices.end(), face.mIndices, face.mIndices + face.mNumIndices);
	}
	
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

	return Mesh(vertices.data(),mesh->mNumVertices,
		indices.data(), indices.size());
}

//std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
//{
//	
//}

void Model::updateUniforms()
{

}
