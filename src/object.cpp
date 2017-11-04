#include "object.h"
#include <cstdlib>
#include <stdio.h>
#include <time.h>
#include <ctime>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/color4.h>
#include <Magick++.h>
#include <string>
#include <GL/glew.h>
#include "texture.h"

using namespace std;

//Object constructor
Object::Object(string myFile)
{ 
	//Object importer
	Assimp::Importer importer;

        //Ensure that RNG is possibly random
	srand(time(0));

        //File reader
	const char* fileName = myFile.c_str();

        //Declare variables for obtaining the models
	const aiScene *pScene = importer.ReadFile(fileName, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);
	const aiMesh *pai_mesh = pScene->mMeshes[0]; 

        //Model the vertices
	for (int i = 0; i < pai_mesh->mNumVertices; i++)
	{
	        aiVector3D pPos = pai_mesh->mVertices[i];

	        glm::vec3 position = glm::vec3(pPos.x, pPos.y, pPos.z);

	        glm::vec2 uvcoords;

	        Vertices.push_back(Vertex(position, uvcoords));
	}

        //Model the faces
	for (int i = 0; i < pai_mesh->mNumFaces; i++)
	{
	        const aiFace* Face = &pai_mesh->mFaces[i];  

	        for (int j = 0; j < 3; j++)
	        {                        
	                Indices.push_back(Face->mIndices[j]);
	                aiVector3D uv = pai_mesh->mTextureCoords[0][Face->mIndices[j]];

	                glm::vec2 uvcoords;

	                uvcoords.x = uv.x;
	                uvcoords.y = uv.y;

	                Vertices[(i*3)+j].uv = uvcoords;
	                
	        }
	}

        //Initialize the angles for rotation and orbits
	angle1 = 0.0f;
	angle2 = 0.0f;

        //Initialize the vertices and indices
	glGenBuffers(1, &VB);
	glBindBuffer(GL_ARRAY_BUFFER, VB);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * Vertices.size(), &Vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &IB);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * Indices.size(), &Indices[0], GL_STATIC_DRAW);

	
}

//Object destructor
Object::~Object()
{
  Vertices.clear();
  Indices.clear();
}

//Updates position, rotation, and orbit of a planet
void Object::planet_update(unsigned int dt, float scale, float radius, float orbit, float rotation)
{	
        angle1 += orbit * dt * M_PI/10000;                                                                      //orbit
        angle2 += rotation* dt * M_PI/10000;                                                                    //rotation

        //perform the transformations that will determine the planet's location and speed of its orbit and rotation
	model = glm::rotate(glm::mat4(1.0f), (angle1), glm::vec3(0.0, 1.0, 0.0));
	model *= glm::translate(glm::mat4(1.0f),glm::vec3(radius, 0.0, 0.0));
        model *= glm::scale(glm::mat4(1.0f), glm::vec3(scale, scale, scale));
	model *= glm::rotate(glm::mat4(1.0f), (angle2), glm::vec3(0.0, 1.0, 0.0));
}

//Updates position, rotation, and orbit of a planet's moon
void Object::moon_update(unsigned int dt, float scale, float radius, float orbit, glm::mat4 parent)
{
	angle1 += orbit * dt * M_PI/10000;                                                                      //orbit
	angle2 += dt * M_PI/10000;                                                                              //rotation

	//perform the transformations that will determine the moon's location and speed of its orbit and rotation
	model = glm::translate(glm::mat4(1.0f),glm::vec3(parent[3][0], parent[3][1], parent[3][2]));            //translate to parent
	model *= glm::rotate(glm::mat4(1.0f), (angle1), glm::vec3(0.0, 1.0, 0.0));                              //rotate to orbit position
	model *= glm::translate(glm::mat4(1.0f),glm::vec3(radius, 0.0, 0.0));                                   //translate to orbit radius
	model *= glm::scale(glm::mat4(1.0f), glm::vec3(scale, scale, scale));                                   //scale to size
	model *= glm::rotate(glm::mat4(1.0f), (angle2), glm::vec3(0.0, 1.0, 0.0));                              //rotation about y-axis
}

//Updates position, rotation, and orbit of a planet's ring
void Object::ring_update(unsigned int dt, float scale, float rotation, glm::mat4 parent)
{
	angle2 += rotation * dt * M_PI/10000;                                                                   //rotation

	//center of rings is not the center of the ring model, need to shift it over 6.0 in the x direction
	model = glm::translate(glm::mat4(1.0f),glm::vec3(parent[3][0] + 145.0, parent[3][1], parent[3][2]));    //translate to parent
	model *= glm::scale(glm::mat4(1.0f), glm::vec3(scale, scale, scale));                                   //scale to size
}

//Returns the model
glm::mat4 Object::GetModel()
{
  return model;
}

void Object::Render(Texture* pTexture)
{
  pTexture->Bind(GL_TEXTURE0);


  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ARRAY_BUFFER, VB);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,uv));

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB);

  glDrawElements(GL_TRIANGLES, Indices.size(), GL_UNSIGNED_INT, 0);

  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
}

