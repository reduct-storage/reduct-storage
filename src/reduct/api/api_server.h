// Copyright 2021 Alexey Timin

#ifndef REDUCT_STORAGE_API_SERVER_H
#define REDUCT_STORAGE_API_SERVER_H

#include <map>
#include <memory>
#include <string>

#include "reduct/core/error.h"

namespace reduct::api {

/**
 * API handler with all the request callbacks
 */
class IApiHandler {
 public:
  /**
   * Response on info request
   */
  struct InfoResponse {
    std::string version;
  };

  /**
   * Callback for info request
   * @param info response
   * @return
   */
  [[nodiscard]] virtual core::Error OnInfoRequest(InfoResponse* info) const = 0;
};

/**
 * HTTP API Server
 */
class IApiServer {
 public:
  struct Options {
    std::string host;
    int port;
    std::string base_path;
  };

  /**
   * Build implementation of API Server
   * @param handler Handler with all the callbacks to process HTTP request separately
   * @param options
   * @return pointer to the implementation
   */
  static std::unique_ptr<IApiServer> Build(std::unique_ptr<IApiHandler> handler, Options options);

  /**
   * Runs HTTP server
   */
  virtual void Run() const = 0;
};

}  // namespace reduct::api

#endif  // REDUCT_STORAGE_API_SERVER_H