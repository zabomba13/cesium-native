#pragma once

#include "ICacheDatabase.h"

#include <spdlog/fwd.h>

#include <cstddef>
#include <memory>
#include <optional>
#include <string>

namespace CesiumAsync {

/**
 * @brief Cache storage using SQLITE to store completed response.
 */
// template <typename TClientDataCacheManager>
class CESIUMASYNC_API SqliteCache : public ICacheDatabase {
public:
  /**
   * @brief Constructs a new instance with a given `databaseName` pointing to a
   * database.
   *
   * The instance will connect to the existing database or create a new one if
   * it doesn't exist
   *
   * @param pLogger The logger that receives error messages.
   * @param databaseName the database path.
   * @param maxItems the maximum number of items should be kept in the database
   * after prunning.
   */
  SqliteCache(
      const std::shared_ptr<spdlog::logger>& pLogger,
      const std::string& databaseName,
      uint64_t maxItems = 4096);
  ~SqliteCache();

  /** @copydoc ICacheDatabase::getEntry*/
  virtual std::optional<CacheItem>
  getEntryAnyThread(const std::string& key) const override;

  /** @copydoc ICacheDatabase::updateLastAccessTimeWriterThread*/
  virtual void updateLastAccessTimeWriterThread(int64_t rowId) override;

  /** @copydoc ICacheDatabase::storeEntry*/
  virtual bool storeEntryWriterThread(
      const std::string& key,
      std::time_t expiryTime,
      const std::string& url,
      const std::string& requestMethod,
      const HttpHeaders& requestHeaders,
      uint16_t statusCode,
      const HttpHeaders& responseHeaders,
      const gsl::span<const std::byte>& responseData,
      const gsl::span<const std::byte>& clientData) override;

  /** @copydoc ICacheDatabase::prune*/
  virtual bool pruneWriterThread() override;

  /** @copydoc ICacheDatabase::clearAll*/
  virtual bool clearAllWriterThread() override;

private:
  struct Impl;
  std::unique_ptr<Impl> _pImpl;
  void createConnection() const;
  void destroyDatabase();
};
} // namespace CesiumAsync
