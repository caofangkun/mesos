#include <signal.h>

#include <gmock/gmock.h>

#include <sys/types.h>

#include <string>

#include <process/gmock.hpp>
#include <process/gtest.hpp>
#include <process/io.hpp>
#include <process/subprocess.hpp>

#include <stout/foreach.hpp>
#include <stout/gtest.hpp>
#include <stout/list.hpp>
#include <stout/os/read.hpp>

using namespace process;

using std::string;


TEST(Subprocess, status)
{
  Clock::pause();

  // Exit 0.
  Try<Subprocess> s = subprocess("exit 0");

  ASSERT_SOME(s);

  // Advance time until the internal reaper reaps the subprocess.
  while (s.get().status().isPending()) {
    Clock::advance(Seconds(1));
    Clock::settle();
  }

  AWAIT_ASSERT_READY(s.get().status());
  ASSERT_SOME(s.get().status().get());

  int status = s.get().status().get().get();
  ASSERT_TRUE(WIFEXITED(status));
  ASSERT_EQ(0, WEXITSTATUS(status));

  // Exit 1.
  s = subprocess("exit 1");

  ASSERT_SOME(s);

  // Advance time until the internal reaper reaps the subprocess.
  while (s.get().status().isPending()) {
    Clock::advance(Seconds(1));
    Clock::settle();
  }

  AWAIT_ASSERT_READY(s.get().status());
  ASSERT_SOME(s.get().status().get());

  status = s.get().status().get().get();
  ASSERT_TRUE(WIFEXITED(status));
  ASSERT_EQ(1, WEXITSTATUS(status));

  // SIGTERM.
  s = subprocess("sleep 60");

  ASSERT_SOME(s);

  kill(s.get().pid(), SIGTERM);

  // Advance time until the internal reaper reaps the subprocess.
  while (s.get().status().isPending()) {
    Clock::advance(Seconds(1));
    Clock::settle();
  }

  AWAIT_ASSERT_READY(s.get().status());
  ASSERT_SOME(s.get().status().get());

  status = s.get().status().get().get();
  ASSERT_TRUE(WIFSIGNALED(status));
  ASSERT_EQ(SIGTERM, WTERMSIG(status));

  // SIGKILL.
  s = subprocess("sleep 60");

  ASSERT_SOME(s);

  kill(s.get().pid(), SIGKILL);

  // Advance time until the internal reaper reaps the subprocess.
  while (s.get().status().isPending()) {
    Clock::advance(Seconds(1));
    Clock::settle();
  }

  AWAIT_ASSERT_READY(s.get().status());
  ASSERT_SOME(s.get().status().get());

  status = s.get().status().get().get();
  ASSERT_TRUE(WIFSIGNALED(status));
  ASSERT_EQ(SIGKILL, WTERMSIG(status));

  Clock::resume();
}



TEST(Subprocess, output)
{
  Clock::pause();

  // Standard out.
  Try<Subprocess> s = subprocess("echo hello");

  ASSERT_SOME(s);

  ASSERT_SOME(os::nonblock(s.get().out()));

  AWAIT_EXPECT_EQ("hello\n", io::read(s.get().out()));

  // Advance time until the internal reaper reaps the subprocess.
  while (s.get().status().isPending()) {
    Clock::advance(Seconds(1));
    Clock::settle();
  }

  AWAIT_ASSERT_READY(s.get().status());
  ASSERT_SOME(s.get().status().get());
  int status = s.get().status().get().get();

  ASSERT_TRUE(WIFEXITED(status));
  ASSERT_EQ(0, WEXITSTATUS(status));

  // Standard error.
  s = subprocess("echo hello 1>&2");

  ASSERT_SOME(s);

  ASSERT_SOME(os::nonblock(s.get().err()));

  AWAIT_EXPECT_EQ("hello\n", io::read(s.get().err()));

  // Advance time until the internal reaper reaps the subprocess.
  while (s.get().status().isPending()) {
    Clock::advance(Seconds(1));
    Clock::settle();
  }

  AWAIT_ASSERT_READY(s.get().status());
  ASSERT_SOME(s.get().status().get());
  status = s.get().status().get().get();

  ASSERT_TRUE(WIFEXITED(status));
  ASSERT_EQ(0, WEXITSTATUS(status));

  Clock::resume();
}


TEST(Subprocess, input)
{
  Clock::pause();

  Try<Subprocess> s = subprocess("read word ; echo $word");

  ASSERT_SOME(s);

  ASSERT_SOME(os::write(s.get().in(), "hello\n"));

  ASSERT_SOME(os::nonblock(s.get().out()));

  AWAIT_EXPECT_EQ("hello\n", io::read(s.get().out()));

  // Advance time until the internal reaper reaps the subprocess.
  while (s.get().status().isPending()) {
    Clock::advance(Seconds(1));
    Clock::settle();
  }

  AWAIT_ASSERT_READY(s.get().status());
  ASSERT_SOME(s.get().status().get());
  int status = s.get().status().get().get();

  ASSERT_TRUE(WIFEXITED(status));
  ASSERT_EQ(0, WEXITSTATUS(status));

  Clock::resume();
}