#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <fstream>
#include <sstream>

// Assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <Magick++.h>
#include <SOIL.h>
#include "Shader.h"
//#include <MagickCore.h>

//using namespace Magick;
using namespace std;

#define INVALID_OGL_VALUE 0xFFFFFFFF
#define SAFE_DELETE(p) if (p) { delete p; p = NULL; }

namespace OglDev {
	struct Vertex {
		glm::fvec3 Position;
		glm::fvec3 Normal;
		glm::fvec2 TexCoords;

		Vertex() {}

		Vertex(const glm::fvec3& pos, const glm::fvec3& norm, const glm::fvec2& tcrd)
			: Position(pos), Normal(norm), TexCoords(tcrd) {}
	};

	class Texture {

		GLuint id;
		string type;
	public:

		Texture(GLenum TextureTarget, const std::string& FileName) :
			m_textureTarget(TextureTarget), m_fileName(FileName) {}

		void Bind(GLenum TextureUnit);
		bool Load();

	private:
		std::string m_fileName;
		GLenum m_textureTarget;
		GLuint m_textureObj;
		Magick::Image m_image;
		Magick::Blob m_blob;
	};

	class Mesh {
	public:
		bool LoadMesh(const std::string& Filename);
		void Render();
	private:
		bool InitFromScene(const aiScene* pScene, const std::string& Filename);
		void InitMesh(unsigned int Index, const aiMesh* paiMesh);
		bool InitMaterials(const aiScene* pScene, const std::string& Filename);
		void Clear();

#define INVALID_MATERIAL 0xFFFFFFFF


		struct MeshEntry {
			MeshEntry();
			~MeshEntry();

			// Look for Source file for implementation.
			void Init(const std::vector<Vertex>& vertices,
				const std::vector<unsigned int>& indices);

			GLuint VB; // Vertex Buffer
			GLuint IB; // Index Buffer
			GLuint EBO; // Element Buffer Object
			unsigned int NumIndices;
			unsigned int MaterialIndex;
		};

		/* Mesh Data */
		vector<MeshEntry> m_Entries; // Each entry corresponds to 1 mesh strcture in the aiScene object.
		vector<Texture*> m_Textures; // Also contains shared materials/textures. MaterialIndex points to primary texture.
	};

}
namespace LearnOpenGL
{
	struct Vertex {
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoords;
	};

	struct Texture {
		GLuint id;
		string type;
		aiString path;
	};

	class Mesh {
	public:
		/* Mesh Data */
		vector<Vertex> vertices;
		vector<GLuint> indices;
		vector<Texture> textures;

		/* Functions */
		Mesh(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures)
		{
			this->vertices = vertices;
			this->indices = indices;
			this->textures = textures;

			this->setupMesh();
		}

		void Draw(Shader shader) {
			GLuint diffuseNr = 1;
			GLuint specularNr = 1;

			for (GLuint i = 0; i < this->textures.size(); i++)
			{
				glActiveTexture(GL_TEXTURE0 + i);

				stringstream ss;
				string number;
				string name = this->textures[i].type;
				if (name == "texture_diffuse")
					ss << diffuseNr++;
				else if (name == "texture_specular")
					ss << specularNr++; 
				number = ss.str();

				glUniform1f(glGetUniformLocation(shader.Program, ("material." + name + number).c_str()), i);
				glBindTexture(GL_TEXTURE_2D, this->textures[i].id);
			}
			glActiveTexture(GL_TEXTURE0);

			// Draw Mesh
			glBindVertexArray(this->VAO);
			glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}
	private:
		/* Render  Data */
		GLuint VAO, VBO, EBO;

