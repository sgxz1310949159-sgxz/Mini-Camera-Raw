#include "editor_session.h"
#include <utility>
namespace mini_camera_raw::app {
EditorSession::EditorSession(DecodedRaw raw)
    : raw_(std::move(raw)), photo_(prepare_editor_photo(raw_)) {}
ImageBuffer EditorSession::render(double ev) const { return render_editor_photo(photo_,ev); }
void EditorSession::rebuild_cache() { photo_=prepare_editor_photo(raw_); }
std::size_t EditorSession::retained_bytes() const noexcept {
  return raw_.image.byte_size()+photo_.camera.byte_size();
}
}
