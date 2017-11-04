#ifndef OBJECT_H
#define OBJECT_H

#include <vector>
#include "graphics_headers.h"
#include <string>
#include "texture.h"

//This class handles all of the movement and rendering of every object, including planets, moons, rings, and orbit displays
class Object
{
  public:
    Object(std::string myFile);                                                                         //takes string of obj file to load, Object constructor
    ~Object();                                                                                          //Object destructor
    void planet_update(unsigned int dt, float scale, float radius, float orbit, float rotation);        //Updates position, rotation, and orbit of a planet
    void moon_update(unsigned int dt, float scale, float radius, float orbit, glm::mat4 parent);        //Updates position, rotation, and orbit of a planet's moon
    void ring_update(unsigned int dt, float scale, float rotation, glm::mat4 parent);                   //Updates position, rotation, and orbit of a planet's ring
    void Render(Texture* pTexture);                                                                     //Commence rendering
        
    glm::mat4 GetModel();                                                                               //Returns the model

  private:
    glm::mat4 model;
    std::vector<Vertex> Vertices;
    std::vector<unsigned int> Indices;
    GLuint VB;
    GLuint IB;

    float angle1;
    float angle2;
};

#endif /* OBJECT_H */
