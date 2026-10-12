#include "editor_queue.h"
#include <gtest/gtest.h>
#include <limits>
#include <stdexcept>
namespace mini_camera_raw::app {
TEST(EditorQueueTest, LatestPendingReplacesIntermediateValuesButKeepsActiveFrame) {
  EditorQueue queue; queue.new_session();
  ASSERT_TRUE(queue.accept(2)); auto first=*queue.begin();
  for(int i=0;i<100;++i) ASSERT_TRUE(queue.accept(i/100.));
  ASSERT_TRUE(queue.accept(0));
  EXPECT_EQ(queue.size(),2U); EXPECT_TRUE(queue.can_accept());
  EXPECT_FALSE(queue.begin());
  EXPECT_THROW(queue.new_session(),std::logic_error);
  queue.rendered(first); EXPECT_FALSE(queue.begin());
  queue.accept(-1); // latest changes even while awaiting paint
  queue.painted(first); EXPECT_EQ(queue.displayed().ev,2);
  auto latest=*queue.begin(); EXPECT_EQ(latest.ev,-1);
  EXPECT_EQ(latest.sequence,103U);
  queue.rendered(latest); queue.painted(latest);
  EXPECT_EQ(queue.size(),0U); EXPECT_EQ(queue.displayed().ev,-1);
}
TEST(EditorQueueTest, BeforeStartOnlyNewestSnapshotRemains) {
  EditorQueue queue; queue.new_session(); queue.accept(1); queue.accept(2);
  EXPECT_EQ(queue.size(),1U); EXPECT_EQ(queue.begin()->ev,2);
}
TEST(EditorQueueTest, FailureRetainsFrameAndRetryRetainsIdentity) {
  EditorQueue queue; queue.new_session(); queue.accept(1);
  auto r=*queue.begin(); queue.accept(2); queue.fail(r);
  EXPECT_EQ(queue.displayed().ev,0); EXPECT_FALSE(queue.begin()); EXPECT_FALSE(queue.accept(3));
  queue.retry(); auto retry=*queue.begin(); EXPECT_EQ(retry.sequence,r.sequence);
  queue.rendered(retry); queue.painted(retry);
  auto second=*queue.begin(); queue.fail(second); queue.discard_failed();
  EXPECT_EQ(queue.size(),0U); EXPECT_EQ(queue.requested_ev(),1); EXPECT_EQ(queue.displayed().ev,1);
  EXPECT_TRUE(queue.accept(4)); EXPECT_GT(queue.begin()->sequence,second.sequence);
}
TEST(EditorQueueTest, RejectsStaleDuplicateAndMismatchedCompletions) {
  EditorQueue queue; queue.new_session(); queue.accept(1); auto r=*queue.begin();
  auto wrong=r; ++wrong.session; EXPECT_THROW(queue.rendered(wrong),std::logic_error);
  wrong=r; wrong.ev=2; EXPECT_THROW(queue.rendered(wrong),std::logic_error);
  EXPECT_THROW(queue.painted(r),std::logic_error);
  queue.rendered(r); queue.painted(r); EXPECT_THROW(queue.painted(r),std::logic_error);
  queue.new_session(); queue.accept(1); auto fresh=*queue.begin();
  EXPECT_NE(fresh.session,r.session); EXPECT_THROW(queue.rendered(r),std::logic_error);
  queue.rendered(fresh); queue.painted(fresh);
}
TEST(EditorQueueTest, InvalidInputCannotEnterQueue) {
  EditorQueue queue; EXPECT_FALSE(queue.accept(0)); queue.new_session();
  EXPECT_THROW(queue.accept(5.01),std::invalid_argument);
  EXPECT_THROW(queue.accept(std::numeric_limits<double>::quiet_NaN()),std::invalid_argument);
  EXPECT_THROW(queue.accept(-5.01),std::invalid_argument);
  EXPECT_EQ(queue.size(),0U);
  EXPECT_TRUE(queue.accept(-5)); EXPECT_TRUE(queue.accept(5));
  EXPECT_THROW(queue.retry(),std::logic_error);
}
}
