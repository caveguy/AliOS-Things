/*
 * Copyright (C) 2016 YunOS Project. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef YSH_BT_H
#define YSH_BT_H

#ifdef __cplusplus
extern "C" {
#endif

int32_t yoc_backtrace(void **buf, int32_t size);
char  **yoc_backtrace_symbols(void *const *buf, int32_t size);

void ysh_reg_cmd_bt(void);

#ifdef __cplusplus
}
#endif

#endif /* YSH_BT_H */

