#include <iostream>
#include <string>
#include "engine.h"

//The main function is the first function the program will run
int main(int argc, char* argv[])
{
  //Check if user has inputted a file name
  if(argc !=2){
    cout<<"Error: incorrect input - expected 1 CL arg for configuration file"<<endl;
    return EXIT_FAILURE;
  }
  std::string config_file = argv[1];

  //Config file is always at the root, so ensure the path starts there
  string filePath = "../" + config_file; 

  // Start an engine and run it then cleanup after
  Engine *engine = new Engine("Tutorial Window Name", 500, 720);

  //Check if the file name the user gives is correct
  if (!std::ifstream(filePath))
  {
    printf("The config file given does not exist.\n");
    delete engine;
    engine = NULL;
    return 1;
  }

  //Check if the file name is valid
  if(!engine->Initialize(filePath))
  {
    printf("The engine failed to start.\n");
    delete engine;
    engine = NULL;
    return 1;
  }

  //begin the program!
  engine->Run();

  //delete everything when the engine gets stopped by the user
  delete engine;
  engine = NULL;
  return 0;
}