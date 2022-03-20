// Copyright 2022 Alexey Timin

#include "reduct/storage/entry.h"

#include <fmt/core.h>
#include <google/protobuf/util/time_util.h>

#include <filesystem>
#include <fstream>

#include "reduct/core/logger.h"
#include "reduct/core/result.h"
#include "reduct/proto/storage/entry.pb.h"

namespace reduct::storage {

using core::Error;
using core::Result;
using google::protobuf::Timestamp;
using google::protobuf::util::TimeUtil;

auto to_time_t = IEntry::Time::clock::to_time_t;

namespace fs = std::filesystem;

class Entry : public IEntry {
 public:
  /**
   * Create a new entry
   * @param options
   */
  explicit Entry(Options options) : options_(std::move(options)), block_set_(), size_counter_{}, record_counter_{} {
    full_path_ = options_.path / options_.name;
    if (!fs::create_directories(full_path_)) {
      for (const auto& file : fs::directory_iterator(full_path_)) {
        auto& path = file.path();
        if (fs::is_regular_file(file) && path.extension() == kMetaExt) {
          try {
            auto ts = TimeUtil::MicrosecondsToTimestamp(std::stoul(path.stem().c_str()));
            proto::Block block;
            auto err = LoadBlockByTimestamp(ts, &block);
            if (err) {
              LOG_ERROR("{}", err.ToString());
              continue;
            }
            block_set_.insert(ts);
            size_counter_ += block.size();
            record_counter_ += block.records_size();
          } catch (std::exception& err) {
            LOG_ERROR("Wrong filename format {}: {}", path.string(), err.what());
          }
        }
      }
    }
  }

  [[nodiscard]] Error Write(std::string_view blob, const Time& time) override {
    enum class RecordType { kLatest, kBelated, kBelatedFirst };

    const auto proto_ts = FromTimePoint(time);

    RecordType type = RecordType::kLatest;
    proto::Block block;
    if (!block_set_.empty()) {
      // Load last block if it is exists
      if (auto err = LoadBlockByTimestamp(*block_set_.rbegin(), &block)) {
        return err;
      }
    }

    if (block.has_latest_record_time() && block.latest_record_time() >= proto_ts) {
      LOG_DEBUG("Timestamp {} is belated. Finding proper block", TimeUtil::ToString(proto_ts));

      if (*block_set_.begin() > proto_ts) {
        LOG_DEBUG("Timestamp earlier than first record");
        type = RecordType::kBelatedFirst;
        block = proto::Block();  // add a new block
      } else {
        type = RecordType::kBelated;
        auto err = FindBlock(proto_ts, &block);
        if (err) {
          return err;
        }
      }
    }
    if (!block.has_begin_time()) {
      LOG_DEBUG("First record_entry for current block");
      block.mutable_begin_time()->CopyFrom(proto_ts);
    }

    if (type == RecordType::kLatest && block.size() > options_.max_block_size) {
      LOG_DEBUG("Block {} is full. Create a new one", TimeUtil::TimestampToMicroseconds(block.begin_time()));
      auto ret = StartNextBlock(proto_ts);
      if (ret.error) {
        LOG_ERROR("Failed create a next block");
        return ret.error;
      }

      block = std::move(ret.result);
    }

    auto block_path = BlockPath(block);
    LOG_DEBUG("Write a record_entry for ts={} ({} kB) to {}", TimeUtil::ToString(proto_ts), blob.size() / 1024,
              block_path.string());
    std::ofstream block_file(block_path, std::ios::app | std::ios::binary);
    if (!block_file) {
      return {.code = 500, .message = "Failed open a block_file for writing"};
    }

    proto::EntryRecord record_entry;
    record_entry.set_blob(std::string{blob});
    record_entry.mutable_meta_data()->Clear();

    std::string data;
    if (!record_entry.SerializeToString(&data)) {
      return {.code = 500, .message = "Failed write a record_entry to a block_file"};
    }

    LOG_TRACE("Record {} bytes to {}", data.size(), block_path.string());
    block_file << data;

    // Update written block
    auto record = block.add_records();
    record->mutable_timestamp()->CopyFrom(proto_ts);
    record->set_begin(block.size());
    record->set_end(block.size() + data.size());

    block.set_size(block.size() + data.size());

    // Update counters
    record_counter_++;
    size_counter_ += data.size();

    switch (type) {
      case RecordType::kLatest:
        block.mutable_latest_record_time()->CopyFrom(proto_ts);
        break;
      case RecordType::kBelatedFirst:
        block.mutable_begin_time()->CopyFrom(proto_ts);
        break;
      case RecordType::kBelated:
        break;
    }

    return SaveBlock(block);
  }

