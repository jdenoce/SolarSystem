#include "graphics.h"
#include <Magick++.h>
#include "texture.h"
#include <GL/glew.h>
#include <fstream>

//Graphics constructor
Graphics::Graphics()
{

}

//Graphics destructor
Graphics::~Graphics()
{

}

//Initialize all of the values, shaders, matrices, objects, camera, etc.
bool Graphics::Initialize(int width, int height, std::string config_file)
{
  // Used for the linux OS
  #if !defined(__APPLE__) && !defined(MACOSX)
    // cout << glewGetString(GLEW_VERSION) << endl;
    glewExperimental = GL_TRUE;

    auto status = glewInit();

    // This is here to grab the error that comes from glew init.
    // This error is an GL_INVALID_ENUM that has no effects on the performance
    glGetError();

    //Check for error
    if (status != GLEW_OK)
    {
      std::cerr << "GLEW Error: " << glewGetErrorString(status) << "\n";
      return false;
    }
  #endif

  // For OpenGL 3
  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // Init Camera
  m_camera = new Camera();
  if(!m_camera->Initialize(width, height))
  {
    printf("Camera Failed to Initialize\n");
    return false;
  }

  //declare a temporary, reusable string
  string ignore;

  //initialize the file
  fstream configFile;
  configFile.open(config_file.c_str());

  //reads in planet info
  for(int i = 0; i < 11; i++)
  {
    getline(configFile, ignore, '\n');
    configFile >> planetInfo[i].scale;
    planetInfo[i].scale *= 35;//used for scaling all the values while still remaining in ratio w/ other planets
    configFile >> planetInfo[i].radius;
	  planetInfo[i].radius *= 2;//used for scaling all the values while still remaining in ratio w/ other planets
    configFile >> planetInfo[i].orbit;
    planetInfo[i].orbit *= 1;//used for scaling all the values while still remaining in ratio w/ other planets
    configFile >> planetInfo[i].rotation;
    planetInfo[i].rotation*= 1;//used for scaling all the values while still remaining in ratio w/ other planets
    configFile >> planetInfo[i].texture;
    getline(configFile, ignore, '\n');
  }

  getline(configFile, ignore, '\n');

  //reads in moon info
  for(int i = 0; i < 7; i++)
  {
    getline(configFile, ignore, '\n');
    configFile >> moonInfo[i].scale;
    moonInfo[i].scale *= 20;
    configFile >> moonInfo[i].radius;
    moonInfo[i].radius *= 35;
    configFile >> moonInfo[i].orbit;
    configFile >> moonInfo[i].texture;
    getline(configFile, ignore, '\n');
  }


  GLuint gSampler;
  //intitialize planets and textures
  for(int i = 0; i < 11; i++)
  {
    pTextureArray[i] = NULL;
    pTextureArray[i] = new Texture(GL_TEXTURE_2D, planetInfo[i].texture);
    pTextureArray[i]->Load();
    planets[i] = new Object("../src/sphere.obj");
  }

  //initialize moons
  for(int i = 0; i < 7; i++)
  {
    pTextureArray[11] = NULL;
    pTextureArray[11] = new Texture(GL_TEXTURE_2D, moonInfo[i].texture);
    pTextureArray[11]->Load();
    moons[i] = new Object("../src/sphere.obj");
  }

  //initialize rings

  pTextureArray[12] = NULL;
  pTextureArray[12] = new Texture(GL_TEXTURE_2D, "../textures/saturn-rings.jpg");
  pTextureArray[12]->Load();
  pTextureArray[13] = NULL;
  pTextureArray[13] = new Texture(GL_TEXTURE_2D, "../textures/jupiter-rings.jpg");
  pTextureArray[13]->Load();


  rings[0] = new Object("../src/saturns_rings.obj");
  rings[1] = new Object("../src/saturns_rings.obj");
  rings[2] = new Object("../src/saturns_rings.obj");

  //initialize speed modifier
  gameSpeed = 1;


  //initialize cam position
  double vectorX = m_camera->focusX - m_camera->eyePosX;
  double vectorY = m_camera->focusY - m_camera->eyePosY;
  double vectorZ = m_camera->focusZ - m_camera->eyePosZ;

  for (int i = 0; i < 600; i++)
  {
    m_camera->eyePosX  = m_camera->eyePosX  + vectorX * (-0.1);
    m_camera->eyePosZ  = m_camera->eyePosZ  + vectorZ * (-0.1);
    m_camera->focusX = m_camera->focusX + vectorX * (-0.1);
    m_camera->focusZ = m_camera->focusZ + vectorZ * (-0.1);
  }

  for (int i = 0; i < 1200; i++)
  {
    m_camera->eyePosZ  = m_camera->eyePosZ  + vectorX * (-0.1);
    m_camera->eyePosX  = m_camera->eyePosX  + (-vectorZ) * (-0.1);
    
    m_camera->focusZ = m_camera->focusZ + vectorX * (-0.1);
    m_camera->focusX = m_camera->focusX + (-vectorZ) * (-0.1);
  }

  for (int i = 0; i < 10; i++)
  {
    m_camera->focusY -= 0.2;
  }

  for (int i = 0; i < 1000; i++)
  {
    m_camera->eyePosY += 0.1;
    m_camera->focusY += 0.1;
  }


  for (int i = 0; i < 480; i++)
  {
    m_camera->focusZ = (float)(m_camera->eyePosZ + sin(0.02)* vectorX + cos(0.02)* vectorZ);
    m_camera->focusX = (float)(m_camera->eyePosX + cos(0.02)* vectorX - sin(0.02)* vectorZ);
  }
  for (int i = 0; i < 480; i++)
  {
    m_camera->focusZ = (float)(m_camera->eyePosZ + sin(0.02)* vectorX + cos(0.02)* vectorZ);
    m_camera->focusX = (float)(m_camera->eyePosX + cos(0.02)* vectorX - sin(0.02)* vectorZ);
  }



  


  // Set up the shaders
  m_shader = new Shader();
  if(!m_shader->Initialize())
  {
    printf("Shader Failed to Initialize\n");
    return false;
  }

  // Add the vertex shader
  if(!m_shader->AddShader(GL_VERTEX_SHADER))
  {
    printf("Vertex Shader failed to Initialize\n");
    return false;
  }

  // Add the fragment shader
  if(!m_shader->AddShader(GL_FRAGMENT_SHADER))
  {
    printf("Fragment Shader failed to Initialize\n");
    return false;
  }

  // Connect the program
  if(!m_shader->Finalize())
  {
    printf("Program to Finalize\n");
    return false;
  }

  // Locate the projection matrix in the shader
  m_projectionMatrix = m_shader->GetUniformLocation("projectionMatrix");
  if (m_projectionMatrix == INVALID_UNIFORM_LOCATION) 
  {
    printf("m_projectionMatrix not found\n");
    return false;
  }

  // Locate the view matrix in the shader
  m_viewMatrix = m_shader->GetUniformLocation("viewMatrix");
  if (m_viewMatrix == INVALID_UNIFORM_LOCATION) 
  {
    printf("m_viewMatrix not found\n");
    return false;
  }

  // Locate the model matrix in the shader
  m_modelMatrix = m_shader->GetUniformLocation("modelMatrix");
  if (m_modelMatrix == INVALID_UNIFORM_LOCATION) 
  {
    printf("m_modelMatrix not found\n");
    return false;
  }

  //enable depth testing
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  return true;
}

