#ifndef MINI_CAMERA_RAW_EDITOR_SESSION_H
#define MINI_CAMERA_RAW_EDITOR_SESSION_H
#include "editor_pipeline.h"
namespace mini_camera_raw::app {
// Application-only owner. No mutable source/cache aliases escape this object.
class EditorSession {
 public:
  explicit EditorSession(DecodedRaw raw);
  [[nodiscard]] ImageBuffer render(double ev) const;
  // Worker-only maintenance; never concurrent with render. Strong exception guarantee.
  void rebuild_cache();
  [[nodiscard]] std::size_t retained_bytes() const noexcept;
 private:
  DecodedRaw raw_;
  EditorPhoto photo_;
};
}
#endif