		/* Functions */
		void setupMesh() {
			glGenVertexArrays(1, &this->VAO);
			glGenBuffers(1, &this->VBO);
			glGenBuffers(1, &this->EBO);

			glBindVertexArray(this->VAO);
			glBindBuffer(GL_ARRAY_BUFFER, this->VBO);

			glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex),
				&this->vertices[0], GL_STATIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint),
						&this->indices[0], GL_STATIC_DRAW);

			// Vertex Positions
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
				(GLvoid*)0);

			// Vertex Normals
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
				(GLvoid*)offsetof(Vertex, Normal));

			// Vertex Texture Coords
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
				(GLvoid*)offsetof(Vertex, TexCoords));

			glBindVertexArray(0);
		}
	};

	class Model
	{
	public:
		/* Functions */
		Model(GLchar* path)
		{
			this->loadModel(path);
		}
		void Draw(Shader shader) {
			for (GLuint i = 0; i < this->meshes.size(); i++) {
				this->meshes[i].Draw(shader);
			}
		}
	private:
		/* Model Data */
		vector<Mesh> meshes;
		string directory;
		vector<Texture> textures_loaded;

		/* Functions */
		void loadModel(string path) {
			Assimp::Importer import;
			const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

			if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
			{
				cout << "ERROR::ASSIMP::"<< import.GetErrorString() << endl;
			}

			this->directory = path.substr(0, path.find_last_of('/'));
			this->processNode(scene->mRootNode, scene);
		}
		void processNode(aiNode* node, const aiScene* scene) {
			for (GLuint i = 0; i < node->mNumMeshes; i++)
			{
				aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
				this->meshes.push_back(this->processMesh(mesh, scene));
			}

			for (GLuint i = 0; i < node->mNumChildren; i++)
			{
				this->processNode(node->mChildren[i], scene);
			}
		}

		Mesh processMesh(aiMesh* mesh, const aiScene* scene) {
			vector<Vertex> vertices;
			vector<GLuint> indices;
			vector<Texture> textures;

			for (GLuint i = 0; i < mesh->mNumVertices; i++)
			{
				Vertex vertex;
				glm::vec3 vector;

				vector.x = mesh->mVertices[i].x;
				vector.y = mesh->mVertices[i].y;
				vector.z = mesh->mVertices[i].z;

				vertex.Position = vector;

				vector.x = mesh->mNormals[i].x;
				vector.y = mesh->mNormals[i].y;
				vector.z = mesh->mNormals[i].z;

				vertex.Normal = vector;

				if (mesh->mTextureCoords[0])
				{
					glm::vec2 vec;
					vec.x = mesh->mTextureCoords[0][i].x;
					vec.y = mesh->mTextureCoords[0][i].y;
					vertex.TexCoords = vec;
				}
				else {
					vertex.TexCoords = glm::vec2(0.0f, 0.0f);
				}

				vertices.push_back(vertex);
			}

			for (GLuint i = 0; i < mesh->mNumFaces; i++)
			{
				aiFace face = mesh->mFaces[i];
				for(GLuint j = 0; j < face.mNumIndices; j++)
					indices.push_back(face.mIndices[j]);
			}

			if (mesh->mMaterialIndex >= 0)
			{
				aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
				vector<Texture> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, 
												"texture_diffuse");
				textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
				vector<Texture> specularMaps = this->loadMaterialTextures(material, 
												aiTextureType_SPECULAR, "texture_specular");

				textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
			}

			return Mesh(vertices, indices, textures);
		}

		GLint TextureFromFile(const char* path, string directory)
		{
			//Generate texture ID and load texture data 
			string filename = string(path);
			filename = directory + '/' + filename;
			GLuint textureID;
			glGenTextures(1, &textureID);
			int width, height;
			unsigned char* image = SOIL_load_image(filename.c_str(), &width, &height, 0, SOIL_LOAD_RGB);
			// Assign texture to ID
			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
			glGenerateMipmap(GL_TEXTURE_2D);

			// Parameters
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glBindTexture(GL_TEXTURE_2D, 0);
			SOIL_free_image_data(image);
			return textureID;
		}

		vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
		{
			vector<Texture> textures;
			for (GLuint i = 0; i < mat->GetTextureCount(type); i++)
			{
				aiString str;
				mat->GetTexture(type, i, &str);
				GLboolean skip = false;
				for (GLuint j = 0; j < textures_loaded.size(); j++)
				{
					if (textures_loaded[j].path == str)
					{
						textures.push_back(textures_loaded[j]);
						skip = true;
						break;
					}
				}
				if (!skip)
				{  // If texture hasn't been loaded already, load it
					Texture texture;
					texture.id = TextureFromFile(str.C_Str(), this->directory);
					texture.type = typeName;
					texture.path = str;
					textures.push_back(texture);
					this->textures_loaded.push_back(texture);
				}
			}

			return textures;
		}
	};
}