  [[nodiscard]] ReadResult Read(const Time& time) const override {
    const auto proto_ts = FromTimePoint(time);

    LOG_DEBUG("Read a record for ts={}", TimeUtil::ToString(proto_ts));

    if (block_set_.empty() || proto_ts < *block_set_.begin()) {
      return {{}, {.code = 404, .message = "No records for this timestamp"}, time};
    }

    proto::Block block;
    if (auto err = LoadBlockByTimestamp(*block_set_.rbegin(), &block)) {
      return {{}, err};
    }

    if (block.latest_record_time() < proto_ts) {
      return {{}, {.code = 404, .message = "No records for this timestamp"}, time};
    }

    auto err = FindBlock(proto_ts, &block);
    if (err) {
      LOG_ERROR("No block in entry '{}' for ts={}", options_.name, TimeUtil::ToString(proto_ts));
      return {{}, {.code = 500, .message = "Failed to find the needed block in descriptor"}, time};
    }

    int record_index = -1;
    for (int i = 0; i < block.records_size(); ++i) {
      const auto& current_record = block.records(i);
      if (current_record.timestamp() == proto_ts) {
        record_index = i;
        break;
      }
    }

    if (record_index == -1) {
      return {{}, {.code = 404, .message = "No records for this timestamp"}, time};
    }

    auto block_path = BlockPath(block);
    LOG_DEBUG("Found block {} with needed record", block_path.string());

    std::ifstream block_file(block_path, std::ios::binary);
    if (!block_file) {
      return {{}, {.code = 500, .message = "Failed open a block for reading"}, time};
    }

    auto record = block.records(record_index);
    auto data_size = record.end() - record.begin();
    std::string data(data_size, '\0');
    block_file.seekg(record.begin());
    block_file.read(data.data(), data_size);

    proto::EntryRecord entry_record;
    if (!entry_record.ParseFromString(data)) {
      return {{}, {.code = 500, .message = "Failed parse a block"}, time};
    }

    return {entry_record.blob(), {}, time};
  }

  [[nodiscard]] ListResult List(const Time& start, const Time& stop) const override {
    auto start_ts = FromTimePoint(start);
    auto stop_ts = FromTimePoint(stop);
    LOG_DEBUG("List records for interval: ({}, {})", TimeUtil::ToString(start_ts), TimeUtil::ToString(stop_ts));
    if (start_ts > stop_ts) {
      return {{}, {.code = 422, .message = "Start timestamp cannot be older stop timestamp"}};
    }

    // Check boarders (is it okay if at least one record inside the interval
    if (block_set_.empty()) {
      return {{}, {.code = 404, .message = "No records in the entry"}};
    }

    if (stop_ts < *block_set_.begin()) {
      return {{}, {.code = 404, .message = "No records for time interval"}};
    }

    proto::Block block;
    if (auto err = LoadBlockByTimestamp(*block_set_.rbegin(), &block)) {
      return {{}, err};
    }

    if (block.latest_record_time() < start_ts) {
      return {{}, {.code = 404, .message = "No records for time interval"}};
    }

    // Find block range
    auto start_block = block_set_.upper_bound(start_ts);
    if (start_block == block_set_.end()) {
      start_block = block_set_.begin();
    } else if (start_block != block_set_.begin()) {
      start_block = std::prev(start_block);
    }

    auto stop_block = block_set_.lower_bound(stop_ts);

    std::vector<RecordInfo> records;
    for (auto block_it = start_block; block_it != stop_block; ++block_it) {
      proto::Block block;
      auto err = LoadBlockByTimestamp(*block_it, &block);
      if (err) {
        return {{}, err};
      }

      for (auto record_index = 0; record_index < block.records_size(); ++record_index) {
        const auto& record = block.records(record_index);
        if (record.timestamp() >= start_ts && record.timestamp() < stop_ts) {
          records.push_back(RecordInfo{.time = ToTimePoint(record.timestamp()), .size = record.end() - record.begin()});
        }
      }
    }

    if (records.empty()) {
      return {{}, {.code = 404, .message = "No records for time interval"}};
    }

    std::ranges::sort(records, {}, &RecordInfo::time);
    return {records, {}};
  }

  Error RemoveOldestBlock() override {
    if (block_set_.empty()) {
      return {.code = 500, .message = "Tries to remove a block in empty entry"};
    }

    proto::Block first_block;
    if (auto err = LoadBlockByTimestamp(*block_set_.begin(), &first_block)) {
      return err;
    }

    fs::remove(BlockPath(first_block));
    fs::remove(BlockPath(first_block, kMetaExt));

    size_counter_ -= first_block.size();
    record_counter_ -= first_block.records_size();
    block_set_.erase(first_block.begin_time());
    return {};
  }

