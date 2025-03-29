#include "Engine.h"

Engine::Engine() 
{}

Engine::~Engine()
{}

void Engine::loop()
{
    clock_t prev = clock();
    double elapsed = 0, lag = 0;
    clock_t curr = clock();
    initialize();
    while(!window.close()) // ! window close
    {
        elapsed = ((double)curr - (double)prev) * 1000 / CLOCKS_PER_SEC;
        lag += elapsed;
        prev = clock();

        input();
/*      while(lag >= ms_per_update)
        {
            update();
            lag -= ms_per_update;
        }
*/
        update();
        render(lag/ms_per_update);
        curr = clock();

        framesCounter++;
        if(lag >= 1000)
        {
          lag = 0;
          currentFPS = 1000.0/(double)framesCounter;
          fpsText = "fps: " + std::to_string(framesCounter);  
          std::cout << framesCounter << " " << std::flush;
          framesCounter = 0;
        }
  
    }
    
}    

void Engine::initialize()
{
    proj::setPerspective(60.0f,0.1f,1000.0f,window.getAspect());
    glClearColor(0.9f, 0.9f, 0.95f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    sun = DirectionalLight((1.0f/255.0f)*Vec3(255,255,255));
    sun.rotate(-45, 0, 0);
    std::cout << sun.getDirection() << std::endl;

    geoTerrain.initialize(64, 2);
    shadowTerrain.initialize(32, 4);

    meshMap.emplace("scary", "scary");
    texMap.emplace("scary", "scaryBrown");
    objects.emplace_back(meshMap["scary"], texMap["scary"]);
    objects[0].setPosition(0,-41, 0);

    texMap.emplace("heightmap", "pilars");
    texMap.emplace("normalmap", "NormalMap");
    
    cascadedShadow.createShadowFBOs(512,512); 
    cascadedShadow.update(cam, sun);
}

void Engine::input()
{
    window.handleKey(translateForward, translateSide, transVal);
    window.handleMouse(rotx, roty); 
    window.handleHold(hold);
    window.handleTerrain(updateTerrain);
    window.handleWireframe(wireframe);
    window.pollEvents(); 
}

void Engine::update()
{
  if(hold)
  {
    cam.setPosition(0,0,0);
    rotx = 0; roty = 0;
    cam.spin[0] = 1; cam.spin[1] = 0; cam.spin[2] = 0; cam.spin[3] = 0;
  }
  else
  {
    cam.rotate(rotx, roty, 0);
    rotx = 0;	roty = 0;
    cam.translate(translateForward, translateSide);
    if(updateTerrain)
    {
      geoTerrain.update(cam.position);
      //shadowTerrain.update(cam.position);
    }
    cascadedShadow.update(cam, sun);
  }


  translateForward = 0; translateSide = 0;
  
}

void Engine::render(double dt)
{
  glEnable(GL_DEPTH_TEST);
  for(int cascadeIndex = 0; cascadeIndex < n_cascades; cascadeIndex++)
  {
    cascadedShadow.shadowFBO[cascadeIndex].bind();
    auto [w,h] = cascadedShadow.shadowFBO[cascadeIndex].getDimensions();
    glViewport(0,0,w,h);
    glClear(GL_DEPTH_BUFFER_BIT);
    //objectRenderer.renderShadows(objects, cascadedShadow, cascadeIndex, cam);
    //shadowTerrain.update(cascadedShadow.getSunCam(cascadeIndex).position);
    geoRenderer.renderShadows(geoTerrain, cascadedShadow, cascadeIndex, cam, texMap["heightmap"]);
    cascadedShadow.shadowFBO[cascadeIndex].unbind();
  }

  auto [winW,winH] = window.getDimensions();
  glViewport(0,0,winW,winH);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  objectRenderer.render(objects, cam, sun);
  if(wireframe)
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  geoRenderer.render(geoTerrain, cam, sun, texMap["heightmap"], texMap["normalmap"], cascadedShadow);
  if(wireframe)
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  glDisable(GL_DEPTH_TEST);
  guiRenderer.render(guiQuad,cascadedShadow.shadowFBO[0].getTextureId(), Vec2(-1.0f+0.1f, 1.0f-0.1f));
  guiRenderer.render(guiQuad,cascadedShadow.shadowFBO[1].getTextureId(), Vec2(-1.0f+0.31f, 1.0f-0.1f));
  guiRenderer.render(guiQuad,cascadedShadow.shadowFBO[2].getTextureId(), Vec2(-1.0f+0.1f, 1.0f-0.31f));
  guiRenderer.render(guiQuad,cascadedShadow.shadowFBO[3].getTextureId(), Vec2(-1.0f+0.31f,1.0f-0.31f));

  window.swap();

}
