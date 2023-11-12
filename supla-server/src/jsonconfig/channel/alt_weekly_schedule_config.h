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

#ifndef ALT_WEEKLY_SCHEDULE_CONFIG_H_
#define ALT_WEEKLY_SCHEDULE_CONFIG_H_

#include <string>

#include "jsonconfig/channel/weekly_schedule_config.h"
#include "proto.h"

class alt_weekly_schedule_config : public weekly_schedule_config {
 protected:
  virtual std::string get_weekly_shedule_key(void);

 public:
  explicit alt_weekly_schedule_config(supla_json_config *root);
  alt_weekly_schedule_config(void);
};

#endif /* ALT_WEEKLY_SCHEDULE_CONFIG_H_ */