  [[nodiscard]] Info GetInfo() const override {
    Time oldest_record, latest_record;
    if (!block_set_.empty()) {
      proto::Block latest_block;
      if (auto err = LoadBlockByTimestamp(*block_set_.rbegin(), &latest_block)) {
        LOG_ERROR("{}", err.ToString());
      }

      oldest_record = ToTimePoint(*block_set_.begin());
      latest_record = ToTimePoint(latest_block.latest_record_time());
    }
    return {
        .block_count = block_set_.size(),
        .record_count = record_counter_,
        .bytes = size_counter_,
        .oldest_record_time = oldest_record,
        .latest_record_time = latest_record,
    };
  }

  [[nodiscard]] const Options& GetOptions() const override { return options_; }

 private:
  [[nodiscard]] std::filesystem::path BlockPath(const proto::Block& block, std::string_view ext = kBlockExt) const {
    auto block_path = full_path_ / fmt::format("{}{}", TimeUtil::TimestampToMicroseconds(block.begin_time()), ext);
    return block_path;
  }

  Error LoadBlockByTimestamp(const Timestamp& proto_ts, proto::Block* block) const {
    auto file_name = full_path_ / fmt::format("{}{}", TimeUtil::TimestampToMicroseconds(proto_ts), kMetaExt);
    std::ifstream file(file_name);
    if (!file) {
      return {.code = 500, .message = fmt::format("Failed to load a block descriptor: {}", file_name.string())};
    }

    if (!block->ParseFromIstream(&file)) {
      return {.code = 500, .message = fmt::format("Failed to parse meta: {}", file_name.string())};
    }
    return {};
  }

  Error SaveBlock(const proto::Block& block) {
    auto file_name = full_path_ / fmt::format("{}{}", TimeUtil::TimestampToMicroseconds(block.begin_time()), kMetaExt);
    std::ofstream file(file_name);
    if (file) {
      block.SerializeToOstream(&file);
      block_set_.insert(block.begin_time());
      return {};
    } else {
      return {.code = 500, .message = "Failed to save a block descriptor"};
    }
  }

  Result<proto::Block> StartNextBlock(const Timestamp& ts) {
    proto::Block block;
    block.mutable_begin_time()->CopyFrom(ts);
    return {block, SaveBlock(block)};
  }

  Error FindBlock(Timestamp proto_ts, proto::Block* block) const {
    auto ts = block_set_.upper_bound(proto_ts);
    if (ts == block_set_.end()) {
      proto_ts = *block_set_.rbegin();
    } else {
      proto_ts = *std::prev(ts);
    }
    return LoadBlockByTimestamp(proto_ts, block);
  }

  static google::protobuf::Timestamp FromTimePoint(const Time& time) {
    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(time.time_since_epoch()).count();
    return TimeUtil::MicrosecondsToTimestamp(microseconds);
  }

  static Time ToTimePoint(const google::protobuf::Timestamp& time) {
    return Time() + std::chrono::microseconds(TimeUtil::TimestampToMicroseconds(time));
  }

  static constexpr std::string_view kBlockExt = ".blk";
  static constexpr std::string_view kMetaExt = ".meta";

  Options options_;
  fs::path full_path_;

  std::set<google::protobuf::Timestamp> block_set_;
  size_t size_counter_;
  size_t record_counter_;
};

std::unique_ptr<IEntry> IEntry::Build(IEntry::Options options) { return std::make_unique<Entry>(std::move(options)); }

/**
 * Streams
 */

std::ostream& operator<<(std::ostream& os, const IEntry::ReadResult& result) {
  os << fmt::format("<IEntry::ReadResult data={}  error={} time={}>", result.blob, result.error.ToString(),
                    IEntry::Time::clock::to_time_t(result.time));
  return os;
}

std::ostream& operator<<(std::ostream& os, const IEntry::Info& info) {
  os << fmt::format(
      "<IEntry::Info block_count={}  record_count={} bytes={} oldest_record_time={} latest_record_time={}>",
      info.block_count, info.record_count, info.bytes, to_time_t(info.oldest_record_time),
      to_time_t(info.latest_record_time));
  return os;
}

std::ostream& operator<<(std::ostream& os, const IEntry::RecordInfo& info) {
  os << fmt::format("<IEntry::RecordInfo time={}, size={}>", to_time_t(info.time), info.size);
  return os;
}
};  // namespace reduct::storage
