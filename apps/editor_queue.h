#ifndef MINI_CAMERA_RAW_EDITOR_QUEUE_H
#define MINI_CAMERA_RAW_EDITOR_QUEUE_H
#include <cstddef>
#include <cstdint>
#include <deque>
#include <optional>
namespace mini_camera_raw::app {
struct EditRequest { std::uint64_t session, sequence; double ev; };
// GUI-thread-only state machine. Pixel processing remains outside this class.
class EditorQueue {
 public:
  enum class Phase { ready, running, awaiting_paint, failed };
  EditorQueue() = default;
  void new_session(); // only with no unfinished requests
  bool accept(double ev); // replaces unstarted previews; false before open/after failure
  std::optional<EditRequest> begin();
  void rendered(const EditRequest& request);
  void painted(const EditRequest& request);
  void fail(const EditRequest& request);
  void retry();
  void discard_failed();
  [[nodiscard]] Phase phase() const { return phase_; }
  [[nodiscard]] std::size_t size() const { return pending_.size(); }
  [[nodiscard]] bool can_accept() const;
  [[nodiscard]] EditRequest displayed() const { return displayed_; }
  [[nodiscard]] double requested_ev() const;
 private:
  void require(const EditRequest& request,Phase phase) const;

  std::uint64_t session_=0, next_=1;
  EditRequest displayed_{0,0,0};
  std::deque<EditRequest> pending_;
  Phase phase_=Phase::ready;
};
}
#endif
