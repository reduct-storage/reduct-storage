// Copyright 2022 Reduct Storage Team

#include "reduct/asset/asset_manager.h"

#include <fmt/core.h>
#include <libzippp/libzippp.h>

#include <fstream>

#include "reduct/core/logger.h"

namespace reduct::asset {

using core::Result;
namespace fs = std::filesystem;

class AssetManager : public IAssetManager {
 public:
  explicit AssetManager(fs::path asset_path) : asset_path_(std::move(asset_path)) {}

  ~AssetManager() override = default;

  Result<std::string> Read(std::string_view relative_path) const override {
    auto full_path = asset_path_ / relative_path;
    if (!fs::exists(full_path)) {
      return {{}, {.code = 404, .message = fmt::format("File '{}' not found", relative_path)}};
    }

    std::ifstream file(full_path);
    if (!file) {
      return {{}, {.code = 500, .message = fmt::format("Failed to open '{}'", relative_path)}};
    }

    std::stringstream ss;
    ss << file.rdbuf();
    return {ss.str(), core::Error::kOk};
  }

 private:
  fs::path asset_path_;
};

std::unique_ptr<IAssetManager> IAssetManager::BuildFromZip(std::string zipped) {
  using libzippp::ZipArchive;
  auto zf = std::unique_ptr<ZipArchive>(ZipArchive::fromBuffer(zipped.data(), zipped.size()));

  if (!zf) {
    LOG_ERROR("Failed to extract asset");
    return nullptr;
  }

  fs::path asset_path = fs::temp_directory_path() /
                        fmt::format("asset_{}", std::chrono::high_resolution_clock::now().time_since_epoch().count());
  fs::create_directory(asset_path);
  for (auto&& entry : zf->getEntries()) {
    auto path = asset_path / fs::path(entry.getName()).relative_path();
    if (entry.isDirectory()) {
      fs::create_directory(path);
    }

    if (entry.isFile()) {
      std::ofstream out(path);
      if (!out) {
        LOG_ERROR("Failed open file {}: {}", path.string(), std::strerror(errno));
      }
      entry.readContent(out);
    }
  }

  zf->close();
  return std::make_unique<AssetManager>(asset_path);
}

class EmptyAssetManager : public IAssetManager {
 public:
  ~EmptyAssetManager() override = default;

  Result<std::string> Read(std::string_view relative_path) const override {
    return {{}, {.code = 404, .message = "No static files supported"}};
  }
};

std::unique_ptr<IAssetManager> IAssetManager::BuildEmpty() { return std::make_unique<EmptyAssetManager>(); }

}  // namespace reduct::asset
