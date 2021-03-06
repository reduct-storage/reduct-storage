# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added:

- Web Console v0.3.0, [PR-133](https://github.com/reduct-storage/reduct-storage/pull/133)

### Changed:

- Use `Keep a log` format for CHANGELOG, [PR-136](https://github.com/reduct-storage/reduct-storage/pull/136)
- SI for max block and read chunk sizes, [PR-137](https://github.com/reduct-storage/reduct-storage/pull/137)
- SHA256 hash for API token is optional, [PR-139](https://github.com/reduct-storage/reduct-storage/pull/139)

### Fixed:

- Typo in API documentation, [PR-124](https://github.com/reduct-storage/reduct-storage/pull/124)
- Style in documentation, [PR-129](https://github.com/reduct-storage/reduct-storage/pull/129)

## [0.6.1] - 2022-06-25

### Added:

- Use Web Console v0.2.1, [PR-120](https://github.com/reduct-storage/reduct-storage/pull/120)

## [0.6.0] - 2022-06-25

### Added:

- `Content-Type` header to responses, [PR-107](https://github.com/reduct-storage/reduct-storage/pull/107)
- `max_block_records` to bucket settings, [PR-108](https://github.com/reduct-storage/reduct-storage/pull/108)
- HEAD `/alive` method for health check, [PR-114](https://github.com/reduct-storage/reduct-storage/pull/114)

### Changed:

- Filter unfinished records in GET /b/:bucket/:entry/list
  endpoint, [PR-106](https://github.com/reduct-storage/reduct-storage/pull/106)

### Fixed:

- Web Console for RS_API_BASE_PATH, [PR-92](https://github.com/reduct-storage/reduct-storage/pull/92)
- Wasting disk space in XFS filesystem, [PR-100](https://github.com/reduct-storage/reduct-storage/pull/100)
- Base path in server url, [PR-105](https://github.com/reduct-storage/reduct-storage/pull/105)
- Updating record state in asynchronous write
  operation, [PR-109](https://github.com/reduct-storage/reduct-storage/pull/109)
- SEGFAULT when entry removed but async writer is
  alive, [PR-110](https://github.com/reduct-storage/reduct-storage/pull/110)
- Removing a block with active readers or
  writers, [PR-111](https://github.com/reduct-storage/reduct-storage/pull/111)
- Loading bucket settings from disk, [PR-112](https://github.com/reduct-storage/reduct-storage/pull/112)
- 404 error for react routes, [PR-116](https://github.com/reduct-storage/reduct-storage/pull/116)
- No token error message, [PR-118](https://github.com/reduct-storage/reduct-storage/pull/118)
- Updating bucket settings, [PR-119](https://github.com/reduct-storage/reduct-storage/pull/119)
- Benchmarks and refactor block management [PR-99](https://github.com/reduct-storage/reduct-storage/pull/99)
- CURL to deploy image [PR-104](https://github.com/reduct-storage/reduct-storage/pull/104)

### Changed:

- Optimise write operation, [PR-96](https://github.com/reduct-storage/reduct-storage/pull/96)
- Disable SSL verification in API tests, [PR-113](https://github.com/reduct-storage/reduct-storage/pull/113)

## [0.5.1] - 2022-05-24

### Fixed:

- GET `/b/:bucket/:entry` to avoid creating an empty
  entry, [PR-95](https://github.com/reduct-storage/reduct-storage/pull/95)
- Update of bucket settings, [PR-138](https://github.com/reduct-storage/reduct-storage/pull/138)

## [0.5.0] - 2022-05-15

### Added:

- Web Console, [PR-77](https://github.com/reduct-storage/reduct-storage/pull/77)
- Add default settings for a new bucket in GET /info, [PR-87](https://github.com/reduct-storage/reduct-storage/pull/87)
- Link to JS SDK to documentation, [PR-88](https://github.com/reduct-storage/reduct-storage/pull/88)

### Changed:

- Only HTTP errors 50x in the logs, [PR-84](https://github.com/reduct-storage/reduct-storage/issues/84)

### Fixed:

- CORS functionality, [PR-72](https://github.com/reduct-storage/reduct-storage/pull/72)
- Quota policy, [PR-83](https://github.com/reduct-storage/reduct-storage/pull/83)

## [0.4.3] - 2022-05-01

### Fixed:

- Sending big blobs [PR-80](https://github.com/reduct-storage/reduct-storage/pull/80)
- Handling offset in tryEnd [PR-81](https://github.com/reduct-storage/reduct-storage/pull/81)

## [0.4.2] - 2022-04-30

### Fixed:

- Deadlock during sending data, [PR-78](https://github.com/reduct-storage/reduct-storage/pull/78)

## [0.4.1] - 2022-04-04

### Fixed:

- Timestamp for oldest record, [PR-68](https://github.com/reduct-storage/reduct-storage/pull/68)

## [0.4.0] - 2022-04-01

### Added:

- Asynchronous write/read operations with data blocks, [PR-62](https://github.com/reduct-storage/reduct-storage/pull/62)

### Fixed:

- Searching start block in Entry List request, [PR-61](https://github.com/reduct-storage/reduct-storage/pull/61)
- Qborting GET requests, [PR-64](https://github.com/reduct-storage/reduct-storage/pull/64)

### Changed:

- Block structure in entry, [PR-58](https://github.com/reduct-storage/reduct-storage/pull/58)

## [0.3.0]  - 2022-03-14

### Added

- Secure HTTP, [PR-49](https://github.com/reduct-storage/reduct-storage/pull/49)
- Stats and list entries to GET /b/:bucket method with
  , [PR-51](https://github.com/reduct-storage/reduct-storage/pull/51)
- Access to the latest record, [PR-53](https://github.com/reduct-storage/reduct-storage/pull/53)

### Fixed:

- Sending two responses for HTTP error, [PR-48](https://github.com/reduct-storage/reduct-storage/pull/48)

### Changed:

- Replace nholmann/json with Protobuf, [PR-47](https://github.com/reduct-storage/reduct-storage/pull/47)

## [0.2.1] - 2022-03-07

### Fixed:

* Crushing when API token is wrong, [PR-42](https://github.com/reduct-storage/reduct-storage/pull/42)
* Order of authentication checks, [PR-43](https://github.com/reduct-storage/reduct-storage/pull/43)

## [0.2.0] - 2022-02-26

### Added:

- HEAD method to Bucket API, [PR-30](https://github.com/reduct-storage/reduct-storage/pull/30)
- Extends information from GET method of Server API, [PR-33](https://github.com/reduct-storage/reduct-storage/pull/33)
- GET /list end point to browse buckets, [PR-34](https://github.com/reduct-storage/reduct-storage/pull/34)
- Bearer token authentication, [PR-36](https://github.com/reduct-storage/reduct-storage/pull/36)

### Changed:

- PUT method of Bucket API has optional parameters, [PR-32](https://github.com/reduct-storage/reduct-storage/pull/32)

### Fixed:

- Docker build on ARM32, [PR-29](https://github.com/reduct-storage/reduct-storage/pull/29)
- IBucket::List error 500 for timestamps between
  blocks, [PR-31](https://github.com/reduct-storage/reduct-storage/pull/31)
- Wrong parameters in Entry API documentation, [PR-38](https://github.com/reduct-storage/reduct-storage/pull/38)

## [0.1.1] - 2022-02-13

### Fixed:

- Default folder for data in Docker image, [PR-23](https://github.com/reduct-storage/reduct-storage/pull/23)

## [0.1.0] - 2022-01-24

- Initial release with basic HTTP API and FIFO bucket quota

[Unreleased]: https://github.com/reduct-storage/reduct-storage/compare/v0.6.1...HEAD
[0.6.1]: https://github.com/reduct-storage/reduct-storage/compare/v0.6.0...v0.6.1
[0.6.0]: https://github.com/reduct-storage/reduct-storage/compare/v0.5.1...v0.6.0
[0.5.1]: https://github.com/reduct-storage/reduct-storage/compare/v0.5.0...v0.5.1
[0.5.0]: https://github.com/reduct-storage/reduct-storage/compare/v0.4.3...v0.5.0
[0.4.3]: https://github.com/reduct-storage/reduct-storage/compare/v0.4.2...v0.4.3
[0.4.2]: https://github.com/reduct-storage/reduct-storage/compare/v0.4.1...v0.4.2
[0.4.1]: https://github.com/reduct-storage/reduct-storage/compare/v0.4.0...v0.4.1
[0.4.0]: https://github.com/reduct-storage/reduct-storage/compare/v0.3.0...v0.4.0
[0.3.0]: https://github.com/reduct-storage/reduct-storage/compare/v0.2.1...v0.3.0
[0.2.1]: https://github.com/reduct-storage/reduct-storage/compare/v0.2.0...v0.2.1
[0.2.0]: https://github.com/reduct-storage/reduct-storage/compare/v0.1.1...v0.2.0
[0.1.1]: https://github.com/reduct-storage/reduct-storage/compare/v0.1.0...v0.1.1
[0.1.0]: https://github.com/reduct-storage/reduct-storage/releases/tag/v0.1.0
