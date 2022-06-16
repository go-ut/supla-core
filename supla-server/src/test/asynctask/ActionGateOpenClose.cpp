/*
 Copyright (C) AC SOFTWARE SP. Z O.O.

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <asynctask/ActionGateOpenCloseTest.h>

#include "actions/action_gate_openclose.h"
#include "device/channel_gate_value.h"
#include "doubles/channeljsonconfig/ChannelJSONConfigGetterStub.h"
#include "doubles/device/GateValueGetterStub.h"
#include "log.h"

namespace testing {

ActionGateOpenCloseIntegrationTest::ActionGateOpenCloseIntegrationTest()
    : AsyncTaskTest() {}

ActionGateOpenCloseIntegrationTest::~ActionGateOpenCloseIntegrationTest() {}

void ActionGateOpenCloseIntegrationTest::WaitForOpenClose(
    ActionExecutorMock *action_executor, int expected_count,
    unsigned int usec) {
  unsigned int steps = usec / 100000;

  for (unsigned int a = 0; a < steps; a++) {
    if (action_executor->getOpenCloseWctCounter() == expected_count) {
      return;
    }
    usleep(100000);
  }

  ASSERT_EQ(action_executor->getOpenCloseWctCounter(), expected_count);
}

void ActionGateOpenCloseIntegrationTest::noActionRequired(bool open) {
  pool->set_thread_count_limit(10);

  ASSERT_EQ(pool->thread_count(), (unsigned int)0);

  GateValueGetterStub *value_getter = new GateValueGetterStub();
  ASSERT_TRUE(value_getter != NULL);

  value_getter->setResult(open ? gsl_open : gsl_closed, gsl_unknown);

  ActionExecutorMock *action_executor = new ActionExecutorMock();
  EXPECT_TRUE(action_executor != NULL);

  supla_action_gate_openclose *task = new supla_action_gate_openclose(
      supla_caller(ctUnknown), queue, pool, 0, false, action_executor,
      value_getter, NULL, 1, 2, 3, 5000000, open);

  ASSERT_TRUE(task != NULL);

  WaitForExec(pool, 1, 1000000);
  usleep(100000);
  EXPECT_EQ(pool->exec_count(), (unsigned int)1);
  EXPECT_EQ(queue->total_count(), (unsigned int)1);
  EXPECT_EQ(action_executor->counterSetCount(), 0);
  EXPECT_EQ(task->get_state(), supla_asynctask_state::SUCCESS);
  delete task;
  EXPECT_EQ(queue->total_count(), (unsigned int)0);
}

void ActionGateOpenCloseIntegrationTest::openClose(bool open, int attemptCount,
                                                   bool success,
                                                   int maxAttemptCount) {
  pool->set_thread_count_limit(1);

  ASSERT_EQ(pool->thread_count(), (unsigned int)0);

  GateValueGetterStub *value_getter = new GateValueGetterStub();
  ASSERT_TRUE(value_getter != NULL);

  value_getter->setResult(open ? gsl_closed : gsl_open, gsl_unknown);

  ChannelJSONConfigGetterStub *config_getter =
      new ChannelJSONConfigGetterStub();
  EXPECT_TRUE(config_getter != NULL);

  if (config_getter) {
    channel_json_config *config = new channel_json_config();
    EXPECT_TRUE(config != NULL);

    if (config) {
      char user_config[100];
      snprintf(user_config, sizeof(user_config), "{\"%s\":%i}",
               open ? "numberOfAttemptsToOpen" : "numberOfAttemptsToClose",
               maxAttemptCount);
      config->set_user_config(user_config);
      config_getter->set_config(config);
    }
  }

  ActionExecutorMock *action_executor = new ActionExecutorMock();
  EXPECT_TRUE(action_executor != NULL);

  supla_action_gate_openclose *task = new supla_action_gate_openclose(
      supla_caller(ctUnknown), queue, pool, 0, false, action_executor,
      value_getter, config_getter, 1, 2, 3, 2000000, open);

  ASSERT_TRUE(task != NULL);
  WaitForExec(pool, 1, 500000);

  for (short a = 0; a < attemptCount; a++) {
    if (a > 0) {
      WaitForExec(pool, 1 + a, 3000000);
    }

    WaitForOpenClose(action_executor, 1 + a, 1000000);
    EXPECT_EQ(pool->exec_count(), (unsigned int)1 + a);
    EXPECT_EQ(queue->total_count(), (unsigned int)1);
    EXPECT_EQ(action_executor->counterSetCount(), 1);
    EXPECT_EQ(task->get_state(), supla_asynctask_state::WAITING);
  }

  if (success) {
    value_getter->setResult(open ? gsl_open : gsl_closed, gsl_unknown);
    WaitForState(task, supla_asynctask_state::SUCCESS, 3000000);
  } else {
    WaitForState(task, supla_asynctask_state::FAILURE, 3000000);
  }

  EXPECT_EQ(pool->exec_count(), (unsigned int)attemptCount + 1);
  EXPECT_EQ(queue->total_count(), (unsigned int)1);
}

void ActionGateOpenCloseIntegrationTest::openClose(bool open, int attemptCount,
                                                   bool success) {
  openClose(open, attemptCount, success, 5);
}

TEST_F(ActionGateOpenCloseIntegrationTest, openWithDisconnectedSensor) {
  pool->set_thread_count_limit(10);

  ASSERT_EQ(pool->thread_count(), (unsigned int)0);

  GateValueGetterStub *value_getter = new GateValueGetterStub();
  ASSERT_TRUE(value_getter != NULL);

  ActionExecutorMock *action_executor = new ActionExecutorMock();
  EXPECT_TRUE(action_executor != NULL);

  supla_action_gate_openclose *task = new supla_action_gate_openclose(
      supla_caller(ctUnknown), queue, pool, 0, false, action_executor,
      value_getter, NULL, 1, 2, 3, 5000000, true);

  ASSERT_TRUE(task != NULL);

  WaitForExec(pool, 1, 1000000);
  usleep(100000);
  EXPECT_EQ(pool->exec_count(), (unsigned int)1);
  EXPECT_EQ(queue->total_count(), (unsigned int)1);
  EXPECT_EQ(action_executor->counterSetCount(), 0);
  EXPECT_EQ(task->get_state(), supla_asynctask_state::FAILURE);
  delete task;
  EXPECT_EQ(queue->total_count(), (unsigned int)0);
}

TEST_F(ActionGateOpenCloseIntegrationTest, openAlreadyOpened) {
  noActionRequired(true);
}

TEST_F(ActionGateOpenCloseIntegrationTest, open_oneVerification) {
  openClose(true, 1, true);
}

TEST_F(ActionGateOpenCloseIntegrationTest, closeAlreadyClosed) {
  noActionRequired(false);
}

TEST_F(ActionGateOpenCloseIntegrationTest, close_oneVerification) {
  openClose(false, 1, true);
}

TEST_F(ActionGateOpenCloseIntegrationTest, close_twoVerifications) {
  openClose(false, 2, true);
}

TEST_F(ActionGateOpenCloseIntegrationTest, close_threVerifications) {
  openClose(false, 3, true);
}

TEST_F(ActionGateOpenCloseIntegrationTest, close_fourVerifications) {
  openClose(false, 4, true);
}

TEST_F(ActionGateOpenCloseIntegrationTest, close_fiveVerifications) {
  openClose(false, 5, true);
}

TEST_F(ActionGateOpenCloseIntegrationTest, fiveAttemptsToCloseWithoutSuccess) {
  openClose(false, 5, false);
}

TEST_F(ActionGateOpenCloseIntegrationTest, limitAttemptsToOne) {
  openClose(false, 1, false, 1);
}

TEST_F(ActionGateOpenCloseIntegrationTest, limitAttemptsToOneWithSuccess) {
  openClose(false, 1, true, 1);
}

TEST_F(ActionGateOpenCloseIntegrationTest, limitAttemptsToThree) {
  openClose(false, 3, false, 3);
}

}  // namespace testing