void Graphics::Update(unsigned int dt)
{
  // Update the objects with dt being multiplied by the speed set by user input to control the speed of the rotation/orbits
  for(int i = 0; i < 11; i++)
  {
    planets[i]->planet_update(dt * gameSpeed, planetInfo[i].scale, planetInfo[i].radius, planetInfo[i].orbit, planetInfo[i].rotation);
  }

  for(int i = 0; i < 6; i++)
  {
    moons[i]->moon_update(dt * gameSpeed, moonInfo[i].scale, moonInfo[i].radius, moonInfo[i].orbit, planets[i+3]->GetModel());
  }

  for(int i = 0; i < 3; i++)
  {
    if(i < 2)
    {
      rings[i]->ring_update(dt * gameSpeed, planetInfo[i+5].scale , 1.0, planets[i+5]->GetModel());
    }
    else
    {
      rings[i]->ring_update(dt * gameSpeed, planetInfo[i+5].scale + 105, 1.0, planets[i+5]->GetModel());
    }
  }
  
  //Update the camera
  m_camera->Update();
}

//Render the objects
void Graphics::Render()
{
  //clear the screen
  glClearColor(0.0, 0.0, 0.2, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Start the correct program
  m_shader->Enable();

  // Send in the projection and view to the shader
  glUniformMatrix4fv(m_projectionMatrix, 1, GL_FALSE, glm::value_ptr(m_camera->GetProjection())); 
  glUniformMatrix4fv(m_viewMatrix, 1, GL_FALSE, glm::value_ptr(m_camera->GetView())); 

  // Render the object
  //planets
  for(int i = 0; i < 11; i++)
  {
    glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(planets[i]->GetModel()));
    planets[i]->Render(pTextureArray[i]);
  }

  //moons
  for(int i = 0; i < 7; i++)
  {
    glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(moons[i]->GetModel()));
    moons[i]->Render(pTextureArray[11]);
  }

  //rings
  glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(rings[0]->GetModel()));
  rings[0]->Render(pTextureArray[12]);
  glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(rings[1]->GetModel()));
  rings[1]->Render(pTextureArray[12]);
  glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(rings[2]->GetModel()));
  rings[2]->Render(pTextureArray[13]);




  // Get any errors from OpenGL
  auto error = glGetError();
  if ( error != GL_NO_ERROR )
  {
    string val = ErrorString( error );
    std::cout<< "Error initializing OpenGL! " << error << ", " << val << std::endl;
  }
}

