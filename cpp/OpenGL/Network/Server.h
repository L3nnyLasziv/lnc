#pragma once

#include <queue>

#include "NetCommon.h"


class ClientConnection {
public:
  ClientConnection(TCPSocket* connectionSocket, uint32_t id, const std::string& key, uint32_t timeout);
  virtual ~ClientConnection();
  bool isConnected();
  std::string checkData();
  
  uint32_t getID() const {return id;}
  
  void enqueueMessage(const std::string& m);

private:
  TCPSocket* connectionSocket;
  uint32_t id;
  uint32_t messageLength{0};
  std::vector<int8_t> recievedBytes;
  std::unique_ptr<AESCrypt> crypt;
  std::unique_ptr<AESCrypt> sendCrypt;
  std::string key;
  uint32_t timeout;
  
  std::mutex messageQueueLock;
  std::queue<std::string> messageQueue;

  bool continueRunning{true};
  std::thread sendThread;

  std::string handleIncommingData(int8_t* data, uint32_t bytes);
  
  void sendRawMessage(std::string message);
  void sendRawMessage(std::vector<int8_t> rawData);
  void sendRawMessage(const int8_t* rawData, uint32_t size);
  std::vector<uint8_t> intToVec(uint32_t i) const;
  
  void sendMessage(std::string message);

  void sendFunc();
};

class Server {
public:
  Server(short port, const std::string& key="", uint32_t timeout = 5000);
  virtual ~Server();
  void start();
  
  bool isStarting() const {return starting;}
  bool isOK() const {return ok;}
  
  virtual void handleClientConnection(uint32_t /* id */) {};
  virtual void handleClientMessage(uint32_t id, const std::string& message) = 0;
  virtual void handleClientDisconnection(uint32_t /* id */) {};
  
  void sendMessage(const std::string& message, uint32_t id=0, bool invertID=false);
  
  std::vector<uint32_t> getValidIDs();
  
private:
  short port;
  uint32_t timeout;
  uint32_t lastClientId{0};
  std::string key;
  
  bool ok{false};
  bool starting{true};
  bool continueRunning{true};
  std::thread connectionThread;
  std::thread clientThread;
  std::recursive_mutex clientVecMutex;
   
  std::shared_ptr<TCPServer> serverSocket;
  std::vector<std::shared_ptr<ClientConnection>> clientConnections;
  
  void shutdownServer();
  void clientFunc();
  void serverFunc();
    
  void removeClient(size_t i);
  
};
