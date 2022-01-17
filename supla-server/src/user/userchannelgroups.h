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

#ifndef USERCHANNELGROUPS_H_
#define USERCHANNELGROUPS_H_

#include <functional>
#include <list>

#include "dcpair.h"
#include "device/device.h"
#include "objcontainer.h"
#include "user.h"
#include "userchannelgroup.h"

class supla_user;
class supla_user_channelgroups : public supla_objcontainer {
 private:
  supla_user *user;
  bool for_each_channel(int GroupID, bool break_on_success,
                        std::function<bool(supla_device *, int, char)> f);
  bool for_each_channel(int GroupID,
                        std::function<bool(supla_device *, int, char)> f);

 protected:
  void _load(database *db, e_objc_scope scope);
  std::list<dcpair> find_channels(int GroupId);
  int available_data_types_for_remote(e_objc_scope scope);

 public:
  explicit supla_user_channelgroups(supla_user *user);
  bool set_char_value(int GroupID, const char value);
  bool set_on(int GroupID, bool on);
  bool set_color(int GroupID, unsigned int color);
  bool set_color_brightness(int GroupID, char color_brightness);
  bool set_brightness(int GroupID, char brightness);
  bool set_rgbw_value(int GroupID, unsigned int *color, char *color_brightness,
                      char *brightness, char *on_off);
  bool set_rgbw_value(int GroupID, int color, char color_brightness,
                      char brightness, char on_off);
  bool action_toggle(int GroupID);
  bool action_shut(int GroupID, const char *closing_percentage);
  bool action_reveal(int GroupID);
  bool action_up(int GroupID);
  bool action_down(int GroupID);
  bool action_up_or_stop(int GroupID);
  bool action_down_or_stop(int GroupID);
  bool action_step_by_step(int GroupID);
  bool action_stop(int GroupID);
  bool action_open(int GroupID);
  bool action_close(int GroupID);
  bool action_open_close(int GroupID);
};

#endif /* USERCHANNELGROUPS_H_ */
