#include <gtest/gtest.h>
#include "g3log/crashhandler.hpp"

#if !(defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
#include <fcntl.h>
#include <unistd.h>

class SignalHandlingTest : public ::testing::Test {
  protected:
   int original_stderr;
   int pipefd[2];
   FILE* temp_stderr;

   void SetUp() override {
      // Redirect stderr to a pipe
      ASSERT_EQ(pipe(pipefd), 0);
      original_stderr = dup(STDERR_FILENO);
      ASSERT_NE(original_stderr, -1);
      ASSERT_NE(dup2(pipefd[1], STDERR_FILENO), -1);
      temp_stderr = fdopen(pipefd[1], "w");
      setvbuf(temp_stderr, NULL, _IONBF, 0);  // Disable buffering

      // Set the read end of the pipe to non-blocking mode
      // so we can verify when buffer is empty
      int flags = fcntl(pipefd[0], F_GETFL, 0);
      fcntl(pipefd[0], F_SETFL, flags | O_NONBLOCK);
   }

   void TearDown() override {
      // Restore the original stderr
      fclose(temp_stderr);
      close(pipefd[0]);
      close(pipefd[1]);
      dup2(original_stderr, STDERR_FILENO);
      close(original_stderr);
   }

   std::string ReadStderr() {
      char buffer[1024];
      ssize_t bytes_read = read(pipefd[0], buffer, sizeof(buffer) - 1);
      if (bytes_read >= 0) {
         buffer[bytes_read] = '\0';  // Null-terminate the string
         return std::string(buffer);
      }
      return "";
   }
};

TEST_F(SignalHandlingTest, WriteErrorMessage_WritesToStderr) {
   const char* test_message = "Test error message";
   g3::internal::writeErrorMessage(test_message);
   std::string output = ReadStderr();
   ASSERT_EQ(output, test_message);
}

TEST_F(SignalHandlingTest, WriteErrorMessage_Nullptr_DoesNotWriteToStderr) {
   g3::internal::writeErrorMessage(nullptr);
   std::string output = ReadStderr();
   ASSERT_TRUE(output.empty());
}

#endif  // #if !(defined(WIN32) || defined(_WIN32) || defined(__WIN32__))