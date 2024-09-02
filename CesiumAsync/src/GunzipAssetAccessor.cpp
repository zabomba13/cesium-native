#include "CesiumAsync/GunzipAssetAccessor.h"

#include "CesiumAsync/AsyncSystem.h"
#include "CesiumAsync/HttpHeaders.h"
#include "CesiumAsync/IAssetAccessor.h"
#include "CesiumAsync/IAssetRequest.h"
#include "CesiumAsync/IAssetResponse.h"
#include "CesiumUtility/Gunzip.h"

#include <gsl/span>

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace CesiumAsync {

namespace {

class GunzippedAssetResponse : public IAssetResponse {
public:
  explicit GunzippedAssetResponse(const IAssetResponse* pOther) noexcept
      : _pAssetResponse{pOther} {
    this->_dataValid = CesiumUtility::gunzip(
        this->_pAssetResponse->data(),
        this->_gunzippedData);
  }

  uint16_t statusCode() const noexcept override {
    return this->_pAssetResponse->statusCode();
  }

  std::string contentType() const override {
    return this->_pAssetResponse->contentType();
  }

  const HttpHeaders& headers() const noexcept override {
    return this->_pAssetResponse->headers();
  }

  gsl::span<const std::byte> data() const noexcept override {
    return this->_dataValid ? this->_gunzippedData
                            : this->_pAssetResponse->data();
  }

private:
  const IAssetResponse* _pAssetResponse;
  std::vector<std::byte> _gunzippedData;
  bool _dataValid;
};

class GunzippedAssetRequest : public IAssetRequest {
public:
  explicit GunzippedAssetRequest(std::shared_ptr<IAssetRequest>&& pOther)
      : _pAssetRequest(std::move(pOther)),
        AssetResponse(_pAssetRequest->response()){};
  const std::string& method() const noexcept override {
    return this->_pAssetRequest->method();
  }

  const std::string& url() const noexcept override {
    return this->_pAssetRequest->url();
  }

  const HttpHeaders& headers() const noexcept override {
    return this->_pAssetRequest->headers();
  }

  const IAssetResponse* response() const noexcept override {
    return &this->AssetResponse;
  }

private:
  std::shared_ptr<IAssetRequest> _pAssetRequest;
  GunzippedAssetResponse AssetResponse;
};

Future<std::shared_ptr<IAssetRequest>> gunzipIfNeeded(
    const AsyncSystem& asyncSystem,
    std::shared_ptr<IAssetRequest>&& pCompletedRequest) {
  const IAssetResponse* pResponse = pCompletedRequest->response();
  if ((pResponse != nullptr) && CesiumUtility::isGzip(pResponse->data())) {
    return asyncSystem.runInWorkerThread(
        [pCompletedRequest = std::move(
             pCompletedRequest)]() mutable -> std::shared_ptr<IAssetRequest> {
          return std::make_shared<GunzippedAssetRequest>(
              std::move(pCompletedRequest));
        });
  }
  return asyncSystem.createResolvedFuture(std::move(pCompletedRequest));
}

} // namespace

GunzipAssetAccessor::GunzipAssetAccessor(
    const std::shared_ptr<IAssetAccessor>& pAssetAccessor)
    : _pAssetAccessor(pAssetAccessor) {}

GunzipAssetAccessor::~GunzipAssetAccessor() noexcept = default;

Future<std::shared_ptr<IAssetRequest>> GunzipAssetAccessor::get(
    const AsyncSystem& asyncSystem,
    const std::string& url,
    const std::vector<THeader>& headers) {
  return this->_pAssetAccessor->get(asyncSystem, url, headers)
      .thenImmediately(
          [asyncSystem](std::shared_ptr<IAssetRequest>&& pCompletedRequest) {
            return gunzipIfNeeded(asyncSystem, std::move(pCompletedRequest));
          });
}

Future<std::shared_ptr<IAssetRequest>> GunzipAssetAccessor::request(
    const AsyncSystem& asyncSystem,
    const std::string& verb,
    const std::string& url,
    const std::vector<THeader>& headers,
    const gsl::span<const std::byte>& contentPayload) {
  return this->_pAssetAccessor
      ->request(asyncSystem, verb, url, headers, contentPayload)
      .thenImmediately(
          [asyncSystem](std::shared_ptr<IAssetRequest>&& pCompletedRequest) {
            return gunzipIfNeeded(asyncSystem, std::move(pCompletedRequest));
          });
}

void GunzipAssetAccessor::tick() noexcept { _pAssetAccessor->tick(); }

} // namespace CesiumAsync
