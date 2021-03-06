/**
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __AUTHENTICATEE_HPP__
#define __AUTHENTICATEE_HPP__

#include <mesos/mesos.hpp>

#include <process/future.hpp>

#include <stout/nothing.hpp>
#include <stout/try.hpp>

namespace mesos {
namespace internal {

class Authenticatee
{
public:
  Authenticatee() {}

  virtual ~Authenticatee() {}

  // 'pid' is the process to authenticate against (master).
  // 'client' is the process to be authenticated (slave / framework).
  // 'credential' is used to authenticate the 'client'.
  // Returns true if successfully authenticated otherwise false or an
  // error. Note that we distinguish authentication failure (false)
  // from a failed future in the event the future failed due to a
  // transient error and authentication can (should) be
  // retried. Discarding the future will cause the future to fail if
  // it hasn't already completed since we have already started the
  // authentication procedure and can't reliably cancel.
  virtual process::Future<bool> authenticate(
    const process::UPID& pid,
    const process::UPID& client,
    const mesos::Credential& credential) = 0;
};

} // namespace internal {
} // namespace mesos {

#endif // __AUTHENTICATEE_HPP__
