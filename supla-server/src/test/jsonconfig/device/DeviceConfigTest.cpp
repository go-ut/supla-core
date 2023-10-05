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

#include "DeviceConfigTest.h"

#include "jsonconfig/device/device_json_config.h"
#include "proto.h"

namespace testing {

DeviceConfigTest::DeviceConfigTest(void) {}

DeviceConfigTest::~DeviceConfigTest(void) {}

TEST_F(DeviceConfigTest, buttonVolume) {
  TSDS_SetDeviceConfig sds_cfg = {};
  sds_cfg.Fields = SUPLA_DEVICE_CONFIG_FIELD_BUTTON_VOLUME;
  sds_cfg.ConfigSize = sizeof(TDeviceConfig_ButtonVolume);
  ((TDeviceConfig_ButtonVolume *)sds_cfg.Config)->Volume = 25;

  device_json_config cfg;
  cfg.set_config(&sds_cfg);
  char *str = cfg.get_user_config();
  ASSERT_TRUE(str != nullptr);
  EXPECT_STREQ(str, "{\"buttonVolume\":25}");
  free(str);
}

TEST_F(DeviceConfigTest, screenBrightness) {
  TSDS_SetDeviceConfig sds_cfg = {};
  sds_cfg.Fields = SUPLA_DEVICE_CONFIG_FIELD_SCREEN_BRIGHTNESS;
  sds_cfg.ConfigSize = sizeof(TDeviceConfig_ScreenBrightness);
  ((TDeviceConfig_ScreenBrightness *)sds_cfg.Config)->Automatic = 1;

  device_json_config cfg;
  cfg.set_config(&sds_cfg);
  char *str = cfg.get_user_config();
  ASSERT_TRUE(str != nullptr);
  EXPECT_STREQ(str, "{\"screenBrightness\":\"auto\"}");
  free(str);

  ((TDeviceConfig_ScreenBrightness *)sds_cfg.Config)->Automatic = 0;
  ((TDeviceConfig_ScreenBrightness *)sds_cfg.Config)->ScreenBrightness = 98;
  cfg.set_config(&sds_cfg);

  str = cfg.get_user_config();
  ASSERT_TRUE(str != nullptr);
  EXPECT_STREQ(str, "{\"screenBrightness\":98}");
  free(str);
}

TEST_F(DeviceConfigTest, allFields) {
  TSDS_SetDeviceConfig sds_cfg = {};
  sds_cfg.Fields = SUPLA_DEVICE_CONFIG_FIELD_STATUS_LED |
                   SUPLA_DEVICE_CONFIG_FIELD_SCREEN_BRIGHTNESS |
                   SUPLA_DEVICE_CONFIG_FIELD_BUTTON_VOLUME |
                   SUPLA_DEVICE_CONFIG_FIELD_DISABLE_USER_INTERFACE |
                   SUPLA_DEVICE_CONFIG_FIELD_AUTOMATIC_TIME_SYNC |
                   SUPLA_DEVICE_CONFIG_FIELD_HOME_SCREEN_DELAY |
                   SUPLA_DEVICE_CONFIG_FIELD_HOME_SCREEN_CONTENT;

  ((TDeviceConfig_StatusLed *)&sds_cfg.Config[sds_cfg.ConfigSize])
      ->StatusLedType = SUPLA_DEVCFG_STATUS_LED_ALWAYS_OFF;
  sds_cfg.ConfigSize += sizeof(TDeviceConfig_StatusLed);
  ASSERT_LE(sds_cfg.ConfigSize, SUPLA_DEVICE_CONFIG_MAXSIZE);

  ((TDeviceConfig_ScreenBrightness *)&sds_cfg.Config[sds_cfg.ConfigSize])
      ->ScreenBrightness = 24;
  sds_cfg.ConfigSize += sizeof(TDeviceConfig_ScreenBrightness);
  ASSERT_LE(sds_cfg.ConfigSize, SUPLA_DEVICE_CONFIG_MAXSIZE);

  ((TDeviceConfig_ButtonVolume *)&sds_cfg.Config[sds_cfg.ConfigSize])->Volume =
      100;
  sds_cfg.ConfigSize += sizeof(TDeviceConfig_ButtonVolume);
  ASSERT_LE(sds_cfg.ConfigSize, SUPLA_DEVICE_CONFIG_MAXSIZE);

  sds_cfg.ConfigSize += sizeof(TDeviceConfig_DisableUserInterface);
  ASSERT_LE(sds_cfg.ConfigSize, SUPLA_DEVICE_CONFIG_MAXSIZE);

  ((TDeviceConfig_AutomaticTimeSync *)&sds_cfg.Config[sds_cfg.ConfigSize])
      ->AutomaticTimeSync = 1;
  sds_cfg.ConfigSize += sizeof(TDeviceConfig_AutomaticTimeSync);
  ASSERT_LE(sds_cfg.ConfigSize, SUPLA_DEVICE_CONFIG_MAXSIZE);

  ((TDeviceConfig_HomeScreenDelay *)&sds_cfg.Config[sds_cfg.ConfigSize])
      ->HomeScreenDelayS = 123;
  sds_cfg.ConfigSize += sizeof(TDeviceConfig_HomeScreenDelay);
  ASSERT_LE(sds_cfg.ConfigSize, SUPLA_DEVICE_CONFIG_MAXSIZE);

  ((TDeviceConfig_HomeScreenContent *)&sds_cfg.Config[sds_cfg.ConfigSize])
      ->HomeScreenContent = SUPLA_DEVCFG_HOME_SCREEN_CONTENT_TIME_DATE;
  ((TDeviceConfig_HomeScreenContent *)&sds_cfg.Config[sds_cfg.ConfigSize])
      ->ContentAvailable = 0xFFFFFFFFFFFFFFFF;
  sds_cfg.ConfigSize += sizeof(TDeviceConfig_HomeScreenContent);
  ASSERT_LE(sds_cfg.ConfigSize, SUPLA_DEVICE_CONFIG_MAXSIZE);

  device_json_config cfg;
  cfg.set_config(&sds_cfg);
  char *str = cfg.get_user_config();
  ASSERT_TRUE(str != nullptr);
  EXPECT_STREQ(
      str,
      "{\"statusLed\":\"ALWAYS_OFF\",\"screenBrightness\":24,\"buttonVolume\":"
      "100,\"userInterfaceDisabled\":false,\"automaticTimeSync\":true,"
      "\"homeScreen\":{\"delay\":123,\"content\":\"TIME_DATE\"}}");
  free(str);

  str = cfg.get_properties();
  ASSERT_TRUE(str != nullptr);
  EXPECT_STREQ(str,
               "{\"homeScreenContentAvailable\":[\"NONE\",\"TEMPERATURE\","
               "\"HUMIDITY\",\"TIME\",\"TIME_DATE\",\"TEMPERATURE_TIME\","
               "\"MAIN_AND_AUX_TEMPERATURE\"]}");
  free(str);
}

TEST_F(DeviceConfigTest, twoFieldsSlightlyApart) {
  TSDS_SetDeviceConfig sds_cfg = {};
  sds_cfg.Fields = SUPLA_DEVICE_CONFIG_FIELD_DISABLE_USER_INTERFACE |
                   SUPLA_DEVICE_CONFIG_FIELD_AUTOMATIC_TIME_SYNC |
                   SUPLA_DEVICE_CONFIG_FIELD_HOME_SCREEN_DELAY;

  ((TDeviceConfig_DisableUserInterface *)&sds_cfg.Config[sds_cfg.ConfigSize])
      ->DisableUserInterface = true;
  sds_cfg.ConfigSize += sizeof(TDeviceConfig_DisableUserInterface);
  ASSERT_LE(sds_cfg.ConfigSize, SUPLA_DEVICE_CONFIG_MAXSIZE);

  ((TDeviceConfig_AutomaticTimeSync *)&sds_cfg.Config[sds_cfg.ConfigSize])
      ->AutomaticTimeSync = 1;
  sds_cfg.ConfigSize += sizeof(TDeviceConfig_AutomaticTimeSync);
  ASSERT_LE(sds_cfg.ConfigSize, SUPLA_DEVICE_CONFIG_MAXSIZE);

  ((TDeviceConfig_HomeScreenDelay *)&sds_cfg.Config[sds_cfg.ConfigSize])
      ->HomeScreenDelayS = 678;
  sds_cfg.ConfigSize += sizeof(TDeviceConfig_HomeScreenDelay);
  ASSERT_LE(sds_cfg.ConfigSize, SUPLA_DEVICE_CONFIG_MAXSIZE);

  device_json_config cfg;
  cfg.set_config(&sds_cfg);
  char *str = cfg.get_user_config();
  ASSERT_TRUE(str != nullptr);
  EXPECT_STREQ(str,
               "{\"userInterfaceDisabled\":true,\"automaticTimeSync\":true,"
               "\"homeScreen\":{\"delay\":678}}");
  free(str);
}

TEST_F(DeviceConfigTest, leaveOnly) {
  TSDS_SetDeviceConfig sds_cfg = {};
  sds_cfg.Fields = SUPLA_DEVICE_CONFIG_FIELD_STATUS_LED |
                   SUPLA_DEVICE_CONFIG_FIELD_SCREEN_BRIGHTNESS |
                   SUPLA_DEVICE_CONFIG_FIELD_BUTTON_VOLUME |
                   SUPLA_DEVICE_CONFIG_FIELD_HOME_SCREEN_DELAY;

  ((TDeviceConfig_StatusLed *)&sds_cfg.Config[sds_cfg.ConfigSize])
      ->StatusLedType = SUPLA_DEVCFG_STATUS_LED_ON_WHEN_CONNECTED;
  sds_cfg.ConfigSize += sizeof(TDeviceConfig_StatusLed);
  ASSERT_LE(sds_cfg.ConfigSize, SUPLA_DEVICE_CONFIG_MAXSIZE);

  ((TDeviceConfig_ScreenBrightness *)&sds_cfg.Config[sds_cfg.ConfigSize])
      ->ScreenBrightness = 100;
  sds_cfg.ConfigSize += sizeof(TDeviceConfig_ScreenBrightness);
  ASSERT_LE(sds_cfg.ConfigSize, SUPLA_DEVICE_CONFIG_MAXSIZE);

  ((TDeviceConfig_ButtonVolume *)&sds_cfg.Config[sds_cfg.ConfigSize])->Volume =
      0;
  sds_cfg.ConfigSize += sizeof(TDeviceConfig_ButtonVolume);
  ASSERT_LE(sds_cfg.ConfigSize, SUPLA_DEVICE_CONFIG_MAXSIZE);

  ((TDeviceConfig_HomeScreenDelay *)&sds_cfg.Config[sds_cfg.ConfigSize])
      ->HomeScreenDelayS = 10;
  sds_cfg.ConfigSize += sizeof(TDeviceConfig_HomeScreenDelay);
  ASSERT_LE(sds_cfg.ConfigSize, SUPLA_DEVICE_CONFIG_MAXSIZE);

  device_json_config cfg;
  cfg.set_config(&sds_cfg);
  char *str = cfg.get_user_config();
  ASSERT_TRUE(str != nullptr);
  EXPECT_STREQ(str,
               "{\"statusLed\":\"ON_WHEN_CONNECTED\",\"screenBrightness\":100,"
               "\"buttonVolume\":0,\"homeScreen\":{\"delay\":10}}");
  free(str);

  cfg.leave_only_thise_fields(SUPLA_DEVICE_CONFIG_FIELD_SCREEN_BRIGHTNESS);

  str = cfg.get_user_config();
  ASSERT_TRUE(str != nullptr);
  EXPECT_STREQ(str, "{\"screenBrightness\":100}");
  free(str);

  cfg.set_user_config("{\"screenBrightness\":24,\"a\":false}");
  cfg.leave_only_thise_fields(0);

  str = cfg.get_user_config();
  ASSERT_TRUE(str != nullptr);
  EXPECT_STREQ(str, "{\"a\":false}");
  free(str);
}

TEST_F(DeviceConfigTest, removeFields) {
  TSDS_SetDeviceConfig sds_cfg = {};
  sds_cfg.Fields = SUPLA_DEVICE_CONFIG_FIELD_STATUS_LED |
                   SUPLA_DEVICE_CONFIG_FIELD_SCREEN_BRIGHTNESS |
                   SUPLA_DEVICE_CONFIG_FIELD_BUTTON_VOLUME |
                   SUPLA_DEVICE_CONFIG_FIELD_HOME_SCREEN_DELAY;

  ((TDeviceConfig_StatusLed *)&sds_cfg.Config[sds_cfg.ConfigSize])
      ->StatusLedType = SUPLA_DEVCFG_STATUS_LED_ON_WHEN_CONNECTED;
  sds_cfg.ConfigSize += sizeof(TDeviceConfig_StatusLed);
  ASSERT_LE(sds_cfg.ConfigSize, SUPLA_DEVICE_CONFIG_MAXSIZE);

  ((TDeviceConfig_ScreenBrightness *)&sds_cfg.Config[sds_cfg.ConfigSize])
      ->ScreenBrightness = 100;
  sds_cfg.ConfigSize += sizeof(TDeviceConfig_ScreenBrightness);
  ASSERT_LE(sds_cfg.ConfigSize, SUPLA_DEVICE_CONFIG_MAXSIZE);

  ((TDeviceConfig_ButtonVolume *)&sds_cfg.Config[sds_cfg.ConfigSize])->Volume =
      0;
  sds_cfg.ConfigSize += sizeof(TDeviceConfig_ButtonVolume);
  ASSERT_LE(sds_cfg.ConfigSize, SUPLA_DEVICE_CONFIG_MAXSIZE);

  ((TDeviceConfig_HomeScreenDelay *)&sds_cfg.Config[sds_cfg.ConfigSize])
      ->HomeScreenDelayS = 15;
  sds_cfg.ConfigSize += sizeof(TDeviceConfig_HomeScreenDelay);
  ASSERT_LE(sds_cfg.ConfigSize, SUPLA_DEVICE_CONFIG_MAXSIZE);

  device_json_config cfg;
  cfg.set_config(&sds_cfg);
  char *str = cfg.get_user_config();
  ASSERT_TRUE(str != nullptr);
  EXPECT_STREQ(str,
               "{\"statusLed\":\"ON_WHEN_CONNECTED\",\"screenBrightness\":100,"
               "\"buttonVolume\":0,\"homeScreen\":{\"delay\":15}}");
  free(str);

  cfg.remove_fields(SUPLA_DEVICE_CONFIG_FIELD_STATUS_LED |
                    SUPLA_DEVICE_CONFIG_FIELD_SCREEN_BRIGHTNESS |
                    SUPLA_DEVICE_CONFIG_FIELD_HOME_SCREEN_DELAY);

  str = cfg.get_user_config();
  ASSERT_TRUE(str != nullptr);
  EXPECT_STREQ(str, "{\"buttonVolume\":0}");
  free(str);
}

TEST_F(DeviceConfigTest, merge) {
  device_json_config cfg1, cfg2;
  cfg1.set_user_config("{\"statusLed\":2,\"homeScreen\":{\"delay\":15}}");
  cfg2.set_user_config(
      "{\"a\":true,\"b\":123,\"statusLed\":1,\"buttonVolume\":100}");

  cfg1.merge(&cfg2);

  char *user_config = cfg2.get_user_config();
  ASSERT_NE(user_config, nullptr);

  EXPECT_STREQ(user_config,
               "{\"a\":true,\"b\":123,\"statusLed\":2,\"buttonVolume\":100,"
               "\"homeScreen\":{\"delay\":15}}");

  free(user_config);
}

TEST_F(DeviceConfigTest, getConfig_AllFields) {
  TSDS_SetDeviceConfig sds_cfg = {};
  device_json_config cfg1, cfg2;

  const char user_config[] =
      "{\"statusLed\":\"ALWAYS_OFF\",\"screenBrightness\":24,\"buttonVolume\":"
      "100,\"userInterfaceDisabled\":false,\"automaticTimeSync\":true,"
      "\"homeScreen\":{\"delay\":123,\"content\":\"TIME_DATE\"}}";
  cfg1.set_user_config(user_config);

  const char properties[] =
      "{\"homeScreenContentAvailable\":[\"NONE\",\"TEMPERATURE\",\"MAIN_AND_"
      "AUX_"
      "TEMPERATURE\"]}";
  cfg1.set_user_config(user_config);
  cfg1.set_properties(properties);

  unsigned _supla_int64_t fields_left = 0xFFFFFFFFFFFFFFFF;
  cfg1.get_config(&sds_cfg, &fields_left);

  EXPECT_EQ(fields_left, 0);
  EXPECT_EQ(sds_cfg.Fields,
            SUPLA_DEVICE_CONFIG_FIELD_STATUS_LED |
                SUPLA_DEVICE_CONFIG_FIELD_SCREEN_BRIGHTNESS |
                SUPLA_DEVICE_CONFIG_FIELD_BUTTON_VOLUME |
                SUPLA_DEVICE_CONFIG_FIELD_DISABLE_USER_INTERFACE |
                SUPLA_DEVICE_CONFIG_FIELD_AUTOMATIC_TIME_SYNC |
                SUPLA_DEVICE_CONFIG_FIELD_HOME_SCREEN_DELAY |
                SUPLA_DEVICE_CONFIG_FIELD_HOME_SCREEN_CONTENT);

  cfg2.set_config(&sds_cfg);
  char *user_config2 = cfg2.get_user_config();
  ASSERT_NE(user_config2, nullptr);

  char *properties2 = cfg2.get_properties();
  ASSERT_NE(properties2, nullptr);

  EXPECT_STREQ(user_config, user_config2);
  EXPECT_STREQ(properties, properties2);

  free(user_config2);
  free(properties2);
}

TEST_F(DeviceConfigTest, getConfig_AutoBrightness) {
  TSDS_SetDeviceConfig sds_cfg = {};
  device_json_config cfg1, cfg2;

  const char user_config[] = "{\"screenBrightness\":\"auto\"}";
  cfg1.set_user_config(user_config);

  unsigned _supla_int64_t fields_left = 0xFFFFFFFFFFFFFFFF;
  cfg1.get_config(&sds_cfg, &fields_left);

  EXPECT_EQ(fields_left, 0);

  EXPECT_EQ(sds_cfg.Fields, SUPLA_DEVICE_CONFIG_FIELD_SCREEN_BRIGHTNESS);

  cfg2.set_config(&sds_cfg);
  char *user_config2 = cfg2.get_user_config();
  ASSERT_NE(user_config2, nullptr);

  EXPECT_STREQ(user_config, user_config2);

  free(user_config2);
}

TEST_F(DeviceConfigTest, getConfig_OneField) {
  TSDS_SetDeviceConfig sds_cfg = {};
  device_json_config cfg1, cfg2;

  cfg1.set_user_config(
      "{\"statusLed\":2,\"screenBrightness\":24,\"buttonVolume\":100,"
      "\"userInterfaceDisabled\":false,\"automaticTimeSync\":true,"
      "\"homeScreen\":{\"delay\":123,\"content\":\"TimeDate\"}}");

  unsigned _supla_int64_t fields_left = 0xFFFFFFFFFFFFFFFF;
  cfg1.get_config(&sds_cfg, SUPLA_DEVICE_CONFIG_FIELD_AUTOMATIC_TIME_SYNC,
                  &fields_left);

  EXPECT_EQ(fields_left, 0);
  EXPECT_EQ(sds_cfg.Fields, SUPLA_DEVICE_CONFIG_FIELD_AUTOMATIC_TIME_SYNC);

  cfg2.set_config(&sds_cfg);
  char *user_config = cfg2.get_user_config();
  ASSERT_NE(user_config, nullptr);

  EXPECT_STREQ(user_config, "{\"automaticTimeSync\":true}");

  free(user_config);
}

TEST_F(DeviceConfigTest, statusLED) {
  TDeviceConfig_StatusLed status_led = {};
  device_json_config cfg1;
  device_json_config cfg2;
  cfg1.set_user_config("{\"statusLed\": \"ALWAYS_OFF\"}");

  EXPECT_TRUE(cfg1.get_status_led(&status_led));
  EXPECT_EQ(status_led.StatusLedType, SUPLA_DEVCFG_STATUS_LED_ALWAYS_OFF);

  TSDS_SetDeviceConfig sds_config = {};
  cfg1.get_config(&sds_config, nullptr);
  cfg2.set_config(&sds_config);
  char *str = cfg2.get_user_config();
  ASSERT_TRUE(str != nullptr);
  EXPECT_STREQ(str, "{\"statusLed\":\"ALWAYS_OFF\"}");
  free(str);

  cfg1.set_user_config("{\"statusLed\": \"ON_WHEN_CONNECTED\"}");

  EXPECT_TRUE(cfg1.get_status_led(&status_led));
  EXPECT_EQ(status_led.StatusLedType,
            SUPLA_DEVCFG_STATUS_LED_ON_WHEN_CONNECTED);

  sds_config = {};
  cfg1.get_config(&sds_config, nullptr);
  cfg2.set_config(&sds_config);
  str = cfg2.get_user_config();
  ASSERT_TRUE(str != nullptr);
  EXPECT_STREQ(str, "{\"statusLed\":\"ON_WHEN_CONNECTED\"}");
  free(str);

  cfg1.set_user_config("{\"statusLed\": \"OFF_WHEN_CONNECTED\"}");

  EXPECT_TRUE(cfg1.get_status_led(&status_led));
  EXPECT_EQ(status_led.StatusLedType,
            SUPLA_DEVCFG_STATUS_LED_OFF_WHEN_CONNECTED);

  sds_config = {};
  cfg1.get_config(&sds_config, nullptr);
  cfg2.set_config(&sds_config);
  str = cfg2.get_user_config();
  ASSERT_TRUE(str != nullptr);
  EXPECT_STREQ(str, "{\"statusLed\":\"OFF_WHEN_CONNECTED\"}");
  free(str);
}

TEST_F(DeviceConfigTest, homeScreenMode) {
  TDeviceConfig_HomeScreenContent content = {};
  device_json_config cfg1;
  device_json_config cfg2;
  cfg1.set_user_config("{\"homeScreen\": {\"content\": \"TIME_DATE\"}}");

  EXPECT_TRUE(cfg1.get_home_screen_content(&content));
  EXPECT_EQ(content.HomeScreenContent,
            SUPLA_DEVCFG_HOME_SCREEN_CONTENT_TIME_DATE);
  EXPECT_EQ(content.ContentAvailable, 0);

  TSDS_SetDeviceConfig sds_config = {};
  cfg1.get_config(&sds_config, nullptr);
  cfg2.set_config(&sds_config);
  char *str = cfg2.get_user_config();
  ASSERT_TRUE(str != nullptr);
  EXPECT_STREQ(str, "{\"homeScreen\":{\"content\":\"TIME_DATE\"}}");
  free(str);

  str = cfg2.get_properties();
  ASSERT_TRUE(str != nullptr);
  EXPECT_STREQ(str, "{\"homeScreenContentAvailable\":[]}");
  free(str);

  cfg1.set_user_config("{\"homeScreen\": {\"content\": \"HUMIDITY\"}}");

  EXPECT_TRUE(cfg1.get_home_screen_content(&content));
  EXPECT_EQ(content.HomeScreenContent,
            SUPLA_DEVCFG_HOME_SCREEN_CONTENT_HUMIDITY);

  sds_config = {};
  cfg1.get_config(&sds_config, nullptr);
  cfg2.set_config(&sds_config);
  str = cfg2.get_user_config();
  ASSERT_TRUE(str != nullptr);
  EXPECT_STREQ(str, "{\"homeScreen\":{\"content\":\"HUMIDITY\"}}");
  free(str);

  cfg1.set_user_config(
      "{\"homeScreen\": {\"content\": \"MAIN_AND_AUX_TEMPERATURE\"}}");
  cfg1.set_properties(
      "{\"homeScreenContentAvailable\":[\"MAIN_AND_AUX_TEMPERATURE\", "
      "\"HUMIDITY\"]}");

  EXPECT_TRUE(cfg1.get_home_screen_content(&content));
  EXPECT_EQ(content.HomeScreenContent,
            SUPLA_DEVCFG_HOME_SCREEN_CONTENT_MAIN_AND_AUX_TEMPERATURE);
  EXPECT_EQ(content.ContentAvailable,
            SUPLA_DEVCFG_HOME_SCREEN_CONTENT_MAIN_AND_AUX_TEMPERATURE |
                SUPLA_DEVCFG_HOME_SCREEN_CONTENT_HUMIDITY);

  sds_config = {};
  cfg1.get_config(&sds_config, nullptr);
  cfg2.set_config(&sds_config);
  str = cfg2.get_user_config();
  ASSERT_TRUE(str != nullptr);
  EXPECT_STREQ(str,
               "{\"homeScreen\":{\"content\":\"MAIN_AND_AUX_TEMPERATURE\"}}");
  free(str);

  str = cfg2.get_properties();
  ASSERT_TRUE(str != nullptr);
  EXPECT_STREQ(str,
               "{\"homeScreenContentAvailable\":[\"HUMIDITY\",\"MAIN_AND_AUX_"
               "TEMPERATURE\"]}");
  free(str);
}

TEST_F(DeviceConfigTest, availableFields) {
  device_json_config cfg;
  cfg.set_user_config("{\"homeScreen\": {\"content\": \"HUMIDITY\"}}");

  EXPECT_EQ(cfg.get_available_fields(),
            SUPLA_DEVICE_CONFIG_FIELD_HOME_SCREEN_CONTENT);

  cfg.set_user_config(
      "{\"statusLed\":2,\"screenBrightness\":24,\"buttonVolume\":100,"
      "\"userInterfaceDisabled\":false,\"automaticTimeSync\":true,"
      "\"homeScreen\": {\"delay\":123,\"content\": \"HUMIDITY\"}}");

  EXPECT_EQ(cfg.get_available_fields(),
            SUPLA_DEVICE_CONFIG_FIELD_STATUS_LED |
                SUPLA_DEVICE_CONFIG_FIELD_SCREEN_BRIGHTNESS |
                SUPLA_DEVICE_CONFIG_FIELD_BUTTON_VOLUME |
                SUPLA_DEVICE_CONFIG_FIELD_DISABLE_USER_INTERFACE |
                SUPLA_DEVICE_CONFIG_FIELD_AUTOMATIC_TIME_SYNC |
                SUPLA_DEVICE_CONFIG_FIELD_HOME_SCREEN_DELAY |
                SUPLA_DEVICE_CONFIG_FIELD_HOME_SCREEN_CONTENT);
}

} /* namespace testing */
