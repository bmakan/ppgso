// Example gl_scene
// - Demonstrates the concept of a scene
// - Uses abstract object interface for Update and Render steps
// - Creates a simple game scene with Player, Asteroid and Space objects
// - Contains a generator object that does not render but adds Asteroids to the scene
// - Some objects use shared resources and all object deallocations are handled automatically
// - Controls: LEFT, RIGHT, "R" to reset, SPACE to fire

#include <iostream>
#include <vector>
#include <map>
#include <list>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "scene.h"
#include "camera.h"
#include "player.h"
#include "ground.h"
#include "generator.h"

const unsigned int WIDTH = 1920;
const unsigned int HEIGTH = 1080;

Scene scene;
float dt;

// Set up the scene
void InitializeScene() {
  scene.objects.clear();

  // Create a camera
  auto camera = CameraPtr(new Camera{ 60.0f, 16.0f/9.0f, 0.1f, 100.0f});
  scene.camera = camera;

  // STarting point for the player.
  auto ground = GroundPtr(new Ground{});
  ground->position.y = 0;
  ground->position.z = 0;
  ground->timeToDetonation = 100.0f;
  scene.objects.push_back(ground);

  auto generator = GeneratorPtr(new Generator{});
  generator->tileScale = ground->scale.y;
  generator->position.z = 5 * 2.0f * ground->scale.y;
  scene.objects.push_back(generator);

//  for(int i = 0; i < 5; i++) {
//    // Adding ground
//    auto ground = GroundPtr(new Ground{});
//    ground->position.y = (float)i * 5.0f;
//    ground->position.z = (float)i * 2.0f * ground->scale.y;
//    scene.objects.push_back(ground);
//  }

//  // Add space background
//  auto space = SpacePtr(new Space{});
//  scene.objects.push_back(space);

//  // Add generator to scene
//  auto generator = GeneratorPtr(new Generator{});
//  generator->position.y = 10.0f;
//  scene.objects.push_back(generator);

  // Add player to the scene
  auto player = PlayerPtr(new Player{});
  player->position.y = 20.0f;
  scene.objects.push_back(player);
  camera->player = player.get();
}

// Keyboard press event handler
void OnKeyPress(GLFWwindow* /* window */, int key, int /* scancode */, int action, int /* mods */) {
  scene.keyboard[key] = action;

  // Reset
  if (key == GLFW_KEY_R && action == GLFW_PRESS) {
    InitializeScene();
  }
}

// Mouse move event handler
void OnMouseMove(GLFWwindow* window, double xpos, double ypos) {
//  scene.mouse.x = xpos;
//  scene.mouse.y = ypos;
//
//  scene.camera->pitch -= scene.camera->mouseSpeed * (HEIGTH/2.0f - ypos) * dt;
//  scene.camera->player->rotate(WIDTH/2.0f - xpos);
//  glfwSetCursorPos(window, WIDTH/2, HEIGTH/2);
}

// Mouse scroll event handler
void OnMouseScroll(GLFWwindow* /* window */, double /* xoffset */, double yoffset){
  scene.camera->distance -= (float)yoffset * 100.0f * dt;
  if(scene.camera->distance < 10.0f){
    scene.camera->distance = 10.0f;
  }
  else if(scene.camera->distance > 25.0f){
    scene.camera->distance = 25.0f;
  }
}

int main() {
  // Initialize GLFW
  if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW!" << std::endl;
    return EXIT_FAILURE;
  }

  // Setup OpenGL context
  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Try to create a window
  auto window = glfwCreateWindow(WIDTH, HEIGTH, "myGame", nullptr, nullptr);
  if (!window) {
    std::cerr << "Failed to open GLFW window, your graphics card is probably only capable of OpenGL 2.1" << std::endl;
    glfwTerminate();
    return EXIT_FAILURE;
  }

  // Finalize window setup
  glfwMakeContextCurrent(window);

  // Initialize GLEW
  glewExperimental = GL_TRUE;
  glewInit();
  if (!glewIsSupported("GL_VERSION_3_3")) {
    std::cerr << "Failed to initialize GLEW with OpenGL 3.3!" << std::endl;
    glfwTerminate();
    return EXIT_FAILURE;
  }

  // Add keyboard and mouse handlers
  glfwSetKeyCallback(window, OnKeyPress);
  glfwSetCursorPosCallback(window, OnMouseMove);
  glfwSetScrollCallback(window, OnMouseScroll);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN); // Hide mouse cursor
  glfwSetInputMode(window, GLFW_STICKY_KEYS, 1);

  // Initialize OpenGL state
  // Enable Z-buffer
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);

  // Enable polygon culling
  glEnable(GL_CULL_FACE);
  glFrontFace(GL_CCW);
  glCullFace(GL_BACK);

  InitializeScene();

  // Track time
  float time = (float)glfwGetTime();

  // Main execution loop
  while (!glfwWindowShouldClose(window) && glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS) {
    // Compute time delta
    dt = (float)glfwGetTime() - time;
    time = (float)glfwGetTime();

//    glfwSetCursorPos(window, WIDTH/2, HEIGTH/2);

    // Set gray background
    glClearColor(.5f,.5f,.5f,0);
    // Clear depth and color buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glfwGetCursorPos(window, &scene.mouse.x, &scene.mouse.y);

    scene.camera->pitch -= scene.camera->mouseSpeed * (HEIGTH/2.0f - (float)scene.mouse.y) * dt;
    scene.camera->player->rotate(WIDTH/2.0f - (float)scene.mouse.x);

    glfwSetCursorPos(window, WIDTH/2.0f, HEIGTH/2.0f);

    // Update and render all objects
    scene.Update(dt);
    scene.Render();

    // Display result
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // Clean up
  glfwTerminate();

  return EXIT_SUCCESS;
}
