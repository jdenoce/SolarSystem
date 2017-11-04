#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <iostream>
#include <array>
using namespace std;

#include "graphics_headers.h"
#include "camera.h"
#include "shader.h"
#include "object.h"
#include "texture.h"

//variables for the planet
struct planet{
    float scale;
    float radius;
    float orbit;
    float rotation;
    std::string texture;
};

//variables for the moon
struct moon{
    float scale;
    float radius;
    float orbit;
    std::string texture;
};

class Graphics
{
  public:
    Graphics();                                                                 //Graphics constructor
    ~Graphics();                                                                //Graphics destructor
    bool Initialize(int width, int height, std::string config_file);            //Initialize all of the values, shaders, matrices, objects, camera, etc.
    void Update(unsigned int dt);                                               //Update the objects and the camera for every frame
    void Render();                                                              //Render the objects
    void getUserInput(char input);                                              //User inputs to control the camera and speed of objects
    void parse_config(std::string config_file);                                 //Obtain values from the config file

  private:

    //arrays for planets and moons
    planet* planet_array;
    moon* moon_array;

    //Displays errors encountered by OpenGL on the terminal
    std::string ErrorString(GLenum error);

    //arrays for setting the values of each planet and each moon
    planet planetInfo[11];
    moon moonInfo[7];

    //the textures!
    Texture* pTextureArray[14];

    //the camera!
    Camera *m_camera;

    //the shader!
    Shader *m_shader;

    //the matrices!
    GLint m_projectionMatrix;
    GLint m_viewMatrix;
    GLint m_modelMatrix;


    //the objects!
    Object *planets[11];                                                        //9 planets + 2 = 11, includes skybox and sun
    Object *moons[7];                                                           //one per planet, none for Mercury and Venus
    Object *rings[3];                                                           //just Saturn for now

    //private variable that controls speed of rotations/orbits
    double gameSpeed;
};


#endif /* GRAPHICS_H */
