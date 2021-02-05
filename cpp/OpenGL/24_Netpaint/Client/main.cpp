#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <optional>

#include <Client.h>
#include <GLApp.h>
#include <Rand.h>

#include "../PainterCommon.h"

class MyClient : public Client {
public:
  MyClient(const std::string& address, short port, const std::string& name) :
  Client{address, port, "asdn932lwnmflj23", 5000},
  name{name},
  color{Vec3::hsvToRgb({360*Rand::rand01(),0.5f,1.0f}), 1.0f}
  {
    for (uint32_t y = 0;y<image.height;++y) {
      for (uint32_t x = 0;x<image.width;++x) {
        const Vec3 rgb{0,0,0};
        image.setNormalizedValue(x,y,0,rgb.x());
        image.setNormalizedValue(x,y,1,rgb.y());
        image.setNormalizedValue(x,y,2,rgb.z());
        image.setValue(x,y,3,255);
      }
    }
  }

  void moveMouse(uint32_t userID, const Vec2& pos) {
    for (size_t i = 0;i<mouseInfos.size();++i) {
      if (mouseInfos[i].id == userID) {
        mouseInfos[i].pos = pos;
        break;
      }
    }
  }

  void addMouse(uint32_t userID, const std::string& name, const Vec4& color) {
    bool found{false};
    for (size_t i = 0;i<mouseInfos.size();++i) {
      if (mouseInfos[i].id == userID) {
        mouseInfos[i].name = name;
        mouseInfos[i].color = color;
        found = true;
        break;
      }
    }
    if (!found) {
      mouseInfos.push_back({userID, name, color, {0,0}});
    }
  }

  void removeMouse(uint32_t userID) {
    for (size_t i = 0;i<mouseInfos.size();++i) {
      if (mouseInfos[i].id == userID) {
        mouseInfos.erase(mouseInfos.begin()+i);
        break;
      }
    }
  }
  
  void initDataFromServer(const Image& serverImage,
                          const std::vector<MouseInfo>& mi) {
    image      = serverImage;
    mouseInfos = mi;
  }
  
  void paint(const Vec4& color, const Vec2i& pos) {
    image.setNormalizedValue(pos.x(),pos.y(),0,color.x());
    image.setNormalizedValue(pos.x(),pos.y(),1,color.y());
    image.setNormalizedValue(pos.x(),pos.y(),2,color.z());
    image.setNormalizedValue(pos.x(),pos.y(),3,color.w());
  }
  
  virtual void handleNewConnection() override {
    NewUserPayload l(name, color);
    sendMessage(l.toString());
  }

  virtual void handleServerMessage(const std::string& message) override {
    PayloadType pt = identifyString(message);
       
    miMutex.lock();
    switch (pt) {
      case PayloadType::MousePosPayload : {
        MousePosPayload l(message);
        moveMouse(l.userID, l.mousePos);
        break;
      }
      case PayloadType::NewUserPayload  : {
        NewUserPayload l(message);
        addMouse(l.userID, l.name, l.color);
        break;
      }
      case PayloadType::LostUserPayload : {
        LostUserPayload l(message);
        removeMouse(l.userID);
        break;
      }
      case PayloadType::InitPayload : {
        InitPayload l(message);
        initDataFromServer(l.image, l.mouseInfos);
        break;
      }
      case PayloadType::CanvasUpdatePayload : {
        CanvasUpdatePayload l(message);
        paint(l.color, l.pos);
        break;
      }
      default:
        std::cout << "unknown message " << int(pt) << " received" << std::endl;
        break;
    };
    miMutex.unlock();
  }
  
  void setMousePos(const Vec2& normPos) {
    MousePosPayload m(normPos);
    sendMessage(m.toString());
  }
  
  void paintSelf(const Vec2i& pos) {
    paint(color, pos);
    CanvasUpdatePayload m{color, pos};
    sendMessage(m.toString());
  }
  
  const std::vector<MouseInfo>& getOtherMouseInfos() const {
    return mouseInfos;
  }
    
  const Image& getImage() const {
    return image;
  }
  
  void lockData() {
    miMutex.lock();
  }

  void unlockData() {
    miMutex.unlock();
  }

  Vec4 getColor() const {
    return color;
  }

private:
  std::mutex miMutex;
  std::vector<MouseInfo> mouseInfos;
  std::string name;
  Vec4 color;
  
  Image image{imageWidth,imageHeight};
};

class MyGLApp : public GLApp {
public:

  MyGLApp(MyClient& client) : GLApp(1024, size_t(1024.0f * float(imageHeight)/float(imageWidth)), 4, "Network Painter"), client(client) {}
  
  virtual void init() override {
    glEnv.setCursorMode(CursorMode::HIDDEN);
    GL(glEnable(GL_BLEND));
    GL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    GL(glBlendEquation(GL_FUNC_ADD));
  }
  
  virtual void mouseMove(double xPosition, double yPosition) override {
    Dimensions s = glEnv.getWindowSize();
    if (xPosition < 0 || xPosition > s.width || yPosition < 0 || yPosition > s.height) return;

    normPos = Vec2{float(xPosition/s.width),float(1.0-yPosition/s.height)};
    Vec2i iPos{int(normPos.x()*client.getImage().width),int(normPos.y()*client.getImage().height)};
    
    if (mouseDown) client.paintSelf(iPos);

    if (iPos != lastMousePos) {
      client.setMousePos(normPos);
    }
    lastMousePos = iPos;
  }
  
  virtual void mouseButton(int button, int state, int mods, double xPosition, double yPosition) override {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
      mouseDown = (state == GLFW_PRESS);
    }
  }
  
  virtual void keyboard(int key, int scancode, int action, int mods) override {
    if (action == GLFW_PRESS) {
      switch (key) {
        case GLFW_KEY_ESCAPE:
          closeWindow();
          break;
      }
    }
  }
    
  virtual void draw() override {
    client.lockData();
    drawImage(client.getImage());
    std::vector<float> glShape;
    const std::vector<MouseInfo> otherMice = client.getOtherMouseInfos();
    for (const MouseInfo& m : otherMice) {
      glShape.push_back(m.pos.x() *2.0f-1.0f); glShape.push_back(m.pos.y() *2.0f-1.0f); glShape.push_back(0.0f);
      glShape.push_back(m.color.x()); glShape.push_back(m.color.y()); glShape.push_back(m.color.z());  glShape.push_back(m.color.w());
    }
    Vec4 color{client.getColor()};
    client.unlockData();
    drawPoints(glShape, 10, true);
    
    glShape.clear();
    glShape.push_back(normPos.x() *2.0f-1.0f); glShape.push_back(normPos.y() *2.0f-1.0f); glShape.push_back(0.0f);
    glShape.push_back(color.r()); glShape.push_back(color.y()); glShape.push_back(color.z());  glShape.push_back(color.w());
    drawPoints(glShape, 40, true);
  }

private:
  MyClient& client;
  Vec2 normPos{};
  bool mouseDown{false};
  Vec2i lastMousePos{-1,-1};

};

int main(int argc, char ** argv) {

 if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " ServerIP YourName" << std::endl;
    return EXIT_FAILURE;
  }
 
  MyClient c{argv[1], 11001, argv[2]};
  std::cout << "connecting ...";
  while (c.isConnecting()) {
    std::cout << "." << std::flush;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
  std::cout << " Done" << std::endl;
  
  if (c.isOK()) {
    MyGLApp myApp(c);
    myApp.run();
    return EXIT_SUCCESS;
  } else {
    std::cerr << "Unable to start client" << std::endl;
    return EXIT_FAILURE;
  }
}
