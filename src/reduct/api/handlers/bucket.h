// Copyright 2021 Alexey Timin

#ifndef REDUCT_STORAGE_BUCKET_H
#define REDUCT_STORAGE_BUCKET_H

#include <App.h>

#include "reduct/api/api_server.h"

namespace reduct::api::handlers {

/**
 * Handle HTTP request to creat a new bucket
 * @tparam SSL
 * @param callback Handler class with all callbacks
 * @param res HTTP response
 * @param req HTTP request
 * @param name name of bucket
 */
template <bool SSL = false>
void HandleCreateBucket(ICreateBucketCallback &callback, uWS::HttpResponse<SSL> *res, uWS::HttpRequest *req,
                        std::string_view name);

}  // namespace reduct::api::handlers

#endif  // REDUCT_STORAGE_BUCKET_H