//User inputs to control the camera
void Graphics::getUserInput(char input)
{
  //declare variables
  double movementX = 0;
  double movementY = 0;
  double movementZ = 0;
  double rotationX = 0;
  double rotationY = 0;
  double vectorX = m_camera->focusX - m_camera->eyePosX;
  double vectorY = m_camera->focusY - m_camera->eyePosY;
  double vectorZ = m_camera->focusZ - m_camera->eyePosZ;

  //speed modifier
  if (input == '-' && gameSpeed > 0)
  {
    gameSpeed -= 0.1;
  }
  if (input == '=')
  {
    gameSpeed += 0.1;
  }
  if (input == 'b')
  {
    gameSpeed = 1;
  }

  if(gameSpeed < 0.001)
  {
     gameSpeed = 0.001;
  }



  //detect input for moving horizontally
  if (input == 'a')
  {
    movementX = -0.1;
  }
  if (input == 'd')
  {
    movementX = 0.1;
  }

  //move the camera horizontally
  if (movementX != 0)
  {
    m_camera->eyePosZ  = m_camera->eyePosZ  + vectorX * movementX;
    m_camera->eyePosX  = m_camera->eyePosX  + (-vectorZ) * movementX;
    
    m_camera->focusZ = m_camera->focusZ + vectorX * movementX;
    m_camera->focusX = m_camera->focusX + (-vectorZ) * movementX;
  }




  //detect input for moving vertically
  if (input == 'h')
  {
    movementY = -0.3;
  }
  if (input == 'y')
  {
    movementY = 0.3;
  }

  //move the camera vertically
  if (movementY != 0)
  {
    m_camera->eyePosY += movementY;
    m_camera->focusY += movementY;
  }




  //detect input for moving forward or backward
  if (input == 'w')
  {
    movementZ = 0.1;
  }
  if (input == 's')
  {
    movementZ = -0.1;
  }

  //move the camera forward or backward
  if (movementZ != 0)
  {
    m_camera->eyePosX  = m_camera->eyePosX  + vectorX * movementZ;
    m_camera->eyePosZ  = m_camera->eyePosZ  + vectorZ * movementZ;
    m_camera->focusX = m_camera->focusX + vectorX * movementZ;
    m_camera->focusZ = m_camera->focusZ + vectorZ * movementZ;
  }






  //detect input for looking to the left or right
  if (input == '4')
  {
    rotationX = -0.02;
  }
  if (input == '6')
  {
    rotationX = 0.02;
  }

  //detect input for looking up or down
  if (input == '8')
  {
    rotationY = -0.2;
  }
  if (input == '2')
  {
    rotationY = 0.2;     
  }

  

  

  //look to the left or right
  if (rotationX != 0)
  {
    m_camera->focusZ = (float)(m_camera->eyePosZ + sin(rotationX)* vectorX + cos(rotationX)* vectorZ);
    m_camera->focusX = (float)(m_camera->eyePosX + cos(rotationX)* vectorX - sin(rotationX)* vectorZ);
  }

  //look up or down
  if (rotationY != 0)
  {
    m_camera->focusY -= rotationY;
  }      

}

//Displays errors encountered by OpenGL on the terminal
std::string Graphics::ErrorString(GLenum error)
{
  if(error == GL_INVALID_ENUM)
  {
    return "GL_INVALID_ENUM: An unacceptable value is specified for an enumerated argument.";
  }

  else if(error == GL_INVALID_VALUE)
  {
    return "GL_INVALID_VALUE: A numeric argument is out of range.";
  }

  else if(error == GL_INVALID_OPERATION)
  {
    return "GL_INVALID_OPERATION: The specified operation is not allowed in the current state.";
  }

  else if(error == GL_INVALID_FRAMEBUFFER_OPERATION)
  {
    return "GL_INVALID_FRAMEBUFFER_OPERATION: The framebuffer object is not complete.";
  }

  else if(error == GL_OUT_OF_MEMORY)
  {
    return "GL_OUT_OF_MEMORY: There is not enough memory left to execute the command.";
  }
  else
  {
    return "None";
  }
}

