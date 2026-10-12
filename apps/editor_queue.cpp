#include "editor_queue.h"
#include <cmath>
#include <limits>
#include <stdexcept>
namespace mini_camera_raw::app {
void EditorQueue::new_session() {
  if(!pending_.empty()) throw std::logic_error("session has unfinished work");
  if(session_==std::numeric_limits<std::uint64_t>::max()) throw std::overflow_error("session identity exhausted");
  ++session_; next_=1; displayed_={session_,0,0}; phase_=Phase::ready;
}
bool EditorQueue::can_accept() const {
  return session_ && phase_!=Phase::failed;
}
bool EditorQueue::accept(double ev) {
  if(!std::isfinite(ev) || ev < -5 || ev > 5) throw std::invalid_argument("invalid exposure");
  if(!can_accept()) return false;
  if(next_==std::numeric_limits<std::uint64_t>::max()) throw std::overflow_error("request identity exhausted");
  const EditRequest request{session_,next_,ev};
  // The active front is immutable through computation and paint acknowledgment.
  // Only the unstarted tail is superseded by the newest complete parameters.
  if(phase_==Phase::ready && !pending_.empty()) pending_.back()=request;
  else if(pending_.size()==2) pending_.back()=request;
  else pending_.push_back(request);
  ++next_; return true;
}
std::optional<EditRequest> EditorQueue::begin() {
  if(phase_!=Phase::ready || pending_.empty()) return std::nullopt;
  phase_=Phase::running; return pending_.front();
}
void EditorQueue::require(const EditRequest& r,Phase phase) const {
  if(phase_!=phase || pending_.empty() || pending_.front().session!=r.session ||
      pending_.front().sequence!=r.sequence || pending_.front().ev!=r.ev)
    throw std::logic_error("unexpected editor completion identity/state");
}
void EditorQueue::rendered(const EditRequest& r) { require(r,Phase::running); phase_=Phase::awaiting_paint; }
void EditorQueue::painted(const EditRequest& r) {
  require(r,Phase::awaiting_paint); displayed_=r; pending_.pop_front(); phase_=Phase::ready;
}
void EditorQueue::fail(const EditRequest& r) { require(r,Phase::running); phase_=Phase::failed; }
void EditorQueue::retry() {
  if(phase_!=Phase::failed) throw std::logic_error("no failed request");
  phase_=Phase::ready;
}
void EditorQueue::discard_failed() {
  if(phase_!=Phase::failed) throw std::logic_error("no failed request");
  pending_.clear(); phase_=Phase::ready;
}
double EditorQueue::requested_ev() const { return pending_.empty()?displayed_.ev:pending_.back().ev; }
}
